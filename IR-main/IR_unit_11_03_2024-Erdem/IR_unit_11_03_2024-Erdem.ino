
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
unsigned long WAIT_PING_ACK_TIMEOUT_MS = 25;
unsigned long DELAY_AFTER_ACK_IS_RECEIVED_MS = 250;
unsigned long DELAY_BETWEEN_DATA_PACKAGES_MS = 25;

void loop() {

  set_data_package(43, 58);  // first_byte, second_byte

  if (mode == 1) { //TRANSMITTER MODE: =================================================================================================
    if (sub_mode == 0) {                                                      //Send a pinging message
      set_ping_package(ROBOT_ID, NUMBER_OF_DATA_PACKAGES, ROBOT_X, ROBOT_Y);
      transmit_buffer();
      delay(DELAY_AFTER_PING_MS);  //To avoid the robot is triggering itself. This ensures receiver is not triggered by this robot. Do not know whether usefull or not.
      sub_mode = 1;
      timer_tic = millis();
    } else if (sub_mode == 1) {                               //Wait for ACK
      if (millis() - timer_tic > WAIT_PING_ACK_TIMEOUT_MS) {  //no ACK is received for ping
        mode = 0;
        sub_mode = 0;
      }
      if (is_receiving_signal() == 1) {
        sub_mode = 2;
        delay(DELAY_AFTER_ACK_IS_RECEIVED_MS);
      }

    } else if (sub_mode == 2) {  // ACK is received, stream data
      for(uint8_t i = 0 ; i < NUMBER_OF_DATA_PACKAGES; i++){
        set_data_package(i, i<<1);
        transmit_buffer();
        delay(DELAY_BETWEEN_DATA_PACKAGES_MS);
      }    
      mode = 0;
      sub_mode = 0;
    }
  } else if (mode == 0) {//RECEIVER MODE: =================================================================================================
    delay(random(1000, 5000));
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

uint8_t mode_of_operation = 0;  //0:listen, 1:ping
unsigned long last_mode_change = 0;

void ping_test() {
  if (mode_of_operation == 0) return;
  Serial.println("\n Pinging the IR module...");



  //INFO ---------------
  uint8_t x = 3;  // 4 bits
  uint8_t y = 4;  // 4 bits

  uint8_t intention = 0;  //4 bits
  uint8_t ID = 2;         //4 bits

  uint8_t first_byte = x * 16 + y;  //x and y position of the robot
  uint8_t second_byte = 12;         //intention and ID of the robot

  //TRANSMIT ---------------
  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(2, CRC_SIG);
  set_buffer(3, CRC_LST);

  unsigned int current_time = millis() / 1000;

  transmit_buffer();

  // long delay_after_ping = random(20, 100);
  // delay(delay_after_ping);
  mode_of_operation = 0;

  // uint8_t listening_result = listen_IR(); //listens for 20ms. 0:no package, 1:successful package, 2:corrupted package

  // if (listening_result != 0){
  //   set_buffer(0, 12);
  //   set_buffer(1, 38);

  //   uint16_t CRC_16 = generate_CRC_16_bit();
  //   uint8_t CRC_SIG = CRC_16 >> 8;
  //   uint8_t CRC_LST = CRC_16 % 256;
  //   set_buffer(2, CRC_SIG);
  //   set_buffer(3, CRC_LST);
  //   transmit_buffer();
  // }
}

void listen_test() {
  if (mode_of_operation == 1) return;

  unsigned long listen_duration = random(500, 600);

  // Serial.println("\nThe random number is: ");
  // Serial.println(random(1000,2000));

  Serial.println("\n Currently Listening...");
  unsigned long start_time = millis();

  while (millis() - start_time < listen_duration) {
    uint8_t listening_result = listen_IR();  //listens for 20ms. 0:no package, 1:successful package, 2:corrupted package
    if (listening_result == 1) {
      Serial.println("Package is received");

      uint8_t first_byte = get_buffer(0);
      uint8_t second_byte = get_buffer(1);

      uint8_t x = first_byte >> 4;
      uint8_t y = first_byte % 16;

      uint8_t intention = second_byte >> 4;
      uint8_t ID = second_byte % 16;

      Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));
      break;
    }
  }
  mode_of_operation = 1;

  //check for ping (intention check)
  //if ping is received, send a ping back
}