#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

//Enter your wifi credentials
const char* ssid = "nyong";
const char* password =  "321321321";

//Enter your mqtt server configurations
const char* mqttServer = "driver.cloudmqtt.com";    //Enter Your mqttServer address
const int mqttPort = 18659;       //Port number
const char* mqttUser = "qgigavut"; //User
const char* mqttPassword = "vhLIo8HOH6wQ"; //Password

WiFiClient espClient;
PubSubClient client(espClient);

char MQTT_send_data[40] = "";

void setup() {
  delay(1000);
  Serial.begin(9600);
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
  
  client.loop();
  sprintf(MQTT_send_data, "X : %.2lf Y : %.2lf Z : %.2lf\n", event.orientation.x, event.orientation.y, event.orientation.z); //데이터값 입력하는 부분
  client.publish("1003rocket/pub", MQTT_send_data);
  
  delay(1000);
}