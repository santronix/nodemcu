/*******************************************************
 * SANTRONIX <info@santronix.in>
 * 
 * This file is part of SANTRONIX Particle Photon IoT Tutorials.
 * 
 * SANTRONIX Particle Photon Tutorials Project can not be copied and/or distributed without the express
 * permission of SANTRONIX
 *******************************************************/

#include <Servo.h>
#include "Ubidots.h"

#define IR 5

const char* UBIDOTS_TOKEN = "TOKEN";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "SANTRONIX";      // Put here your Wi-Fi SSID
const char* WIFI_PASS = "XXXXXX";      // Put here your Wi-Fi password 

Servo servo;
Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);

int detection = HIGH;

bool hold = false;

void setup() {
    Serial.begin(9600);
    ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
    servo.attach(16);
}

void loop() {
  detection = digitalRead(IR);
  if(hold == false){
    if(detection == LOW)
    {
      Serial.println("Obstacle detected");
      servo.write(90);
      delay(1000);
      servo.write(0);
      delay(1000);
      ubidots.add("Sanitser_Count", 1);
      bool bufferSent = false;
      bufferSent = ubidots.send();
      if (bufferSent) {
       Serial.println("Reported Sanitiser use to Cloud");
      }
      hold = true;
    }
  }
  

  if(detection == HIGH){
    //
    Serial.println("Obstacle removed");
    hold = false;
  }

}
