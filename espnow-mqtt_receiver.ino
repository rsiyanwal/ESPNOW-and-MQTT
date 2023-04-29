/*
The code sets up an ESP8266 WiFi module to connect to a WiFi network and a MQTT broker. 
It initializes the ESP-NOW protocol to receive data packets from a sender node. Once data is received, 
it is published to a specified MQTT topic using the PubSubClient library. 
The MQTT broker can then be subscribed to this topic to receive the published data.

The code also contains a callback function that executes when data is received. 
The received data is then printed to the serial monitor for debugging purposes.

Finally, the code runs in a loop that restarts the ESP8266 module after a specified time delay.
*/

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

/*
Fetching the crucial data required for connecting to WiFi and MQTT
*/
// IPAddress
IPAddress ip(192, 168, 4, 45);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(8, 8, 8, 8);
IPAddress dns2(8, 8, 4, 4);

// Creating a Message
Message message;

// Credentials 
const char* ssid = "ESP-SoftAP";
const char* wifi_passwd = "thereisnospoon";

// MQTT
const char* mqtt_server = "192.168.4.4";
const char* mqtt_topic = "test_1";
const char* mqtt_username = "pi-101";
const char* mqtt_passwd = "pi";
const char* clientID = "ESP-004";

/* 
Creating a WiFiClient (as previously done in https://github.com/rsiyanwal/Handling-Motion-Sensor-With-ESPNOW) 
and a PubSubClient library instance to set up MQTT
*/
WiFiClient wifi_client;
PubSubClient client(mqtt_server, 1883, wifi_client);

/* 
Publishing a Message to MQTT Topic when received.
*/
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len){
  memcpy(&message, incomingData, sizeof(message));
  Serial.println(message.a);
  Serial.println(message.b);
  client.publish(mqtt_topic, message.a);
  client.publish(mqtt_topic, message.b);
  client.subscribe(mqtt_topic);
}

/*
Setting up the serial monitor, connecting to WiFi, configuring the MQTT server, subscribing to a topic, 
initializing ESP-NOW, and registering a callback function
*/
void setup(){
  Serial.begin(115200);
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

// Callback!
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

/*
The loop is designed to restart the NodeMCU after a specified time period. 
We have observed that when the NodeMCU is running with both ESP-NOW and MQTT protocols, 
it can freeze if no data is sent by the sensor for an extended period of time. 
To ensure the system remains operational, we restart the NodeMCU periodically. 
However, this process incurs an overhead of approximately 5 seconds for the NodeMCU to restart and reconnect to the WiFi network.
*/
void loop(){
  if((millis() - lastTime) > timerDelay){
    ESP.restart();
    lastTime = millis();
  }
}
