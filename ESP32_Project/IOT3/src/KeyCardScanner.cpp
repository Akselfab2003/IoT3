#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <KeyCardScanner.h>
#include "DataTransporter.h"

#define SS_PIN  5   // SDA/SS on the RC522
#define RST_PIN 27  // RST pin on the RC522

MFRC522 rfid(SS_PIN, RST_PIN);

void setup2() {
  SPI.begin(18, 19, 23, 5);
  rfid.PCD_Init();

  Serial.print("MFRC522 Software Version: ");
  rfid.PCD_DumpVersionToSerial();
  Serial.println("RC522 RFID reader initialized.");
  Serial.println("Bring your card or fob closer to the reader...");

  Serial.println("Press 'w' to write data to the card.");
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'w') { // Press 'w' in the serial monitor to write to the card
      Serial.println("Writing data to card...");
      write_to_card();
    }
  }
 

}


void write_to_card() {
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF; // Default key A
  }

  byte block = 4; // Block to write to (ensure it's writable)
  byte data[16] = { 'H', 'E', 'L', 'L', 'O', '1', '2', '3', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Data to write

  // Check if a card is present
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    Serial.println("No card detected or unable to read card.");
    return;
  }

  // Authenticate the block
  if (!rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid))) {
    Serial.println("Authentication failed.");
    return;
  }

  // Write data to the block
  MFRC522::StatusCode status = rfid.MIFARE_Write(block, data, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Write failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
  } else {
    Serial.println("Data written successfully!");
  }

  // Halt the card and stop encryption
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void loop2() {
  if (!rfid.PICC_IsNewCardPresent()) {
    //Serial.println("No new card present");
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    Serial.println("Card present, but unable to read UID");
    return;
  }

  // Convert the UID to a string
  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidStr += (rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    uidStr += String(rfid.uid.uidByte[i], HEX);
  }
  Serial.println("Card UID: " + uidStr);

  // Publish the UID over MQTT (using your existing DataTransporter functionality)
  PublishData(Topics::KeyCardDetected, uidStr.c_str());

  // Halt communication with the card
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
