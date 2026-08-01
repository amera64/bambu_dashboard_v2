#include "UIUpdateFlags.h"

UIUpdateFlags uiUpdates =
{
    false,  // header
    false,  // progress
    false,  // progressStatus

    false,  // nozzle
    false,  // bed
    false,  // bottomInfo

    false,  // tempNozzle
    false,  // tempBed
    false,  // tempChamber

    false,  // ams
    false,  // controls
    false   // system
};

void markAllUIUpdates()
{
    uiUpdates.header = true;
    uiUpdates.progress = true;
    uiUpdates.progressStatus = true;

    uiUpdates.nozzle = true;
    uiUpdates.bed = true;
    uiUpdates.bottomInfo = true;

    uiUpdates.tempNozzle = true;
    uiUpdates.tempBed = true;
    uiUpdates.tempChamber = true;

    uiUpdates.ams = true;
    uiUpdates.controls = true;
    uiUpdates.system = true;
}

void clearAllUIUpdates()
{
    uiUpdates.header = false;
    uiUpdates.progress = false;
    uiUpdates.progressStatus = false;

    uiUpdates.nozzle = false;
    uiUpdates.bed = false;
    uiUpdates.bottomInfo = false;

    uiUpdates.tempNozzle = false;
    uiUpdates.tempBed = false;
    uiUpdates.tempChamber = false;

    uiUpdates.ams = false;
    uiUpdates.controls = false;
    uiUpdates.system = false;
}