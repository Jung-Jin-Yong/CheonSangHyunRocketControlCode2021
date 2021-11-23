#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
/*#include <SPI.h>
#include <SD.h>*/

Adafruit_BNO055 bno = Adafruit_BNO055(55);
//File myFile;

// change this to match your SD shield or module;
//const int chipSelect = D3;

//Enter your wifi credentials
const char* ssid = "HUAWEI_AF21";
const char* password = "NY7Y3T587A9";

//Enter your mqtt server configurations
const char* mqttServer = "driver.cloudmqtt.com";    //Enter Your mqttServer address
const int mqttPort = 18659;       //Port number
const char* mqttUser = "qgigavut"; //User
const char* mqttPassword = "vhLIo8HOH6wQ"; //Password

const PROGMEM  uint16_t motorControlSet[]  = {0, 1, 2};
unsigned int motorControl;

WiFiClient espClient;
PubSubClient client(espClient);

char MQTT_send_data[100] = "";
//int i = 0;
void setup() {
  delay(1000);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  //Serial.print("Initializing SD card...");
  /*
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  */
  //Serial.println("initialization done.");
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  //SD.remove("test.txt");
  //myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  /*if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }*/

  //////////SDCARD//////////////////
  
  Serial.println("Orientation Sensor Test"); Serial.println("");

  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(true);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());

  client.setServer(mqttServer, mqttPort);
  client.setCallback(MQTTcallback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.println(client.state());  //If you get state 5: mismatch in configuration
      delay(2000);

    }
  }

  client.publish("1003rocket/pub", "Hello from ESP8266");
  client.subscribe("1003rocket/sub");

}

void MQTTcallback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");

  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];  //Conver *byte to String
  }
  Serial.println(message);
  Serial.println("-----------------------");
}

void loop() {
  /* Get a new sensor event */ 
  sensors_event_t event;
  bno.getEvent(&event);
  
  if(event.orientation.y > 2 && event.orientation.y < 52){
    if(event.orientation.z < 25) motorControl = pgm_read_word_near(motorControlSet + 1);
    else if(event.orientation.z < 167 && event.orientation.z>=25) motorControl = pgm_read_word_near(motorControlSet + 2);
  }
  else motorControl = pgm_read_word_near(motorControlSet + 0);
  /*
  myFile = SD.open("test.txt", FILE_WRITE);
  if(myFile){
    myFile.print("X : ");
    myFile.print(event.orientation.x);
    myFile.print(" Y : ");
    myFile.print(event.orientation.y);
    myFile.print(" Z : ");
    myFile.print(event.orientation.z);
    myFile.print(" motorControl : ");
    myFile.print(motorControl);
    myFile.println();
  }else{
    Serial.println("error opening test.txt");
  }
  */
  /*if((event.orientation.y>2 && event.orientation.y<52)&&(event.orientation.z>167)) answer = 'L';
  else if((event.orientation.y>2 && event.orientation.y<52)&&(event.orientation.z<11.75)) answer = 'R';*/
  /*if(event.orientation.z > 150) i = 1;
  if(event.orientation.z < 11.75) i = 2;*/
  client.loop();
  sprintf(MQTT_send_data, "X : %.2lf Y : %.2lf Z : %.2lf motorControl : %d\n", event.orientation.x, event.orientation.y, event.orientation.z, motorControl);
  client.publish("1003rocket/pub", MQTT_send_data);
  Serial.print("X : ");
  Serial.print(event.orientation.x);
  Serial.print(" Y : ");
  Serial.print(event.orientation.y);
  Serial.print(" Z : ");
  Serial.print(event.orientation.z);
  Serial.print(" motorControl : ");
  Serial.println(motorControl);
  event.orientation.x = 0;
  event.orientation.y = 0;
  event.orientation.z = 0;
  motorControl = 0;
  delay(100);
}
