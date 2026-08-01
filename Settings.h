#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

extern String wifiSSID;
extern String wifiPassword;
extern String printerIP;

void loadSettings();
void saveSettings();
void clearSettings();

#endif