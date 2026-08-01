#pragma once

#include <Arduino.h>

void setupMQTT();
void loopMQTT();
void request_status();
void reconnectMQTT();
void loopMQTT();
void callback(char* topic, byte* payload, unsigned int length);


#include <Arduino.h>
#include <PubSubClient.h>


//extern PubSubClient client;


extern PubSubClient mqttClient;

extern const char* mqtt_server;
extern const int mqtt_port;


extern const char* printer_serial;

extern unsigned long lastPrinterUpdate;

extern const char* bambu_user_id;
extern const char* bbl_access_token;


void setupMQTT();
void startMQTTTask();


void callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT();
void loopMQTT();
void request_status();

void pausePrint();
void resumePrint();
void stopPrint();
void toggleChamberLight();
void requestAMSUpdate();



