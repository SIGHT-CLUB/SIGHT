
#include "IR_module_header.h";

uint8_t mode_of_operation = 0; //0:listen for ping, 1:ping, 2: ack for ping, 3: send data, 4: listen for data, 5: ack for data
unsigned long last_mode_change = 0;
uint8_t package_count = 0;
uint8_t ID = 2; //4 bits

void setup() {
  Serial.begin(115200);
  initialize_IR_module();
}

void loop() {
//communication_test();
ping_test();
transmit_data();
listen_for_ping_test();
listen_for_data_test();
}

void ping_test() {
  if (mode_of_operation != 1)return;
  Serial.println("\n Pinging the IR module...");

  set_number_of_package_bytes(4);

  //INFO ---------------
  uint8_t x = 3; // 4 bits
  uint8_t y = 4; // 4 bits
 
  uint8_t intention = 1; //4 bits

  //SEND MESSAGE -------------
  send_message(x, y, intention, ID); // x, y, intenion, ID
  
  unsigned int current_time = millis()/1000;

  long delay_after_ping = random(20, 100); // wait time for ack
  //delay(delay_after_ping);

  unsigned long start_time = millis() ;

  while(millis() - start_time < delay_after_ping){
    uint8_t listening_result = listen_IR(); //listens for 20ms. 0:no package, 1:successful package, 2:corrupted package
    if (listening_result == 1){

        // check the intention
        uint8_t incoming_first_byte = get_buffer(0);
        uint8_t incoming_second_byte = get_buffer(1);

        uint8_t  incoming_x = incoming_first_byte >> 4;
        uint8_t  incoming_y = incoming_first_byte % 16;

        uint8_t  incoming_intention = incoming_second_byte >> 4;
        uint8_t  incoming_ID = incoming_second_byte % 16;

       // Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));

      if (incoming_intention == 2){

      mode_of_operation = 3;
      package_count = 0;
      Serial.println("\n Acknowledgement Received...");
      break;
      }
    } else mode_of_operation = 0;
  }


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

void transmit_data(){
  if (mode_of_operation != 3)return;

  uint8_t ack_data = 0;

  Serial.println("\n Transmitting Data...");
  unsigned long start_time = millis() ;

  set_number_of_package_bytes(4);

  /// FIRST PACKAGE ///
  Serial.println("\n Transmitting First Package...");
  //INFO ---------------
  uint8_t first_x = 8; // 4 bits
  uint8_t first_y = 9; // 4 bits
  uint8_t first_intention = 3; //4 bits

  //SEND MESSAGE -------------
  send_message(first_x, first_y, first_intention, ID); // x, y, intenion, ID

  // wait for acknowledgement
  unsigned long listen_duration = random(1000,2000);
  while(millis() - start_time < listen_duration){
    uint8_t listening_result = listen_IR(); // listens for 20ms. 0:no package, 1:successful package, 2:corrupted package
    if (listening_result == 1){

      // check intention
      uint8_t incoming_first_byte = get_buffer(0);
      uint8_t incoming_second_byte = get_buffer(1);

      uint8_t  incoming_x = incoming_first_byte >> 4;
      uint8_t  incoming_y = incoming_first_byte % 16;

      uint8_t  incoming_intention = incoming_second_byte >> 4;
      uint8_t  incoming_ID = incoming_second_byte % 16;

      // Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));


      if (incoming_intention == 5){
        Serial.println("Ping for Data is received");
        ack_data = 1;
        break;
      }
    } 
  }

  mode_of_operation = 0;

}

void listen_for_ping_test(){
  if (mode_of_operation != 0)return;

  uint8_t ack_sent = 0;
  unsigned long listen_duration = random(1000,2000);

  // Serial.println("\nThe random number is: ");
  // Serial.println(random(1000,2000));

  Serial.println("\n Currently Listening for Ping...");
  unsigned long start_time = millis() ;

  while(millis() - start_time < listen_duration){
    uint8_t listening_result = listen_IR(); //listens for 20ms. 0:no package, 1:successful package, 2:corrupted package
    if (listening_result == 1){

      // check intention

      uint8_t incoming_first_byte = get_buffer(0);
      uint8_t incoming_second_byte = get_buffer(1);

      uint8_t  incoming_x = incoming_first_byte >> 4;
      uint8_t  incoming_y = incoming_first_byte % 16;

      uint8_t  incoming_intention = incoming_second_byte >> 4;
      uint8_t  incoming_ID = incoming_second_byte % 16;

      // Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));
      // Serial.println("Something is received");

      if (incoming_intention == 1){
        Serial.println("Ping is received");

        // // display ping message
        // uint8_t first_byte = get_buffer(0);
        // uint8_t second_byte = get_buffer(1);

        // uint8_t  x = first_byte >> 4;
        // uint8_t  y = first_byte % 16;

        // uint8_t  intention = second_byte >> 4;
        // uint8_t  ID = second_byte % 16;

        // Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));


        // transmit ack message
        set_number_of_package_bytes(4);

        //Data ---------------
        uint8_t x = 5; // 4 bits
        uint8_t y = 6; // 4 bits
        
        uint8_t intention = 2; //4 bits

        //SEND MESSAGE -------------
        send_message(x, y, intention, ID); // x, y, intenion, ID

        Serial.println("Acknowledgement sent");
        ack_sent = 1;
        mode_of_operation = 4;

        return;
      }
    }
  }

  if(ack_sent == 0) mode_of_operation = 1;
}

void listen_for_data_test(){
  if (mode_of_operation != 4)return;

  unsigned long listen_duration = random(1000,2000);

  // Serial.println("\nThe random number is: ");
  // Serial.println(random(1000,2000));

  Serial.println("\n Currently Listening for Data...");
  unsigned long start_time = millis() ;

  while(millis() - start_time < listen_duration){
    uint8_t listening_result = listen_IR(); //listens for 20ms. 0:no package, 1:successful package, 2:corrupted package
    if (listening_result == 1){

      // check intention

      uint8_t incoming_first_byte = get_buffer(0);
      uint8_t incoming_second_byte = get_buffer(1);

      uint8_t  incoming_x = incoming_first_byte >> 4;
      uint8_t  incoming_y = incoming_first_byte % 16;

      uint8_t  incoming_intention = incoming_second_byte >> 4;
      uint8_t  incoming_ID = incoming_second_byte % 16;

      if (incoming_intention == 3){
        Serial.println("Data is received");
        Serial.println("Data is" + String(incoming_x) + String(incoming_y) + String(incoming_intention) + String(incoming_ID));

        // // display data message
        // uint8_t first_byte = get_buffer(0);
        // uint8_t second_byte = get_buffer(1);

        // uint8_t  x = first_byte >> 4;
        // uint8_t  y = first_byte % 16;

        // uint8_t  intention = second_byte >> 4;
        // uint8_t  ID = second_byte % 16;

        // Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));


        // transmit ack message
        set_number_of_package_bytes(4);

        //Data ---------------
        uint8_t x = 5; // 4 bits
        uint8_t y = 6; // 4 bits
        
        uint8_t intention = 5; //4 bits
        //SEND MESSAGE -------------
        send_message(x, y, intention, ID); // x, y, intenion, ID

        Serial.println("Acknowledgement for Data sent");
        mode_of_operation = 1;

        break;
      }
    }
  } mode_of_operation = 1;
}

void send_message(uint8_t x, uint8_t y, uint8_t intention, uint8_t ID){
  // input: x, y, intention, ID
  // send the data

  uint8_t first_byte = x*16 + y; //x and y position of the robot
  uint8_t second_byte = intention*16 + ID; //intention and ID of the robot

  //TRANSMIT ---------------
  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(2, CRC_SIG);
  set_buffer(3, CRC_LST);
  
  transmit_buffer(); 
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
