#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h> 
#include <WiFiClientSecure.h>
#include <MQTT.h>

#define LOOP_DELAY_TIME         2000        // Refresh delay for messages
#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 32          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C        // Adresse de "mon" écran OLED sur le bus i2c (généralement égal à 0x3C ou 0x3D)
#define adresseI2CduBME280      0x76        // Adresse I2C du BME280 (0x76, dans mon cas, ce qui est souvent la valeur par défaut)

#define LAST_IP_NUMBER          201         // Must be between 200 and 254.

#define MQTT_BROKER_IP          "ludhome"   // MQTT broker address
#define MQTT_BROKER_PORT        1883        // MQTT broker port
#define MQTT_USERNAME           "ludovic"
#define MQTT_PASSWORD           "sdiaea"

char MY_MQTT_ID[20] ;                       // MQTT Unique ID
MQTTClient myMQTTClient; 
bool broker_status;
bool wifi_status;

Adafruit_SSD1306 oled(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);

Adafruit_BME280 bme; 
bool bme_status;

const unsigned long scroll_interval = 60000; // Refresh delay for data
unsigned long last_scroll_time = 60000;

String warnings[4];
int warning_count = 0;
int current_warning_index = 0;

WiFiClient myWiFiClient;
const char* mySSID   = "SFR_03AF";
const char* mySecKey = "fywci4lz9h8qpfyk944c";


// 'warning', 16x16px
const unsigned char icon_warning [] PROGMEM = {
	0x00, 0x80, 0x01, 0xc0, 0x01, 0xc0, 0x03, 0xe0, 0x03, 0x60, 0x07, 0x70, 0x06, 0x30, 0x0e, 0xb8, 
	0x0c, 0x98, 0x1c, 0x9c, 0x18, 0x8c, 0x38, 0x0e, 0x30, 0x86, 0x7f, 0xff, 0x7f, 0xff, 0x00, 0x00
};

// 'droplet_black', 16x16px
const unsigned char icon_droplet [] PROGMEM = {
	0x01, 0x80, 0x02, 0x40, 0x04, 0x40, 0x0c, 0x30, 0x08, 0x10, 0x10, 0x10, 0x10, 0x18, 0x30, 0x04, 
	0x20, 0x04, 0x20, 0x04, 0x28, 0x04, 0x24, 0x0c, 0x32, 0x08, 0x10, 0x18, 0x0c, 0x30, 0x07, 0xc0
};

// 'bat_0', 16x16px
const unsigned char icon_bat_0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x7f, 0xfc, 0xff, 0xfe, 0xe0, 0x06, 0xe0, 0x07, 
	0xe0, 0x07, 0xe0, 0x06, 0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'bat_1', 16x16px
const unsigned char icon_bat_1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x7f, 0xfc, 0xff, 0xfe, 0xec, 0x06, 0xec, 0x07, 
	0xec, 0x07, 0xec, 0x06, 0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'bat_2', 16x16px
const unsigned char icon_bat_2 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x7f, 0xfc, 0xff, 0xfe, 0xed, 0x86, 0xed, 0x87, 
	0xed, 0x87, 0xed, 0x86, 0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'bat_3', 16x16px
const unsigned char icon_bat_3 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x7f, 0xfc, 0xff, 0xfe, 0xed, 0xb6, 0xed, 0xb7, 
	0xed, 0xb7, 0xed, 0xb6, 0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'bat_4', 16x16px
const unsigned char icon_bat_4 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x7f, 0xfc, 0xff, 0xfe, 0xe0, 0x06, 0xe0, 0x67, 
	0xe7, 0xf7, 0xe3, 0x06, 0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'signal_1', 16x16px
const unsigned char icon_signal_1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xc0, 0x00, 0xc0, 0x00
};
// 'signal_2', 16x16px
const unsigned char icon_signal_2 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x5c, 0x00, 0xdc, 0x00, 0xdc, 0x00
};
// 'signal_3', 16x16px
const unsigned char icon_signal_3 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xc0, 0x01, 0xc0, 
	0x01, 0xc0, 0x05, 0xc0, 0x0d, 0xc0, 0x1d, 0xc0, 0x1d, 0xc0, 0x5d, 0xc0, 0xdd, 0xc0, 0xdd, 0xc0
};
// 'signal_4', 16x16px
const unsigned char icon_signal_4 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x5c, 0x00, 0xdc, 0x01, 0xdc, 
	0x01, 0xdc, 0x05, 0xdc, 0x0d, 0xdc, 0x1d, 0xdc, 0x1d, 0xdc, 0x5d, 0xdc, 0xdd, 0xdc, 0xdd, 0xdc
};
// 'no_con', 16x16px
const unsigned char icon_no_con [] PROGMEM = {
	0x00, 0x00, 0x03, 0xe0, 0x0f, 0xf8, 0x1f, 0xfc, 0x3e, 0x3e, 0x38, 0x7e, 0x70, 0xff, 0x71, 0xf7, 
	0x73, 0xc7, 0x77, 0x87, 0x3f, 0x0e, 0x3e, 0x1e, 0x1f, 0xfc, 0x0f, 0xf8, 0x03, 0xe0, 0x00, 0x00
};


struct Battery {
  float analogValue;
  float voltage;
  float percentage;
  bool lowBattery;
};

// DATA
Battery battery;
int rssi;
int temperature;
int humidity;


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
  if(WiFi.status() != WL_CONNECTED) {
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
  if (battery.lowBattery) {
    add_warning("Low baterry");
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
  }
  else {
    temperature = -99;
    humidity = 100;
  }
}

void read_battery_level() {
  const float Vmax = 4.2;
  const float Vmin = 3.0;

  battery.analogValue = analogRead(A0);
  battery.voltage = mapf(battery.analogValue, 0.0, 825.0, 0.0, 4.2);
  battery.percentage = ((battery.voltage - Vmin) / (Vmax - Vmin)) * 100.0;
  battery.percentage = constrain(battery.percentage, 0.0, 100.0);
  battery.lowBattery = battery.percentage <= 20.0; // Low battery when it's < 20%
}

bool wifi_is_connected() {
  int tryNumber = 1;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (++tryNumber > 10) {
      Serial.println("Wifi connection failed !");
      wifi_status = false;
      return false;
    }
  }

  wifi_status = true;
  return true;
}

void disable_wifi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
}

void enable_wifi() {
  IPAddress ip(192, 168, 1, LAST_IP_NUMBER);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  
  WiFi.forceSleepWake();
  delay(1);

  // Disable the WiFi persistence. The ESP8266 will not load and save WiFi settings in the flash memory.
  WiFi.persistent(false);

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(mySSID, mySecKey);

  wifi_is_connected();
}

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

void send_data_to_broker(String topic, String message) {
  myMQTTClient.subscribe(topic);
  myMQTTClient.publish(topic, message, true, 1);
}


void send_data_to_broker(String topic, float sensor_value) {
  myMQTTClient.subscribe(topic);
  myMQTTClient.publish(topic, String(sensor_value), true, 1);
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
  display_message("Init BME ...", "");
  bme_status = bme.begin(adresseI2CduBME280);

  display_message("Init Wifi ...", "");
  enable_wifi();

  display_message("Init Broker ...", "");
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
  rssi = WiFi.RSSI();
}

void display_all_data() {
  oled.clearDisplay();
  display_temphum();
  display_battery();
  display_wifi(rssi);
}

void send_all_data() {
  // Reconnect to broker
  myMQTTClient.loop();
  delay(10); // Problèmes de stabilité Wi-Fi ? (Cf. doc MQTT)
  if (!myMQTTClient.connected()) {
    connect_to_broker();
  }

  // Send results to broker
  if (myMQTTClient.connected()) {
    Serial.println(F("Sending sensors data to broker"));
  
    if(bme_status) {
      send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/temperature/value", temperature);
      send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/humidity/value", humidity);
    }

    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/battery/analog/value", battery.analogValue);
    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/battery/voltage/value", battery.voltage);
    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/battery/percentage/value", battery.percentage);
    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/battery/low_battery/status", battery.lowBattery);

    send_data_to_broker("devices/" + String(MY_MQTT_ID) + "/sensors/rssi/value", rssi);
  }
}

void loop() {
  if (millis() - last_scroll_time >= scroll_interval) {
    enable_wifi();

    read_all_data();
    display_all_data();
    send_all_data();
    last_scroll_time = millis();    
    
    disable_wifi();
  }

  display_warning();
  scroll_warnings();

  delay(LOOP_DELAY_TIME);
}
