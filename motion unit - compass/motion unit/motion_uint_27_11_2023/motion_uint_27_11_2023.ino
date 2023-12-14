#define right_offset 0
#define max_pwm 70
#define motion_duration 3500
void setup() {
  pinMode(8, OUTPUT);   //IN2
  pinMode(9, OUTPUT);   //IN1
  pinMode(10, OUTPUT);  //Enable Pin (IN1 & IN2)
  pinMode(6, OUTPUT);   // IN3
  pinMode(7, OUTPUT);   //IN4
  pinMode(11, OUTPUT);  // Enable Pin (IN3 & IN4)
  delay(3000);
}
void loop() {

  //forward motion
  for (int i = 25; i < max_pwm; i++) {
    right_forward(i + right_offset);
    left_forward(i);
    delay(40);
  }
  delay(motion_duration);
  for (int i = max_pwm; i > 0; i--) {
    right_forward(i);
    left_forward(i);
    delay(10);
  }

  delay(2500);

  // Backward motion
  for (int i = 25; i < max_pwm; i++) {
    right_backward(i + right_offset);
    left_backward(i);
    delay(40);
  }
  delay(motion_duration);
  for (int i = max_pwm; i > 0; i--) {
    right_backward(i);
    left_backward(i);
    delay(10);
  }

  delay(2500);

  // rotate around itself
  for (int i = 25; i < max_pwm; i++) {
    right_forward(i + right_offset);
    left_backward(i);
    delay(30);
  }
  for (int i = max_pwm; i > 0; i--) {
    right_forward(i);
    left_backward(i);
    delay(30);
  }
  delay(5000);
}


void right_backward(uint8_t pwm_val) {
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  analogWrite(10, pwm_val);
}


void right_forward(uint8_t pwm_val) {
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  analogWrite(10, pwm_val);
}

void left_forward(uint8_t pwm_val) {
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  analogWrite(11, pwm_val);
}

void left_backward(uint8_t pwm_val) {
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  analogWrite(11, pwm_val);
}
