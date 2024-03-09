//Refer to: https://www.ersinelektronik.com/class/INNOVAEditor/assets/Datasheets/TSOP1136.pdf

uint8_t NUMBER_OF_PACKAGE_BYTES = 4;  //cannot be smaller than 3.
unsigned long TRIGGER_DURATION_US = (BURST_HALF_PERIOD_US * 2) * K_NUMBER_OF_BURSTS;

uint8_t IR_module_buffer[8];

void initialize_IR_module() {
  pinMode(IR_RECEIVE_PIN, INPUT);
  pinMode(IR_LED, OUTPUT);
}

// =============================================================
uint16_t get_number_of_package_bytes() {
  return NUMBER_OF_PACKAGE_BYTES;
}
void set_number_of_package_bytes(uint16_t new_package_size) {
  NUMBER_OF_PACKAGE_BYTES = new_package_size;
}

void set_buffer(uint16_t byte_index, uint8_t byte_value) {
  IR_module_buffer[byte_index] = byte_value;
}

void reverse_buffer() {
  for (int j = 0; j < NUMBER_OF_PACKAGE_BYTES; j++) {
    uint8_t reversed_byte = 0;
    for (int i = 0; i < 8; i++) {
      if (IR_module_buffer[j] & (1 << i)) {
        reversed_byte |= (1 << (7 - i));
      }
    }
    IR_module_buffer[j] = reversed_byte;
  }
}

void transmit_buffer() {

  reverse_buffer();
  //=================

  transmit_zero();  //start bit

  for (uint16_t byte_index = 0; byte_index < NUMBER_OF_PACKAGE_BYTES; byte_index++) {
    for (uint8_t package_bit = 0; package_bit < 8; package_bit++) {
      uint8_t send_bit = (IR_module_buffer[byte_index] & 1);
      if (send_bit == 0) {
        transmit_zero();
      } else {
        transmit_one();
      }
      IR_module_buffer[byte_index] = IR_module_buffer[byte_index] >> 1;
    }
  }

  //=================
  reverse_buffer();
}
//TRANSITTERS ========================================================
unsigned long TRANSMISSION_START_TIME = 0;

void transmit_zero() {
  TRANSMISSION_START_TIME = micros();
  while (micros() - TRANSMISSION_START_TIME < (TRIGGER_DURATION_US)) {
    digitalWrite(IR_LED, HIGH);
    delayMicroseconds(BURST_HALF_PERIOD_US);
    digitalWrite(IR_LED, LOW);
    delayMicroseconds(BURST_HALF_PERIOD_US);
  }
}

void transmit_one() {
  TRANSMISSION_START_TIME = micros();
  digitalWrite(IR_LED, LOW);
  delayMicroseconds(TRIGGER_DURATION_US);
}

uint8_t listen_IR() {

  unsigned long listen_start_time = millis();
  uint8_t is_received = 0;
  while (millis() - listen_start_time < LISTEN_DURATION_MS) {
    if (digitalRead(IR_RECEIVE_PIN) == 0) {
      is_received = 1;
      break;
    }
  }

  //check if transmission is detected
  if (counter > 5) {
    delayMicroseconds(TRIGGER_DURATION_US * 1.5);
    unsigned long listen_starts = micros();
    for (uint8_t i = 0; i < (NUMBER_OF_PACKAGE_BYTES * 8); i++) {
      uint8_t byte_no = i / 8;

      IR_module_buffer[byte_no] = (IR_module_buffer[byte_no] << 1) + digitalRead(IR_RECEIVE_PIN);
      while (micros() < listen_starts + (i + 1) * TRIGGER_DURATION_US) {
        continue;
      }
    }

    uint16_t CRC_16 = generate_CRC_16_bit();
    uint8_t CRC_SIG = CRC_16 >> 8;
    uint8_t CRC_LST = CRC_16 % 256;
    if (IR_module_buffer[NUMBER_OF_PACKAGE_BYTES - 1] == CRC_LST && IR_module_buffer[NUMBER_OF_PACKAGE_BYTES - 2] == CRC_SIG) {
      for (uint8_t i = 0; i < NUMBER_OF_PACKAGE_BYTES; i++) {
        Serial.print(IR_module_buffer[i]);
        Serial.print(' ');
      }
      Serial.println();
      return 1;  //Package is valid. return 1
    } else {
      Serial.println("Package corrupted");
      return 2;  // CRC check is failed. return 2
    }
  }

  return 0;  // No signal is detected. return 0
}

//MAGICAL CRC_16 MODBUS code.
uint16_t generate_CRC_16_bit() {
  uint16_t remainder = CRC_16_bit_for_1BYTE(IR_module_buffer[0], 65535);
  for (uint8_t i = 1; i < NUMBER_OF_PACKAGE_BYTES - 2; i++) {
    remainder = CRC_16_bit_for_1BYTE(IR_module_buffer[i], remainder);
  }
  return remainder;
}
uint16_t CRC_16_bit_for_1BYTE(uint16_t data, uint16_t last_data) {
  //if this is first data (i.e LAST_DATA==null), LAST_DATA= 65535 = FFFF
  uint16_t key = 40961;     //1010 0000 0000 0001
  data = data ^ last_data;  //XOR
  for (int i = 0; i < 8; i++) {
    boolean should_XOR = false;
    if (data % 2 == 1) should_XOR = true;
    data = data >> 1;
    if (should_XOR) data = data ^ key;
  }
  return data;
}
