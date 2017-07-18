
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);

// Reading of the tag
#define unknwonTag "?"
#define noRecordFound "n"
#define idxRecordNotFound "i"
String lastTag = unknwonTag;

int lastMode = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("NDEF Writer");
  nfc.begin();
}

void loop() {

  Serial.println("Place your tag. Waiting for reading");
  Serial.println("1 Second wait.");
  delay(1000);

  if (nfc.tagPresent()) {

    // Record 0 : is the name
    // Record 1 : is timeStamp (millis :/)
    // Record 2 : is the size expected
    int newMode = readSizeAsIntMode();
    
    Serial.print("New Tag : [");
    Serial.print(String(newMode));
    Serial.println("]");

    if (newMode != lastMode) {
      Serial.println("Brand New !!!!!!!!!!!!");
      lastMode = newMode;
    }

  }

  Serial.print("Old Tag is : [");
  Serial.print(String(lastMode));
  Serial.println("] !!!!");

}

// ------------------------------------------------------------
// Reads the tag and return the size of
// ------------------------------------------------------------
String readUID() {

  NfcTag tag = nfc.read();
  return tag.getUidString();

}

String readRecord(int idxRecord) {

  NfcTag tag = nfc.read();

  if (tag.hasNdefMessage()) // every tag won't have a message
  {

    NdefMessage message = tag.getNdefMessage();

    int recordCount = message.getRecordCount();

    // Trapping unexpected behaviors
    if (recordCount == 0) {
      Serial.print("No Record found.");
      return noRecordFound;
    }
    if (idxRecord >= recordCount) {
      Serial.print("Idx record not found.");
      return idxRecordNotFound;
    }

    // So idx seems good, let's get it !
    NdefRecord record = message.getRecord(idxRecord);

    int payloadLength = record.getPayloadLength();
    byte payload[payloadLength];
    record.getPayload(payload);

    // Force the data into a String (might work depending on the content)
    // Real code should use smarter processing
    String payloadAsString = "";
    for (int c = 0; c < payloadLength; c++) {
      payloadAsString += (char)payload[c];
    }

    Serial.print("NDEF Record : "); Serial.print(idxRecord);
    Serial.print(" : Payload (as String): ");
    Serial.print(payloadAsString);
    Serial.println();

    // Second Record : Name of the tag
    return payloadAsString;

  }
}

int readSizeAsIntMode() {

  NfcTag tag = nfc.read();

  if (tag.hasNdefMessage()) // every tag won't have a message
  {

    NdefMessage message = tag.getNdefMessage();

    int recordCount = message.getRecordCount();

    // Trapping unexpected behaviors
    if (recordCount == 0) {
      Serial.print("No Record found.");
      return noRecordFound;
    }
    
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

    Serial.print("NDEF Record : ");
    Serial.print(" : Payload (as String):");
    Serial.print(payloadAsString);
    Serial.println();

    // Second Record : Name of the tag
    int mode = 0;
    
    if(payloadAsString.startsWith("X")){
      mode = 1;
    }else if(payloadAsString.startsWith("S")){
      mode = 2;
    }else if(payloadAsString.startsWith("M")){
      mode = 3;
    }else if(payloadAsString.startsWith("L")){
      mode = 4;
    }

    return mode;

  }
}
