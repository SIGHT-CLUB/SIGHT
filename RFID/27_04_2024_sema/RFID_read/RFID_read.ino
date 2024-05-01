#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Place card to read coordinates.");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  byte block = 13;
  byte buffer[18];
  byte size = sizeof(buffer);

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  if (rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &rfid.uid) != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    return;
  }
  // Read the block
  if (rfid.MIFARE_Read(block, buffer, &size) != MFRC522::STATUS_OK) {
    Serial.println("Read failed");
  } else {
    Serial.print("Coordinates: (");
    Serial.print(buffer[0]);
    Serial.print(",");
    Serial.print(buffer[1]);
    Serial.print(") to (");
    Serial.print(buffer[2]);
    Serial.print(",");
    Serial.print(buffer[3]);
    Serial.println(")");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
