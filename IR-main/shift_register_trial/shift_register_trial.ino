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

uint8_t counter = 0;
void loop() {
  transmit_test();
}

void receive_test() {
  change_to_receive_mode();
  set_active_LED_as(1);
  if (digitalRead(RECEIVE_PIN) == 0) {
    Serial.println("Data is received");
  }
}
void transmit_test() {
  change_to_receive_mode();
  set_active_LED_as(1);
  change_to_transmit_mode();

  delay(5000);
}
void change_to_transmit_mode() {
  digitalWrite(TRANSMIT_PIN, HIGH);
  Serial.println("T-HIGH");
}

void change_to_receive_mode() {
  digitalWrite(TRANSMIT_PIN, LOW);
  Serial.println("T-LOW");
}

void set_active_LED_as(uint8_t LED_index) {
  LED_index = 7 - LED_index;  //this is line is not important. Just to ease the debuggin (order of leds)
  if (LED_index < 0 || LED_index > 7) {
    LED_index = 0;
  }

  for (uint8_t i = 0; i < 8; i++) {
    if (i == LED_index) {
      digitalWrite(SHIFT_REG_INPUT, HIGH);
    } else {
      digitalWrite(SHIFT_REG_INPUT, LOW);
    }
    toggle_clk();
  }
}

void toggle_clk() {
  digitalWrite(SHIFT_REG_CLK_PIN, LOW);
  delay(50);
  digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
  delay(50);
}
