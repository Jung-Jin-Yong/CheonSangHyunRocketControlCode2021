#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <SoftwareSerial.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);

int Index;
/* SoftwareSerial espSerial(2, 3); // RX, TX
String str; */

void setup(void) 
{
  Serial.begin(9600);
  espSerial.begin(9600);
  Serial.println("Orientation Sensor Test"); Serial.println("");
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);


  digitalWrite(10,LOW);
  digitalWrite(6,LOW);
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(true);
}

void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  
  /* Display the floating point data */
  Serial.print("X: ");
  Serial.print(event.orientation.x, 4);
  Serial.print("\tY: ");
  Serial.print(event.orientation.y, 4);
  Serial.print("\tZ: ");
  Serial.print(event.orientation.z, 4);
  Serial.println("");
  
  if(event.orientation.x>=100) {

    digitalWrite(8,HIGH);
    digitalWrite(4,HIGH);
    for(Index = 0; Index < 470; Index++)
    {
      
      digitalWrite(9,HIGH);
      digitalWrite(5,HIGH);
      delayMicroseconds(1000);
      digitalWrite(9,LOW);
      digitalWrite(5,LOW);
      delayMicroseconds(1000);
    }

    digitalWrite(8,LOW);
    digitalWrite(4,LOW);

    for(Index = 0; Index < 470; Index++)
    {
      digitalWrite(9,HIGH);
      digitalWrite(5,HIGH);
      delayMicroseconds(1000);
      digitalWrite(9,LOW);
      digitalWrite(5,LOW);
      delayMicroseconds(1000);
    }
  }
}
