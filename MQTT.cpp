#include "MQTT.h"
#include "PrinterData.h"
#include "UI.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "SleepManager.h"
#include "SystemStatus.h"
#include "ControlPage.h"
#include "UIUpdateFlags.h"

extern bool wifiSetupActive;

extern String current_gcode_state;
extern String current_file_name;

extern int current_progress;
extern int current_time_remaining;

extern float current_nozzle_temp;
extern float current_nozzle_target;



extern float current_bed_temp;
extern float current_bed_target;

extern float current_chamber_temp;

unsigned long lastPrinterUpdate = 0;

extern String subscribe_topic;
extern String publish_topic;
String request_topic;

extern bool temperatureDisplayChanged;

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

extern int currentPage;
extern bool pageChanged;

// --- Bambu Cloud MQTT Broker Settings ---
// CHANGE THESE TO YOUR P1S SETTINGS
const char* mqtt_server = "192.168.1.215";
const int mqtt_port = 8883;

const char* mqttUser = "bblp";
const char* mqttPassword = "c53eae0a";

const char* mqttTopic = "device/XXXX/report";

const char* printer_serial = "01P00C632500950";  // Your P1S Printer Serial Number


TaskHandle_t mqttTaskHandle = nullptr;

extern volatile bool displayWakeRequested;


static bool waitingForGcodeState = false;
static unsigned long mqttConnectedTime = 0;
static unsigned long lastStatusRequestTime = 0;

// --------------------------------------
// CALLBACK
// --------------------------------------
void callback(char* topic, byte* payload, unsigned int length)
{

    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, payload, length);
    serializeJson(doc, Serial);
    Serial.println();




    if(error)
    {
        Serial.println("MQTT JSON ERROR");
        return;
    }


    if(!doc.containsKey("print"))
        return;


    JsonObject print = doc["print"];


    if (print.containsKey("gcode_state"))
    {
        String receivedState =
            print["gcode_state"].as<String>();

        Serial.print("Received gcode_state: ");
        Serial.println(receivedState);
    }


    lastPrinterUpdate = millis();

    //Serial.println("===== FULL MQTT MESSAGE =====");
    //serializeJsonPretty(doc, Serial);
    //Serial.println();
    //Serial.println("=============================");

    //Serial.print("Keys received: ");

    for(JsonPair kv : print)
    {
       // Serial.print(kv.key().c_str());
        //Serial.print(" ");
    }

    //Serial.println();

    //Serial.println("===== MQTT PRINT DATA =====");

    //serializeJsonPretty(print, Serial);

    //Serial.println();
    //Serial.println("===========================");

    if (print.containsKey("gcode_state"))
    {
        String newState =
            print["gcode_state"].as<String>();

        newState.toUpperCase();

        if (newState != current_gcode_state)
        {
            String previousState =
                current_gcode_state;

            current_gcode_state =
                newState;

            uiUpdates.header = true;
            uiUpdates.progressStatus = true;
            uiUpdates.controls = true;

            // Wake only when a print begins
            if (
                current_gcode_state == "RUNNING" &&
                previousState != "RUNNING"
            )
            {
                displayWakeRequested = true;
            }
        }
    }
    //Serial.println("===== PRINT OBJECT =====");
    //serializeJsonPretty(print, Serial);
    //Serial.println();
    //Serial.println("========================");

    bool changed = false;



    // -----------------------------
    // File name
    // -----------------------------

    if(print.containsKey("subtask_name"))
    {
        current_file_name =
            print["subtask_name"].as<String>();

        if(current_file_name.length() == 0)
            current_file_name = "None";

        changed = true;
    }



    // ----------------------------
    // Gcode State
    // ----------------------------
    if (print.containsKey("gcode_state"))
    {
        String newState =
            print["gcode_state"].as<String>();

        newState.trim();
        newState.toUpperCase();

        if (newState != current_gcode_state)
        {
            current_gcode_state = newState;

            // Mark only the UI elements that depend on printer state
            uiUpdates.header = true;
            uiUpdates.progress = true;
            uiUpdates.controls = true;

            changed = true;
        }
    }



    // -----------------------------
    // Progress
    // -----------------------------

    if (print.containsKey("mc_percent"))
    {
        int newProgress =
            print["mc_percent"].as<int>();

        newProgress =
            constrain(
                newProgress,
                0,
                100
            );

        if (newProgress != current_progress)
        {
            current_progress =
                newProgress;

            uiUpdates.progress =
                true;
        }
    }

    // -----------------------------
    // Remaining time
    // -----------------------------

    if(print.containsKey("mc_remaining_time"))
    {
        int newRemainingTime =
            print["mc_remaining_time"].as<int>();

        if(newRemainingTime != remaining_time)
        {
            remaining_time = newRemainingTime;
            changed = true;
        }
    }



    // -----------------------------
    // Remaining time
    // -----------------------------

    if(print.containsKey("mc_remaining_time"))
    {
        current_time_remaining =
            print["mc_remaining_time"].as<int>();

        changed = true;
    }

    // -----------------------------
    // Layer Information
    // -----------------------------

    if(print.containsKey("layer_num"))
    {
        current_layer =
            print["layer_num"].as<int>();

        changed = true;
    }


    if(print.containsKey("total_layer_num"))
    {
        total_layers =
            print["total_layer_num"].as<int>();

        changed = true;
    }


    //Serial.print("Layer: ");
    //Serial.print(current_layer);
    //Serial.print("/");
    //Serial.println(total_layers);

    // -----------------------------
    // Temperatures
    // -----------------------------


    if (print.containsKey("nozzle_temper"))
    {
        float newTemp =
            print["nozzle_temper"].as<float>();

        if (newTemp != current_nozzle_temp)
        {
            current_nozzle_temp = newTemp;

            // Home-page nozzle card
            uiUpdates.nozzle = true;

            // Temperature-page nozzle row
            uiUpdates.tempNozzle = true;
        }
    }


    if (print.containsKey("nozzle_target_temper"))
    {
        float newTarget =
            print["nozzle_target_temper"].as<float>();

        if (newTarget != current_nozzle_target)
        {
            current_nozzle_target = newTarget;

            uiUpdates.nozzle = true;
            uiUpdates.tempNozzle = true;
        }
    }


    if (print.containsKey("bed_temper"))
    {
        float newTemp =
            print["bed_temper"].as<float>();

        if (newTemp != current_bed_temp)
        {
            current_bed_temp = newTemp;

            // Home-page bed card
            uiUpdates.bed = true;

            // Temperature-page bed row
            uiUpdates.tempBed = true;
        }
    }

    if (print.containsKey("bed_target_temper"))
    {
        float newTarget =
            print["bed_target_temper"].as<float>();

        if (newTarget != current_bed_target)
        {
            current_bed_target = newTarget;

            uiUpdates.bed = true;
            uiUpdates.tempBed = true;
        }
    }


    if (print.containsKey("chamber_temper"))
    {
        float raw =
            print["chamber_temper"].as<float>();

        float newChamberTemp;

        if (raw > 0 && raw < 15)
        {
            newChamberTemp = raw + 30;
        }
        else
        {
            newChamberTemp = raw;
        }

        if (newChamberTemp != current_chamber_temp)
        {
            current_chamber_temp =
                newChamberTemp;

            uiUpdates.tempChamber = true;
        }
    }



    // -----------------------------
    // AMS DATA
    // -----------------------------

    if(print.containsKey("ams"))
    {

        JsonObject amsData = print["ams"];



        //Serial.println("===== AMS UNIT DATA =====");
        //serializeJson(amsData, Serial);
        //Serial.println();

    if (amsData.containsKey("tray_pre"))
    {
        //Serial.print("Raw tray_pre: ");
        //serializeJson(amsData["tray_pre"], Serial);
        //Serial.println();

        String trayStr =
            amsData["tray_pre"].as<String>();

        int newTray =
            trayStr.toInt();

        // 255 means no filament source selected
        if (newTray == 255)
        {
            newTray = -1;
        }

        //Serial.print("Parsed tray: ");
        //Serial.println(newTray);

        currentAMSTray = newTray;

        // Physical AMS slots are 0 through 3
        for (int i = 0; i < 4; i++)
        {
            ams[i].active =
                newTray == i;
        }

        // External spool is virtual tray 254
        externalSpool.active =
            newTray == 254;

        //Serial.print("External spool active: ");
        //Serial.println(
        //    externalSpool.active ? "YES" : "NO"
        //);

        changed = true;
    }



        if(amsData.containsKey("ams"))
        {

            JsonArray units =
                amsData["ams"];


            if(units.size())
            {


                JsonObject unit = units[0];

                //Serial.println("===== AMS UNIT KEYS =====");

                for(JsonPair kv : unit)
                {
                    //Serial.println(kv.key().c_str());
                }

                Serial.println("=========================");

                // AMS humidity level (1-5)
                if(unit.containsKey("humidity"))
                {
                    currentAMSHumidity =
                        unit["humidity"].as<String>().toInt();
                }

                // Raw humidity value
                if(unit.containsKey("humidity_raw"))
                {
                    currentAMSHumidityRaw =
                        unit["humidity_raw"].as<String>().toInt();
                }

                // AMS internal temperature
                if(unit.containsKey("temp"))
                {
                    currentAMSTemp =
                        unit["temp"].as<String>().toFloat();
                }


                JsonArray trays =
                    units[0]["tray"];


                for (int i = 0; i < 4; i++)
                {
                    ams[i].type = "";
                    ams[i].color = "#404040";
                    ams[i].empty = true;

                    if (i >= trays.size())
                    {
                        continue;
                    }

                    JsonObject tray =
                        trays[i];

                    // Filament type
                    if (tray.containsKey("tray_type"))
                    {
                        ams[i].type =
                            tray["tray_type"].as<String>();
                    }

                    // Filament color
                    if (tray.containsKey("tray_color"))
                    {
                        String color =
                            tray["tray_color"].as<String>();

                        if (color.length() >= 6)
                        {
                            ams[i].color =
                                "#" + color.substring(0, 6);

                            ams[i].empty = false;
                        }
                    }

                    // A valid type also means the slot is occupied
                    if (ams[i].type.length() > 0)
                    {
                        ams[i].empty = false;
                    }
                

                }

            }

        }

    }

    // -----------------------------
    // External spool / virtual tray
    // -----------------------------

    if (print.containsKey("vt_tray"))
    {
        JsonObject virtualTray =
            print["vt_tray"];

       // Serial.println(
        //    "===== EXTERNAL SPOOL DATA ====="
        //);

        //serializeJsonPretty(
       //     virtualTray,
       //     Serial
       // );

        //Serial.println();
        //Serial.println(
        //    "==============================="
        //);

        // Only reset when a vt_tray update is received
        externalSpool.type = "";
        externalSpool.color = "#404040";
        externalSpool.empty = true;

        if (virtualTray.containsKey("tray_type"))
        {
            externalSpool.type =
                virtualTray["tray_type"].as<String>();
        }

        if (virtualTray.containsKey("tray_color"))
        {
            String color =
                virtualTray["tray_color"].as<String>();

            //Serial.print(
            //    "External spool color received: "
            //);
            //Serial.println(color);

            if (color.length() >= 6)
            {
                externalSpool.color =
                    "#" + color.substring(0, 6);

                externalSpool.empty = false;
            }
        }

        if (externalSpool.type.length() > 0)
        {
            externalSpool.empty = false;
        }

        //Serial.print("External spool type: ");
        //Serial.println(externalSpool.type);

        //Serial.print("External spool stored color: ");
        //Serial.println(externalSpool.color);

        changed = true;
    }



    if (changed)
    {
        uiUpdates.header = true;
        uiUpdates.nozzle = true;
        uiUpdates.bed = true;
        uiUpdates.bottomInfo = true;
        uiUpdates.ams = true;
        uiUpdates.controls = true;
        uiUpdates.system = true;
    }
}



void setupMQTT()
{
    secureClient.setInsecure();

    // Limit the time spent attempting an unavailable connection
    secureClient.setTimeout(2000);
    mqttClient.setSocketTimeout(2);

    mqttClient.setServer(
        mqtt_server,
        mqtt_port
    );

    mqttClient.setCallback(callback);
    mqttClient.setBufferSize(16384);
    mqttClient.setKeepAlive(60);

    subscribe_topic =
        "device/" + String(printer_serial) + "/report";

    publish_topic =
        "device/" + String(printer_serial) + "/request";

    Serial.print("Subscribe topic: ");
    Serial.println(subscribe_topic);

    Serial.println("MQTT setup complete");
}

void loopMQTT()
{
    static unsigned long lastReconnect = 0;

    const unsigned long reconnectInterval = 30000;

    if (!mqttClient.connected())
    {
        if (millis() - lastReconnect >= reconnectInterval)
        {
            lastReconnect = millis();

            Serial.println("MQTT reconnecting...");

            String clientId =
                "ESP32_P1S_" +
                String(random(0xffff), HEX);

            if (mqttClient.connect(
                    clientId.c_str(),
                    mqttUser,
                    mqttPassword))
            {
                Serial.println("MQTT connected");

                bool subscribed =
                    mqttClient.subscribe(
                        subscribe_topic.c_str()
                    );

                Serial.print("Subscribe: ");
                Serial.println(
                    subscribed ? "OK" : "FAILED"
                );

                requestAMSUpdate();

                waitingForGcodeState = true;
                mqttConnectedTime = millis();
                lastStatusRequestTime = 0;
            }
            else
            {
                Serial.print("Connect failed. State=");
                Serial.println(mqttClient.state());
            }
        }

        return;
    }

    mqttClient.loop();

    if (waitingForGcodeState)
    {
        const unsigned long now = millis();

        if (
            now - mqttConnectedTime >= 1000 &&
            now - lastStatusRequestTime >= 3000
        )
        {
            lastStatusRequestTime = now;

            Serial.println("Requesting full printer status...");
            request_status();
        }
    }
}

void request_status()
{
    if (!mqttClient.connected())
    {
        return;
    }

    StaticJsonDocument<256> doc;

    JsonObject pushing =
        doc.createNestedObject("pushing");

    pushing["sequence_id"] = "0";
    pushing["command"] = "pushall";
    pushing["version"] = 1;
    pushing["push_target"] = 1;

    String payload;
    serializeJson(doc, payload);

    bool published =
        mqttClient.publish(
            "device/01P00C632500950/request",
            payload.c_str()
        );

    Serial.print("Pushall request: ");
    Serial.println(
        published ? "SENT" : "FAILED"
    );

    Serial.print("Payload: ");
    Serial.println(payload);
}


void pausePrint()
{
    String payload =
    "{\"print\":{\"sequence_id\":\"0\",\"command\":\"pause\"}}";

    mqttClient.publish(
        publish_topic.c_str(),
        payload.c_str()
    );

    //Serial.println(payload);
    Serial.println("Sent PAUSE command");
}



void resumePrint()
{
    String payload =
    "{\"print\":{\"sequence_id\":\"0\",\"command\":\"resume\"}}";

    mqttClient.publish(
        publish_topic.c_str(),
        payload.c_str()
    );

    //Serial.println(payload);
    Serial.println("Sent RESUME command");
}


void stopPrint()
{
    String payload =
    "{\"print\":{\"sequence_id\":\"0\",\"command\":\"stop\"}}";


    mqttClient.publish(
        publish_topic.c_str(),
        payload.c_str()
    );


    Serial.println("Sent STOP command");
}




//
// TOGGLE CHAMBER LIGHT
//
void toggleChamberLight()
{
    bool newState = !chamberLightOn;


    String payload =
    "{\"system\":{\"sequence_id\":\"0\",\"command\":\"ledctrl\",\"led_node\":\"chamber_light\",\"led_mode\":\""
    + String(newState ? "on" : "off")
    + "\"}}";


    //Serial.println(payload);


    mqttClient.publish(
        publish_topic.c_str(),
        payload.c_str()
    );


    Serial.println(
        newState ?
        "Sent LIGHT ON" :
        "Sent LIGHT OFF"
    );


    // Update display state
    chamberLightOn = newState;


    // Refresh control page
    drawLightButton();
    drawNavigation();


}

void requestAMSUpdate()
{
    String payload =
    "{\"print\":{\"sequence_id\":\"0\",\"command\":\"pushall\"}}";


    mqttClient.publish(
        publish_topic.c_str(),
        payload.c_str()
    );


    //Serial.println(payload);
    Serial.println("Requested AMS update");
}

//
// MQTT background task
//
// Runs independently from the display, touch,
// and WebAdmin code.
//
void mqttTask(void* parameter)
{
    const unsigned long reconnectInterval = 30000;

    unsigned long lastReconnectAttempt = 0;

    for (;;)
    {


        // Do not attempt MQTT without Wi-Fi
        if (WiFi.status() != WL_CONNECTED)
        {
            systemStatus.mqttConnected = false;

            vTaskDelay(
                pdMS_TO_TICKS(1000)
            );

            continue;
        }

        if (!mqttClient.connected())
        {
            systemStatus.mqttConnected = false;

            unsigned long now = millis();

            if (
                lastReconnectAttempt == 0 ||
                now - lastReconnectAttempt >= reconnectInterval
            )
            {
                lastReconnectAttempt = now;

                Serial.println();
                Serial.println("MQTT task: connecting...");

                String clientId =
                    "ESP32_P1S_" +
                    String(
                        random(0xffff),
                        HEX
                    );

                Serial.println(
                    "MQTT task: before connect"
                );

                bool connected =
                    mqttClient.connect(
                        clientId.c_str(),
                        mqttUser,
                        mqttPassword
                    );

                Serial.println(
                    "MQTT task: after connect"
                );

                if (connected)
                {
                    systemStatus.mqttConnected = true;

                    Serial.println(
                        "MQTT task: connected"
                    );

                    bool subscribed =
                        mqttClient.subscribe(
                            subscribe_topic.c_str()
                        );

                    Serial.print(
                        "Subscribe topic: "
                    );

                    Serial.println(
                        subscribe_topic
                    );

                    Serial.print(
                        "Subscribe: "
                    );

                    Serial.println(
                        subscribed
                            ? "OK"
                            : "FAILED"
                    );

                    // Request fresh printer information
                    requestAMSUpdate();
                    request_status();
                }
                else
                {
                    systemStatus.mqttConnected = false;

                    Serial.print(
                        "MQTT connect failed. State="
                    );

                    Serial.println(
                        mqttClient.state()
                    );
                }
            }
        }
        else
        {
            systemStatus.mqttConnected = true;

            // Process incoming MQTT packets
            mqttClient.loop();
        }

        // Give the processor time for other tasks
        vTaskDelay(
            pdMS_TO_TICKS(10)
        );
    }
}


void startMQTTTask()
{
    if (mqttTaskHandle != nullptr)
    {
        return;
    }

    BaseType_t result =
        xTaskCreatePinnedToCore(
            mqttTask,           // Task function
            "MQTT_Task",        // Task name
            12288,              // Stack size
            nullptr,            // Parameter
            1,                  // Priority
            &mqttTaskHandle,    // Task handle
            0                   // Core 0
        );

    if (result == pdPASS)
    {
        Serial.println(
            "MQTT task started on Core 0"
        );
    }
    else
    {
        Serial.println(
            "ERROR: MQTT task failed to start"
        );

        mqttTaskHandle = nullptr;
    }
}