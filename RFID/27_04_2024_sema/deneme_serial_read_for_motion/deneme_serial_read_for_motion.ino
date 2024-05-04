void setup() {
    Serial.begin(9600); // Initialize serial communication
}

void loop() {
    if (Serial.available() > 0) { // Check if data is available to read
        uint8_t receivedNum = Serial.read(); // Read the incoming byte
        Serial.print("Received: ");
        Serial.println(receivedNum); // Print the received character
        uint8_t x = int(receivedNum) / 16;
        uint8_t y = int(receivedNum) % 16;
        Serial.println(x);
        Serial.println(y);
    }
}