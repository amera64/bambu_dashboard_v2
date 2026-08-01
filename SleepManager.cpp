#include <Arduino.h>
#include "Display.h"
#include "SleepManager.h"

static unsigned long lastActivityTime = 0;

static bool dimmed = false;
static bool sleeping = false;
static volatile bool wakeRequested = false;


constexpr unsigned long DIM_TIME   = 120000UL;
constexpr unsigned long SLEEP_TIME = 300000UL;

void initSleepManager()
{
    lastActivityTime = millis();

    dimmed = false;
    sleeping = false;
    wakeRequested = false;

    lcd.setBrightness(255);
}



void requestScreenWake()
{
    wakeRequested = true;
    Serial.println("Screen wake requested");
}

void resetSleepTimer()
{
    lastActivityTime = millis();

    lcd.setBrightness(255);

    dimmed = false;
    sleeping = false;

    Serial.println("Screen awake");
}

bool isScreenDimmed()
{
    return dimmed;
}

bool isScreenSleeping()
{
    return sleeping;
}

void updateSleepManager()
{
    // Perform display access from the normal Arduino loop
    if (wakeRequested)
    {
        wakeRequested = false;
        resetSleepTimer();
    }

    const unsigned long idleTime =
        millis() - lastActivityTime;

    if (!dimmed && idleTime >= DIM_TIME)
    {
        lcd.setBrightness(50);

        dimmed = true;

        Serial.println("Display dimmed");
    }

    if (!sleeping && idleTime >= SLEEP_TIME)
    {
        lcd.setBrightness(0);

        sleeping = true;

        Serial.println("Display sleeping");
    }
}