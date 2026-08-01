#include <Arduino.h>

#include "AMSPage.h"
#include "Display.h"
#include "Theme.h"
#include "PrinterData.h"

// ----------------------------------------------------
// AMS page layout
// ----------------------------------------------------

static constexpr int AMS_CARD_START_X = 8;
static constexpr int AMS_CARD_SPACING = 62;
static constexpr int AMS_CARD_Y = 55;

static constexpr int AMS_CARD_WIDTH = 57;
static constexpr int AMS_CARD_HEIGHT = 85;


// ----------------------------------------------------
// Private function declarations
// ----------------------------------------------------

static uint16_t hexTo565(String hex);

static uint16_t getContrastTextColor(
    uint16_t color
);

static void drawAMSSpool(
    int x,
    int y,
    int slot
);

static void drawExternalSpool(
    int x,
    int y
);

static void drawFilamentPath();

static void drawHotendIcon(
    int x,
    int y,
    uint16_t color
);

static void drawHumidityBar(
    int x,
    int y,
    int level
);


// ----------------------------------------------------
// Draw complete AMS page
// ----------------------------------------------------

void drawAMS()
{
    lcd.setTextDatum(
        middle_center
    );

    // -----------------------------
    // Four AMS slots
    // -----------------------------

    for (int i = 0; i < 4; i++)
    {
        drawAMSSpool(
            AMS_CARD_START_X +
                (i * AMS_CARD_SPACING),
            AMS_CARD_Y,
            i
        );
    }

    // -----------------------------
    // External spool
    // -----------------------------

    drawExternalSpool(
        AMS_CARD_START_X +
            (4 * AMS_CARD_SPACING),
        AMS_CARD_Y
    );

    // -----------------------------
    // Active filament path
    // -----------------------------

    drawFilamentPath();

    // -----------------------------
    // Humidity
    // -----------------------------

    lcd.setFont(
        &fonts::Font2
    );

    lcd.setTextColor(
        COLOR_TEXT
    );

    lcd.drawString(
        "Humidity",
        45,
        170
    );

    drawHumidityBar(
        10,
        180,
        currentAMSHumidity
    );

    lcd.setTextColor(
        TFT_WHITE,
        COLOR_CARD
    );

    lcd.setTextSize(1);

    lcd.setCursor(
        86,
        186
    );

    lcd.print(
        String(currentAMSHumidityRaw) +
        "%"
    );

    // -----------------------------
    // AMS temperature
    // -----------------------------

    lcd.setFont(
        &fonts::Font2
    );

    lcd.setTextDatum(
        middle_center
    );

    lcd.setTextColor(
        COLOR_TEXT
    );

    lcd.drawString(
        "AMS Temp",
        275,
        170
    );

    lcd.drawString(
        String(currentAMSTemp, 1) +
        " C",
        275,
        190
    );
}


// ----------------------------------------------------
// Draw one physical AMS spool card
// ----------------------------------------------------

static void drawAMSSpool(
    int x,
    int y,
    int slot
)
{
    uint16_t cardColor =
        TFT_DARKGREY;

    if (!ams[slot].empty)
    {
        cardColor =
            hexTo565(
                ams[slot].color
            );
    }

    // Active-slot highlight
    if (ams[slot].active)
    {
        lcd.fillRoundRect(
            x - 2,
            y - 2,
            AMS_CARD_WIDTH + 4,
            AMS_CARD_HEIGHT + 4,
            9,
            COLOR_ACCENT
        );
    }

    // Card background
    lcd.fillRoundRect(
        x,
        y,
        AMS_CARD_WIDTH,
        AMS_CARD_HEIGHT,
        7,
        cardColor
    );

    // Card border
    lcd.drawRoundRect(
        x,
        y,
        AMS_CARD_WIDTH,
        AMS_CARD_HEIGHT,
        7,
        ams[slot].active
            ? COLOR_ACCENT
            : COLOR_BORDER
    );

    uint16_t textColor =
        getContrastTextColor(
            cardColor
        );

    lcd.setTextDatum(
        middle_center
    );

    lcd.setTextColor(
        textColor
    );

    lcd.setFont(
        &fonts::Font2
    );

    // Filament type
    if (ams[slot].empty)
    {
        lcd.drawString(
            "EMPTY",
            x + AMS_CARD_WIDTH / 2,
            y + 34
        );
    }
    else
    {
        String filamentType =
            ams[slot].type;

        if (filamentType.length() > 7)
        {
            filamentType =
                filamentType.substring(
                    0,
                    7
                );
        }

        lcd.drawString(
            filamentType,
            x + AMS_CARD_WIDTH / 2,
            y + 34
        );
    }

    // Slot number
    lcd.drawString(
        String(slot + 1),
        x + AMS_CARD_WIDTH / 2,
        y + 69
    );
}


// ----------------------------------------------------
// Draw external spool card
// ----------------------------------------------------

static void drawExternalSpool(
    int x,
    int y
)
{
    uint16_t cardColor =
        TFT_DARKGREY;

    if (!externalSpool.empty)
    {
        cardColor =
            hexTo565(
                externalSpool.color
            );
    }

    // Active-spool highlight
    if (externalSpool.active)
    {
        lcd.fillRoundRect(
            x - 2,
            y - 2,
            AMS_CARD_WIDTH + 4,
            AMS_CARD_HEIGHT + 4,
            9,
            COLOR_ACCENT
        );
    }

    // Card background
    lcd.fillRoundRect(
        x,
        y,
        AMS_CARD_WIDTH,
        AMS_CARD_HEIGHT,
        7,
        cardColor
    );

    // Card border
    lcd.drawRoundRect(
        x,
        y,
        AMS_CARD_WIDTH,
        AMS_CARD_HEIGHT,
        7,
        externalSpool.active
            ? COLOR_ACCENT
            : COLOR_BORDER
    );

    uint16_t textColor =
        getContrastTextColor(
            cardColor
        );

    lcd.setTextDatum(
        middle_center
    );

    lcd.setTextColor(
        textColor
    );

    lcd.setFont(
        &fonts::Font2
    );

    // Filament type
    if (externalSpool.empty)
    {
        lcd.drawString(
            "EMPTY",
            x + AMS_CARD_WIDTH / 2,
            y + 34
        );
    }
    else
    {
        String filamentType =
            externalSpool.type;

        if (filamentType.length() > 7)
        {
            filamentType =
                filamentType.substring(
                    0,
                    7
                );
        }

        lcd.drawString(
            filamentType,
            x + AMS_CARD_WIDTH / 2,
            y + 34
        );
    }

    lcd.drawString(
        "EXT",
        x + AMS_CARD_WIDTH / 2,
        y + 69
    );
}


// ----------------------------------------------------
// Draw active PTFE tube and filament
// ----------------------------------------------------

static void drawFilamentPath()
{
    int activeSlot =
        currentAMSTray;

    // The external spool has its own active flag.
    if (externalSpool.active)
    {
        activeSlot = 4;
    }

    if (
        activeSlot < 0 ||
        activeSlot > 4
    )
    {
        return;
    }

    constexpr int spoolCenterOffset = 27;

    const int spoolX =
        AMS_CARD_START_X +
        (activeSlot * AMS_CARD_SPACING) +
        spoolCenterOffset;

    constexpr int tubeStartY = 137;
    constexpr int tubeTurnY = 157;
    constexpr int hotendX = 160;
    constexpr int hotendY = 154;

    uint16_t filamentColor =
        COLOR_ACCENT;

    if (activeSlot <= 3)
    {
        if (!ams[activeSlot].empty)
        {
            filamentColor =
                hexTo565(
                    ams[activeSlot].color
                );
        }
    }
    else
    {
        if (!externalSpool.empty)
        {
            filamentColor =
                hexTo565(
                    externalSpool.color
                );
        }
    }

    // Vertical PTFE tube
    lcd.fillRoundRect(
        spoolX - 4,
        tubeStartY,
        8,
        tubeTurnY -
            tubeStartY +
            4,
        4,
        TFT_DARKGREY
    );

    // Horizontal PTFE tube
    const int horizontalStart =
        min(
            spoolX,
            hotendX
        );

    const int horizontalWidth =
        abs(
            hotendX -
            spoolX
        ) + 1;

    lcd.fillRoundRect(
        horizontalStart,
        tubeTurnY - 4,
        horizontalWidth,
        8,
        4,
        TFT_DARKGREY
    );

    // Vertical filament
    lcd.fillRoundRect(
        spoolX - 1,
        tubeStartY + 2,
        3,
        tubeTurnY -
            tubeStartY,
        1,
        filamentColor
    );

    // Horizontal filament
    lcd.fillRoundRect(
        horizontalStart,
        tubeTurnY - 1,
        horizontalWidth,
        3,
        1,
        filamentColor
    );

    drawHotendIcon(
        hotendX,
        hotendY,
        filamentColor
    );
}


// ----------------------------------------------------
// Draw hotend icon
// ----------------------------------------------------

static void drawHotendIcon(
    int x,
    int y,
    uint16_t color
)
{
    // Make very dark filament visible
    if (color < 0x0841)
    {
        color =
            TFT_DARKGREY;
    }

    // Heatsink
    lcd.fillRoundRect(
        x - 10,
        y,
        20,
        18,
        3,
        COLOR_BORDER
    );

    // Brass nozzle
    lcd.fillTriangle(
        x - 5,
        y + 18,
        x + 5,
        y + 18,
        x,
        y + 25,
        0xA4C2
    );

    // Filament
    lcd.drawLine(
        x,
        y + 10,
        x,
        y + 30,
        color
    );

    // Heater glow
    lcd.fillCircle(
        x,
        y + 10,
        4,
        TFT_RED
    );
}


// ----------------------------------------------------
// Draw AMS humidity level
// ----------------------------------------------------

static void drawHumidityBar(
    int x,
    int y,
    int level
)
{
    constexpr int boxWidth = 10;
    constexpr int boxHeight = 16;
    constexpr int gap = 4;

    level =
        constrain(
            level,
            0,
            5
        );

    for (int i = 0; i < 5; i++)
    {
        const int boxX =
            x +
            i *
            (boxWidth + gap);

        if (i < level)
        {
            lcd.fillRoundRect(
                boxX,
                y,
                boxWidth,
                boxHeight,
                2,
                TFT_CYAN
            );
        }
        else
        {
            lcd.drawRoundRect(
                boxX,
                y,
                boxWidth,
                boxHeight,
                2,
                COLOR_GRAY_TEXT
            );
        }
    }
}


// ----------------------------------------------------
// Convert "#RRGGBB" into RGB565
// ----------------------------------------------------

static uint16_t hexTo565(
    String hex
)
{
    if (hex.startsWith("#"))
    {
        hex.remove(
            0,
            1
        );
    }

    if (hex.length() < 6)
    {
        return TFT_DARKGREY;
    }

    const uint8_t red =
        strtol(
            hex.substring(
                0,
                2
            ).c_str(),
            nullptr,
            16
        );

    const uint8_t green =
        strtol(
            hex.substring(
                2,
                4
            ).c_str(),
            nullptr,
            16
        );

    const uint8_t blue =
        strtol(
            hex.substring(
                4,
                6
            ).c_str(),
            nullptr,
            16
        );

    return lcd.color565(
        red,
        green,
        blue
    );
}


// ----------------------------------------------------
// Choose readable text color
// ----------------------------------------------------

static uint16_t getContrastTextColor(
    uint16_t color
)
{
    const uint8_t red =
        ((color >> 11) & 0x1F) << 3;

    const uint8_t green =
        ((color >> 5) & 0x3F) << 2;

    const uint8_t blue =
        (color & 0x1F) << 3;

    const uint16_t brightness =
        (
            red * 299 +
            green * 587 +
            blue * 114
        ) / 1000;

    return brightness > 140
        ? TFT_BLACK
        : TFT_WHITE;
}