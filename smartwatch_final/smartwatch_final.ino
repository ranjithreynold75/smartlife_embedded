#include <stdio.h>
#include <DS3231.h>
#include <LCD5110_Graph.h>
#include <Wire.h>
#include<SoftwareSerial.h>
SoftwareSerial serial(2, 3); //rx 2 tx 3

//Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 6;// pin to blink led at each beat
int notify=13;
int flash=5;
int fadePin = 4;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin
int speaker=7;
// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Regards Serial OutPut 
static boolean serialVisual = true;  


// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
LCD5110 lcd(8,9,10,11,12);
extern unsigned char BigNumbers[];
extern unsigned char SmallFont[];
extern uint8_t TinyFont[];
#define BUFF_MAX 128

uint8_t time[8];
char recv[BUFF_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 1000;
extern uint8_t clock[];
extern uint8_t apj[]; 
char a;
void setup()
{
  // Setup Serial connection
//  Serial.begin(115200);
serial.begin(9600);
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
pinMode(7,OUTPUT);
pinMode(notify,OUTPUT);
pinMode(flash,OUTPUT);
digitalWrite(flash,LOW);
  Serial.begin(9600);             // we agree to talk fast!

  // Uncomment the next line if you are using an Arduino Leonardo
  //while (!Serial) {}
 Wire.begin();

  lcd.InitLCD();
     lcd.setFont(SmallFont);

    lcd.clrScr();
    //lcd.drawBitmap(0,0,clock,84,48);
   
     lcd.drawBitmap(0, 0, clock, 84, 48);
 lcd.update();
 digitalWrite(7,HIGH);
 delay(300);
 digitalWrite(7,LOW);  
 delay(3000);

    lcd.clrScr();
  // Initialize the rtc object
  rtc.begin();
    interruptSetup();
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014

}
String str1="";
String data="l";
String a1="";
void loop()
{
  
serial.print(a1[0]+a1);

 lcd.print(rtc.getTimeStr(), CENTER, 0);
 String str=(String)rtc.getTemp();
 str="Temp:"+str+"'C";
 lcd.print(str, CENTER, 20);

lcd.drawRect(7, 17, 78, 28);
a1=(String)BPM;
str1=(String)BPM;
 str1="Heartrate:("+str1+")";
  lcd.print(str1, CENTER, 40);
lcd.update();

  if (QS == true){     // A Heartbeat Was Found
       
        fadeRate = 255;         // Makes the LED Fade Effect Happen
       
            
        String str=(String)BPM;
  
        QS = false;                      // reset the Quantified Self flag for next time    
  }

ledFadeToBeat();
delay(20);


if(serial.read()=='w')
{
  //Serial.setTimeout(700);
data=serial.readString();

if(data.equals("fo"))
{
  digitalWrite(flash,HIGH);
data="Flash ON";
}
else if(data.equals("ff"))
{
  digitalWrite(flash,LOW);
data="Flash OFF";
  
}

digitalWrite(7,HIGH);  
digitalWrite(notify,HIGH);
  delay(500);
  digitalWrite(7,LOW);
 digitalWrite(notify,LOW);
  delay(300);
digitalWrite(7,HIGH);  
  digitalWrite(notify,HIGH);
  delay(500);
  digitalWrite(7,LOW);
digitalWrite(notify,LOW);
//  delay(500);

  
  //data="whatsapp message";
Serial.println(data);
lcd.clrScr();
lcd.drawRect(7, 17, 78, 28);
 lcd.print(data, CENTER, 20);
lcd.update();
//serial.print("hai");
delay(3000);  
lcd.clrScr();
data="";

serial.flush(); 
}

}


void ledFadeToBeat(){
    fadeRate -= 15;                         //  set LED fade value
    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin,fadeRate);          //  fade LED
  }


