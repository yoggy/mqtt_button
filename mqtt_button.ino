//
// mqtt_button.ino - Publish states of buttons using MQTT protocol.
//
// https://github.com/yoggy/mqtt_button
//
// license:
//     Copyright (c) 2016 yoggy <yoggy0@gmail.com>
//     Released under the MIT license
//     http://opensource.org/licenses/mit-license.php;
//
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient/

char *wifi_ssid = "ssid";
char *wifi_password = "password";

char *mqtt_server    = "mqtt.example.com";
int  mqtt_port      = 1883;

char *mqtt_username = "username";
char *mqtt_password = "password";

#define PIN_LED 16
#define PIN_BUTTON1 13
#define PIN_BUTTON2 14

char *topic_button1 = "dev/button/1";
char *topic_button2 = "dev/button/2";

int button1_low_count = 0;
int button2_low_count = 0;

WiFiClient wifi_client;
PubSubClient mqtt_client(mqtt_server, mqtt_port, NULL, wifi_client);

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON1, INPUT_PULLUP);
  pinMode(PIN_BUTTON2, INPUT_PULLUP);

  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  int wifi_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (wifi_count % 2 == 0) digitalWrite(PIN_LED, HIGH);
    else                     digitalWrite(PIN_LED, LOW);
    wifi_count ++;
    delay(500);
  }

  if (mqtt_client.connect("client_id", mqtt_username, mqtt_password) == false) {
    Serial.println("conn failed...  ");
    delay(1000);
    reboot();
  }
  Serial.println("connected!");
}

void loop() {
  // for MQTT
  if (!mqtt_client.connected()) {
    reboot();
  }
  mqtt_client.loop();

  checkPinStatus();
}

void checkPinStatus() {
  if (digitalRead(PIN_BUTTON1) == LOW) {
    if (button1_low_count < 100) {
      button1_low_count ++;
      if (button1_low_count == 100) {
        mqtt_client.publish(topic_button1, "press");
      }
    }
  }
  else {
    button1_low_count = 0;
  }

  if (digitalRead(PIN_BUTTON2) == LOW) {
    if (button2_low_count < 100) {
      button2_low_count ++;
      if (button2_low_count == 100) {
        mqtt_client.publish(topic_button2, "press");
      }
    }
  }
  else {
    button2_low_count = 0;
  }
}

// reboot for ESP8266
void reboot() {
  for ( int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      digitalWrite(PIN_LED, LOW);
      delay(100);
      digitalWrite(PIN_LED, HIGH);
      delay(100);
    }
  }
  ESP.reset();
  while (true) {};
}

