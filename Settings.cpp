#include <Preferences.h>
#include "Settings.h"

Preferences preferences;


String wifiSSID = "";
String wifiPassword = "";
String printerIP = "192.168.1.159";


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


    preferences.end();


    Serial.println("Settings loaded");
    Serial.println(wifiSSID);
    Serial.println(printerIP);
}


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


    preferences.end();


    Serial.println("Settings saved");
}

void clearSettings()
{
    preferences.begin("bambu", false);

    preferences.clear();

    preferences.end();

    Serial.println("Settings erased");
}