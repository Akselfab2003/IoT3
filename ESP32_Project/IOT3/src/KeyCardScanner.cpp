#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <KeyCardScanner.h>
#include "DataTransporter.h"

// Define the pins for the RC522 RFID reader
#define SS_PIN  5   // SDA/SS pin on the RC522
#define RST_PIN 27  // RST pin on the RC522

// Create an instance of the MFRC522 RFID reader
MFRC522 rfid(SS_PIN, RST_PIN);

/**
 * Initializes the RFID reader.
 * Sets up SPI communication and initializes the RC522 module.
 */
void InitKeycard() {
  // Initialize SPI communication with custom pins
  SPI.begin(18, 19, 23, 5); // SCK, MISO, MOSI, SS
  rfid.PCD_Init(); // Initialize the RC522 RFID reader

  // Print the software version of the RFID reader
  Serial.print("MFRC522 Software Version: ");
  rfid.PCD_DumpVersionToSerial();
  Serial.println("RC522 RFID reader initialized.");
}

/**
 * Continuously checks for new RFID cards.
 * If a card is detected, its UID is read and published via MQTT.
 */
void KeycardDetection() {
  // Check if a new card is present
  if (!rfid.PICC_IsNewCardPresent()) {
    // No new card detected
    return;
  }

  // Attempt to read the card's UID
  if (!rfid.PICC_ReadCardSerial()) {
    Serial.println("Card present, but unable to read UID");
    return;
  }

  // Convert the UID to a string
  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidStr += (rfid.uid.uidByte[i] < 0x10 ? "0" : ""); // Add leading zero if necessary
    uidStr += String(rfid.uid.uidByte[i], HEX); // Convert byte to hexadecimal
  }
  Serial.println("Card UID: " + uidStr);

  // Publish the UID over MQTT using the DataTransporter functionality
  PublishData(Topics::KeyCardDetected, uidStr.c_str());

  // Halt communication with the card and stop encryption
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}