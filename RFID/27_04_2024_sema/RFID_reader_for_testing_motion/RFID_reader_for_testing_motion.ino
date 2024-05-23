#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 3
#define SS_PIN 10
#define TALK_PIN 8


MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  pinMode(ISRESET_PIN, OUTPUT);
  pinMode(TALK_PIN, OUTPUT);
  digitalWrite(TALK_PIN, LOW);
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  //Serial.println("Place the card to the reader...");
}

uint8_t coded_coordinates = 0;  // Variable to store the mapped coordinate code

void loop() {
  rfid_read();
  delay(100);
}

void rfid_read(){

    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    //Serial.println("Card detected!");
    coded_coordinates = readFromCard();  // Get the coordinate code
    //Serial.print("Mapped Coordinate Code: ");
    //Serial.println(coordinateCode);

    digitalWrite(TALK_PIN, HIGH);
    Serial.write(coded_coordinates);
    delay(1000);
    digitalWrite(TALK_PIN, LOW);

  }

  //Serial.println("Place the card to the reader...");
  rfid.PICC_HaltA(); // Halt PICC
  rfid.PCD_StopCrypto1(); // Stop encryption on PCD
}

uint8_t readFromCard() {
  byte block = 14;
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
    uint8_t x = buffer[0];
    uint8_t y = buffer[1];
    uint8_t coded_coordinates = x*16 + y;
    //Serial.print("Read Coordinates: (");
    //Serial.print(x);
    //Serial.print(", ");
    //Serial.print(y);
    //Serial.println(")");

    return coded_coordinates;  // Return 0 if coordinates don't match any predefined pairs
  }
}

void clearSerialBuffer() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}
