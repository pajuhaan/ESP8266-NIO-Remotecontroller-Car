# ESP8266-NIO-Remotecontroller-Car

With this sample you can connect and control two motors by ESP8266 (NodeMCU - Arduino) via Wi-Fi to Android devices. 

Android application can created by NIO as simple applets.

Get NIO from this link: http://niomatic.com


### Schematic:
![alt text](https://github.com/pajuhaan/ESP8266-NIO-Remotecontroller-Car/blob/master/Schematic/wifi-nio-nodemcu-esp8266-RCcar.jpg?raw=true)

Implement web socket server in an ESP8266. Command send from NIO to ESP in the JSON structure via Websocket.

Parse Json:
```javascript
JsonObject &root = jsonBuffer.parseObject(payload);
int led  = root["led"];
int m1   = root["m1"];
int m2   = root["m2"];
```

### NIO Back Panel:
![alt text](https://github.com/pajuhaan/ESP8266-NIO-Remotecontroller-Car/blob/master/Schematic/BackPanel.png?raw=true)

### NIO File:
Download RC car with gyro.nio and open in your tablet.

<p align="center">
  <img src="https://github.com/pajuhaan/ESP8266-NIO-Remotecontroller-Car/blob/master/images/car1.jpg?raw=true" width="350"/>
  <img src="https://github.com/pajuhaan/ESP8266-NIO-Remotecontroller-Car/blob/master/images/car2.jpg?raw=true" width="350"/>
</p>