#pragma once

struct UIUpdateFlags
{
    volatile bool header;
    volatile bool progress;
    volatile bool progressStatus;
    volatile bool nozzle;
    volatile bool bed;
    volatile bool bottomInfo;
    volatile bool ams;
    volatile bool controls;
    volatile bool system;
    volatile bool tempNozzle;
    volatile bool tempBed;
    volatile bool tempChamber;

};

extern UIUpdateFlags uiUpdates;



void markAllUIUpdates();
void clearAllUIUpdates();

