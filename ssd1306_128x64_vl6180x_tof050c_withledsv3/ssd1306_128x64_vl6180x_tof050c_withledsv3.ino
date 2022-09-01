/**************************************************************************
 sensor TOF050C-VL6180X
 oled 0,9 i2c
 pushbutton for teaching
 mini esp8266 (with usb connect)
 board generic esp8266
 DFRobot_VL6180X.h beste lib andere niks anders dan shit
 serial niet hoger dan 9600
 1ste detectie 255mm en dan correct is geen rechte beam range 10 -180mm
 nu teach in eeprom steken
 trigger op 0.4 sec nu

 info
 https://github.com/G6EJD/Using-ESP8266-EEPROM/blob/master/ESP8266_Reading_and_Writing_EEPROM.ino
 **************************************************************************/


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <elapsedMillis.h>
#include <DFRobot_VL6180X.h>
#include <EEPROM.h>
//oled
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define teachinput 14 //d5
#define redled 12 // d6
#define greenled 13 //d7
#define LED  2  //gpio2 tx1

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//oled
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//distance sensor
DFRobot_VL6180X VL6180X;

//elapsed millis
int heartbeatinterval = 1000;
elapsedMillis timeheartbeat;
int showinterval = 200;
elapsedMillis timeshowoled;
elapsedMillis timecycle;
//distance
uint8_t gl_distance = 0;
uint8_t gl_teachdistance = 40; //in mm
uint8_t gl_teachdistanceOld = 40;
uint8_t gl_maxdiff = 5;
//eeprom
uint8_t gl_eepromreadvalue = 0;
int EEaddress = 0;
//i2c
uint8_t iSDA = 4;
uint8_t iSCL = 5;

void setup() 
{
  Serial.begin(9600);
  //io
  pinMode(LED, OUTPUT);
  pinMode(greenled, OUTPUT);
  pinMode(redled, OUTPUT);
  pinMode(teachinput ,INPUT);  
  
  digitalWrite(redled,HIGH);
  digitalWrite(greenled,LOW);
 
  Wire.begin(iSDA,iSCL);
  //Wire.setClock(400000);
  delay(5000);
 
  Serial.println("start setup");
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();
  display.display();
  delay(2000);

  while(!(VL6180X.begin())){
    Serial.println("Please check that the IIC device is properly connected!");
    
    delay(1000);
  }
  //eeprom
  EEPROM.begin(1);  //EEPROM.begin(Size) 1byte 0-255
  
  EEPROM.get(EEaddress,gl_eepromreadvalue); 
  Serial.print("Setup EEPROM contents at Address=0 is  : ");
  Serial.println(gl_eepromreadvalue);  
  
  if  ( (gl_eepromreadvalue >15) &&  (gl_eepromreadvalue < 255) )
   { 
      gl_teachdistance = gl_eepromreadvalue;
      gl_teachdistanceOld = gl_eepromreadvalue;
   }
     
}

void loop()
{
  timecycle =0;
  if ( timeshowoled >showinterval and !digitalRead(teachinput))
   {  String result =  f_measure();
      showdistance(result); 
      timeshowoled = 0;
   }
  if (timecycle>200)
    {
     Serial.println("Cycletime:=");
     Serial.println(timecycle);
    }
  
  f_checkteach();
  f_controloutput();
  f_heartbeat();
}  
//////
//SUBS
//////
void showdistance(String i_measurement) 
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setTextSize(2); 
  display.setCursor(16,0);             
  display.println(F("Distance!"));
  display.println(i_measurement);
  display.setTextSize(1);
  display.println(gl_teachdistance);
  display.display();

}

void f_heartbeat()
{
  if (timeheartbeat > heartbeatinterval)
  {
    digitalWrite(LED,!digitalRead(LED));
    timeheartbeat = 0;
  }    
}

void f_checkteach()
{ 
  if (digitalRead(teachinput))
   {
     Serial.println("teachinput= High");
     if (15<gl_distance < 255 ) 
      {gl_teachdistance = gl_distance;
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setTextSize(2); 
        display.setCursor(16,0);             
        display.println(F("Teach!"));
        display.println(String(gl_teachdistance));
        display.display();
      }   
   }
   //write to eeprom when finished
   if (gl_teachdistance != gl_teachdistanceOld)
    {Serial.println("Writing EEPROM");
     gl_teachdistanceOld = gl_teachdistance;
     EEPROM.write(EEaddress,gl_teachdistance); // Writes the value  to EEPROM greater numbers use put
     EEPROM.commit();
    }
}

void f_controloutput()
{ uint8_t min = gl_teachdistance - gl_teachdistance/2;//once set must keep on deeper
  uint8_t max = gl_teachdistance + gl_maxdiff;
  // Serial.println(gl_distance);
  // Serial.println(gl_teachdistance);
  // Serial.println(min);
  // Serial.println(max);
 
  
  if (gl_distance < 255)
    {  if ( (min < gl_distance) && (gl_distance< max) )
       {
        Serial.println("greenled = HIGH");
        digitalWrite(greenled,HIGH);
       }
       else
       {
        Serial.println("greenled = LOW");
        digitalWrite(greenled,LOW);   
       } 
    }    
}



