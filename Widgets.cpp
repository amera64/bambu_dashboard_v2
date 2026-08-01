#include "Widgets.h"

#include "Display.h"
#include "Theme.h"


void drawCard(
    int x,
    int y,
    int width,
    int height
)
{
    lcd.fillRoundRect(
        x,
        y,
        width,
        height,
        8,
        COLOR_PANEL
    );

    lcd.drawRoundRect(
        x,
        y,
        width,
        height,
        8,
        COLOR_GRAY_TEXT
    );
}


void drawTempValue(
    int x,
    int y,
    float temperature
)
{
    lcd.setTextColor(
        COLOR_TEXT
    );

    String value =
        String(
            static_cast<int>(
                temperature
            )
        );

    lcd.drawString(
        value,
        x,
        y
    );

    const int degreeX =
        x +
        lcd.textWidth(value) +
        2;

    lcd.drawCircle(
        degreeX,
        y - 5,
        2,
        COLOR_TEXT
    );

    lcd.drawString(
        "C",
        degreeX + 7,
        y
    );
}