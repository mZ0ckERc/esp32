/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"

// Replace with your network credentials
const char* ssid = "ESP32";
const char* password = "GeWer5%Ad)/";
const int hidden = 1;

#define IDENTITY "mail"
#define USERNAME "mail"
#define PASSWORD "password"
int counter = 0;

bool ledState26 = 0;
const int ledPin26 = 26;
bool ledState27 = 0;
const int ledPin27 = 27;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

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
    Serial.println(" isn´t a vaild parameter for the WiFi function. Setting up a WAP.");
    WiFiStuff(1);
  }
}

void notifyClients(String t) {
  ws.textAll(t);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle26") == 0) {
      ledState26 = !ledState26;
      if(ledState26)
        notifyClients(String("{\"l\":\"26\", \"s\": \"ON\"}"));
      else
        notifyClients(String("{\"l\":\"26\", \"s\": \"OFF\"}"));
    }
    if (strcmp((char*)data, "toggle27") == 0) {
      ledState27 = !ledState27;
      if(ledState27)
        notifyClients(String("{\"l\":\"27\", \"s\": \"ON\"}"));
      else
        notifyClients(String("{\"l\":\"27\", \"s\": \"OFF\"}"));
      
    }

  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE26"){
    if (ledState26){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  if(var == "STATE27"){
    if (ledState27){
      return "ON";
    }
    else{
      return "OFF";
    }
  }  
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

   if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
   } else {
    Serial.println("SPIFFS was mounted succesfully");
  }
 
  pinMode(ledPin26, OUTPUT);
  digitalWrite(ledPin26, LOW);
  pinMode(ledPin27, OUTPUT);
  digitalWrite(ledPin27, LOW);
    
  // Connect to Wi-Fi
  WiFiStuff(1);
  
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/html", "/index_html", processor);
    request->send(SPIFFS, "/index.html", "text/html", false, processor);
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin26, ledState26);
  digitalWrite(ledPin27, ledState27);
}
