#pragma once

#include <Arduino.h>


struct AMS_Slot
{
    bool empty;
    String color;
    String type;
    bool active;
};


extern String current_gcode_state;
extern String current_file_name;

extern int current_progress;
extern int current_time_remaining;
extern int remaining_minutes;

extern float current_nozzle_temp;
extern float current_nozzle_target;

extern float current_bed_temp;
extern float current_bed_target;

extern float current_chamber_temp;

extern int current_layer;
extern int total_layers;

extern int print_progress;

extern int remaining_time;

extern String current_filename;

extern String subscribe_topic;
extern String publish_topic;

extern String current_sequence_id;
extern int currentAMSTray;

extern AMS_Slot ams[4];
extern AMS_Slot externalSpool;

extern int currentAMSHumidity;
extern int currentAMSHumidityRaw;
extern float currentAMSTemp;

extern bool chamberLightOn;

extern bool pageChanged;

extern String previous_gcode_state;

bool isPrintActive();