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
