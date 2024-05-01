#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Place card to write coordinates.");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; // default key for authentication

  // Block to write
  byte block = 13;
  byte buffer[18];
  byte size = sizeof(buffer);
  // Prepare the data in buffer (16 bytes per block)
  byte dataBlock[] = {
    1, 1, 5, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
  // Authenticate using key A
  if (rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &rfid.uid) != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    return;
  }
  // Write the block
  if (rfid.MIFARE_Write(block, dataBlock, 16) != MFRC522::STATUS_OK) {
    Serial.println("Write failed");
  } else {
    Serial.println("Write successful");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}


