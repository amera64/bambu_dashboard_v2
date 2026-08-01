#pragma once

#include <Arduino.h>

#include "LGFX_Config.h"


// -----------------------------
// Screen Pages
// -----------------------------

#define HOME_PAGE      0
#define AMS_PAGE       1
#define TEMP_PAGE      2
#define CONTROL_PAGE   3
#define SYS_PAGE       4



extern LGFX lcd;

extern int currentPage;

void drawUI();

void drawHeader();

void drawNavigation();

void drawHome();

void drawControls();

void drawSystem();

void drawTemperatureCard();

void drawPrintInfo();

void handleTouch();

void drawCurrentPage();

void updateStatus(
    float nozzle,
    float bed,
    float chamber,
    int remaining
);

void drawTemperatureCard();

void drawNozzleIcon(int x, int y, uint16_t color);

void drawBedIcon(int x, int y, uint16_t color);

void updateMQTTIcon();

void drawChamberIcon(int x, int y, uint16_t color);

void drawHumidityIcon(int x, int y, uint16_t color);

uint16_t getContrastTextColor(uint16_t color);

