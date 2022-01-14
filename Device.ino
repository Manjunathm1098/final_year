

#include<Servo.h>

#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;

int hourvalue,minvalue;
#define STEP_PIN 4
#define DIR_PIN 3
#define enpin 2

//Adjust this to adjust the speed*********
int delay_time=2000;

int stepsmultiplier=10;
//these pins can not be changed 2/3 are special pins

String jsonstring="";




Servo myservo;
Servo myservo1;

int topLeftLight = 0;     
int topRightLight = 0;
int bottomLeftLight = 0;
int bottomRightLight = 0;
int LeftLight = 0;
int RightLight = 0;
int TopLight = 0;
int BottomLight = 0;

int servoangle=30;

int hourvalconst=0;

float temperature=0;
double pressure=0;

int curtime=0;
int prevtime=0;
int dustval=0;
void setup()
{
   
  
  pinMode(STEP_PIN,OUTPUT);
pinMode(DIR_PIN,OUTPUT);
pinMode(enpin,OUTPUT);
pinMode(5,OUTPUT);
digitalWrite(5,HIGH);


digitalWrite(DIR_PIN,0);
digitalWrite(enpin,HIGH);
  
    Serial.begin(9600);
     



  pinMode(A0, INPUT);   //Light sensor up - left
  pinMode(A1, INPUT);   //Light sensor up - right
  pinMode(A2, INPUT);   //Light sensor bottom - left
  pinMode(A3, INPUT);   //Light sensor bottom - right  
  pinMode(12,INPUT_PULLUP);
  
 Wire.begin();
    RTC.begin();
    myservo.attach(9);
     myservo1.attach(8);
     
    myservo.write(30);
       myservo1.write(0);
       delay(2000);
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  while (1) {}
  }
  
    

}

void loop()
{
  curtime=millis();

  if((curtime-prevtime)>20000)
  {
    Serial.println(jsonstring);
    prevtime=curtime;
  }
DateTime now = RTC.now(); 

temperature=bmp.readTemperature();

    
pressure=bmp.readPressure();
dustval=analogRead(A2);



    hourvalue=now.hour();
   
   minvalue=now.minute();
jsonstring="{\"dustj\":"+String(dustval)+",\"windj\":"+String(pressure)+",\"tempj\":"+String(temperature)+"}";

   
 
  if(digitalRead(12)==0)
  {

   hourvalconst=constrain(hourvalue,8,19);
   servoangle=map(hourvalconst,8,19,30,120);
   myservo.write(servoangle);
 

  
 topLeftLight = map(analogRead(A2),0,800,0,100);
    topRightLight = map(analogRead(A1),0,800,0,100);
    bottomLeftLight = map(analogRead(A0),0,800,0,100);
    bottomRightLight = map(analogRead(A3),0,800,0,100);
    //Calculate the average light conditions
    TopLight = ((topRightLight + topLeftLight) / 2);
    BottomLight = ((bottomRightLight + bottomLeftLight) / 2);
    LeftLight = ((topLeftLight + bottomLeftLight) / 2);
    RightLight = ((topRightLight + bottomRightLight) / 2);



   if (abs((RightLight - LeftLight)) > 12) {    //Change position only if light difference is bigger then 4%
      if (RightLight < LeftLight) {
        runstepper(10,0,2000);
      }
      if (RightLight > LeftLight) {
           runstepper(10,1,2000);
      }
    }
  }
  else
  {
     myservo.write(30);
     delay(1000);
  }

  if(pressure>95000)
  {
     runstepper(10,1,5000);
     delay(3000);
     runstepper(10,0,5000);
     
  }
if(hourvalue>8 && hourvalue<19)
{
  if(analogRead(A2)<50)
  {
    myservo1.write(180);
    delay(10000);
    myservo1.write(0);
    delay(1000);
  }
  
}

}

void runstepper(int steps, int dir, int speedd)
{
  
if(dir==0)
{
 
 for(int i=0;i<steps;i++)
{
  digitalWrite(enpin,HIGH);
  digitalWrite(DIR_PIN,dir);
  digitalWrite(STEP_PIN,HIGH);
delayMicroseconds(speedd);
digitalWrite(STEP_PIN,LOW);
delayMicroseconds(speedd);
}
stopstepper();
  
}
else if(dir==1)
{
   
 for(int i=0;i<steps;i++)
{
  digitalWrite(enpin,HIGH);
  digitalWrite(DIR_PIN,dir);
  digitalWrite(STEP_PIN,HIGH);
delayMicroseconds(speedd);
digitalWrite(STEP_PIN,LOW);
delayMicroseconds(speedd);

stopstepper();
  }
}

}



void stopstepper()
{
 digitalWrite(STEP_PIN,LOW); 
 digitalWrite(enpin,HIGH);
}
