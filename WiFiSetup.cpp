
#include <WiFi.h>
#include <WebServer.h>
#include "WiFiSetup.h"
#include "Settings.h"
#include "WebAdmin.h"


bool wifiSetupActive = false;

WebServer server(80);
// Function declarations
void handleWiFiPage();
void handleWiFiSave();
void restartAfterWiFiSave();
void buildWiFiList();

String wifiOptions;

// --- Configuration ---
//const char* ssid = "AMnetgear";
//const char* password = "malibu2515";

void setupWiFi()
{
    Serial.print("Connecting WiFi");

    //WiFi.begin(ssid,password);
    WiFi.begin(
    wifiSSID.c_str(),
    wifiPassword.c_str()
    );

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");

    Serial.println(WiFi.localIP());
}

// -----------------------------
// WIFI SETUP ACCESS POINT
// -----------------------------

void startWiFiSetup()
{
    // Prevent starting setup twice
    if (wifiSetupActive)
    {
        return;
    }

    wifiSetupActive = true;

    Serial.println();
    Serial.println("Starting WiFi setup mode...");

    // Free port 80 currently used by WebAdmin
    stopWebAdmin();

    delay(100);

    // Stop the setup server in case it was previously started
    server.stop();

    /*
     * AP+STA mode permits the ESP32 to create its setup access point
     * while also allowing Wi-Fi network scanning.
     */
    WiFi.mode(WIFI_AP_STA);

    bool accessPointStarted =
        WiFi.softAP("Bambu_P1S_Setup");

    if (!accessPointStarted)
    {
        Serial.println(
            "ERROR: Unable to start setup access point."
        );

        return;
    }

    Serial.println("Setup access point started.");
    Serial.print("Network: ");
    Serial.println("Bambu_P1S_Setup");

    Serial.print("Setup address: http://");
    Serial.println(WiFi.softAPIP());

    Serial.println("Scanning for WiFi networks...");

    buildWiFiList();

    // Wi-Fi setup routes
    server.on(
        "/",
        HTTP_GET,
        handleWiFiPage
    );

    server.on(
        "/save",
        HTTP_POST,
        handleWiFiSave
    );

    server.onNotFound([]()
    {
        server.sendHeader(
            "Location",
            "/",
            true
        );

        server.send(
            302,
            "text/plain",
            ""
        );
    });

    server.begin();

    Serial.println("WiFi setup web server started.");
}

//
// WiFi SSID Scan
//
void buildWiFiList()
{
    wifiOptions = "";

    int networks = WiFi.scanNetworks();

    for (int i = 0; i < networks; i++)
    {
        wifiOptions += "<option value='";
        wifiOptions += WiFi.SSID(i);
        wifiOptions += "'>";

        wifiOptions += WiFi.SSID(i);
        wifiOptions += " (";
        wifiOptions += String(WiFi.RSSI(i));
        wifiOptions += " dBm)";
        wifiOptions += "</option>";
    }

    WiFi.scanDelete();
}


void handleWiFiPage()
{
    Serial.println("Web page requested");


    String page = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>

body {
    font-family: Arial;
    background:#202020;
    color:white;
    text-align:center;
}

input, select {
    width:90%;
    padding:12px;
    margin:8px;
    font-size:18px;
}

button {
    width:90%;
    padding:14px;
    margin-top:20px;
    font-size:20px;
    background:#2196F3;
    color:white;
    border:none;
    border-radius:8px;
}

.card {
    background:#303030;
    padding:20px;
    margin:20px;
    border-radius:12px;
}

</style>

</head>


<body>

<div class="card">

<h2>Bambu P1S Setup</h2>


<form action="/save" method="POST">


WiFi Network:<br>

<select name="ssid">

)rawliteral";

page += wifiOptions;

page += R"rawliteral(

</select>


Password:<br>

<input 
type="password"
name="pass"
value="">
<br>


Printer IP:<br>

<input 
name="ip"
value="192.168.1.159">
<br>


<button type="submit">
SAVE
</button>


</form>

</div>

</body>

</html>
)rawliteral";


    server.send(
        200,
        "text/html",
        page
    );
}


void handleWiFiSave()
{
    Serial.println("Saving WiFi settings");


    wifiSSID =
        server.arg("ssid");


    wifiPassword =
        server.arg("pass");


    printerIP =
        server.arg("ip");


    Serial.println(wifiSSID);
    Serial.println(printerIP);


    saveSettings();

    server.send(
        200,
        "text/html",
        "<html><body style='background:#202020;color:white;text-align:center;'>"
        "<h2>Settings Saved</h2>"
        "<p>Restarting...</p>"
        "</body></html>"
    );


    delay(1000);

    restartAfterWiFiSave();
}


// -----------------------------
// Web server handler
// -----------------------------

void handleWiFiServer()
{
    if (wifiSetupActive)
    {
        server.handleClient();
    }
}


void restartAfterWiFiSave()
{
    Serial.println("Restarting ESP32...");

    delay(2000);

    ESP.restart();
}

void forgetWiFiSettings()
{
    Serial.println("Clearing WiFi settings");

    clearSettings();

    delay(1000);

    ESP.restart();
}