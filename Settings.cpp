#include <Preferences.h>
#include "Settings.h"

Preferences preferences;


// ----------------------------------------------------
// Stored settings
// ----------------------------------------------------

// Wi-Fi
String wifiSSID = "";
String wifiPassword = "";

// Bambu printer
String printerIP = "192.168.1.159";
String printerSerial = "";
String printerAccessCode = "";


// ----------------------------------------------------
// Load settings from ESP32 Preferences
// ----------------------------------------------------

void loadSettings()
{
    preferences.begin("bambu", true);

    wifiSSID =
        preferences.getString(
            "ssid",
            ""
        );

    wifiPassword =
        preferences.getString(
            "pass",
            ""
        );

    printerIP =
        preferences.getString(
            "ip",
            "192.168.1.159"
        );

    printerSerial =
        preferences.getString(
            "serial",
            ""
        );

    printerAccessCode =
        preferences.getString(
            "access",
            ""
        );

    preferences.end();


    Serial.println();
    Serial.println("Settings loaded");

    Serial.print("Wi-Fi SSID: ");
    Serial.println(
        wifiSSID.length() > 0
            ? wifiSSID
            : "(not configured)"
    );

    Serial.print("Printer IP: ");
    Serial.println(
        printerIP.length() > 0
            ? printerIP
            : "(not configured)"
    );

    Serial.print("Printer serial: ");
    Serial.println(
        printerSerial.length() > 0
            ? printerSerial
            : "(not configured)"
    );

    Serial.print("LAN access code: ");
    Serial.println(
        printerAccessCode.length() > 0
            ? "(configured)"
            : "(not configured)"
    );
}


// ----------------------------------------------------
// Save settings to ESP32 Preferences
// ----------------------------------------------------

void saveSettings()
{
    preferences.begin("bambu", false);

    preferences.putString(
        "ssid",
        wifiSSID
    );

    preferences.putString(
        "pass",
        wifiPassword
    );

    preferences.putString(
        "ip",
        printerIP
    );

    preferences.putString(
        "serial",
        printerSerial
    );

    preferences.putString(
        "access",
        printerAccessCode
    );

    preferences.end();


    Serial.println();
    Serial.println("Settings saved");

    Serial.print("Printer IP: ");
    Serial.println(printerIP);

    Serial.print("Printer serial: ");
    Serial.println(printerSerial);

    // Do not print the actual access code.
    Serial.print("LAN access code: ");
    Serial.println(
        printerAccessCode.length() > 0
            ? "(configured)"
            : "(not configured)"
    );
}


// ----------------------------------------------------
// Erase all stored settings
// ----------------------------------------------------

void clearSettings()
{
    preferences.begin("bambu", false);

    preferences.clear();

    preferences.end();


    wifiSSID = "";
    wifiPassword = "";

    printerIP = "192.168.1.159";
    printerSerial = "";
    printerAccessCode = "";


    Serial.println();
    Serial.println("All stored settings erased");
}