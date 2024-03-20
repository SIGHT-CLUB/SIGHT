#define SHIFT_REG_INPUT 8
#define SHIFT_REG_CLK_PIN 9

void setup() {
  pinMode(SHIFT_REG_INPUT, OUTPUT);
  pinMode(SHIFT_REG_CLK_PIN, OUTPUT);
  digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
}

uint8_t counter = 0;
void loop() {

  set_active_LED_as(counter%8);
  counter = counter +1;
  delay(5000);
}


void set_active_LED_as(uint8_t LED_index) {
  LED_index = 7-LED_index; //this is line is not important. Just to ease the debuggin (order of leds)
  if (LED_index < 0 || LED_index > 7) {
    LED_index = 0;
  }

  for (uint8_t i = 0; i < 8; i++) {
    if (i == LED_index) {
      digitalWrite(SHIFT_REG_INPUT, HIGH);
    }else{
      digitalWrite(SHIFT_REG_INPUT, LOW);
    }
    toggle_clk();
  }
}

void toggle_clk() {
  digitalWrite(SHIFT_REG_CLK_PIN, LOW);
  delayMicroseconds(5000);
  digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
  delayMicroseconds(5000);
}
