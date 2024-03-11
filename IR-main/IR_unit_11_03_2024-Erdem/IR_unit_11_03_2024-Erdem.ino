
#include "IR_module_header.h";

void setup() {
  Serial.begin(115200);
  initialize_IR_module();
  set_number_of_package_bytes(4);
}


uint8_t mode = 0;  // 0: Receiver mode, 1: Transmitter mode
uint8_t sub_mode = 0;

uint8_t ROBOT_ID = 2;
uint8_t NUMBER_OF_DATA_PACKAGES = 3;
uint8_t ROBOT_X = 3;
uint8_t ROBOT_Y = 7;

unsigned long timer_tic = 0;
unsigned long timer_toc = 0;

unsigned long DELAY_AFTER_PING_MS = 5;
unsigned long DELAY_BEFORE_SENDING_ACK_MS = 10;
unsigned long WAIT_PING_ACK_TIMEOUT_MS = 25;
unsigned long DELAY_AFTER_ACK_IS_RECEIVED_MS = 75;
unsigned long DELAY_BETWEEN_DATA_PACKAGES_MS = 25;
unsigned long LISTEN_TIMEOUT_MS = 1000;

void loop() {

  if (mode == 1) {        //TRANSMITTER MODE: =================================================================================================
    if (sub_mode == 0) {  //Send a pinging message
      set_ping_package(ROBOT_ID, NUMBER_OF_DATA_PACKAGES, ROBOT_X, ROBOT_Y);
      transmit_buffer();
      delay(DELAY_AFTER_PING_MS);  //To avoid the robot is triggering itself. This ensures receiver is not triggered by this robot. Do not know whether usefull or not.
      sub_mode = 1;
      timer_tic = millis();
    } else if (sub_mode == 1) {                               //Wait for ACK
      if (millis() - timer_tic > WAIT_PING_ACK_TIMEOUT_MS) {  //no ACK is received for ping
        mode = 0;
        sub_mode = 0;
        timer_tic = millis();
      }
      if (is_receiving_signal() == 1) {
        sub_mode = 2;
        delay(DELAY_AFTER_ACK_IS_RECEIVED_MS);
      }

    } else if (sub_mode == 2) {  // ACK is received, stream data
      for (uint8_t i = 0; i < NUMBER_OF_DATA_PACKAGES; i++) {
        set_data_package(i, i << 1);
        transmit_buffer();
        delay(DELAY_BETWEEN_DATA_PACKAGES_MS);
      }
      mode = 0;
      sub_mode = 0;
      timer_tic = millis();
    }
  } else if (mode == 0) {  //RECEIVER MODE: =================================================================================================
    if (millis()-timer_tic > LISTEN_TIMEOUT_MS){
      mode = 1;
      sub_mode = 0;
      timer_tic = millis();
    }
    if (sub_mode == 0) {                       //Wait for ping signal
      uint8_t listening_result = listen_IR();  //listens for 50ms (Check header!). 0:no package, 1:successful package, 2:corrupted package
      //50 to 100ms is passed since started to listening
      if (listening_result == 1) {
        uint8_t second_byte = get_buffer(1); // x (4 bit), y(4 bit)
        uint8_t x = second_byte >>4;
        uint8_t y = second_byte %16;
        Serial.println();
        Serial.println(x);
        Serial.println(y);
        delay(DELAY_BEFORE_SENDING_ACK_MS)
        
      }
    }
    mode = 1;
    sub_mode = 0;
  }
}

void set_ping_package(uint8_t ID, uint8_t number_of_packages, uint8_t x, uint8_t y) {
  uint8_t PING_INTENTION_NO = 0;
  //Most sig to least significant -> ID (3 bit), Number of packages to be sent (2 bit), Intention(3 bit)

  uint8_t first_byte = (ID << 5) + (number_of_packages << 3) + (PING_INTENTION_NO);
  uint8_t second_byte = (x << 4) + y;

  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(2, CRC_SIG);
  set_buffer(3, CRC_LST);
}
void set_data_package(uint8_t first_byte, uint8_t second_byte) {
  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(2, CRC_SIG);
  set_buffer(3, CRC_LST);
}
