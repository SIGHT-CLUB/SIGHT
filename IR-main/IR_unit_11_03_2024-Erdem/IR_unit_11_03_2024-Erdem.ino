
#include "IR_module_header.h";

void setup() {
  Serial.begin(115200);
  initialize_IR_module();
  set_number_of_package_bytes(4);

}


uint8_t mode = 0;  // 0: Receiver mode, 1: Transmitter mode
void loop() {
  set_ping_package(2,3,3,7); // ID, #Packages, x, y
}

void set_ping_package(uint8_t ID, uint8_t number_of_packages, uint8_t x, uint8_t y) {
  uint8_t PING_INTENTION_NO = 0;
  //Most sig to least significant -> ID (3 bit), Number of packages to be sent (2 bit), Intention(3 bit)


  uint8_t first_byte = (ID<<5) + (number_of_packages<<3) + (PING_INTENTION_NO); 
  uint8_t second_byte = (x<<4) + y;

  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  Serial.println(first_byte);
  Serial.println(second_byte);
  Serial.println();

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(2, CRC_SIG);
  set_buffer(3, CRC_LST);

}

void set_data_package(uint8_t byte_1, uint8_t byte_2) {

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


void communication_test() {
  //To be used in test 1 and test 2
  static unsigned long succesful_package_counter = 0;
  static unsigned long corrupted_package_counter = 0;

  uint8_t listening_result = listen_IR();
  if (listening_result == 1) {
    succesful_package_counter = succesful_package_counter + 1;
  } else if (listening_result == 2) {
    corrupted_package_counter = corrupted_package_counter + 1;
  } else if (listening_result == 0) {
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
        delay(25);
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
