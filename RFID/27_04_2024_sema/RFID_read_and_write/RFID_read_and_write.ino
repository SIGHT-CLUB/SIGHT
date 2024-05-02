#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);
byte block = 13;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Place the card to the reader...");
}

void loop() {
  // Check for card presence
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println("Card detected!");
    Serial.println("Type 'W' to write or 'R' to read:");

    if (waitForInput(5000)) {  // Wait for 5 seconds for input
      char input = Serial.read();  // Read the choice of operation
      clearSerialBuffer();

      if (input == 'W' || input == 'w') {
        Serial.println("Enter two numbers separated by space (e.g., 5 9):");
        if (waitForInput(5000)) {  // Wait for 5 seconds for number input
          String line = Serial.readStringUntil('\n');
          int x = line.toInt();  // Extract the first integer
          int y = line.substring(line.indexOf(' ') + 1).toInt();  // Extract the second integer

          writeToCard(x, y);
        } else {
          Serial.println("Timeout reached, no numbers entered.");
        }
      } else if (input == 'R' || input == 'r') {
        readFromCard();
      } else {
        Serial.println("Invalid input. Type 'W' to write or 'R' to read.");
      }
    } else {
      Serial.println("Timeout reached, no operation selected.");
    }

    Serial.println("Place the card to the reader...");
    // rfid.PICC_HaltA(); // Halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption on PCD
  }
}

void writeToCard(int x, int y) {
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte dataBlock[] = {
    (byte)x, (byte)y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  if (rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &rfid.uid) != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    return;
  }

  if (rfid.MIFARE_Write(block, dataBlock, 16) != MFRC522::STATUS_OK) {
    Serial.println("Write failed");
  } else {
    Serial.println("Write successful");
  }
}

void readFromCard() {
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
