// This example shows how to connect to LOTODA using an ESP8266 and send/receive sample data.
// Make sure you install the ESP8266 Board Package via the Arduino IDE Board Manager and select the correct ESP8266 board before compiling.

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_SSID "..."   //Enter your WIFI SSID
#define WIFI_PASSWORD "..."   //Enter your WiFi password

char LotodaServer[]  = "app.lotoda.vn";     //app.lotoda.vn is the default servername
int LotodaPort  = 1883;                     //1883 is the default port
char deviceId[]  = "LotodaEsp8266-0001";  //every device should have a different name
char topic[]  = "<User_ID_Key>/lamp/power"; //Enter <User_ID_Key> from LOTODA system, the topic should be different for each device as well
                                            //Please create this new topic on LOTODA mobile app, in this case is "lamp/power"
char User_ID_Key[]  = "<User_ID_Key>";         //Enter <User_ID_Key> from LOTODA system
char Password_ID_Key[]  = "<Password_ID_Key>";  //Enter <Password_ID_Key> from LOTODA system

WiFiClient wifiClient;
PubSubClient client(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output led
  pinMode(5, OUTPUT);               // Initialize the pin GPIO5 as an output replay
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
    client.setServer(LotodaServer, LotodaPort);
    client.setCallback(callback);
    if (client.connect(deviceId, User_ID_Key, Password_ID_Key)) {
      client.subscribe(topic);
      Serial.println("Connected to LOTODA's Server");
    } else {
      Serial.print("[FAILED] [ rc = ");
      Serial.print(client.state() );
      Serial.println(" : retrying in 5 seconds]");
      delay(5000);
    }
  }
}
