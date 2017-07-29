#include <DHT.h>
#include <Nextion.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<SoftwareSerial.h>
#include<EEPROM.h>

const char* ssid = "was";
const char* password = "1234qwerty";
const char* mqtt_server = "192.168.43.63";


SoftwareSerial serial(D6, D5); //rx,tx

int dht11=D3;
int gled1=D1;
int gled2=D2;

int ac=D0;
int heater=D7;
int motor=D8;


int speaker=D4;
int ldr=A0;
float temp_t=0.0;
float temp_h=0.0;
DHT dht(dht11,DHT11);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


Nextion nex(serial,9600);
void setup() {


pinMode(gled1,OUTPUT);
pinMode(gled2,OUTPUT);
pinMode(speaker,OUTPUT);
 
 pinMode(ac,OUTPUT);
 pinMode(heater,OUTPUT);
 pinMode(motor,OUTPUT);
  digitalWrite(motor,LOW);
  digitalWrite(heater,LOW);
  digitalWrite(ac,LOW);
  
  Serial.begin(115200);

 nex.sendCommand("page start");
      EEPROM.begin(512);

 
 
 
 setup_wifi();

 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
reconnect();
}
String message="Message:";

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
WiFi.begin(ssid, password);
  nex.setComponentText("t0","Connecting to wifi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  nex.setComponentText("t0","Connected!!");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void blinks()
{

digitalWrite(gled1,HIGH);
digitalWrite(gled2,HIGH);  
delay(200);
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
  if(msg=="fire detected")
  {
//client.publish("house_hold/living/msg","don't worry water is on the way :)");
   message+=" Fire detected Evacuate to exit door";

 //  digitalWrite(motor,HIGH);
   nex.setComponentText("g0",message);

digitalWrite(motor,HIGH);
   
    digitalWrite(speaker,HIGH);
    delay(200);
    digitalWrite(speaker,LOW);
    delay(200);
    digitalWrite(speaker,HIGH);
    delay(200);
    digitalWrite(speaker,LOW);
    delay(200);
    digitalWrite(speaker,HIGH);
    delay(200);
    digitalWrite(speaker,LOW); 

  // delay(500);
   
   /* digitalWrite(motor,HIGH);
    delay(4000);
    digitalWrite(motor,LOW);
  */
delay(2000);
digitalWrite(motor,LOW);
  
  }
  else if(msg=="door opened")
  {
    
//client.publish("house_hold/living/msg","Welcome Johnson control Smart Users :)");
message+=" Welcome user";
 nex.setComponentText("t2",message);
  digitalWrite(speaker,HIGH);
  delay(200);
  digitalWrite(speaker,LOW);  
  }
  
  

else if(msg=="smoke detected")
  {
//client.publish("house_hold/living/msg","don't worry water is on the way :)");
   message+=" Fire detected Evacuate to exit door";

 //  digitalWrite(motor,HIGH);
   nex.setComponentText("g0",message);

digitalWrite(motor,HIGH);
   
    digitalWrite(speaker,HIGH);
    delay(200);
    digitalWrite(speaker,LOW);
    delay(200);
    digitalWrite(speaker,HIGH);
    delay(200);
    digitalWrite(speaker,LOW); 

  // delay(500);
   
   /* digitalWrite(motor,HIGH);
    delay(4000);
    digitalWrite(motor,LOW);
  */
delay(2000);
digitalWrite(motor,LOW);
  
  }

else
{
   nex.setComponentText("g0",msg);
digitalWrite(speaker,HIGH);
delay(800);
digitalWrite(speaker,LOW);
}


Serial.println();
  msg="";



}
void autho()
{
  Serial.println("autho");
Serial.println((char)EEPROM.read(1));
String tempo="";
if(((char)EEPROM.read(1))=='s')
{
  Serial.println("sss");
for(int i=0;i<EEPROM.read(2);i++)
{
  tempo+=(char)EEPROM.read(i+3);
}
Serial.println(tempo);
   if(tempo=="1234")
   {
   nex.sendCommand("page mainpage");
nex.setComponentText("g0","Welcome to Smart Connected");
Serial.println("sent");
   }
  
}
 else if(((char)EEPROM.read(1))=='n' ||EEPROM.read(1)==255)
   {
    Serial.println("NNN");
     nex.sendCommand("page login");
   }



}




void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("master")) {
      Serial.println("connected");
      
      autho();
      digitalWrite(speaker,HIGH);
      delay(500);
      digitalWrite(speaker,LOW);
      // Once connected, publish an announcement...
      client.publish("house_hold/living/status", "Master System active");
     // client.publish("house_hold/fire_fighter/help", "Don't worry water is on the way");
      // ... and resubscribe
      client.subscribe("fire_fighter/kitchen/status");
      client.subscribe("doorlock/front/status");
      client.subscribe("thor/8754623583/status");
      
      client.subscribe("fire_fighter/kitchen/msg");
      client.subscribe("doorlock/front/msg");
      client.subscribe("thor/8754623583/msg");
      client.subscribe("co/house_hold/msg");
      
      //client.subscribe("fire_fighter//household/help");
      //client.subscribe("doorlock/household/help");
        

      
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

String tempera="";

String message1="";
void loop() {


  
if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
 long now = millis();
  if (now - lastMsg > 5000) 
  {
Serial.println(dht.readHumidity());
Serial.println(dht.readTemperature());
    if((tempera=String(dht.readTemperature()))!="nan")
    { 
      tempera+=" C";
      nex.setComponentText("t2",tempera);
tempera="";
    }
    Serial.println("C");
    lastMsg = now;
        blinks();
        }


message1=nex.listen();
if(message1!="")
{
  Serial.print(message1);

if(message1=="1234")
{
  nex.sendCommand("page mainpage");
  EEPROM.write(1,'s');
EEPROM.write(2,message1.length());
Serial.print("length:");
Serial.println(message1.length());
for(int i=0;i<message1.length();i++)
{
 EEPROM.write(i+3,message1[i]);
}
  
  Serial.println("done");
EEPROM.commit();
}
else if(message1=="logout")
{
  EEPROM.write(1,'n');
EEPROM.write(2,0);
Serial.print("length:");
Serial.println(message1.length());
for(int i=0;i<message1.length();i++)
{
 EEPROM.write(i+3,0);
}
  EEPROM.commit();
  nex.sendCommand("page login");
}

else
{
  nex.setComponentText("t1","Invalid Pin");
}



}






}
