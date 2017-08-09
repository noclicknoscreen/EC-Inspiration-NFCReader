// NFC Dependencies ----------------------------
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

// Wifi Dependencies ----------------------------
#include <WiFiLink.h>
#include <WiFiUdp.h>

// OSC Dependencies -----------------------------
#include <OSCMessage.h>

// -------------------------------------------------
// NFC Section
// -------------------------------------------------


PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
// Reading of the tag
#define unknwonTag '?'
char lastTag = unknwonTag;

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

#define CTRL_LED 13

void setup() {
  Serial.begin(115200);
  // NFC Init -------------------------------------
  Serial.println("NDEF Writer");
  nfc.begin();

  // WiFi Init --------------------------------
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  pinMode(CTRL_LED, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    errorBlink(50);
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(localPort);

}

void loop() {

  // Control the connection (led #0)
  if (WiFi.status() != WL_CONNECTED) {
    // not connected => Message + Blink Short
    Serial.println("Wifi Not Connected :(");
    errorBlink(50);
  } else {
    /*
      OSCMessage msgHeartBeat("/heartbeat");
      //the message wants an OSC address as first argument
      msgHeartBeat.add((long)millis());

      if (Udp.beginPacket(outIp, outPort)) {
      msgHeartBeat.send(Udp); // send the bytes to the SLIP stream
      } else {
      Serial.println("UDP did not begin");
      }
      Udp.endPacket(); // mark the end of the OSC Packet
      msgHeartBeat.empty(); // free space occupied by message
    */
  }

  if (nfc.tagPresent()) {

    // Record 0 : is the name
    // Record 1 : is timeStamp (millis :/)
    // Record 2 : is the size expected
    char newTag = readSizeAsChar();

    Serial.print("New Tag : [");
    Serial.print(String(newTag));
    Serial.println("]");

    if (newTag != lastTag) {

      Serial.println("Brand New !!!!!!!!!!!!");
      lastTag = newTag;

      OSCMessage msgLaunch("/tagUnknown");
      OSCMessage msgRelease("/tagUnknown");
      if (newTag == 'X') {
        msgLaunch.setAddress("/tagX");
        msgRelease.setAddress("/tagX");
      } else if (newTag == 'S') {
        msgLaunch.setAddress("/tagS");
        msgRelease.setAddress("/tagS");
      } else if (newTag == 'M') {
        msgLaunch.setAddress("/tagM");
        msgRelease.setAddress("/tagM");
      } else if (newTag == 'L') {
        msgLaunch.setAddress("/tagL");
        msgRelease.setAddress("/tagL");
      }
      msgLaunch.add((uint8_t)1);
      msgRelease.add((uint8_t)0);

      // Send the new message -------------------------------
      Serial.println("Ready to send : ");
      /*char readAddress[255];
        msgLaunch.getAddress(readAddress, 255);
        Serial.println(readAddress);
      */

      Udp.beginPacket(outIp, outPort);
      msgLaunch.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      msgLaunch.empty(); // free space occupied by message

      delay(50);
      Udp.beginPacket(outIp, outPort);
      msgRelease.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      msgRelease.empty(); // free space occupied by message

    }

  } else {

    Serial.println("Place your tag. Waiting for reading");
    //  Serial.println("1 Second wait.");

  }
  delay(50);
  /*
    Serial.print("Old Tag is : [");
    Serial.print(String(lastTag));
    Serial.println("] !!!!");
  */
}

void errorBlink(unsigned int delayMs) {
  digitalWrite(CTRL_LED, HIGH);
  delay(delayMs / 2);
  digitalWrite(CTRL_LED, LOW);
  delay(delayMs / 2);
}

// ------------------------------------------------------------
// Reads the tag and return the size of
// ------------------------------------------------------------
char readSizeAsChar() {

  NfcTag tag = nfc.read();

  if (tag.hasNdefMessage()) // every tag won't have a message
  {

    NdefMessage message = tag.getNdefMessage();

    int recordCount = message.getRecordCount();

    // Trapping unexpected behaviors
    if (recordCount == 0) {
      Serial.print("No Record found.");
      return 'n';
    }

    if (recordCount >= 2) {

      // So idx seems good, let's get it !
      NdefRecord record = message.getRecord(2);

      int payloadLength = record.getPayloadLength();
      byte payload[payloadLength];
      record.getPayload(payload);

      // Force the data into a String (might work depending on the content)
      // Real code should use smarter processing
      String payloadAsString = "";
      for (int c = 0; c < payloadLength; c++) {
        payloadAsString += (char)payload[c];
      }

      char result = payloadAsString.charAt(1);
      //byte byteResult = payloadAsString.getBytesAt(1);

      Serial.print("NDEF Record : ");
      Serial.print("Payload (as String):");
      Serial.print(payloadAsString);
      Serial.print(" Char 0 :");
      Serial.print(String(result));
      Serial.print(" Char 0 as Int:");
      Serial.print(String(result, HEX));
      Serial.println();

      return result;

    } else {
      return 'e';
    }

  }
}
