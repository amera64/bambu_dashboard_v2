#pragma once

struct SystemStatus
{
    volatile bool wifiConnected;
    volatile bool mqttConnected;
    volatile bool printerOnline;
    volatile bool webAdminRunning;
};

extern SystemStatus systemStatus;