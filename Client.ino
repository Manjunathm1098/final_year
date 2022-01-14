#include <WiFi.h>
#include<LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
String CtrlJsonDataString;

#include "ThingSpeak.h"


LiquidCrystal_I2C lcd(0x3f,16,2);

unsigned long myChannelNumber = 1117197;
const char * myWriteAPIKey = "KIMY1YC8DY7IRDEN";
int keyIndex = 0;


const char *ssid =  "robosa";     // replace with your wifi ssid and password
const char *pass =  "mahalaxmi";
const char* server = "api.thingspeak.com";
WiFiClient client;

#include "ACS712.h"
ACS712 sensor(ACS712_05B, 39);

int voltagesensor=36;

float voltreading=0;
float currentreading=0;

int powerr=0;

float windpressure=0;

int dust=0;

float temperature;

void setup() 
{
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Solar Monotoring");
  lcd.setCursor(0,1);
  lcd.print("System");
  delay(3000);
  lcd.clear();
  lcd.print("Initializing..");
  delay(3000);
   sensor.calibrate();
  lcd.clear();    
  Serial.begin(9600);

    delay(10);
    Serial.println("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    lcd.clear();
    lcd.print("Done");
    delay(1000);
    ThingSpeak.begin(client);  // Initialize ThingSpeak
    lcd.clear();
 
}

void loop() 
{

  float tempv=analogRead(voltagesensor);
voltreading=tempv*5.0/657;

 currentreading = sensor.getCurrentDC();
 powerr=voltreading*currentreading;
 lcd.setCursor(0,0);
 lcd.print("V:");
 lcd.setCursor(4,0);
 lcd.print(voltreading);
  lcd.setCursor(7,0);
 lcd.print("I:");
 lcd.setCursor(10,0);
 lcd.print(currentreading);
  lcd.setCursor(0,1);
 lcd.print("POWER:");
 lcd.setCursor(7,1);
 lcd.print(powerr);
 
 
  
  // Send it to serial

  // Wait one second before the new cycle
  delay(1000);
  
  while (Serial.available() > 0)
  {

    CtrlJsonDataString = Serial.readStringUntil('\n');
    if (CtrlJsonDataString.indexOf('{') == (-1) || CtrlJsonDataString.indexOf('}') == (-1)) {
      //Serial.println("Serial Not Useful");
      return;
    }
    else {
      //Serial.println("Serial Useful");
      pdata(CtrlJsonDataString);
  }
}
 

}

void pdata(String j)
{
  StaticJsonBuffer<200> jsonBuffer;

  String injson=j;
  //Serial.println(injson);

  if(injson.indexOf('{') && injson.indexOf('}'))
  {
    int a=injson.indexOf('{');
    int b=injson.indexOf('}');

    
    injson=injson.substring(a,b+1);
  }
  JsonObject& root = jsonBuffer.parseObject(injson);

  // Test if parsing succeeds.
  if (!root.success()) {
    //Serial.println("parseObject() failed");

    return;
  }

  else
  {
Serial.println("Json Success");

dust=root["dustj"];

windpressure=root["windj"];
temperature=root["tempj"];

  ThingSpeak.setField(1, voltreading);
  ThingSpeak.setField(2, currentreading);
  ThingSpeak.setField(3, dust);
  ThingSpeak.setField(4, windpressure); 
  ThingSpeak.setField(5, powerr);
   ThingSpeak.setField(6, temperature);
   int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(10000);
  



  


  }

}
