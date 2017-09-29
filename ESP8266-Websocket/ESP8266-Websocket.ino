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

#define MOTOR_1_A     15   //D8
#define MOTOR_2_A   12      //D6
#define LED_BLUE    13      /D7


#define MOTOR_1_B     16  //D0
#define MOTOR_2_B     4   //D2


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
        USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

      /*  if(payload[0] == 'p') {
            // we get RGB data

            //  analogWriteResolution(8);
             // analogWrite(11, map(sensorVal, 0, 1023, 0 ,255));

            // decode rgb data
            uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

            analogWrite(MOTOR_1_A,    ((rgb >> 16) & 0xFF)*4);
            analogWrite(MOTOR_2_A,  ((rgb >> 8) & 0xFF)*4);
            analogWrite(LED_BLUE,   ((rgb >> 0) & 0xFF)*4);
        }*/


      //   char json[] = "{\"led\":200,\"m1\":150,\"m2\":180}";
      //char json[] = "{\"sensor\":\"gps\",\"led\":200,\"motor\":[52,150]}";

      StaticJsonBuffer<200> jsonBuffer;

      JsonObject& root = jsonBuffer.parseObject(payload);

      //const char* sensor = root["sensor"];


      //........... JOY STICK ...........
    /*  int led          = root["led"];
      int amp    = root["amp"];
      int teta   = root["teta"];
      float teta_radian = degreesToRadians(teta);
      int delta = amp * cos(teta_radian);
      int m1 = delta;
      int m2 = amp;
      if (m1 > 1023)m1 = 1023;
      if (m2 > 1023)m2 = 1023;

      if (teta_radian > 0) {
        analogWrite(MOTOR_2_A,   abs( 0));
        analogWrite(MOTOR_2_B,  abs(m2));
      } else {
        analogWrite(MOTOR_2_A,   abs( m2));
        analogWrite(MOTOR_2_B,  abs(0));
      }
      if (delta > 0) {
        analogWrite(MOTOR_1_A,   abs( m1));
        analogWrite(MOTOR_1_B,  abs(0));

      } else {
        analogWrite(MOTOR_1_A,   abs( 0));
        analogWrite(MOTOR_1_B,  abs(m1));

      }
*/

      //........... Gyroscop ...........
      int led          = root["led"];
      int m1    = root["m1"];
      int m2   = root["m2"];

      if (m1 > 1023)m1 = 1023;
      if (m2 > 1023)m2 = 1023;
      if (m1 < -1023)m1 = -1023;
      if (m2 < -1023)m2 = -1023;
      if (m2 > 0) {
        analogWrite(MOTOR_2_A,   abs( 0));
        analogWrite(MOTOR_2_B,  abs(m2));
      } else {
        analogWrite(MOTOR_2_A,   abs( m2));
        analogWrite(MOTOR_2_B,  abs(0));
      }
      if (m1 > 0) {
        analogWrite(MOTOR_1_A,   abs( m1));
        analogWrite(MOTOR_1_B,  abs(0));

      } else {
        analogWrite(MOTOR_1_A,   abs( 0));
        analogWrite(MOTOR_1_B,  abs(m1));

      }




      USE_SERIAL.printf("Motor: m1=%i  m2=%i    \n", m1, m2);

      analogWrite(LED_BLUE,   abs(led));
      /*
        if(teta_radian>0){
        analogWrite(MOTOR_1_B,  abs(0));
                      analogWrite(MOTOR_2_B,  abs(0));

          analogWrite(MOTOR_1_A,   abs( m1));
                      analogWrite(MOTOR_2_A,  abs(m2));
        }else{
        analogWrite(MOTOR_1_A,   abs( 0));
                      analogWrite(MOTOR_2_A,  abs(0));

        analogWrite(MOTOR_1_B,  abs(m2));
                      analogWrite(MOTOR_2_B,  abs(m2));
        }
      */



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



  pinMode(MOTOR_1_A, OUTPUT);
  pinMode(MOTOR_2_A, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(MOTOR_1_B, OUTPUT);
  pinMode(MOTOR_2_B, OUTPUT);


  digitalWrite(MOTOR_1_A, 0);
  digitalWrite(MOTOR_2_A, 0);
  digitalWrite(LED_BLUE, 0);
  digitalWrite(MOTOR_1_B, 0);
  digitalWrite(MOTOR_2_B, 0);

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

  digitalWrite(MOTOR_1_A, 0);
  digitalWrite(MOTOR_2_A, 0);
  digitalWrite(LED_BLUE, 0);
  digitalWrite(MOTOR_1_B, 0);
  digitalWrite(MOTOR_2_B, 0);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  webSocket.loop();
  server.handleClient();
}

