#define BLACK_THRESHOLD 950  //if analogRead() value is greater than this, black is detected

#define MOTOR1_PWM 3
#define MOTOR1_A 4
#define MOTOR1_B 5
#define MOTOR2_PWM 11
#define MOTOR2_A 10
#define MOTOR2_B 9

uint8_t line_follower_analog_pins[8] = { A0, A1, A2, A3, A4, A5, A6, A7 };  // A0-> 1 (left) A7->8 (right)Define the analog input pins

void setup() {
  // Pins D3 and D11 - 8 kHz
  TCCR2B = 0b00000010;  // x8
  TCCR2A = 0b00000011;  // fast pwm

  delay(2500);
  Serial.begin(9600);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(line_follower_analog_pins[i], INPUT);
  }

  pinMode(MOTOR1_PWM, OUTPUT);
  pinMode(MOTOR1_A, OUTPUT);
  pinMode(MOTOR1_B, OUTPUT);

  pinMode(MOTOR2_PWM, OUTPUT);
  pinMode(MOTOR2_A, OUTPUT);
  pinMode(MOTOR2_B, OUTPUT);

  digitalWrite(MOTOR1_A, HIGH);
  digitalWrite(MOTOR1_B, LOW);
  analogWrite(MOTOR1_PWM, 255);

  digitalWrite(MOTOR2_A, LOW);
  digitalWrite(MOTOR2_B, HIGH);
  analogWrite(MOTOR2_PWM, 255);
}

void loop() {
  update_black_detections();
  test_print_is_black_array();
  //delay(2500);
  analogWrite(MOTOR1_PWM, 0);
  analogWrite(MOTOR2_PWM, 0);
}

uint8_t is_black[8] = { 0, 0, 0, 0, 0, 0 };  // if i'th sensor is black, set to 1. otherwise 0
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




