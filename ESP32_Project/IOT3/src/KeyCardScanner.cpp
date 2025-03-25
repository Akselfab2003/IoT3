#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <KeyCardScanner.h>

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

  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print("PICC Type: ");
  Serial.println(rfid.PICC_GetTypeName(piccType));

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
