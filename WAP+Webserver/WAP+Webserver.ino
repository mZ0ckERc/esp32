
// Load Wi-Fi library

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"

// Replace with your network credentials
const char* ssid     = "ESP32";
const char* password = "GeWer5%Ad)/";

//If connecting to a business network
#define IDENTITY "mail"
#define USERNAME "mail"
#define PASSWORD "password"
int counter = 0;


const int hidden = 1;

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


void WiFiStuff(int wtd) {
  if (wtd == 1) {
    WiFi.softAP(ssid, password, 3, hidden);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP Address: ");
    Serial.println(IP);
    }
  else if (wtd == 2) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }
  else if (wtd == 3) {
    Serial.print("Connecting to network: ");
    Serial.println(ssid);
    WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
    WiFi.mode(WIFI_STA); //init wifi mode
  
    // Example (most common): a cert-file-free eduroam with PEAP (or TTLS)
    WiFi.begin(ssid, WPA2_AUTH_PEAP, IDENTITY, USERNAME, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if(counter>=60){ //after 30 seconds timeout - reset board
        ESP.restart();
      }
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address set: "); 
    Serial.println(WiFi.localIP()); //print LAN IP
  }
  else {
    Serial.print(wtd);
    Serial.println(" isn´t a vaild parameter for the WiFi function. Setting up a WAP");
    WiFiStuff(1);
  }
}

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
  //delay(15000);
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
  else {
    Serial.println("SPIFFS was mounted succesfully");
  }

  // Connect to Wi-Fi network with SSID and password
  WiFiStuff(4);

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
