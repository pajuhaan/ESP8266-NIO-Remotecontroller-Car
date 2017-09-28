# ESP8266-NIO-Remotecontroller-Car

With this sample you can connect and control two motors by ESP8266 (NodeMCU - Arduino) via Wi-Fi to Android devices. 

Android application can created by NIO as simple applets.

Get NIO from this link: http://niomatic.com


### Schematic:
![alt text](https://github.com/pajuhaan/ESP8266-NIO-Remotecontroller-Car/blob/master/Schematic/wifi-nio-nodemcu-esp8266-RCcar.jpg?raw=true)



Parse Json:
```javascript
JsonObject &root = jsonBuffer.parseObject(payload);
int led  = root["led"];
int m1   = root["m1"];
int m2   = root["m2"];
```
