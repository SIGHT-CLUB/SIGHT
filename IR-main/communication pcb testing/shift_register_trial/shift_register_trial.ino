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

  Serial.begin(9600);
}

void loop() {
  set_active_output_as(1);
  digitalWrite(TRANSMIT_PIN, HIGH);
  delay(1000);
}

void set_active_output_as(uint8_t output_index) {
  digitalWrite(TRANSMIT_PIN, LOW);
  output_index = 7 - output_index;  //this is line is not important. Just to ease the debuggin (order of leds)
  if (output_index < 0 || output_index > 7) {
    output_index = 0;
  }

  for (uint8_t i = 0; i < 8; i++) {
    if (i == output_index) {
      digitalWrite(SHIFT_REG_INPUT, HIGH);
    } else {
      digitalWrite(SHIFT_REG_INPUT, LOW);
    }
    toggle_clk();
  }
}

void toggle_clk() {
  digitalWrite(SHIFT_REG_CLK_PIN, LOW);
  delay(10);
  digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
  delay(10);
}
