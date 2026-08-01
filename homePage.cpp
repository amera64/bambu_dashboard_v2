#include "HomePage.h"

#include <Arduino.h>

#include "Display.h"
#include "Theme.h"
#include "PrinterData.h"
#include "Widgets.h"
#include "Layout.h"

static String formatRemainingTime();

static void drawDashboardV2();
static void drawProgressCard();
static void drawNozzleCard();
static void drawBedCard();
static void drawBottomInfo();
static void drawProgressStatus();
static void drawProgressRingGraphic();
static void drawProgressPercent();


void drawClockIcon(
    int x,
    int y,
    uint16_t color
);

void drawLayerIcon(
    int x,
    int y,
    uint16_t color
);

void drawHumidityIcon(
    int x,
    int y,
    uint16_t color
);

void drawHome()
{
    drawDashboardV2();
}


void updateHomeProgressStatus()
{
    drawProgressStatus();
}

static void drawDashboardV2()
{
    // Main progress card
    drawProgressCard();


    // Nozzle temperature card
    drawNozzleCard();

    // Bed temperature card
    drawBedCard();


    // Bottom information row
    drawCard(
      Layout::BOTTOM_INFO_X,
      Layout::BOTTOM_INFO_Y,
      Layout::BOTTOM_INFO_WIDTH,
      Layout::BOTTOM_INFO_HEIGHT
    );

    drawBottomInfo();
}


void drawBottomInfo()
{
    int x = 10;
    int y = 170;

    lcd.setTextDatum(middle_center);

    lcd.setFont(&fonts::Font0);

    lcd.setTextColor(
        COLOR_GRAY_TEXT,
        COLOR_PANEL
    );

//
// Time Remaining and Layers Display
//
    drawClockIcon(
        x + 20,
        y + 15,
        COLOR_ACCENT
    );

    lcd.setTextColor(COLOR_TEXT);

    lcd.setFont(&fonts::Font2);

    lcd.setTextColor(
        COLOR_TEXT,
        COLOR_PANEL
    );

    lcd.drawString(
        formatRemainingTime(),
        x + 54,
        y + 15
    );

    drawLayerIcon(
        x + 100,
        y + 9,
        COLOR_ACCENT
    );

    lcd.drawString(
    String(current_layer),
    x + 136,
    y + 15);


    drawHumidityIcon(
        x + 176,
        y + 12,
        TFT_BLUE
    );

    lcd.setTextColor(
        TFT_WHITE,
        COLOR_CARD
    );

    lcd.setTextSize(1);

    lcd.setCursor(
        x + 188,
        y + 15
    );

    lcd.print(
        String(currentAMSHumidityRaw)
        + "%"
    );

}







void drawNozzleCard()
{
    const int x =
        Layout::NOZZLE_X;

    const int y =
        Layout::NOZZLE_Y;

    const int w =
        Layout::NOZZLE_WIDTH;

    const int h =
        Layout::NOZZLE_HEIGHT;


    drawCard(x,y,w,h);


    lcd.setTextDatum(middle_center);


    // Title
    lcd.setFont(&fonts::Font0);
    lcd.setTextColor(
        COLOR_GRAY_TEXT,
        COLOR_PANEL
    );

    lcd.drawString(
        "NOZZLE",
        x + 35,
        y + 12
    );


    // Temperature
    lcd.setFont(&fonts::Font4);

    lcd.setTextColor(
        COLOR_TEXT,
        COLOR_PANEL
    );


    String temp =
        String((int)current_nozzle_temp)
        + " / "
        + String((int)current_nozzle_target)
        + " C";


    lcd.drawString(
        temp,
        x + 75,
        y + 29
    );


    // Heating indicator
    uint16_t barColor = COLOR_GRAY_TEXT;

    if(current_nozzle_target > 0)
    {
        if(current_nozzle_temp >= current_nozzle_target - 2)
            barColor = COLOR_GREEN;
        else
            barColor = COLOR_ERROR;
    }


    int barWidth = map(
        current_nozzle_temp,
        0,
        current_nozzle_target > 0 ? current_nozzle_target : 250,
        0,
        w - 20
    );


    // Limit bar to card width
    if(barWidth < 0)
        barWidth = 0;

    if(barWidth > w - 20)
        barWidth = w - 20;


    lcd.fillRoundRect(
        x + 10,
        y + 40,
        barWidth,
        5,
        2,
        barColor
    );
}


void drawBedCard()
{
    const int x =
        Layout::BED_X;

    const int y =
        Layout::BED_Y;

    const int w =
        Layout::BED_WIDTH;

    const int h =
        Layout::BED_HEIGHT;


    drawCard(x,y,w,h);


    lcd.setTextDatum(middle_center);


    lcd.setFont(&fonts::Font0);

    lcd.setTextColor(
        COLOR_GRAY_TEXT,
        COLOR_PANEL
    );

    lcd.drawString(
        "BED",
        x + 25,
        y + 12
    );


    lcd.setFont(&fonts::Font4);

    lcd.setTextColor(
        COLOR_TEXT,
        COLOR_PANEL
    );


    String temp =
        String((int)current_bed_temp)
        + " / "
        + String((int)current_bed_target)
        + " C";


    lcd.drawString(
        temp,
        x + 80,
        y + 29
    );


    uint16_t barColor = COLOR_GRAY_TEXT;


    if(current_bed_target > 0)
    {
        if(current_bed_temp >= current_bed_target - 2)
            barColor = COLOR_GREEN;
        else
            barColor = COLOR_ERROR;
    }


    int barWidth = map(
        current_bed_temp,
        0,
        current_bed_target > 0 ? current_bed_target : 100,
        0,
        w - 20
    );


    lcd.fillRoundRect(
        x + 10,
        y + 40,
        barWidth,
        5,
        2,
        barColor
    );
}


void drawProgressCard()
{
    const int x =
        Layout::PROGRESS_X;

    const int y =
        Layout::PROGRESS_Y;

    const int w =
        Layout::PROGRESS_WIDTH;

    const int h =
        Layout::PROGRESS_HEIGHT;


    // Card background
    drawCard(
        x,
        y,
        w,
        h
    );


    // Draw the ring
    drawProgressRingGraphic();
    drawProgressPercent();


    // Title
    lcd.setTextDatum(middle_center);

    lcd.setFont(&fonts::Font0);

    lcd.setTextColor(
        COLOR_GRAY_TEXT,
        COLOR_PANEL
    );

    //lcd.drawString(
    //    "PRINT JOB",
    //    x + (w / 2),
    //    y + 12
    //);

    drawProgressRingGraphic();
    drawProgressPercent();
    drawProgressStatus();

   
}


//
// Draw the progress ring graphics
//
static void drawProgressRingGraphic()
{
    const int cardX =
        Layout::PROGRESS_X;

    const int cardY =
        Layout::PROGRESS_Y;

    const int cardWidth =
        Layout::PROGRESS_WIDTH;

    const int centerX =
        cardX + cardWidth / 2;

    // Moved slightly upward to make room for status text
    const int centerY =
        cardY + 47;

    // Larger ring
    const int radius = 38;
    const int ringThickness = 10;

    // Clear the complete ring area
    lcd.fillCircle(
        centerX,
        centerY,
        radius + 4,
        COLOR_PANEL
    );

    // Background ring
    lcd.fillCircle(
        centerX,
        centerY,
        radius,
        COLOR_GRAY_TEXT
    );

    lcd.fillCircle(
        centerX,
        centerY,
        radius - ringThickness,
        COLOR_PANEL
    );

    const int progressAngle =
        map(
            constrain(
                current_progress,
                0,
                100
            ),
            0,
            100,
            0,
            360
        );

    if (progressAngle > 0)
    {
        if (progressAngle >= 360)
        {
            lcd.fillCircle(
                centerX,
                centerY,
                radius,
                COLOR_BLUE
            );

            lcd.fillCircle(
                centerX,
                centerY,
                radius - ringThickness,
                COLOR_PANEL
            );
        }
        else
        {
            lcd.fillArc(
                centerX,
                centerY,
                radius,
                radius - ringThickness,
                270,
                270 + progressAngle,
                COLOR_BLUE
            );
        }
    }

    // Center area
    lcd.fillCircle(
        centerX,
        centerY,
        radius - ringThickness - 2,
        COLOR_PANEL
    );
}

//
// Draw the percentage inside the progress ring
//
static void drawProgressPercent()
{
    const int cardX =
        Layout::PROGRESS_X;

    const int cardY =
        Layout::PROGRESS_Y;

    const int cardWidth =
        Layout::PROGRESS_WIDTH;

    const int centerX =
        cardX + cardWidth / 2;

    const int centerY =
        cardY + 47;

    const int radius = 38;
    const int ringThickness = 10;

    // Clear only inside the ring.
    // This removes the old percentage without touching the ring.
    lcd.fillCircle(
        centerX,
        centerY,
        radius - ringThickness - 2,
        COLOR_PANEL
    );

    String progressText =
        String(
            constrain(
                current_progress,
                0,
                100
            )
        ) +
        "%";

    lcd.setTextDatum(
        middle_center
    );

    lcd.setFont(
        &fonts::Font4
    );

    // One color means transparent text background.
    lcd.setTextColor(
        COLOR_TEXT
    );

    lcd.drawString(
        progressText,
        centerX,
        centerY
    );
}



static String formatRemainingTime()
{
    int hours =
        remaining_time / 60;

    int minutes =
        remaining_time % 60;

    char buffer[10];

    snprintf(
        buffer,
        sizeof(buffer),
        "%02d:%02d",
        hours,
        minutes
    );

    return String(buffer);
}


void updateHomeProgress()
{
    drawProgressRingGraphic();
    drawProgressPercent();
}

void updateHomeNozzle()
{
    drawNozzleCard();
}

void updateHomeBed()
{
    drawBedCard();
}

void updateHomeBottomInfo()
{
    drawCard(
        Layout::BOTTOM_INFO_X,
        Layout::BOTTOM_INFO_Y,
        Layout::BOTTOM_INFO_WIDTH,
        Layout::BOTTOM_INFO_HEIGHT
    );

    drawBottomInfo();
}
static void drawProgressStatus()
{
    const int x = Layout::PROGRESS_X;
    const int y = Layout::PROGRESS_Y;
    const int w = Layout::PROGRESS_WIDTH;
    const int h = Layout::PROGRESS_HEIGHT;

    // Clear only the status line
    lcd.fillRect(
        x + 10,
        y + h - 24,
        w - 20,
        18,
        COLOR_PANEL
    );

    uint16_t statusColor = COLOR_GREEN;

    if (current_gcode_state == "PAUSE")
        statusColor = COLOR_ACCENT;
    else if (current_gcode_state == "FAILED" ||
             current_gcode_state == "OFFLINE")
        statusColor = COLOR_RED;

    lcd.setTextDatum(middle_center);
    lcd.setFont(&fonts::Font0);
    lcd.setTextColor(statusColor, COLOR_PANEL);

    lcd.drawString(
        current_gcode_state,
        x + w / 2,
        y + h - 12
    );
}








