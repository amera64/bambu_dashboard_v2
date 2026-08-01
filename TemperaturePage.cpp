#include "TemperaturePage.h"

#include <Arduino.h>

#include "Display.h"
#include "Theme.h"
#include "PrinterData.h"


// ----------------------------------------------------
// Page layout
// ----------------------------------------------------

static constexpr int BAR_X = 15;
static constexpr int BAR_WIDTH = 290;
static constexpr int BAR_HEIGHT = 38;

static constexpr int NOZZLE_Y = 58;
static constexpr int BED_Y = 106;
static constexpr int CHAMBER_Y = 154;


// ----------------------------------------------------
// Private declarations
// ----------------------------------------------------

static void drawTemperatureBar(
    int y,
    const char* label,
    float currentTemperature,
    float targetTemperature,
    float maximumTemperature
);

static uint16_t getTemperatureBarColor(
    float currentTemperature,
    float targetTemperature
);


// ----------------------------------------------------
// Draw complete temperature page
// ----------------------------------------------------

void drawTemperaturePage()
{
    drawTemperatureBar(
        NOZZLE_Y,
        "NOZZLE",
        current_nozzle_temp,
        current_nozzle_target,
        300.0f
    );

    drawTemperatureBar(
        BED_Y,
        "BED",
        current_bed_temp,
        current_bed_target,
        120.0f
    );

    drawTemperatureBar(
        CHAMBER_Y,
        "CHAMBER",
        current_chamber_temp,
        0.0f,
        70.0f
    );
}


// ----------------------------------------------------
// Individual partial-update functions
// ----------------------------------------------------

void updateTemperatureNozzle()
{
    drawTemperatureBar(
        NOZZLE_Y,
        "NOZZLE",
        current_nozzle_temp,
        current_nozzle_target,
        300.0f
    );
}


void updateTemperatureBed()
{
    drawTemperatureBar(
        BED_Y,
        "BED",
        current_bed_temp,
        current_bed_target,
        120.0f
    );
}


void updateTemperatureChamber()
{
    drawTemperatureBar(
        CHAMBER_Y,
        "CHAMBER",
        current_chamber_temp,
        0.0f,
        70.0f
    );
}


// ----------------------------------------------------
// Draw one large temperature bar
// ----------------------------------------------------

static void drawTemperatureBar(
    int y,
    const char* label,
    float currentTemperature,
    float targetTemperature,
    float maximumTemperature
)
{
    const int cardX = BAR_X;
    const int cardY = y;
    const int cardWidth = BAR_WIDTH;
    const int cardHeight = BAR_HEIGHT;

    const int barX = cardX + 10;
    const int barY = cardY + 26;
    const int barWidth = cardWidth - 20;
    const int barHeight = 7;

    // Clear and redraw this complete temperature row
    lcd.fillRoundRect(
        cardX,
        cardY,
        cardWidth,
        cardHeight,
        8,
        COLOR_PANEL
    );

    lcd.drawRoundRect(
        cardX,
        cardY,
        cardWidth,
        cardHeight,
        8,
        COLOR_BORDER
    );

    // Label
    lcd.setTextDatum(
        middle_left
    );

    lcd.setFont(
        &fonts::Font2
    );

    lcd.setTextColor(
        COLOR_GRAY_TEXT,
        COLOR_PANEL
    );

    lcd.drawString(
        label,
        cardX + 10,
        cardY + 12
    );

    // Current / target temperature
    String temperatureText;

    if (targetTemperature > 0)
    {
        temperatureText =
            String(
                currentTemperature,
                0
            ) +
            " / " +
            String(
                targetTemperature,
                0
            ) +
            " C";
    }
    else
    {
        temperatureText =
            String(
                currentTemperature,
                1
            ) +
            " C";
    }

    lcd.setTextDatum(
        middle_right
    );

    lcd.setFont(
        &fonts::Font2
    );

    lcd.setTextColor(
        COLOR_TEXT,
        COLOR_PANEL
    );

    lcd.drawString(
        temperatureText,
        cardX + cardWidth - 10,
        cardY + 12
    );

    // Bar background
    lcd.fillRoundRect(
        barX,
        barY,
        barWidth,
        barHeight,
        3,
        COLOR_GRAY_TEXT
    );

    float displayedMaximum =
        maximumTemperature;

    // When heating, scale the bar around the target so it remains useful
    if (
        targetTemperature > 0 &&
        targetTemperature < displayedMaximum
    )
    {
        displayedMaximum =
            targetTemperature;
    }

    int filledWidth =
        static_cast<int>(
            (
                constrain(
                    currentTemperature,
                    0.0f,
                    displayedMaximum
                ) /
                displayedMaximum
            ) *
            barWidth
        );

    filledWidth =
        constrain(
            filledWidth,
            0,
            barWidth
        );

    const uint16_t barColor =
        getTemperatureBarColor(
            currentTemperature,
            targetTemperature
        );

    if (filledWidth > 0)
    {
        lcd.fillRoundRect(
            barX,
            barY,
            filledWidth,
            barHeight,
            3,
            barColor
        );
    }
}


// ----------------------------------------------------
// Determine temperature status color
// ----------------------------------------------------

static uint16_t getTemperatureBarColor(
    float currentTemperature,
    float targetTemperature
)
{
    // Chamber has no target
    if (targetTemperature <= 0)
    {
        return TFT_CYAN;
    }

    const float difference =
        targetTemperature -
        currentTemperature;

    // At or very near target
    if (abs(difference) <= 2.0f)
    {
        return COLOR_GREEN;
    }

    // Heating toward target
    if (currentTemperature < targetTemperature)
    {
        return TFT_ORANGE;
    }

    // Above target
    return TFT_RED;
}