/*
   WebSocketServer_LEDcontrol.ino

    Created on: 26.11.2015

*/
#include <stdio.h>

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#include <ArduinoJson.h>

#define PIN_LED     15   //D8
#define PIN_WATER    5      //D1



#define USE_SERIAL Serial


ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

#define degreesToRadians(angleDegrees) (angleDegrees * 3.14 / 180.0)


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload); USE_SERIAL.flush();

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

      StaticJsonBuffer<200> jsonBuffer;

      JsonObject& root = jsonBuffer.parseObject(payload);

      //const char* sensor = root["sensor"];




      //........... Gyroscop ...........
      bool led_mod          = root["led"];
      bool water_mode    = root["water"];

      if (led_mod) {
        digitalWrite(PIN_LED, 1);

      } else {
        digitalWrite(PIN_LED, 0);
      }

      if (water_mode) {
        digitalWrite(PIN_WATER, 1);

      } else {
        digitalWrite(PIN_WATER, 0);
      }






      break;
  }

}

void setup() {
  //USE_SERIAL.begin(921600);
  USE_SERIAL.begin(115200);



  //USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();




  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());










  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(100);
  }

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_WATER, OUTPUT);


  digitalWrite(PIN_LED, 0);
  digitalWrite(PIN_WATER, 0);




  WiFiMulti.addAP("HODAK", "1442514425");

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  if (MDNS.begin("esp8266")) {
    USE_SERIAL.println("MDNS responder started");
  }

  // handle index
  server.on("/", []() {
    // send index.html
    server.send(200, "text/html", "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/></body></html>");
  });

  server.begin();

  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);

  digitalWrite(PIN_LED, 0);
  digitalWrite(PIN_WATER, 0);


  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  USE_SERIAL.flush();
}

void loop() {
  webSocket.loop();
  server.handleClient();
}

