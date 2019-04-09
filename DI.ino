#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define WLAN_SSID       "HSG"
#define WLAN_PASS       "goyyale123"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "HSGSIVAK"
#define AIO_KEY         "65affc8256a747cca4d64c7190519337"
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe relaymotor = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay-motor");
Adafruit_MQTT_Publish soilmoisture = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil-moisture-sensor");
void MQTT_connect();
const int sensor_pin = A0;
int relayInput =D1;
void setup() {
  pinMode(relayInput, OUTPUT);
  Serial.begin(9600);
  digitalWrite(relayInput, HIGH);
  delay(10);
  Serial.println(F("Drip Irrigation"));
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  mqtt.subscribe(&relaymotor);
}
uint32_t x=0;
void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  float moisture_percentage;
  moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
  Serial.print("Soil Moisture(in Percentage) = ");
  Serial.print(moisture_percentage);
  Serial.println("%");
  if(! soilmoisture.publish((float)moisture_percentage))
    Serial.println(F("Failed")); 
  else
    Serial.println(F("Uploaded"));  
  if(moisture_percentage<20.0)
   digitalWrite(relayInput, LOW);
  else
   digitalWrite(relayInput, HIGH); 
  while ((subscription = mqtt.readSubscription(5000)))
  {
    if (subscription == &relaymotor)
    {
      Serial.println((char*)relaymotor.lastread);
      if(strcmp((char*)relaymotor.lastread,"ON")==0)
       digitalWrite(relayInput, LOW);
      else
       digitalWrite(relayInput, HIGH);
    }
  }
}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
