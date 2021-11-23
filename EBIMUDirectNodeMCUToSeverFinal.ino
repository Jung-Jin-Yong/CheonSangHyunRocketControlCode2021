#include <ESP8266WiFi.h>
#include <PubSubClient.h>

////////////////////////////////////////////////////////////////////
/////////////////////// EBIMU FUNCTION /////////////////////////////
#define SBUF_SIZE 64
 
char sbuf[SBUF_SIZE];
signed int sbuf_cnt=0;
const PROGMEM  uint16_t motorControlSet[]  = {0, 1, 2};
unsigned int motorControl;

int EBimuAsciiParser(float *item, int number_of_item)
{
  int n,i;
  int rbytes;
  char *addr; 
  int result = 0;
  
  rbytes = Serial.available();
  for(n=0;n<rbytes;n++)
  {
    sbuf[sbuf_cnt] = Serial.read();
    if(sbuf[sbuf_cnt]==0x0a)
       {
           addr = strtok(sbuf,",");
           for(i=0;i<number_of_item;i++)
           {
              item[i] = atof(addr);
              addr = strtok(NULL,",");
           }
 
           result = 1;
       }
     else if(sbuf[sbuf_cnt]=='*')
       {   sbuf_cnt=-1;
       }
 
     sbuf_cnt++;
     if(sbuf_cnt>=SBUF_SIZE) sbuf_cnt=0;
  }
  
  return result;
}
/////////////////////// EBIMU FUNCTION /////////////////////////////

//Enter your wifi credentials
const char* ssid = "nyong";//"HUAWEI_AF21";
const char* password = "321321321";//"NY7Y3T587A9";

//Enter your mqtt server configurations
const char* mqttServer = "driver.cloudmqtt.com";    //Enter Your mqttServer address
const int mqttPort = 18659;       //Port number
const char* mqttUser = "qgigavut"; //User
const char* mqttPassword = "vhLIo8HOH6wQ"; //Password

WiFiClient espClient;
PubSubClient client(espClient);

char MQTT_send_data[50] = "";
int a = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);

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
  client.loop();
  float euler[3];
  if(EBimuAsciiParser(euler, 3)){
    Serial.print("\n\r");
    Serial.print(euler[0]); Serial.print(" ");
    Serial.print(euler[1]); Serial.print(" ");
    Serial.print(euler[2]); Serial.print(" ");
  }
  sprintf(MQTT_send_data, "X : %.2f Y : %.2f Z : %.2f\n", euler[0], euler[1], euler[2]); //데이터값 입력하는 부분
  client.publish("1003rocket/pub", MQTT_send_data);
  
  delay(1000);
}
