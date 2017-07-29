#include <Nextion.h>
#include<EEPROM.h>


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<SoftwareSerial.h>


const char* ssid = "was";
const char* password = "1234qwerty";
const char* mqtt_server = "192.168.43.63";
SoftwareSerial serial(D6, D5); //(rx,tx)

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

int gled1=D0;
int vibe=D7;




Nextion nex(serial,9600);

void setup() {
Serial.begin(115200);

pinMode(gled1,OUTPUT);
pinMode(vibe,OUTPUT);
//pinMode(but,INPUT_PULLUP);
//digitalWrite(but,HIGH);
//  nex.sendCommand("sleep=0");
nex.sendCommand("page start");
      EEPROM.begin(512);
setup_wifi();
client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
reconnect();

//autho();

}

void autho()
{
  
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
Serial.println("sent");
   }
  
}
 else if(((char)EEPROM.read(1))=='n' ||EEPROM.read(1)==0)
   {
    Serial.println("NNN");
     nex.sendCommand("page Login");
   }



}

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
    //nex.setComponentText("t1",".");
  }

  Serial.println("");
nex.setComponentText("t0","Connected!!");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
//nex.setComponentText("t1",String(WiFi.localIP()));
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
  nex.setComponentText("t0","connecting to cordinator");
  
    // Attempt to connect
    if (client.connect("thor")) {
     viber();
      Serial.println("connected");
      autho();

      // Once connected, publish an announcement...
      client.publish("thor/8754623583/status", "Thor active");
     // client.publish("doorlock/fire_fighter/help", "Don't worry openning the door");
      // ... and resubscribe
      client.subscribe("fire_fighter/kitchen/status");
      client.subscribe("house_hold/living/status");
      client.subscribe("doorlock/front/status");
      client.subscribe("co/msg");
     // client.subscribe("fire_fighter/doorlock/help");
      //client.subscribe("house_hold/doorlock/help");
    
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      nex.sendCommand("page start");
      nex.setComponentText("t0","Connecting to coordinator");
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}







void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg;
  for (int i = 0; i < length; i++) {
    msg+=(char)payload[i];
  }
  if(msg=="fire detected")
  {
    msg="";
viber();
    nex.setComponentText("g0","Fire detected: kitchen");
//client.publish("doorlock/fire_fighter/help","dont worry i will open the door :)");

//delay(5000);
  
  }
  else if(msg=="Security breach")
  {
    nex.setComponentText("g0","Security breach->911 Emergency");
  viber();
  }
  Serial.println();


}


void blinks()
{

digitalWrite(gled1,HIGH);
//digitalWrite(gled2,HIGH);  
delay(500);
digitalWrite(gled1,LOW);
//digitalWrite(gled2,LOW);

  
}


void viber()
{

digitalWrite(vibe,HIGH);
//digitalWrite(gled2,HIGH);  
delay(1000);
digitalWrite(vibe,LOW);
//digitalWrite(gled2,LOW);

  
}

String message1="";
int once1=0;
void loop() {
//serial.println(digitalRead(but));
/*if(digitalRead(but)==0 && once1==0)
{
  once1=1;
  Serial.println(digitalRead(but));
   nex.sendCommand("sleep=1");
}
else if(digitalRead(but)==1 && once1==1)
{
  once1=0;
  Serial.println(digitalRead(but));
   nex.sendCommand("sleep=0");
}

*/

if (!client.connected()) {
    reconnect();
  }
  client.loop();

  


 long now = millis();
  if (now - lastMsg > 5000) 
  {
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
  nex.sendCommand("page Login");
}
else if(message1=="door open")
{
  client.publish("thor/8754623583/msg","door open");
}
else if(message1=="70 a")
{
  client.publish("thor/8754623583/msg","panic");
}
else
{
  nex.setComponentText("t1","Invalid Pin");
}



}


 

}
