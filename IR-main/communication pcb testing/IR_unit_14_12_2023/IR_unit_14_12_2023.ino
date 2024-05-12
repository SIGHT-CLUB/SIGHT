
#include "IR_module_header.h";

void setup() {
  Serial.begin(115200);
  initialize_IR_module();

  pinMode(SHIFT_REG_INPUT, OUTPUT);
  pinMode(SHIFT_REG_CLK_PIN, OUTPUT);
  digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
  Serial.begin(115200);
  initialize_IR_module();
  set_active_s(0);
}

void loop() {
communication_test();
  // unsigned int start_time;
  // unsigned int finish_time;
  // unsigned int duration;
  // unsigned long TRANSMISSION_START_TIME = 0;

  // start_time = millis();
  // for (uint16_t i = 0; i<1000; i++){
  // TRANSMISSION_START_TIME = micros();
  // digitalWrite(IR_LED, LOW);
  // delayMicroseconds(TRIGGER_DURATION_US-12);
  // }
  // finish_time = millis();
  // duration = finish_time - start_time;
  // Serial.println(duration);
}


void communication_test() {
  //To be used in test 1 and test 2
  static unsigned long succesful_package_counter = 0;
  static unsigned long corrupted_package_counter = 0;

  uint8_t listening_result = listen_IR();
  if (listening_result == 1) {
    succesful_package_counter = succesful_package_counter + 1;
  } else if (listening_result == 2) {
    corrupted_package_counter = corrupted_package_counter + 1;
  } else if (listening_result == 0){
    //pass (no data is received)
  }

  if (Serial.available() > 0) {
    int incoming_byte = Serial.read();
    uint16_t number_of_package_bytes = get_number_of_package_bytes();

    if (incoming_byte == 'S') {
      Serial.println("Sending the data...");
      for (uint16_t i = 0; i < 256; i++) {
        for (uint16_t j = 0; j < number_of_package_bytes - 2; j++) {
          set_buffer(j, i);
        }
        uint16_t CRC_16 = generate_CRC_16_bit();
        uint8_t CRC_SIG = CRC_16 >> 8;
        uint8_t CRC_LST = CRC_16 % 256;
        set_buffer(number_of_package_bytes - 2, CRC_SIG);
        set_buffer(number_of_package_bytes - 1, CRC_LST);
        transmit_buffer();
        Serial.println(String(i) + " / " + String(255));
        delay(100);
      }
      Serial.println("Transmitting process is completed.");
    } else if (incoming_byte == 'B') {
      Serial.println("\nSending random data...");
      for (uint16_t i = 0; i < number_of_package_bytes - 2; i++) {
        uint8_t rand_number = random(256);
        set_buffer(i, rand_number);
        Serial.print(String(rand_number) + " ");
      }
      uint16_t CRC_16 = generate_CRC_16_bit();
      uint8_t CRC_SIG = CRC_16 >> 8;
      uint8_t CRC_LST = CRC_16 % 256;
      Serial.println(String(CRC_SIG) + " " + String(CRC_LST));
      set_buffer(number_of_package_bytes - 2, CRC_SIG);
      set_buffer(number_of_package_bytes - 1, CRC_LST);

      transmit_buffer();
      delay(20);
    } else if (incoming_byte == 'P') {
      while (Serial.available() == 0) {}
      int package_number = Serial.read();
      if ('3' <= package_number && package_number <= '8') {
        set_number_of_package_bytes(package_number - '0');
        Serial.println("Number of package byte is set to: " + String(package_number - '0'));
      } else {
        Serial.println("Error: Number of package is not in limits");
      }
    } else if (incoming_byte == 'L') {
      Serial.println("\nTotal packages received:" + String(succesful_package_counter + corrupted_package_counter));
      Serial.println("Successful packages:" + String(succesful_package_counter));
      Serial.println("Corrupted packages:" + String(corrupted_package_counter));
      Serial.println("Success ratio is left as an exercise = " + String(succesful_package_counter) + " / " + String(succesful_package_counter + corrupted_package_counter));
    } else if (incoming_byte == 'R') {
      Serial.println("\nCounters are reseted");
      succesful_package_counter = 0;
      corrupted_package_counter = 0;
    }
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
