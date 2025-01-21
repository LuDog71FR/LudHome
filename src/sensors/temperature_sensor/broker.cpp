#include "broker.h"
#include "wifi.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>

bool broker_status;                         // Broker status: true if connected, false otherwise
char MY_MQTT_ID[20];                        // MQTT Unique ID

MQTTClient myMQTTClient;                    // MQTT client
WiFiClient myWiFiClient;                    // WiFi client

#define MQTT_BROKER_IP          "ludhome"   // MQTT broker address
#define MQTT_BROKER_PORT        1883        // MQTT broker port
#define MQTT_USERNAME           "ludovic"   // MQTT username
#define MQTT_PASSWORD           "sdiaea"    // MQTT password

void init_mqtt() {
  myMQTTClient.begin(MQTT_BROKER_IP, MQTT_BROKER_PORT, myWiFiClient);

  // Unique ID
  strncpy(MY_MQTT_ID, String(ESP.getChipId()).c_str(),sizeof(MY_MQTT_ID));
  MY_MQTT_ID[sizeof(MY_MQTT_ID)-1] = '\0';
  Serial.println("MQTT unique ID: " + String(MY_MQTT_ID));
}

void connect_to_broker() {
  broker_status = true;

  if(!wifi_is_connected()) {
    broker_status = false;
    return;
  }

  int tryNumber = 1;
  while (!myMQTTClient.connect(MY_MQTT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    if (tryNumber++ > 10) {
      broker_status = false;
      return;
    }
    delay(500);
  }
}

void reconnect_to_broker() {
  // Reconnect to broker
  myMQTTClient.loop();
  delay(10); // Problèmes de stabilité Wi-Fi ? (Cf. doc MQTT)
  if (!myMQTTClient.connected()) {
    connect_to_broker();
  }
}

void send_data_to_broker(String topic, String message) {
  myMQTTClient.subscribe(topic);
  myMQTTClient.publish(topic, message, true, 1);
}

void send_data_to_broker(String topic, int sensor_value) {
  send_data_to_broker(topic, String(sensor_value));
}

void send_data_to_broker(String topic, float sensor_value) {
  send_data_to_broker(topic, String(sensor_value));
}
