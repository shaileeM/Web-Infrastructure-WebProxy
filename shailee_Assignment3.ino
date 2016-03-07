#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

const char* ssid = "SCU-Guest";
const char* password = "";

const char* newssid = "madhani";
const char* newpassword = "";

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Enter the url(without the 'http://' part)after 192.168.4.1/");
}

void handleNotFound(){

  const char* host;
  WiFiClient client;

  Serial.print("Requesting uri: ");
  String requestString = server.uri();
  Serial.println(requestString);
  
  requestString.remove(0,1);
  int i = requestString.indexOf('/');
  String domain_name = requestString.substring(0,i);
  host = domain_name.c_str();
  requestString.remove(0,i);

  Serial.print("Host: ");
  Serial.println(host);
  Serial.println(requestString);

  while (!client.connect(host, 80)) {
    Serial.println("connection failed, retrying...");
  }
  
  client.print(String("GET ") + requestString);
  
  client.print(String(" HTTP/1.1\r\n") +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
               
  while(!client.available()){
    yield();
  }

  String response;
  while(client.available()){
    response = client.readStringUntil('\r');
    response.replace("HTTP", "WHAT-IS-THAT");
    response.replace("http", "WHAT-IS-THAT");
  }
  server.send(200, "text/html", response);

  client.stop();
}

void setup(void){
  Serial.begin(115200);
  for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
  
  WiFi.softAP(newssid, newpassword);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  WiFiMulti.addAP(ssid, password);
}

void loop(void){
  server.handleClient();
}
