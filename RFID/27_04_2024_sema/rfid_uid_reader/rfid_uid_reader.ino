#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);

uint8_t module_buffer[64];  // Adjust the size as needed
uint8_t lastUID[64];        // Variable to store the last UID
unsigned long last_time_read = 0; // Variable to store the last time when a new reading was obtained
bool readRFIDCardFlag = true;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println(F("Ready to read RFID card. Set 'readRFIDCardFlag' to true to initiate reading..."));
}

void loop() {
  if (readRFIDCardFlag) {
    readRFIDCard();
  }
}

void readRFIDCard() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    //Serial.println(F("Error reading card."));
    return;
  }

  saveUIDToBuffer(mfrc522.uid.uidByte, mfrc522.uid.size);

  // Check if the current UID is different from the last UID
  if (!compareUID(module_buffer, mfrc522.uid.size)) {
    // Print the new UID
    Serial.print(F("New UID: "));
    for (uint8_t i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(module_buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Update the last UID
    saveLastUID(module_buffer, mfrc522.uid.size);

    // Update the last time when a new reading was obtained
    last_time_read = millis();
    Serial.print(F("Last Time Read: "));
    Serial.println(last_time_read / 1000.0); // Convert to seconds
  }

}

// This function saves the UID of the RFID card/tag
void saveUIDToBuffer(uint8_t* uidBytes, uint8_t uidSize) {
  if (uidSize <= sizeof(module_buffer)) {
    memcpy(module_buffer, uidBytes, uidSize);
  } else {
    Serial.println(F("UID buffer size is not sufficient!"));
  }
}

// This function compares the current UID with the last UID
bool compareUID(uint8_t* currentUID, uint8_t uidSize) {
  return (memcmp(currentUID, lastUID, uidSize) == 0);
}

// This function saves the current UID as the last UID
void saveLastUID(uint8_t* currentUID, uint8_t uidSize) {
  if (uidSize <= sizeof(lastUID)) {
    memcpy(lastUID, currentUID, uidSize);
  } else {
    Serial.println(F("Last UID buffer size is not sufficient!"));
  }
}

// This function returns the last timestamp when a new reading was obtained
unsigned long get_timestamp() {
  return last_time_read;
}
