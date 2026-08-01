#include <Arduino.h>

#include "PrinterData.h"
#include "LGFX_Config.h"
#include "Config.h"
#include "UI.h"
#include "MQTT.h"
#include "WiFiSetup.h"
#include "Settings.h"
#include "SleepManager.h"
#include "WebAdmin.h"
#include <WiFi.h>
#include "SystemStatus.h"

#include "UIUpdateFlags.h"
#include "HomePage.h"
#include "AMSPage.h"
#include "ControlPage.h"
#include "SystemPage.h"
#include "TemperaturePage.h"


LGFX lcd;

int currentPage = 0;
bool temperatureDisplayChanged = false;
String printerState = "ONLINE";

volatile bool displayWakeRequested = false;



void setup()
{
    Serial.begin(115200);


    delay(1000);

    Serial.print("Reset reason number: ");
    Serial.println((int)esp_reset_reason());


    randomSeed(micros());

    lcd.init();
    lcd.setRotation(3);
    lcd.setBrightness(255);
    initSleepManager();

    drawHome();

    // Load WiFi settings from storage
    loadSettings();

    if (wifiSSID.length() == 0)
    {
        Serial.println("No WiFi settings found");
        startWiFiSetup();
    }
    else
    {
        setupWiFi();
    }

    initWebAdmin();
    Serial.print("Admin IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
    
    setupMQTT();
    startMQTTTask();
    Serial.println("Bambu Dashboard V2 Started");
}

void loop()
{
    static unsigned long lastHeartbeatCheck = 0;
    static bool lastMQTTState = false;
    static bool printerWasOnline = false;

    // Web server, touch, and display stay on the Arduino loop
    updateWebAdmin();
    updateSleepManager();
    handleTouch();
    handleWiFiServer();

    if (displayWakeRequested)
    {
        displayWakeRequested = false;

        requestScreenWake();
    }

    // MQTT runs independently on Core 0
    bool currentMQTTState =
        systemStatus.mqttConnected;

    if (currentMQTTState != lastMQTTState)
    {
        lastMQTTState =
            currentMQTTState;

        updateMQTTIcon();

        // Redraw the header when MQTT changes state
        pageChanged = true;
    }

    // Check printer heartbeat once per second
    if (
        millis() - lastHeartbeatCheck >= 1000
    )
    {
        lastHeartbeatCheck =
            millis();

        bool printerOnline =
            currentMQTTState &&
            lastPrinterUpdate != 0 &&
            millis() - lastPrinterUpdate <= 10000;

        if (
            printerOnline !=
            printerWasOnline
        )
        {
            printerWasOnline =
                printerOnline;

            pageChanged = true;

            Serial.print(
                "Printer status changed: "
            );

            Serial.println(
                printerOnline
                    ? "ONLINE"
                    : "OFFLINE"
            );
        }
    }

    // Full redraw only for page changes
    if (pageChanged)
    {
        Serial.print("Full page redraw: ");
        Serial.println(currentPage);

        drawUI();

        pageChanged = false;

        // The full redraw already refreshed everything
        clearAllUIUpdates();
    }
    else
    {
        // Partial updates only
        if (uiUpdates.header)
        {
            drawHeader();
            uiUpdates.header = false;
        }

        if (currentPage == HOME_PAGE)
        {
            if (uiUpdates.progress)
            {
                updateHomeProgress();
                uiUpdates.progress = false;
            }
            
            if (uiUpdates.progressStatus)
            {
                updateHomeProgressStatus();
                uiUpdates.progressStatus = false;
            }

            if (uiUpdates.nozzle)
            {
                updateHomeNozzle();
                uiUpdates.nozzle = false;
            }

            if (uiUpdates.bed)
            {
                updateHomeBed();
                uiUpdates.bed = false;
            }

            if (uiUpdates.bottomInfo)
            {
                updateHomeBottomInfo();
                uiUpdates.bottomInfo = false;
            }
        }
        else if (currentPage == AMS_PAGE)
        {
            if (uiUpdates.ams)
            {
                drawAMS();
                uiUpdates.ams = false;
            }
        }
        else if (currentPage == CONTROL_PAGE)
        {
            if (uiUpdates.controls)
            {
                drawPrintControl();
                drawNavigation();

                uiUpdates.controls = false;
            }
        }
        else if (currentPage == SYS_PAGE)
        {
            if (uiUpdates.system)
            {
                drawSystem();
                uiUpdates.system = false;
            }
        }

        else if (currentPage == TEMP_PAGE)
        {
            if (uiUpdates.tempNozzle)
            {
                updateTemperatureNozzle();
                uiUpdates.tempNozzle = false;
            }

            if (uiUpdates.tempBed)
            {
                updateTemperatureBed();
                uiUpdates.tempBed = false;
            }

            if (uiUpdates.tempChamber)
            {
                updateTemperatureChamber();
                uiUpdates.tempChamber = false;
            }
        }
    }

    if (temperatureDisplayChanged)
    {
        uiUpdates.nozzle = true;
        uiUpdates.bed = true;

        temperatureDisplayChanged = false;
    }

    // Small yield for background system processing
    delay(1);
}