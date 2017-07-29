#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "was";
const char* password = "1234qwerty";
const char* mqtt_server = "192.168.43.63";

int pin1=A0; //gas sensor
int pin2=D0;  //flame detection sensor
int gled1=D2;
int gled2=D3;
int rled1=D1;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void setup() {

pinMode(pin1,INPUT);
pinMode(pin2,INPUT);
pinMode(gled1,OUTPUT);
pinMode(gled2,OUTPUT);
pinMode(rled1,OUTPUT);
  
  Serial.begin(115200);
 setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
WiFi.begin(ssid, password);
  

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void blinks()
{
digitalWrite(rled1,LOW);
digitalWrite(gled1,HIGH);
digitalWrite(gled2,HIGH);  
delay(500);
digitalWrite(gled1,LOW);
digitalWrite(gled2,LOW);

  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg;
  for (int i = 0; i < length; i++) {
    msg+=(char)payload[i];
  }
  Serial.println(msg);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("fire fighter")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("fire_fighter/status", "Fire fighter System active");
      
      // ... and resubscribe
      client.subscribe("doorlock/front/status");
      client.subscribe("house_hold/living/status");
      client.subscribe("thor/8754623583/status");
      
      //client.subscribe("doorlock/fire_fighter/help");
      client.subscribe("house_hold/living/msg");
    client.subscribe("thor/8754623583/msg");
    
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



int gas_value=0;
int flame_value=0;




void loop() {

if (!client.connected()) {
    reconnect();
  }
  client.loop();
  gas_value=analogRead(pin1);
flame_value=digitalRead(pin2);
  Serial.println(gas_value);
Serial.println(flame_value);

if(flame_value==0)
{
digitalWrite(rled1,HIGH);
  client.publish("fire_fighter/kitchen/status","fire detected");
delay(10000);
}

if(gas_value>400)
{
  digitalWrite(rled1,HIGH);
   client.publish("fire_fighter/kitchen/status","Gas leakage detected");
delay(10000);
}
 
 
 long now = millis();
  if (now - lastMsg > 5000) 
  {
    lastMsg = now;
        blinks();
    
      
      }


}
