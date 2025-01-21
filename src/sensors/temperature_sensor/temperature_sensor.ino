#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h> 

#include "icons.h"
#include "wifi.h"
#include "broker.h"

#define LOOP_DELAY_TIME         2000        // Refresh delay for messages
#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 32          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C        // Adresse de "mon" écran OLED sur le bus i2c (généralement égal à 0x3C ou 0x3D)
#define adresseI2CduBME280      0x76        // Adresse I2C du BME280 (0x76, dans mon cas, ce qui est souvent la valeur par défaut)

const unsigned long scroll_interval = 600000; // Refresh delay for data: 10 minutes
unsigned long last_scroll_time = 600000;

Adafruit_SSD1306 oled(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);

Adafruit_BME280 bme; 
bool bme_status;

String warnings[4];
int warning_count = 0;
int current_warning_index = 0;

struct Battery {
  float analogValue;
  float voltage;
  float percentage;
};

// DATA
Battery battery;
int temperature;
int humidity;
int pressure;


float mapf(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  float result;
  result = (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
  return result;
}

void display_temphum() {
  oled.setTextSize(2);
  oled.setCursor(0, 17);
  oled.setTextColor(SSD1306_WHITE);

  oled.print(temperature);
  oled.print((char)247); // Caractère °
  oled.print("C");

  oled.setCursor(77, 17);
  oled.print(humidity);
  oled.print("%");
  
  oled.drawBitmap(62, 16, icon_droplet, 16, 16, WHITE);
}

void display_battery() {
  const unsigned char* icon = nullptr;
  if (battery.percentage <= 25) {
    icon = icon_bat_0;
  } else if (battery.percentage <= 50) {
    icon = icon_bat_1;
  } else if (battery.percentage <= 75) {
    icon = icon_bat_2;
  } else {
    icon = icon_bat_3;
  }

  oled.drawBitmap(0, 0, icon, 16, 16, WHITE);
}

void display_wifi(int rssi) {
  if(!wifi_status) {
    oled.drawBitmap(111, 0, icon_no_con, 16, 16, WHITE);
    return;
  }

  int scaledRSSI = map(rssi, -100, -20, 1, 4);

  const unsigned char* icon = nullptr;
  switch (scaledRSSI) {
    case 1:
      icon = icon_signal_1;
      break;
    case 2:
      icon = icon_signal_2;
      break;
    case 3:
      icon = icon_signal_3;
      break;
    case 4:
      icon = icon_signal_4;
      break;
    default:
      icon = icon_signal_1; // Par défaut, utiliser l'icône de signal faible
      break;
  }

  oled.drawBitmap(111, 0, icon, 16, 16, WHITE);
}

void add_warning(String warning) {
  if (warning_count < 4) {
    warnings[warning_count] = warning;
    warning_count++;
  } else {
    Serial.println("Tableau de warnings plein");
  }
}

void scroll_warnings() {
  // Incrémenter l'index du message d'erreur
  if (warning_count <= 2) {
    current_warning_index = 0;
    return;
  }
  current_warning_index = (current_warning_index + 1) % warning_count;
}

void display_warning() {
  warning_count = 0;

  if (!wifi_status) {
    add_warning("WiFi KO");
  }
  if (!broker_status) {
    add_warning("Broker KO");
  }
  if (!bme_status) {
    add_warning("BME KO");
  }

  String warning_line1 = "            ";
  String warning_line2 = "            ";

  oled.fillRect(18, 0, 80, 17, SSD1306_BLACK);

  if (warning_count > 0) {
    warning_line1 = warnings[current_warning_index];
    oled.drawBitmap(18, 0, icon_warning, 16, 16, WHITE);
  }
  if (warning_count > 1) {
    warning_line2 = warnings[(current_warning_index + 1) % warning_count];
  }

  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);

  oled.setCursor(35, 0);
  oled.print(warning_line1);

  oled.setCursor(35, 9);
  oled.print(warning_line2);

  oled.display();
}

void display_message(String msg1, String msg2) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.setTextColor(SSD1306_WHITE);
  oled.println(msg1);
  oled.println(msg2);
  oled.display();
}

void read_temp_hum() {
  if(bme_status) {
    temperature = bme.readTemperature(); // °C
    humidity = bme.readHumidity(); // %
    pressure = bme.readPressure(); // hPa
  }
  else {
    temperature = -99;
    humidity = 100;
    pressure = -99;
  }
}

void read_battery_level() {
  const float Vmax = 4.2;
  const float Vmin = 3.0;

  battery.analogValue = analogRead(A0);
  battery.voltage = mapf(battery.analogValue, 0.0, 825.0, 0.0, 4.2);
  battery.percentage = ((battery.voltage - Vmin) / (Vmax - Vmin)) * 100.0;
  battery.percentage = constrain(battery.percentage, 0.0, 100.0);
}

void setup() {
  // Serial link
  Serial.begin(9600);
  Serial.println();

  // Reduce CPU frequency (80MHz) to optimize power consumption
  system_update_cpu_freq(80);

  // Battery level
  pinMode(A0, INPUT);

  // OLED screen init
  if(!oled.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED)) {
    Serial.println(F("Erreur de communication avec le chipset SSD1306… arrêt du programme."));
    while(1); // Arrêt du programme (boucle infinie)
  }

  // BME280 Init
  // display_message("Init BME ...", "");
  bme_status = bme.begin(adresseI2CduBME280);

  // display_message("Init Wifi ...", "");
  enable_wifi();

  // display_message("Init Broker ...", "");
  init_mqtt();
  connect_to_broker();

  if(broker_status) {
    // Send hello message to broker
    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/hello", "hi");
  }

  Serial.println(F("Setup END"));
}

void read_all_data() {
  read_temp_hum();
  read_battery_level();
}

void display_all_data() {
  oled.clearDisplay();
  display_temphum();
  display_battery();
  display_wifi(rssi);
}

void send_all_data() {
  reconnect_to_broker();

  if (broker_status) {
    Serial.println(F("Sending sensors data to broker"));
  
    if(bme_status) {
      send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/temperature/value", temperature);
      send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/humidity/value", humidity);
      send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/pressure/value", pressure);
    }
    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/battery/analog/value", battery.analogValue);
    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/battery/voltage/value", battery.voltage);
    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/battery/percentage/value", battery.percentage);

    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/rssi/value", rssi);
  }
}

void loop() {
  if (millis() - last_scroll_time >= scroll_interval) {
    read_all_data(); // Read all sensors data before Wifi connection to reduce power consumption
    enable_wifi();
    display_all_data();
    send_all_data();
    disable_wifi();
    last_scroll_time = millis();
  }

  display_warning();
  scroll_warnings();

  if (warning_count <= scroll_interval) {
    ESP.deepSleep(scroll_interval * 1000);
  }

  delay(LOOP_DELAY_TIME);
}
