#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

// Wi-Fi settings
extern String wifiSSID;
extern String wifiPassword;

// Bambu printer settings
extern String printerIP;
extern String printerSerial;
extern String printerAccessCode;

// Settings functions
void loadSettings();
void saveSettings();
void clearSettings();

#endif