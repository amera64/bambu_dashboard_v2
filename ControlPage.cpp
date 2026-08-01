#include "ControlPage.h"

#include <Arduino.h>

#include "Display.h"
#include "Theme.h"
#include "PrinterData.h"
#include "Icons.h"


void drawHeader();




void drawPrintInfo()
{
    int y = 215;


    lcd.fillRoundRect(
        10,
        y,
        300,
        20,
        6,
        COLOR_PANEL
    );


    lcd.drawRoundRect(
        10,
        y,
        300,
        20,
        6,
        COLOR_BORDER
    );


    lcd.setTextDatum(middle_center);

    lcd.setTextColor(
        COLOR_TEXT
    );

    lcd.setFont(&fonts::Font2);


    lcd.drawString(
        current_file_name,
        160,
        y + 10
    );


    lcd.setFont(&fonts::Font2);
}



void drawLightButton()
{
    // Button background
    lcd.fillRoundRect(
        215,
        155,
        80,
        38,
        10,
        COLOR_GREY
    );


    // Draw current state icon
    if(chamberLightOn)
    {
        drawLightIcon(
            255,
            170,
            TFT_YELLOW
        );
    }
    else
    {
        drawLightIcon(
            255,
            170,
            TFT_WHITE
        );
    }
}


void flashControlButton(
    int x,
    int y,
    int w,
    int h,
    const char* label
)
{
    const int radius = 10;

    // Pressed appearance
    lcd.fillRoundRect(x, y, w, h, radius, TFT_DARKGREY);
    lcd.drawRoundRect(x, y, w, h, radius, TFT_WHITE);

    lcd.setTextDatum(middle_center);
    lcd.setTextColor(TFT_WHITE, TFT_DARKGREY);
    lcd.setFont(&fonts::Font2);

    lcd.drawString(
        label,
        x + (w / 2),
        y + (h / 2)
    );

    lcd.setTextDatum(top_left);

    delay(100);
}

