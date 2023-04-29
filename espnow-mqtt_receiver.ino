/* Getting the Libraries */
#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#include<SPI.h>
#include<espnow.h>

/* Variables */
unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

/* Reciever's Data: Must match the Sender's Data Structure */
typedef struct struct_message{
  char a[32];
  char b[32];
}Message;

// IPAddress
IPAddress ip(192, 168, 4, 45);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(8, 8, 8, 8);
IPAddress dns2(8, 8, 4, 4);

/* Creating a Message */
Message message;

/* Credentials */
// WiFi
//const char* ssid = "snsriramalab";
const char* ssid = "ESP-SoftAP";
//const char* wifi_passwd = "thereisnospoon";
//const char* wifi_passwd = "Cloud&Smart";

// MQTT
const char* mqtt_server = "192.168.4.4";
const char* mqtt_topic = "test_1";
const char* mqtt_username = "pi-101";
const char* mqtt_passwd = "pi";
const char* clientID = "ESP-004";

/* MQTT WiFi and MQTT Client Objects */
WiFiClient wifi_client;
PubSubClient client(mqtt_server, 1883, wifi_client);

/* Callback Function that will be executed when data is received */
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len){
//  stop = clock();
  memcpy(&message, incomingData, sizeof(message));
  Serial.println(message.a);
  Serial.println(message.b);
  client.publish(mqtt_topic, message.a);
  client.publish(mqtt_topic, message.b);
  client.subscribe(mqtt_topic);
}

/* Run once */
void setup(){
//  start = clock();
  Serial.begin(115200);
  /*
  if(WiFi.config(ip, dns1, gateway, subnet)){
    Serial.println("Static IP Configured");
  }else{
    Serial.println("Static IP Configuration Failed");
  }
  */
  WiFi.begin(ssid);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi Network!");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while(!client.connected()){
    Serial.println("Connecting to MQTT..");
    if(client.connect(clientID, mqtt_username, mqtt_passwd)){
      Serial.println("Connected..");
    }else{
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.publish(mqtt_topic, "Hello Raspberry Pi..");
  client.subscribe(mqtt_topic);

  WiFi.mode(WIFI_STA);
  // Initialize ESP-NOW
  if(esp_now_init() != 0){
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Registering Receiver for Recv Packets
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

/* Callback! */
void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived in the topic ");
  Serial.println(topic);
  Serial.print("Message");
  for(int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("_______________");
}

/* Runs in a loop */
void loop(){
  if((millis() - lastTime) > timerDelay){
    ESP.restart();
    lastTime = millis();
  }
}
