#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  5
#define RST_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();            // Init SPI bus
  mfrc522.PCD_Init();     // Init RC522
  Serial.println("Scan a card...");
}

void loop() {
  // Look for new card
  if (!mfrc522.PICC_IsNewCardPresent()) return;

  // Select the card
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("Card UID: ");
  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
    if (i != mfrc522.uid.size - 1) uidStr += ":";
  }
  uidStr.toUpperCase();
  Serial.println(uidStr);

  mfrc522.PICC_HaltA();      // Stop reading
  mfrc522.PCD_StopCrypto1(); // Stop encryption
}