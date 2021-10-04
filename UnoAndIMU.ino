#include <SoftwareSerial.h>

SoftwareSerial espSerial(5, 6);
String str;

void setup(){
  Serial.begin(115200);
  espSerial.begin(115200);
  delay(2000);
}

int i = 0;
void loop()
{
  str = String("From Arduino : ") + String(i);
  Serial.print(i);
  espSerial.println(str);
  i++;
  if(i>30) i = 0;
  delay(1000);
}
