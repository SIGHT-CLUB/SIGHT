#define BLACK_THRESHOLD 950 //if analogRead() value is greater than this, black is detected
uint8_t line_follower_analog_pins[8] = { A0, A1, A2, A3, A4, A5, A6, A7 };  // A0-> 1 (left) A7->8 (right)Define the analog input pins

void setup() {
  Serial.begin(9600);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(line_follower_analog_pins[i], INPUT);
  }
}

void loop() {
  update_black_detections();
  test_print_is_black_array();

}

uint8_t is_black[8] = { 0, 0, 0, 0, 0, 0 }; // if i'th sensor is black, set to 1. otherwise 0
uint8_t update_black_detections() {
  for (uint8_t i = 0; i < 8; i++) {
    int analog_value = analogRead(line_follower_analog_pins[i]);
    uint8_t digital_val = 0;
    if (analog_value > BLACK_THRESHOLD) digital_val = 1;
    is_black[i] = digital_val;
  }
}

void test_print_is_black_array() {
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(String(is_black[i]) + " ");
  }
  Serial.println("");
}

