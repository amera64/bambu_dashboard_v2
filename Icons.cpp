#include "Icons.h"

#include "Display.h"
#include "Theme.h"











void drawBedIcon(int x, int y, uint16_t color)
{
    // print bed surface
    lcd.fillRoundRect(
        x,
        y + 14,
        24,
        5,
        2,
        color
    );

    // supports
    lcd.drawLine(
        x + 4,
        y + 19,
        x + 4,
        y + 24,
        color
    );

    lcd.drawLine(
        x + 20,
        y + 19,
        x + 20,
        y + 24,
        color
    );

    // top layer
    lcd.drawLine(
        x + 2,
        y + 12,
        x + 22,
        y + 12,
        color
    );
}

void drawPauseIcon(int x, int y, uint16_t color)
{
    lcd.fillRoundRect(
        x,
        y,
        7,
        24,
        2,
        color
    );

    lcd.fillRoundRect(
        x + 14,
        y,
        7,
        24,
        2,
        color
    );
}


void drawPlayIcon(int x, int y, uint16_t color)
{
    lcd.fillTriangle(
        x,
        y,
        x,
        y + 24,
        x + 22,
        y + 12,
        color
    );
}


void drawStopIcon(int x, int y, uint16_t color)
{
    lcd.fillRoundRect(
        x,
        y,
        24,
        24,
        3,
        color
    );
}


void drawLightIcon(
    int x,
    int y,
    uint16_t color
)
{
    // Bulb globe
    lcd.fillCircle(
        x,
        y,
        5,
        color
    );

    // Neck
    lcd.fillRoundRect(
        x - 3,
        y + 5,
        6,
        4,
        1,
        color
    );

    // Base
    lcd.drawLine(
        x - 4,
        y + 10,
        x + 4,
        y + 10,
        color
    );

    lcd.drawLine(
        x - 3,
        y + 12,
        x + 3,
        y + 12,
        color
    );
}


void drawLayerIcon(
    int x,
    int y,
    uint16_t color
)
{
    // Bottom layer
    lcd.fillRoundRect(
        x,
        y + 8,
        16,
        4,
        1,
        color
    );

    // Middle layer
    lcd.fillRoundRect(
        x + 2,
        y + 4,
        16,
        4,
        1,
        color
    );

    // Top layer
    lcd.fillRoundRect(
        x + 4,
        y,
        16,
        4,
        1,
        color
    );
}


void drawClockIcon(
    int x,
    int y,
    uint16_t color
)
{
    // Clock face
    lcd.drawCircle(
        x,
        y,
        8,
        color
    );

    // Center
    lcd.fillCircle(
        x,
        y,
        1,
        color
    );

    // Hour hand
    lcd.drawLine(
        x,
        y,
        x,
        y - 4,
        color
    );

    // Minute hand
    lcd.drawLine(
        x,
        y,
        x + 3,
        y + 2,
        color
    );
}


//
// Draw WIFI Icon
//
void drawWiFiIcon(
    int x,
    int y,
    uint16_t color)
{
    // Outer arc
    lcd.drawArc(
        x,
        y,
        10,
        10,
        220,
        320,
        color
    );

    // Middle arc
    lcd.drawArc(
        x,
        y,
        6,
        6,
        220,
        320,
        color
    );

    // Center dot
    lcd.fillCircle(
        x,
        y,
        2,
        color
    );
}



void drawChamberIcon(int x, int y, uint16_t color)
{
    // small thermometer bulb
    lcd.fillCircle(
        x,
        y + 5,
        3,
        color
    );

    // thin thermometer stem
    lcd.fillRoundRect(
        x - 1,
        y - 8,
        3,
        12,
        1,
        color
    );

    // inner cutout
    lcd.fillCircle(
        x,
        y + 5,
        1,
        COLOR_PANEL
    );
}

void drawHumidityIcon(int x, int y, uint16_t color)
{
    // Water drop shape
    lcd.fillCircle(
        x,
        y + 5,
        5,
        color
    );

    lcd.fillTriangle(
        x - 5,
        y + 5,
        x + 5,
        y + 5,
        x,
        y - 6,
        color
    );
}



//
// Home icon
//
void drawHomeIcon(
    int x,
    int y,
    uint16_t color
)
{
    // Roof
    lcd.fillTriangle(
        x,
        y - 10,
        x - 11,
        y,
        x + 11,
        y,
        color
    );

    // House body
    lcd.fillRoundRect(
        x - 8,
        y,
        16,
        12,
        2,
        color
    );

    // Door
    lcd.fillRect(
        x - 2,
        y + 5,
        4,
        7,
        COLOR_CARD
    );
}


//
// AMS / spool icon
//
void drawAMSIcon(
    int x,
    int y,
    uint16_t color
)
{
    lcd.drawCircle(
        x,
        y,
        10,
        color
    );

    lcd.drawCircle(
        x,
        y,
        4,
        color
    );

    lcd.fillCircle(
        x,
        y,
        2,
        color
    );

    // Filament tail
    lcd.drawLine(
        x + 8,
        y + 5,
        x + 13,
        y + 9,
        color
    );
}


//
// Temperature icon
//
void drawTemperatureIcon(
    int x,
    int y,
    uint16_t color
)
{
    // Stem
    lcd.drawRoundRect(
        x - 3,
        y - 11,
        6,
        17,
        3,
        color
    );

    lcd.fillRoundRect(
        x - 1,
        y - 7,
        3,
        12,
        1,
        color
    );

    // Bulb
    lcd.fillCircle(
        x,
        y + 7,
        6,
        color
    );
}


//
// Print-control icon
//
void drawControlIcon(
    int x,
    int y,
    uint16_t color
)
{
    // Play triangle
    lcd.fillTriangle(
        x - 8,
        y - 9,
        x - 8,
        y + 9,
        x + 5,
        y,
        color
    );

    // Pause bars
    lcd.fillRoundRect(
        x + 8,
        y - 8,
        3,
        16,
        1,
        color
    );

    lcd.fillRoundRect(
        x + 13,
        y - 8,
        3,
        16,
        1,
        color
    );
}


//
// System / gear icon
//
void drawSystemIcon(
    int x,
    int y,
    uint16_t color
)
{
    lcd.drawCircle(
        x,
        y,
        7,
        color
    );

    lcd.fillCircle(
        x,
        y,
        3,
        color
    );

    // Gear teeth
    lcd.fillRect(
        x - 2,
        y - 12,
        4,
        4,
        color
    );

    lcd.fillRect(
        x - 2,
        y + 8,
        4,
        4,
        color
    );

    lcd.fillRect(
        x - 12,
        y - 2,
        4,
        4,
        color
    );

    lcd.fillRect(
        x + 8,
        y - 2,
        4,
        4,
        color
    );

    lcd.fillRect(
        x - 9,
        y - 9,
        4,
        4,
        color
    );

    lcd.fillRect(
        x + 5,
        y - 9,
        4,
        4,
        color
    );

    lcd.fillRect(
        x - 9,
        y + 5,
        4,
        4,
        color
    );

    lcd.fillRect(
        x + 5,
        y + 5,
        4,
        4,
        color
    );
}