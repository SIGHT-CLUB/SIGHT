#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  //Serial.println("Place the card to the reader...");
}

uint8_t isCardDetected = 0;
uint8_t coordinateCode = 0;  // Variable to store the mapped coordinate code

void loop() {
  rfid_read();
  delay(100);
  isCardDetected = 0;
}

void rfid_read(){

    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    isCardDetected = 1;
    //Serial.println("Card detected!");
    coordinateCode = readFromCard();  // Get the coordinate code
    //Serial.print("Mapped Coordinate Code: ");
    //Serial.println(coordinateCode);
    Serial.print(coordinateCode);
  }
  //Serial.println("Place the card to the reader...");
  // rfid.PICC_HaltA(); // Halt PICC
  rfid.PCD_StopCrypto1(); // Stop encryption on PCD
}

uint8_t readFromCard() {
  byte block = 2;
  byte buffer[18];
  byte size = sizeof(buffer);
  MFRC522::MIFARE_Key key;

  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  if (rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &rfid.uid) != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    return 0;
  }

  if (rfid.MIFARE_Read(block, buffer, &size) != MFRC522::STATUS_OK) {
    Serial.println("Read failed");
    return 0;
  } else {
    int x = buffer[0];
    int y = buffer[1];
    //Serial.print("Read Coordinates: (");
    //Serial.print(x);
    //Serial.print(", ");
    //Serial.print(y);
    //Serial.println(")");

    // Mapping coordinates to specific codes
    if (x == 1 && y == 1) return 1;
    if (x == 1 && y == 2) return 2;
    if (x == 2 && y == 1) return 3;
    if (x == 2 && y == 2) return 4;

    return 0;  // Return 0 if coordinates don't match any predefined pairs
  }
}

void clearSerialBuffer() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}
