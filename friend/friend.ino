#include<EEPROM.h>


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<SoftwareSerial.h>


const char* ssid = "was";
const char* password = "1234qwerty";
const char* mqtt_server = "192.168.43.63";
SoftwareSerial serial(D6, D5); //(rx,tx)

WiFiClient espClient;
void setup() {

}

void loop() {
  // put your main code here, to run repeatedly:

}
