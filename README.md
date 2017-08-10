# EC-Inspiration-NFCReader
Arduino sketch

## Hardware used
Arduino Uno Wifi Dev Edition - http://www.arduino.org/products/boards/arduino-uno-wifi

NFC shield V2 from seedstudio - http://wiki.seeed.cc/NFC_Shield_V2.0/

## Requirements and dependecies :
* Arduino IDE 1.8.x
* Library WifiLink
* Library ArdOsc

To use WifiLink with The arduino Dev Edition :
* do the firmware update, and follow the threads below
  * Regular Arduino threads : http://www.arduino.org/learning/tutorials/advanced-guides/how-to-change-the-firmware-on-an-arduino-uno-wifi
  * But, finally this one tutorial was the most useful : https://projetsdiy.fr/esptool-update-firmware-nodemcu-esp8266-osx/#.WXHLPYppzdQ

## OSC Commands
You can find the OSC Api for millumin here
https://github.com/anome/millumin-dev-kit/wiki/OSC-documentation

We directly send Millumin embedded commands, which are

/millumin/action/launchColumn [index or "name"] 
/millumin/myElementName/... to target the element named "myElementName"
/millumin/layer:myLayerName/... to target the layer named "myLayerName"
