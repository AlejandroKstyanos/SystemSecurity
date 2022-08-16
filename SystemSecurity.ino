
#include <ESP8266WiFi.h>
#include <MQTT.h>

//PINS
#define alarm 14 // PIN D5 ESP8266
#define led 2 // BUILD_IN LED

const int MOTION_SENSOR_PIN = 12; //GIOP12 pin connected to OUTPUT PIN OF SENSOR
int pinStateCurrent   = LOW;      // current state of pin
int pinStatePrevious  = LOW;// previous state of pin
const char SecurityKey[] ="KstyanosCasa#$%&";
int SecurityActive = 0;


const char ssid[] = "grupo_Hardcore";
const char pass[] = "equipo1Said"; 
/*const char ssid[] = "INFINITUM64DC";
const char pass[] = "UQs3CTyPyg";*/

WiFiClient net;
MQTTClient client;


unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("main_security_compute", "systemsecurity", "ssH75frHWVEy0N9s")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  digitalWrite(alarm,HIGH);
  delay(500);
  digitalWrite(alarm,LOW);
  

  client.subscribe("system/status");
  client.subscribe("system/password");
}

void messageReceived(String &topic, String &payload) {
  if(topic == "system/password" && payload == SecurityKey && SecurityActive == 0){
    SecurityActive = 1;
    Serial.println("System ON");
    client.publish("system/status", "ON");
    digitalWrite(alarm,HIGH);
    delay(1000);
    digitalWrite(alarm,LOW);
  }else if(topic=="system/password" && payload==SecurityKey && SecurityActive == 1){
    SecurityActive = 0;
    client.publish("system/status", "OFF");
    digitalWrite(alarm,HIGH);
    delay(2000);
    digitalWrite(alarm,LOW);
  }
  
  Serial.println(topic + " : " + payload);
  
  if(topic == "system/alarm" && payload == "ON"){
    digitalWrite(alarm,HIGH);
  }else if(topic=="system/alarm" && payload=="OFF"){
    digitalWrite(alarm,LOW);
  }
}

void setup() {

  //Initialize pins
  pinMode(led,OUTPUT);
  pinMode(alarm,OUTPUT); //BUZZER ENERGIZE PIN
  pinMode(MOTION_SENSOR_PIN, INPUT); //MOTION SENSOR PIN TO READ VALUE FROM OUTPUN PIN
  
  Serial.begin(9600);

  // start wifi and mqtt
  WiFi.begin(ssid, pass);
  client.begin("systemsecurity.cloud.shiftr.io", net);
  client.onMessage(messageReceived);
   
  connect();
}

void loop() {
  pinStatePrevious = pinStateCurrent; // store old state
  pinStateCurrent = digitalRead(MOTION_SENSOR_PIN);   // read new state
  
  client.loop();
  delay(10);

  // check if connected
  if (!client.connected()) {
    connect();
  }

  if(SecurityActive == 1) {
    if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {   // pin state change: LOW -> HIGH
    Serial.println("Motion detected!");
    // TODO: turn on alarm, light or activate a device ... here
    digitalWrite(alarm,HIGH);
    client.publish("system/alarm", "Motion detected!");
    }else if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
      Serial.println("Motion stopped!");
      // TODO: turn off alarm, light or deactivate a device ... here
      digitalWrite(alarm,LOW);
    }  
  }
  

  /*// publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    //client.publish("system/alarm", "1");
    
  }*/
}
