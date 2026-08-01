#include "UI.h"
#include "Theme.h"
#include "Display.h"
#include "LGFX_Config.h"
#include "PrinterData.h"
#include "MQTT.h"
#include <WiFi.h>
#include "WiFiSetup.h"
#include "Settings.h"
#include "SleepManager.h"
#include "SystemStatus.h"
#include "AMSPage.h"
#include "HomePage.h"
#include "Icons.h"
#include "SystemPage.h"
#include "ControlPage.h"
#include "Widgets.h"
#include "Icons.h"
#include "Layout.h"
#include "TemperaturePage.h"




// Function prototypes
void drawTempBox(
    int x,
    int y,
    String label,
    float current,
    float target,
    uint16_t color
);

void drawPauseIcon(
    int x,
    int y,
    uint16_t color);
void drawPlayIcon(
    int x,
    int y, 
    uint16_t color);
void drawStopIcon(
    int x, 
    int y, 
    uint16_t color);
void drawLightIcon(
    int x, 
    int y, 
    uint16_t color);





void drawWiFiIcon(
    int x,
    int y,
    uint16_t color);

void forgetWiFiSettings();


void flashControlButton(
    int x,
    int y,
    int w,
    int h,
    const char* label
);







void drawUI()
{

    Serial.print("drawUI currentPage = ");
    Serial.println(currentPage);
    lcd.fillScreen(COLOR_BG);

    drawHeader();

    switch(currentPage)
    {
        case HOME_PAGE:
            drawHome();
            break;

        case AMS_PAGE:
            drawAMS();
            break;

        case TEMP_PAGE:
            drawTemperaturePage();
            break;

        case CONTROL_PAGE:
            drawPrintControl();
            break;

        case 4:
            drawSystem();
            break;
    }

    drawNavigation();
}



void drawHeader()
{
    int x = 5;
    int y = 5;
    int w = 310;
    int h = 45;


    // Header background
    lcd.fillRoundRect(
        x,
        y,
        w,
        h,
        8,
        COLOR_CARD
    );


    lcd.drawRoundRect(
        x,
        y,
        w,
        h,
        8,
        COLOR_BORDER
    );


    // Title
    lcd.setTextDatum(middle_left);

    lcd.setFont(&fonts::Font2);

    lcd.setTextColor(
        COLOR_TEXT
    );

    lcd.drawString(
        "Bambu Lab P1S",
        15,
        22
    );


// Determine if the printer is actually online
bool printerOnline =
    systemStatus.mqttConnected &&
    lastPrinterUpdate != 0 &&
    (millis() - lastPrinterUpdate) < 10000;

    // Determine status color and text
    uint16_t statusColor;
    String statusText;

    if (!printerOnline)
    {
        statusColor = TFT_DARKGREY;
        statusText = "OFFLINE";
    }
    else if (current_gcode_state == "RUNNING")
    {
        statusColor = TFT_BLUE;
        statusText = "PRINTING";
    }
    else if (current_gcode_state == "PAUSE")
    {
        statusColor = TFT_YELLOW;
        statusText = "PAUSED";
    }
    else if (current_gcode_state == "IDLE")
    {
        statusColor = TFT_GREEN;
        statusText = "READY";
    }
    else if (current_gcode_state == "FINISH")
    {
        statusColor = TFT_GREEN;
        statusText = "COMPLETE";
    }
    else if (current_gcode_state == "PREPARE")
    {
        statusColor = TFT_CYAN;
        statusText = "PREPARING";
    }
    else if (current_gcode_state == "SLICING")
    {
        statusColor = TFT_CYAN;
        statusText = "SLICING";
    }
    else if (current_gcode_state == "CALIBRATING")
    {
        statusColor = TFT_MAGENTA;
        statusText = "CALIBRATING";
    }
    else if (
        current_gcode_state == "FAILED" ||
        current_gcode_state == "ERROR"
    )
    {
        statusColor = TFT_RED;
        statusText = "ERROR";
    }
    else if (
        current_gcode_state == "UNKNOWN" ||
        current_gcode_state == ""
    )
    {
        statusColor = TFT_ORANGE;
        statusText = "UNKNOWN";
    }
    else
    {
        statusColor = TFT_ORANGE;
        statusText = current_gcode_state;
    }


    // Status dot
   // lcd.fillCircle(
  //     245,
  //      22,
   //     6,
   //     statusColor
   // );


    // Status text
    lcd.setTextDatum(
        middle_right
    );

    lcd.setTextColor(
        statusColor
    );

    lcd.drawString(
        statusText,
        308,
        22
    );
        // Current print file
    lcd.setTextDatum(middle_left);

    lcd.setFont(&fonts::Font2);

    lcd.setTextColor(
        COLOR_GRAY_TEXT
    );

    String file = current_file_name;

    if(file.length() > 28)
    {
        file = file.substring(0,28);
    }

    lcd.drawString(
        file,
        15,
        38);



    if (systemStatus.mqttConnected)
    {
        drawWiFiIcon(
            225,
            25,
            TFT_GREEN
        );
    }
    else
    {
        drawWiFiIcon(
            225,
            25,
            COLOR_GRAY_TEXT
        );
    }


}




void drawNavigation()
{
    int y = 205;
    int w = 64;

    const char* labels[] =
    {
        "HOME",
        "AMS",
        "TEMP",
        "CTRL",
        "SYS"
    };

    for (int i = 0; i < 5; i++)
    {
        int x = i * w;

        uint16_t bg =
            (i == currentPage)
            ? COLOR_ACCENT
            : COLOR_CARD;

        uint16_t fg =
            (i == currentPage)
            ? TFT_BLACK
            : COLOR_TEXT;

        lcd.fillRoundRect(
            x + 2,
            y,
            w - 4,
            30,
            6,
            bg
        );

        lcd.drawRoundRect(
            x + 2,
            y,
            w - 4,
            30,
            6,
            COLOR_BORDER
        );

        lcd.setTextDatum(
            middle_center
        );

        lcd.setFont(
            &fonts::Font2
        );

        lcd.setTextColor(
            fg,
            bg
        );

        lcd.drawString(
            labels[i],
            x + w / 2,
            y + 15
        );
    }
}





void drawControls()
{
    lcd.drawString(
        "CONTROL PAGE",
        160,
        100
    );
}





void drawTemperatureCard()
{
    return;
    int y = 146;

    drawTempBox(
        8,
        y,
        "NOZZLE",
        current_nozzle_temp,
        current_nozzle_target,
        TFT_ORANGE
    );


    drawTempBox(
        112,
        y,
        "BED",
        current_bed_temp,
        current_bed_target,
        TFT_CYAN
    );


    drawTempBox(
        216,
        y,
        "CHAMBER",
        current_chamber_temp,
        0,
        TFT_GREEN
    );
}

void drawTempBox(
    int x,
    int y,
    String label,
    float current,
    float target,
    uint16_t color
)
{
    int w = 96;
    int h = 55;


    lcd.fillRoundRect(
        x,
        y,
        w,
        h,
        8,
        COLOR_PANEL
    );


    lcd.drawRoundRect(
        x,
        y,
        w,
        h,
        8,
        COLOR_BORDER
    );


    // Accent line
    lcd.fillRoundRect(
        x,
        y,
        w,
        5,
        3,
        color
    );


    lcd.setTextDatum(
        middle_center
    );


    // Label
    lcd.setFont(
        &fonts::Font2
    );

    lcd.setTextColor(
        color
    );

    lcd.drawString(
        label,
        x + w/2,
        y + 14
    );


    // Current temperature
    lcd.setFont(
        &fonts::Font4
    );

    lcd.setTextColor(
        COLOR_TEXT
    );

    lcd.drawString(
        String(current,1) + "C",
        x + w/2,
        y + 34
    );





    // Target temperature
    if(target > 0)
    {
        lcd.setFont(
            &fonts::Font2
        );

        lcd.setTextColor(
            COLOR_GRAY_TEXT
        );

        drawNozzleIcon(
            x + 35,
            y + 40,
            COLOR_GRAY_TEXT
        );

        lcd.drawString(
            String(target,0),
            x + w/2 + 10,
            y + 48
        );
    }
    drawNozzleIcon(
    x + 10,
    y + 10,
    TFT_RED
);
}



void handleTouch()
{
    static bool touchWasDown = false;

    lgfx::touch_point_t tp;
    bool touchDown = lcd.getTouch(&tp);

    // Finger released
    if (!touchDown)
    {
        touchWasDown = false;
        return;
    }

    // Only process the first reading of each touch
    if (touchWasDown)
    {
        return;
    }

    touchWasDown = true;

    int x = tp.x;
    int y = tp.y;

    // Correct touch rotation
    x = 320 - x;
    y = 240 - y;

    // Reject invalid coordinates
    if (x < 0 || x >= 320 || y < 0 || y >= 240)
    {
        return;
    }

    // Wake display and reset inactivity timer
    bool wasDimmed = isScreenDimmed();
    bool wasSleeping = isScreenSleeping();

    resetSleepTimer();

    // First touch only wakes the display
    if (wasDimmed || wasSleeping)
    {
        return;
    }

    Serial.print("Touch X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);

    // -----------------------------
    // Bottom navigation bar
    // -----------------------------
    if (y >= 200)
    {
        int oldPage = currentPage;

        if (x < 65)
        {
            currentPage = HOME_PAGE;
        }
        else if (x < 130)
        {
            currentPage = AMS_PAGE;
        }
        else if (x < 195)
        {
            currentPage = TEMP_PAGE;
        }
        else if (x < 260)
        {
            currentPage = CONTROL_PAGE;
        }
        else
        {
            currentPage = SYS_PAGE;
        }

        if (oldPage != currentPage)
        {
            pageChanged = true;

            if (currentPage == AMS_PAGE)
            {
                requestAMSUpdate();
            }
        }

        // Do not also process page buttons
        return;
    }

    // -----------------------------
    // SYS PAGE BUTTONS
    // -----------------------------
    if (currentPage == SYS_PAGE)
    {
        // WiFi setup
        if (x >= 10 && x <= 155 &&
            y >= 157 && y <= 192)
        {
            drawWiFiSetupScreen();
            delay(300);
            startWiFiSetup();
            return;
        }

        // Forget WiFi
        if (x >= 165 && x <= 310 &&
            y >= 157 && y <= 192)
        {
            drawForgetWiFiScreen();
            delay(800);
            forgetWiFiSettings();
            return;
        }
    }

    // -----------------------------
    // Print control buttons
    // -----------------------------
    if (currentPage == CONTROL_PAGE)
    {
        // Pause / Resume
        if (x >= 25 && x <= 105 &&
            y >= 155 && y <= 193)
        {
            Serial.println("Pause/Resume pressed");

            if (current_gcode_state == "PAUSE")
            {

                resumePrint();flashControlButton(
                25,
                155,
                80,
                38,
                ""
                );
            }
            else if (current_gcode_state == "RUNNING")
            {
                flashControlButton(
                25,
                155,
                80,
                38,
                ""
                );
                pausePrint();
            }
            else
            {
                flashControlButton(
                25,
                155,
                80,
                38,
                "NOT RUNNING"
                 );

                Serial.print("Cannot pause. State=");
                Serial.println(current_gcode_state);
            }

            return;
        }

        // Stop
        if (x >= 120 && x <= 200 &&
            y >= 155 && y <= 193)
        {

        if (isPrintActive())
        {
            flashControlButton(
                120,
                155,
                80,
                38,
                ""
            );
            stopPrint();
        }

            Serial.println("Stop pressed");

            return;
        }

        // Chamber light
        if (x >= 215 && x <= 295 &&
            y >= 155 && y <= 193)
        {

            flashControlButton(
            215,
            155,
            80,
            38,
            ""
            );

            Serial.println("Light pressed");
            toggleChamberLight();
            return;
        }
    }
}







// -------------------------------
// drawPrintControl
// -------------------------------
void drawPrintControl()
{
    lcd.fillScreen(COLOR_PANEL);

    // Common header
    drawHeader();


    lcd.setTextDatum(middle_center);



    // -------------------------------
    // Information Card
    // -------------------------------

    lcd.fillRoundRect(
        20,
        60,
        280,
        80,
        10,
        COLOR_CARD
    );

    lcd.drawRoundRect(
        20,
        60,
        280,
        80,
        10,
        COLOR_BORDER
    );


    // Status

    String status = current_gcode_state;

    uint16_t statusColor = COLOR_GRAY_TEXT;


    if(status == "RUNNING")
    {
        status = "PRINTING";
        statusColor = TFT_GREEN;
    }
    else if(status == "PAUSE")
    {
        status = "PAUSED";
        statusColor = TFT_YELLOW;
    }



    lcd.setFont(&fonts::Font2);
    lcd.setTextColor(
        COLOR_TEXT
    );

    lcd.drawString(
        status,
        160,
        70
    );

    // File name

    String file = current_file_name;

    if(file.length() > 28)
    {
        file = file.substring(0,28);
    }


    lcd.setTextColor(
        COLOR_GRAY_TEXT
    );

    lcd.drawString(
        file,
        160,
        95
    );


    // Progress

    lcd.setTextColor(
        COLOR_TEXT
    );

    lcd.drawString(
        "Progress   " + String(current_progress) + "%",
        160,
        120
    );



    // -------------------------------
    // Bottom Buttons
    // -------------------------------


    // Pause / Resume

    lcd.fillRoundRect(
        25,
        155,
        80,
        38,
        10,
        TFT_BLUE
    );

    if(current_gcode_state == "PAUSE")
    {
        drawPlayIcon(
            55,
            163,
            TFT_BLACK
        );
    }
    else
    {
        drawPauseIcon(
            55,
            163,
            TFT_BLACK
        );
    }



    // Stop

    lcd.fillRoundRect(
        120,
        155,
        80,
        38,
        10,
        TFT_RED
    );


    drawStopIcon(
        148,
        161,
        TFT_WHITE
    );



    // Chamber Light

    lcd.fillRoundRect(
        215,
        155,
        80,
        38,
        10,
        COLOR_GREY
    );




    // Chamber light icon

    if(chamberLightOn)
    {
        Serial.println("Drawing LIGHT ON icon");

        drawLightIcon(
            255,
            170,
            TFT_YELLOW
        );
    }
    else
    {
        Serial.println("Drawing LIGHT OFF icon");

        drawLightIcon(
            255,
            170,
            TFT_WHITE
        );
}
}




void updateMQTTIcon()
{
    // Clear previous icon area
    lcd.fillRect(
        210,
        10,
        30,
        30,
        COLOR_PANEL
    );


    if(systemStatus.mqttConnected)
    {
        drawWiFiIcon(
            225,
            25,
            TFT_GREEN
        );
    }
    else
    {
        drawWiFiIcon(
            225,
            25,
            COLOR_GRAY_TEXT
        );
    }
}



void drawHotendIcon(
    int x,
    int y,
    uint16_t color
)
{

    // Make very dark filament visible
    if(color < 0x0841)
    {
        color = TFT_DARKGREY;
    }


    // heatsink
    lcd.fillRoundRect(
        x - 10,
        y,
        20,
        18,
        3,
        COLOR_BORDER
    );


    // nozzle
    lcd.fillTriangle(
        x - 5,
        y + 18,
        x + 5,
        y + 18,
        x,
        y + 25,
        0xa4c2   // brass color
    );

    lcd.drawLine(
    x,
    y + 10,
    x,
    y + 30,
    color
    );

    // heater glow
    lcd.fillCircle(
        x,
        y + 10,
        4,
        TFT_RED
    );
}





