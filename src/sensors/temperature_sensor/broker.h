#ifndef BROKER_H
#define BROKER_H

#include <Arduino.h>

extern bool broker_status;                         // Broker status: true if connected, false otherwise
extern char MY_MQTT_ID[20];                        // MQTT Unique ID

void init_mqtt();
void connect_to_broker();
void reconnect_to_broker();
void send_data_to_broker(String topic, String message);
void send_data_to_broker(String topic, int sensor_value);
void send_data_to_broker(String topic, float sensor_value);

#endif
