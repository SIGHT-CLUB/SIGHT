#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Place the card to the reader...");
}

uint8_t isCardDetected = 0;

void loop() {
  // Check for card presence
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    isCardDetected = 1;
    Serial.println("Card detected!");
    readFromCard();
    delay(3000);
    }
    isCardDetected = 0;
    delay(1000);

    Serial.println("Place the card to the reader...");
    rfid.PICC_HaltA(); // Halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption on PCD
  }


void readFromCard() {
  byte block = 2;
  byte buffer[18];
  byte size = sizeof(buffer);

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  if (rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &rfid.uid) != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    return;
  }

  if (rfid.MIFARE_Read(block, buffer, &size) != MFRC522::STATUS_OK) {
    Serial.println("Read failed");
  } else {
    Serial.print("Read Coordinates: (");
    Serial.print(buffer[0]);
    Serial.print(", ");
    Serial.print(buffer[1]);
    Serial.println(")");
  }
}

bool waitForInput(unsigned long timeout) {
  unsigned long startTime = millis();
  while (!Serial.available()) {
    if (millis() - startTime > timeout) {
      return false;  // Timeout reached
    }
    delay(10);
  }
  return true;
}

void clearSerialBuffer() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}
