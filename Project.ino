#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include<Servo.h>
#define led1 D1
#define led2 D2
Servo s1;

#define WLAN_SSID       ""    //wifi access name      
#define WLAN_PASS       ""    // password

#define AIO_SERVER      "io.adafruit.com" 
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    ""            
#define AIO_KEY         ""  

//WIFI CLIENT
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/LED1"); // Feeds name should be same everywhere
Adafruit_MQTT_Subscribe Light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LED2");
Adafruit_MQTT_Subscribe Servo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Servo");

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  s1.attach(D3);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
 
  }
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&Light2);
  mqtt.subscribe(&Servo);

}

void loop() {
 
  MQTT_connect();
  

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &Light1) {
      int Light1_State = atoi((char *)Light1.lastread);
      digitalWrite(led1, Light1_State);
      
    }
    if (subscription == &Light2) {
      int Light2_State = atoi((char *)Light2.lastread);
      digitalWrite(led2, Light2_State);
    }
    if (subscription == &Servo) {
      int Servo_State = atoi((char *)Servo.lastread);
      if(Servo_State==0){
        s1.write(0);        
      }
      if(Servo_State==1){
        s1.write(180);
      }
      
    }
  }

  
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection");
    mqtt.disconnect();
    delay(5000); 
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  
}