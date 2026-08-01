#include "SystemPage.h"

#include <Arduino.h>
#include <WiFi.h>

#include "Display.h"
#include "Theme.h"
#include "Settings.h"
#include "SystemStatus.h"






// -----------------------------
// SYSTEM PAGE
// -----------------------------

void drawSystem()
{
    lcd.setTextDatum(top_left);


    // -----------------------------
    // Title
    // -----------------------------

    lcd.setFont(&fonts::Font2);
    lcd.setTextColor(COLOR_ACCENT);



    // -----------------------------
    // WIFI CARD
    // -----------------------------

    lcd.drawRoundRect(
        10,
        55,
        300,
        45,
        8,
        COLOR_BORDER
    );


    lcd.setTextColor(COLOR_ACCENT);
    lcd.drawString(
        "WIFI",
        20,
        61
    );


    lcd.setTextColor(COLOR_TEXT);

    lcd.drawString(
        WiFi.SSID(),
        85,
        61
    );


    lcd.drawString(
        "Signal: " + String(WiFi.RSSI()) + " dBm",
        20,
        79
    );


    // -----------------------------
    // PRINTER CARD
    // -----------------------------

    lcd.drawRoundRect(
        10,
        105,
        300,
        45,
        8,
        COLOR_BORDER
    );


    lcd.setTextColor(COLOR_ACCENT);

    lcd.drawString(
        "PRINTER",
        20,
        111
    );


    lcd.setTextColor(COLOR_TEXT);

    lcd.drawString(
        "MQTT: " + String(systemStatus.mqttConnected ? "CONNECTED" : "OFFLINE"),
        20,
        129
    );

    lcd.drawString(
        "IP: " + printerIP,
        170,
        129
    );
    // -----------------------------
    // WIFI SETUP BUTTON
    // -----------------------------

    lcd.fillRoundRect(
        10,
        157,
        145,
        35,
        8,
        TFT_BLUE
    );

    lcd.setTextDatum(middle_center);

    lcd.setTextColor(COLOR_TEXT);

    lcd.drawString(
        "WIFI SETUP",
        82,
        174
    );
    //
    // Forget Wifi Button
    //
        lcd.fillRoundRect(
        165,
        157,
        145,
        35,
        8,
        TFT_RED
    );

    lcd.drawString(
        "FORGET WIFI",
        237,
        174
    );

}


void drawWiFiSetupScreen()
{
    lcd.fillScreen(TFT_BLACK);

    lcd.setTextDatum(MC_DATUM);

    lcd.setTextColor(TFT_WHITE);
    lcd.setTextSize(2);
    lcd.drawString("WiFi Setup Mode", 160, 70);

    lcd.setTextSize(1);
    lcd.setTextColor(TFT_LIGHTGREY);
    lcd.drawString("Connect your phone to:", 160, 115);

    lcd.setTextColor(TFT_CYAN);
    lcd.setTextSize(2);
    lcd.drawString("Bambu_P1S_Setup", 160, 145);

    lcd.setTextColor(TFT_LIGHTGREY);
    lcd.setTextSize(1);
    lcd.drawString("Then open:", 160, 180);

    lcd.setTextColor(TFT_YELLOW);
    lcd.setTextSize(2);
    lcd.drawString("192.168.4.1", 160, 205);

    lcd.setTextDatum(TL_DATUM);
}

void drawForgetWiFiScreen()
{
    lcd.fillScreen(TFT_BLACK);

    lcd.setTextDatum(MC_DATUM);

    lcd.setTextColor(TFT_YELLOW);
    lcd.setTextSize(2);
    lcd.drawString("Forgetting WiFi", 160, 95);

    lcd.setTextColor(TFT_LIGHTGREY);
    lcd.setTextSize(1);
    lcd.drawString("Clearing saved settings...", 160, 135);

    lcd.setTextColor(TFT_WHITE);
    lcd.drawString("Restarting", 160, 165);

    lcd.setTextDatum(TL_DATUM);
}
