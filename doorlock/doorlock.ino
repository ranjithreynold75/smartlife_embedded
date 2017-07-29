#include <Nextion.h>

#include <Servo.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<SoftwareSerial.h>


const char* ssid = "was";
const char* password = "1234qwerty";
const char* mqtt_server = "192.168.43.63";
String access_pin="";
SoftwareSerial serial(D6, D5); //(rx,tx)

Nextion nex(serial,9600);
int getir();
String door="locked";
int door_status=getir();
int prev_status=0;
int gled1=D0;
int speaker=D1;
int ir=D7;
int pir=D8;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void setup() {

pinMode(speaker,OUTPUT);
pinMode(gled1,OUTPUT);
pinMode(ir,INPUT_PULLUP);
 //int door_status=getir();
  /*
  if(getir())
  {
    angle=80;
  servo.write(angle);
  }
  else
  {
    
    angle=8;
  servo.write(angle);
  }*/
  Serial.begin(115200);
 setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
serial.begin(9600);

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

digitalWrite(gled1,HIGH);
//digitalWrite(gled2,HIGH);  
delay(500);
digitalWrite(gled1,LOW);
//digitalWrite(gled2,LOW);

  
}

void unlock_door()
{
    door="unlock";
   
   /* 
    if(angle!=8)
    {
    for(;angle>8;angle--)
    {
      servo.write(angle);
      delay(8);
    }
    
    }
*/
   digitalWrite(speaker,HIGH);
    delay(200);
    digitalWrite(speaker,LOW);
  delay(200);
   digitalWrite(speaker,HIGH);
    delay(200);
    digitalWrite(speaker,LOW);

delay(3000);


 
        

}

void lock_door()
{
  door="locked";
 /*
   if(angle!=80)
    {
    for(;angle<80;angle++)
    {
      servo.write(angle);
      delay(8);
    }
    
    }
    */
      digitalWrite(speaker,HIGH);
    delay(100);
    digitalWrite(speaker,LOW);


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
    
client.publish("doorlock/fire_fighter/help","dont worry i will open the door :)");
     nex.setComponentText("t5", "FIRE!!");
 Serial.println("unlock door");
  unlock_door();
delay(5000);
  
  }
  else if(msg=="door open")
  {
 nex.setComponentText("t5","DOOR DEACTIVATED");
 Serial.println("unlock door");
  unlock_door();
//delay(5000);
    
  }
  else if(msg==access_pin)
  {
    Serial.println(access_pin);
    unlock_door();
  access_pin="";
  }
  Serial.println();


}

void alert()
{
  Serial.println(prev_status);
digitalWrite(speaker,HIGH);
 
  if(prev_status!=door_status)
 {
  Serial.println("sending notify");
   prev_status=door_status;
   client.publish("doorlock/front/msg","Security breach");
  
 }
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("doorlock")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("doorlock/front/status", "doorlock System active");
     // client.publish("doorlock/fire_fighter/help", "Don't worry openning the door");
      // ... and resubscribe
      client.subscribe("fire_fighter/kitchen/status");
      client.subscribe("house_hold/living/status");
      client.subscribe("thor/8754623583/status");

      client.subscribe("fire_fighter/kitchen/msg");
      client.subscribe("house_hold/living/msg");
      client.subscribe("thor/8754623583/msg");
      client.subscribe("co/front/doorlock");
      //client.subscribe("fire_fighter/doorlock/help");
      //client.subscribe("house_hold/doorlock/help");
    
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


char* a;
char pin[]={'1','2','3','4'};
int pos=0;
String message1="";

int getir()
{
return digitalRead(ir);
  
}
int ff=0;
void loop() {

/*
if(digitalRead(pir)==1 && ff==0)
{
  ff=1;
nex.sendCommand("sleep=0");
}
else if(digitalRead(pir)==0 && ff==1)
{
  ff=0;
  nex.sendCommand("sleep=1");
}

*/
if(!getir())
{
  prev_status=getir();
  //Serial.println(getir());
  if(door=="unlock")
  {
    Serial.println(door);
    digitalWrite(speaker,HIGH);
    delay(200);
    digitalWrite(speaker,LOW);
  }
  door="locked";
  digitalWrite(speaker,LOW);
  //prev_status=getir();
  door_status=getir();

message1=nex.listen();
if(message1!="")
{
  Serial.print(message1);
  
client.publish("doorlock/front/co","need pin");

access_pin=message1;
/*if(message1==1234)
{
  unlock_door();
}
*/

}
}

else if(getir()==1 && door=="locked")
{
  door_status=getir();
  // Serial.println(getir);
  alert();
}



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



}
