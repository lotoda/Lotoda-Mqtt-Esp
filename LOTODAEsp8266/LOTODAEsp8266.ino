#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_SSID "...."  //Enter WiFi SSID at here
#define WIFI_PASSWORD "...." //Enter WiFi Password at here

char mqttServer[]  = "app.lotoda.vn"; //change it for your MQTT server IP or network name
int mqttPort  = 1883; //1883 is the default port for MQTT. Change if necessary
char deviceId[]  = "LotodaEsp8266-0001"; //every device should have a different name
char topic[]  = "1c52c46eb8/lamp/power"; //the topic should be different for each device as well
char mqttUser[]  = "1c52c46eb8"; //
char mqttPassword[]  = "4gq6K4vlnW/26hHlLN";

WiFiClient wifiClient;
PubSubClient client(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;

int status = WL_IDLE_STATUS;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(5, OUTPUT);
  Serial.begin(115200);
  delay(20);
  Serial.println("Let' start now");
}

void loop() {
  if ( !client.connected() ) {
    connect();
  }
  client.loop();
}

unsigned long lastOperation;
const long MIN_OPERATION_INTERVAL = 2000L;

const String openString = "OPEN";
const String closeString = "CLOSE";
byte close[] = {0xA0, 0x01, 0x01, 0xA2};
byte open[] = {0xA0, 0x01, 0x00, 0xA1};
/*
void callback(char* topic, byte* payload, unsigned int length) {
    signed long now = millis();
    long deltaTime = now - lastOperation;
    if (deltaTime > MIN_OPERATION_INTERVAL) {
      String message = "";
      for (int i = 0; i < length; i++) {
        message = message + (char)payload[i];
      }
      if(message == openString) { 
        Serial.println("Performing open command");
        Serial.write(open, sizeof(open)); 
        lastOperation = now;
      } else if(message == closeString) {
         Serial.println("Performing close command");
        Serial.write(close, sizeof(close)); 
        lastOperation = now;
      }
  } else {
    Serial.println("Operation denied right now. Try again later.");
  }
}*/

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    digitalWrite(5, HIGH);
  } else if ((char)payload[0] == '0') {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    digitalWrite(5, LOW);
  }

}

void connect() {
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.print("Connected to ");
      Serial.println(WIFI_SSID);
    }
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    if (client.connect(deviceId, mqttUser, mqttPassword)) {
      client.subscribe(topic);
      Serial.println("Connected to MQTT Server");
    } else {
      Serial.print("[FAILED] [ rc = ");
      Serial.print(client.state() );
      Serial.println(" : retrying in 5 seconds]");
      delay(5000);
    }
  }
}
