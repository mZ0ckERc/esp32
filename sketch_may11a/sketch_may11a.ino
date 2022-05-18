
/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
//#include <libssh_esp32.h>

// Replace with your network credentials
const char* ssid     = "ESP32";
const char* password = "pasword";

const int ledPin_26 = 26, ledPin_27 = 27;
String ledState;

// Set web server port number to 80
AsyncWebServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE_26"){
    if(digitalRead(ledPin_26)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  if(var == "STATE_27"){
    if(digitalRead(ledPin_27)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}

void setup() {
  //    libssh_begin();
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password,3,1);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/on_1", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin_26, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/off_1", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin_26, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/on_2", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin_27, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/off_2", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin_27, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.begin();
}

void loop(){
}
