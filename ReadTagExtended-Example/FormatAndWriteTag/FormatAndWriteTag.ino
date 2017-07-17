
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);

void setup() {
  Serial.begin(9600);
  Serial.println("NDEF Writer");
  nfc.begin();
}

void loop() {

  bool success;

  Serial.println("Place your tag. It will be formatted as NDEF");
  if (nfc.tagPresent()) {

    // ---------------------------------------------
    // First clean it.
    success = nfc.clean();
    if (success) {
      Serial.println("Success, tag cleaned.");
    } else {
      Serial.println("Clean failed.");
    }
    // ---------------------------------------------

    // ---------------------------------------------
    // Then format.
    success = nfc.format();
    if (success) {
      Serial.println("Success, tag formatted as NDEF.");
    } else {
      Serial.println("Format failed.");
    }
    // ---------------------------------------------

    // ---------------------------------------------
    // Great, we can write ----
    NdefMessage message = NdefMessage();
    Serial.println("Waiting for an order : 1 (XS Tag), 2 (S Tag), 3 (M Tag) or 4 (XL Tag)");
    bool orderDone = false;
    String inputString = "";

    while (!orderDone) {


      // if there's any serial available, read it:
      while (Serial.available() > 0) {
        // look for the newline. That's the end of your
        // sentence:
        char inChar = Serial.read();
        inputString += inChar;
        if (inChar == '\n') {
          orderDone = true;
        }
      }

      if (orderDone == true) {

        String genericMessage = "Inspiration:Size=";

        Serial.print("Order is : ");
        Serial.print(inputString);
        Serial.println();

        if (inputString[0] == "1") {
          genericMessage.concat("X");
          message.addUriRecord("Totototototototo");
        } else if (inputString[0] == "2") {
          genericMessage.concat("S");
          message.addUriRecord(genericMessage);
        } else if (inputString[0] == "3") {
          genericMessage.concat("M");
          message.addUriRecord(genericMessage);
        } else if (inputString[0] == "4") {
          genericMessage.concat("L");
          message.addUriRecord(genericMessage);
        } else {
          //Serial.println("Waiting for an order : 1 (X as XS Tag), 2 (S Tag), 3 (M Tag) or 4 (L Tag)");
        }
      }
    }
    delay(2000);

    success = nfc.write(message);
    if (success) {
      Serial.println("Success, tag written.");
    } else {
      Serial.println("Write failed.");
    }
    // ---------------------------------------------

    // ---------------------------------------------
    // Then read it to confirm
    Serial.println("Reading confirmation.");

    NfcTag tag = nfc.read();
    Serial.print("UID: "); Serial.println(tag.getUidString());
    if (tag.hasNdefMessage()) // every tag won't have a message
    {
      NdefMessage message = tag.getNdefMessage();
      // cycle through the records, printing some info from each
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++)
      {
        Serial.print("NDEF Record : "); Serial.print(i + 1);
        NdefRecord record = message.getRecord(i);

        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);
        // Print the Hex and Printable Characters
        Serial.print("  Payload (HEX): ");
        PrintHexChar(payload, payloadLength);

        // Force the data into a String (might work depending on the content)
        // Real code should use smarter processing
        String payloadAsString = "";
        for (int c = 0; c < payloadLength; c++) {
          payloadAsString += (char)payload[c];
        }
        Serial.print("  Payload (as String): ");
        Serial.print(payloadAsString);

        // end
        Serial.println();
        delay(5000);
      }

    }
    // ---------------------------------------------

  }

  Serial.println("1 Second wait.");
  delay(1000);

}
