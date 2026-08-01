#include "PrinterData.h"

String current_gcode_state = "UNKNOWN";
String current_file_name = "None";

int current_progress = 0;
int current_time_remaining = 0;
int remaining_minutes = 0;

float current_nozzle_temp = 0;
float current_nozzle_target = 0;


float current_bed_temp = 0;
float current_bed_target = 0;

float current_chamber_temp = 0;

String subscribe_topic = "";
String publish_topic = "";

String current_sequence_id = "0";

int current_layer = 0;
int total_layers = 0;

int print_progress = 0;

int remaining_time = 0;

String current_filename = "";

int currentAMSTray = -1;

AMS_Slot ams[4];
AMS_Slot externalSpool;

bool chamberLightOn = true;
bool pageChanged = true;

int currentAMSHumidity = 0;
int currentAMSHumidityRaw = 0;
float currentAMSTemp = 0.0f;

String previous_gcode_state = "";

bool isPrintActive()
{
    return current_gcode_state == "RUNNING" ||
           current_gcode_state == "PRINTING" ||
           current_gcode_state == "PAUSE" ||
           current_gcode_state == "PREPARE";
}