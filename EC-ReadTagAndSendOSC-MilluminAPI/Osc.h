#include <Arduino.h>
#include <NCNS-ArduinoTools.h>

#define CTRL_LED 13

// Wifi Dependencies ----------------------------
#include <WiFiLink.h>
#include <WiFiUdp.h>

// OSC Dependencies -----------------------------
#include <OSCMessage.h>

// -------------------------------------------------
// Wifi Section
// -------------------------------------------------
char ssid[] = "InspirationHub";          // your network SSID (name)
char pass[] = "Inspiration";                    // your network password

WiFiUDP Udp;
const unsigned int localPort = 2390;        // local port to listen for UDP packets (here's where we send the packets)

// -------------------------------------------------
// OSC Section
// -------------------------------------------------
//destination IP
IPAddress outIp(192, 168, 2, 131);
const unsigned int outPort = 5000;

// -------------------------------------------------
// Specials Eurocave Section
// -------------------------------------------------
#define X_BASE_COL 2
#define S_BASE_COL 4
#define M_BASE_COL 6
#define L_BASE_COL 8

#define IN_BONUS  0
#define OUT_BONUS 1

void OscInit() {

  // WiFi Init --------------------------------
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // Blink Led
    errorBlink(CTRL_LED, 50);
  }
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(localPort);

}

void wifiCheck() {

  // Control the connection (led #0)
  if (WiFi.status() != WL_CONNECTED) {
    // not connected => Message + Blink Short
    Serial.println("Wifi Not Connected :(");
    errorBlink(CTRL_LED, 50);
  }

}

int getColumnIdx(char _tag){

  if(_tag == 'X'){
    return X_BASE_COL;
  }else if(_tag == 'S'){
    return S_BASE_COL;
  }else if(_tag == 'M'){
    return M_BASE_COL;
  }else if(_tag == 'L'){
    return L_BASE_COL;
  }else{
    return -1;
  }

}

void sendIt(String _address, int _intValue) {

  char _addr[255];
  _address.toCharArray(_addr, 255);
  
  Serial.print("Sending message Addr=");
  Serial.print(_address);
  /*
  Serial.print(" len=");
  Serial.print(_address.length());
  Serial.print(" Char Addr=");
  Serial.print(_addr);
  */
  Serial.print(" Value=");
  Serial.println(String(_intValue));
  
  OSCMessage msg(_addr);

  msg.add((uint8_t) _intValue);

  Udp.beginPacket(outIp, outPort);
  msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message

}

void sendIt(String _address, String _strValue) {

  char _addr[255];
  _address.toCharArray(_addr, 255);
  
  Serial.print("Sending message Addr=");
  Serial.print(_address);
  /*
  Serial.print(" len=");
  Serial.print(_address.length());
  Serial.print(" Char Addr=");
  Serial.print(_addr);
  */
  Serial.print(" Value=");
  Serial.println(_strValue);
  
  OSCMessage msg(_addr);

  msg.add(_strValue);

  Udp.beginPacket(outIp, outPort);
  msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message

}

// ------------------------------------------------------------
// Sending TAG
// To send a tag :
// 1st step : get the base column index
// 2st step : set a bonus
//    - bonus to In animation = 0
//    - bonus to Out animation = 1
// 3rd step : get a second bonus value determined by the decorum style you want
// ------------------------------------------------------------

void sendTag(char _tag, int _bonus) {
  
  int columnIdx = getColumnIdx(_tag);
  
  if(columnIdx >= 1){
    // Idx is right, send it !
    sendIt("/millumin/action/launchColumn", _bonus + columnIdx);
  }else{
    // not right, print a tip
    Serial.print("Column Idx is out of range, probably wrong tag (");
    Serial.print(_tag);
    Serial.print(",");
    Serial.print(")");
    Serial.println();
  }
  
}

