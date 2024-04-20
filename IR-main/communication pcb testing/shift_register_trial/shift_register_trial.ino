#include "IR_module_header.h"

#define RECEIVE_PIN 7
#define SHIFT_REG_INPUT 8
#define SHIFT_REG_CLK_PIN 9
#define TRANSMIT_PIN 10

void setup() {
  pinMode(RECEIVE_PIN, INPUT);
  pinMode(SHIFT_REG_INPUT, OUTPUT);
  pinMode(SHIFT_REG_CLK_PIN, OUTPUT);
  pinMode(TRANSMIT_PIN, OUTPUT);
  digitalWrite(SHIFT_REG_CLK_PIN, HIGH);

  digitalWrite(TRANSMIT_PIN, HIGH);
  digitalWrite(SHIFT_REG_INPUT, LOW);

  Serial.begin(9600);
}

void loop() {
  set_active_s(3);
  uint8_t result = listen_IR();
  if(result != 0){
    Serial.println(result);
  }
  
}

void set_active_s(uint8_t pick_this_s) {
  
  int led_to_i_mapping[9] = { 4, 5, 6, 7, 0, 1, 2, 3, 999 };  // do not alter, physically linked
  digitalWrite(SHIFT_REG_INPUT, LOW);
  for (uint8_t i = 0; i < 8; i++) {
    if (i == led_to_i_mapping[pick_this_s]) {
      digitalWrite(SHIFT_REG_INPUT, HIGH);
    } else {
      digitalWrite(SHIFT_REG_INPUT, LOW);
    }
    delayMicroseconds(5);

    digitalWrite(SHIFT_REG_CLK_PIN, LOW);
    delayMicroseconds(25);
    digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
    delayMicroseconds(25);
  }
}
void rotate_led_test() {

  static int pick_this_led = 0;  //
  pick_this_led = (pick_this_led + 1) % 8;
  int led_to_i_mapping[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };

  Serial.println("pick_this= " + String(pick_this_led));
  digitalWrite(SHIFT_REG_INPUT, LOW);
  for (uint8_t i = 0; i < 8; i++) {

    if (i == led_to_i_mapping[pick_this_led]) {
      digitalWrite(SHIFT_REG_INPUT, HIGH);
    } else {
      digitalWrite(SHIFT_REG_INPUT, LOW);
    }
    delay(1);

    digitalWrite(SHIFT_REG_CLK_PIN, LOW);
    delay(10);
    digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
    delay(10);
  }
  delay(250);
}
