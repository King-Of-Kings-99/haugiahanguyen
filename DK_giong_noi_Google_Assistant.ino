//----------------------------------------TAPIT - 06/2018-------------------------------------------
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "IP wifi"
#define WIFI_PASS "pass wifi"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "name user"
#define MQTT_PASS "2f5c6bbe7e034eb9b1d8564f27e95108"

//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/light");
Adafruit_MQTT_Subscribe fan   = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/fan");


void setup()
{
  Serial.begin(9600);

  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  Serial.println("OK!");

  //Subscribe to the onoff feed
  mqtt.subscribe(&light);
  mqtt.subscribe(&fan);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  while (WiFi.status() == WL_CONNECTED)
  {
  digitalWrite(LED_BUILTIN, LOW);    
    MQTT_connect();
  
  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &light)
    {
      //Print the new value to the serial monitor
      Serial.print("onoff: ");
      Serial.println((char*) light.lastread);
      
      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) light.lastread, "ON"))
      {
        //Active low logic
        digitalWrite(D1, HIGH);
      }
      else
      {
        digitalWrite(D1, LOW);
      }
    }

    if (subscription == &fan)
    {
      //Print the new value to the serial monitor
      Serial.print("onoff: ");
      Serial.println((char*) fan.lastread);
      
      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) fan.lastread, "ON"))
      {
        //Active low logic
        digitalWrite(D2, HIGH);
      }
      else
      {
        digitalWrite(D2, LOW);
      }
    }
  }

  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}
}

void MQTT_connect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
