
#include "IR_module_header.h";

void setup() {
  Serial.begin(115200);
  initialize_IR_module();
  set_number_of_package_bytes(4);
}

uint8_t mode = 0;  // 0: Receiver mode, 1: Transmitter mode
uint8_t sub_mode = 0;

uint8_t ROBOT_ID = 2;
uint8_t NUMBER_OF_DATA_PACKAGES =3;
uint8_t ROBOT_X = 3;
uint8_t ROBOT_Y = 7;

unsigned long timer_tic = 0;
unsigned long timer_toc = 0;

const unsigned long DELAY_BEFORE_PING_MIN_MS = 25;
const unsigned long DELAY_BEFORE_PING_MAX_MS = 100;
const unsigned long CHIRPING_DURATION_MS = 10;
const unsigned long DELAY_AFTER_CHIRPING_MS = 2.5;
const unsigned long DELAY_BEFORE_SENDING_ACK_MS = 15;
const unsigned long WAIT_CHIRPING_ACK_TIMEOUT_MS = 30;
const unsigned long DELAY_AFTER_RESPONDING_TO_ACK_MS = 25;

const unsigned long DELAY_AFTER_ACK_IS_RECEIVED_MS = 50;
const unsigned long DELAY_BETWEEN_DATA_PACKAGES_MS = 10;
const unsigned long LISTEN_TIMEOUT_MS = 500;


void loop() {
  //RECEIVER MODE: =================================================================================================
  if (mode == 0) {
    if (millis() - timer_tic > LISTEN_TIMEOUT_MS) {
      mode = 1;
      sub_mode = 0;
      timer_tic = millis();
      Serial.println("changing to transmitter mode");

    } else if (sub_mode == 0) {  //Wait for chirping signal (! may be DATA), and send ACK-chirping if received.
      if (is_receiving_signal() == 1) {
        delay(DELAY_BEFORE_SENDING_ACK_MS);
        transmit_chirping(CHIRPING_DURATION_MS);
        delay(DELAY_AFTER_RESPONDING_TO_ACK_MS);
        sub_mode = sub_mode + 1;
      }
    } else if (sub_mode == 1) {
      //listen for header
      uint8_t listening_result = listen_IR();
      if (listening_result == 1) {
        uint8_t first_byte = get_buffer(0);
        uint8_t second_byte = get_buffer(1);
        uint8_t number_of_packages = (first_byte >> 4) % 4;
        Serial.println("\n#" + String(first_byte) + "," + String(second_byte));
        for (uint8_t package_received = 1; package_received < number_of_packages; package_received++) {
          uint8_t result = listen_IR();
          Serial.println(result);
        }
      }
      mode = 1;
      sub_mode = 0;
      Serial.println("changing to transmitter mode");
    }
  }

  //TRANSMITTER MODE: =================================================================================================
  else if (mode == 1) {
    if (sub_mode == 0) {  //introduce random delay to avoid robots sending packages at the same time when the listening period is finished.
      delay(random(DELAY_BEFORE_PING_MIN_MS, DELAY_BEFORE_PING_MAX_MS));
      sub_mode = sub_mode + 1;
    } else if (sub_mode == 1) {  //Send a chirping
      transmit_chirping(CHIRPING_DURATION_MS);
      delay(DELAY_AFTER_CHIRPING_MS);  //To avoid the robot is triggering itself. This ensures receiver is not triggered by this robot. Do not know whether usefull or not.
      sub_mode = sub_mode + 1;
      timer_tic = millis();
    } else if (sub_mode == 2) {  //Wait for ACK
      if (is_receiving_signal() == 1) {
        sub_mode = sub_mode + 1;
        Serial.println("ACK-r");
        delay(DELAY_AFTER_ACK_IS_RECEIVED_MS);
      } else if (millis() - timer_tic > WAIT_CHIRPING_ACK_TIMEOUT_MS) {  //no ACK is received for ping
        mode = 0;
        sub_mode = 0;
        timer_tic = millis();
        Serial.println("changing to receiver mode - No ACK");
      }
    } else if (sub_mode == 3) {  // ACK is received, stream data
      set_header_package(NUMBER_OF_DATA_PACKAGES, ROBOT_ID, ROBOT_X, ROBOT_Y);
      transmit_buffer();
      for (uint8_t i = 1; i < NUMBER_OF_DATA_PACKAGES; i++) {
        set_data_package(i, i * 2, i * 4);  // uint8_t package_no, uint8_t first_byte, uint8_t second_byte )
        transmit_buffer();
        delay(DELAY_BETWEEN_DATA_PACKAGES_MS);
      }
      mode = 0;
      sub_mode = 0;
      timer_tic = millis();
      Serial.println("changing to receiver mode");
    }
  }
}

void set_header_package(uint8_t number_of_packages, uint8_t ID, uint8_t x, uint8_t y) {
  //Most sig to least significant -> Package No(2 bit), Number of packages to be sent (2 bit), ID (4 bit)
  const uint8_t PACKAGE_NO = 0;
  uint8_t first_byte = (PACKAGE_NO << 6) + (number_of_packages << 4) + (ID);
  uint8_t second_byte = (x << 4) + y;

  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(2, CRC_SIG);
  set_buffer(3, CRC_LST);
}
void set_data_package(uint8_t package_no, uint8_t first_byte, uint8_t second_byte) {
  // Most sig to least significant;
  // first_byte ->  Package No(2 bit), package_data (6 bit)
  // second_byte -> package_data (8 bit)
  first_byte = (package_no << 6) + (first_byte % 64);

  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(2, CRC_SIG);
  set_buffer(3, CRC_LST);
}
