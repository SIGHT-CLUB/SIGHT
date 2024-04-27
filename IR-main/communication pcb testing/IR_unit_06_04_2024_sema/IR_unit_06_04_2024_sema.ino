
#include "IR_module_header.h";

uint8_t mode_of_operation = 0; //0: listen for ping or ack, 1:ping, 2: ack for ping, 3: send data, 4: listen for data, 5: ack for data
unsigned long last_mode_change = 0;
uint8_t package_count = 0;
uint8_t ID = 2; //4 bits

void setup() {
  Serial.begin(115200);
  initialize_IR_module();

  pinMode(SHIFT_REG_INPUT, OUTPUT);
  pinMode(SHIFT_REG_CLK_PIN, OUTPUT);
  digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
  Serial.begin(115200);
  initialize_IR_module();
  set_active_s(4);
}

uint8_t active_s = 4;

void loop() {

set_active_s(active_s);

// communication_test();
communication();

active_s = active_s + 1;
if (active_s == 8){active_s = 0;}

}

void communication() {

ping_test();
listen_test();
transmit_data();
listen_for_data_test();

}

void ping_test() {
  //check the mode of operation
  if (mode_of_operation != 1)return;
  Serial.println("\n Pinging the IR module...");

  //set the number of package bytes to 4
  set_number_of_package_bytes(4);

  //INFO ---------------
  uint8_t x = 3; // 4 bits
  uint8_t y = 4; // 4 bits
 
  //SEND MESSAGE -------------
  send_ping_message(x, y); // x, y, intenion, ID
  mode_of_operation = 0;
}

void listen_test(){
  //check the mode of operation
  if (mode_of_operation != 0)return;

  // ping and ack is 4 bytes to change the number of bytes accordingly
  set_number_of_package_bytes(4);

  Serial.println("\n Currently Listening for Ping or Ack...");

  // set the listen time
  unsigned long listen_duration = 300;
  // Serial.println("\nThe random number is: ");

  // check the time to listen
  unsigned long start_time = millis() ;

  // if the time passed is shorter than the wait time, listen
  while(millis() - start_time < listen_duration){

    // listen
    uint8_t listening_result = listen_IR(); //listens for 20ms. 0:no package, 1:successful package, 2:corrupted package

    // if smt is received, check it
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

      //if the intention is suitable for ping
      if (incoming_intention == 2){

      //change the mode to transmit message
      mode_of_operation = 3;
      package_count = 0;
      Serial.println("\n Acknowledgement Received...");
      return;

      } //if the intention is suitable for ack
      
      else if (incoming_intention == 1){
        Serial.println("Ping is received");

        // display ping message
        display_ping_message();

        // transmit ack message

        //Data ---------------
        uint8_t x = 5; // 4 bits
        uint8_t y = 6; // 4 bits

        //SEND MESSAGE -------------
        send_ack_message(x, y); // x, y, intenion, ID

        Serial.println("Acknowledgement sent");

        // set the mode of operation accordingly
        mode_of_operation = 4; // if ping is received, change to data listening mode
        return;
      }
    }
  }

  mode_of_operation = 1; //if no ping or ack is received, change to ping mode
}

void transmit_data(){
  //check the mode of operation
  if (mode_of_operation != 3)return;

  // keep the info of whether the ack is received
  uint8_t ack_data = 0;

  Serial.println("\n Transmitting Data...");

  // set the number of package bytes suitable for data
  set_number_of_package_bytes(8);

  /// FIRST PACKAGE ///
  Serial.println("\n Transmitting First Package...");
  //INFO ---------------
  uint8_t first_x = 8; // 4 bits
  uint8_t first_y = 9; // 4 bits

  //SEND MESSAGE -------------
  send_data_message(first_x, first_y); // x, y, data

  // set the wait time for ack
  unsigned long listen_duration = random(500,1000);

  // check the time to wait for ack
  unsigned long start_time = millis() ;

  // set the number of package bytes suitable for ack
  set_number_of_package_bytes(4);

  // if the time passed is shorter than the wait time, listen for ack
  while(millis() - start_time < listen_duration){

    //listen
    uint8_t listening_result = listen_IR(); // listens for 20ms. 0:no package, 1:successful package, 2:corrupted package

    // if something is received, check it
    if (listening_result == 1){

      // check intention
      uint8_t incoming_first_byte = get_buffer(0);
      uint8_t incoming_second_byte = get_buffer(1);

      uint8_t  incoming_x = incoming_first_byte >> 4;
      uint8_t  incoming_y = incoming_first_byte % 16;

      uint8_t  incoming_intention = incoming_second_byte >> 4;
      uint8_t  incoming_ID = incoming_second_byte % 16;

      // Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));

      //if the intention is suitable for ack
      if (incoming_intention == 5){
        Serial.println("Ack for Data is received");

        // ack is received, update it
        ack_data = 1;
        break;
      }
    } 
  }

  mode_of_operation = 0; // change to listen mode

}

void listen_for_data_test(){
  //check the mode of operation
  if (mode_of_operation != 4)return;

  //data message has 8 bytes
  set_number_of_package_bytes(8);

  Serial.println("\n Currently Listening for Data...");

  // set the listen time for data
  unsigned long listen_duration = 1000;
  // Serial.println("\nThe random number is: ");

  //check the time to listen for data
  unsigned long start_time = millis() ;

  // if the time passed is shorter than the wait time, listen for data
  while(millis() - start_time < listen_duration){

    // listen
    uint8_t listening_result = listen_IR(); //listens for 20ms. 0:no package, 1:successful package, 2:corrupted package

    // if (listening_result == 2){
    //   uint8_t incoming_first_byte = get_buffer(0);
    //   uint8_t incoming_second_byte = get_buffer(1);

    //   uint8_t  incoming_x = incoming_first_byte >> 4;
    //   uint8_t  incoming_y = incoming_first_byte % 16;

    //   uint8_t  incoming_intention = incoming_second_byte >> 4;
    //   uint8_t  incoming_ID = incoming_second_byte % 16;

    //   Serial.println("\n\nx"+ String(incoming_x));
    //   Serial.println("y"+ String(incoming_y));
    //   Serial.println("intention"+ String(incoming_intention));
    //   Serial.println("ID"+ String(incoming_ID));
    //   Serial.println("\n1st byte"+ String(get_buffer(2)));
    //   Serial.println("2st byte"+ String(get_buffer(3)));
    //   Serial.println("3st byte"+ String(get_buffer(4)));
    //   Serial.println("4st byte"+ String(get_buffer(5)));
    //   Serial.println("CRC1"+ String(get_buffer(6)));
    //   Serial.println("CRC2"+ String(get_buffer(7)));
    // }

    // if smt is received, check it
    if (listening_result == 1){
      
      // check intention
      uint8_t incoming_first_byte = get_buffer(0);
      uint8_t incoming_second_byte = get_buffer(1);
      uint8_t incoming_data[4] = {get_buffer(2), get_buffer(3), get_buffer(4), get_buffer(5)};

      uint8_t  incoming_x = incoming_first_byte >> 4;
      uint8_t  incoming_y = incoming_first_byte % 16;

      uint8_t  incoming_intention = incoming_second_byte >> 4;
      uint8_t  incoming_ID = incoming_second_byte % 16;

      //if the intention is suitable for data
      if (incoming_intention == 3){

        Serial.println("Data is received");

        Serial.println("Data is" );
        for(int i = 0; i < 4; i++){
          Serial.print(incoming_data[i]);
        }
         Serial.println("");

        // display data message
        // Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));

        // TRANSMIT ACK FOR DATA--------------------
        // set the number of package bytes to 4
        set_number_of_package_bytes(4);

        //Data ---------------
        uint8_t x = 5; // 4 bits
        uint8_t y = 6; // 4 bits

        //SEND MESSAGE -------------
        send_ack_for_data_message(x, y); // x, y, intenion, ID

        Serial.println("Acknowledgement for Data sent");
        mode_of_operation = 1; // change to ping mode

        break;
      }
    }
  } mode_of_operation = 1; // change to ping mode
}

void display_ping_message(){
  uint8_t first_byte = get_buffer(0);
  uint8_t second_byte = get_buffer(1);

  uint8_t  x = first_byte >> 4;
  uint8_t  y = first_byte % 16;

  uint8_t  intention = second_byte >> 4;
  uint8_t  ID = second_byte % 16;

  Serial.println("Incoming Ping Message With x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));
}

void send_ping_message(uint8_t x, uint8_t y){
  // input: x, y, intention, ID
  // send ping
  uint8_t intention = 1;

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

void send_ack_message(uint8_t x, uint8_t y){
  // input: x, y, intention, ID
  // send ack

  uint8_t intention = 2;

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

void send_ack_for_data_message(uint8_t x, uint8_t y){
  // input: x, y, intention, ID
  // send ack
  uint8_t intention = 5;

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

void send_data_message(uint8_t x, uint8_t y){
  // input: x, y, intention, ID
  // send the data
  uint8_t intention = 3;

  uint8_t first_byte = x*16 + y; //x and y position of the robot
  uint8_t second_byte = intention*16 + ID; //intention and ID of the robot

  //TRANSMIT ---------------
  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  uint8_t third_byte = 3;
  uint8_t fourth_byte = 4;
  uint8_t fifth_byte = 5;
  uint8_t sixth_byte = 6;

  set_buffer(2, third_byte);
  set_buffer(3, fourth_byte);
  set_buffer(4, fifth_byte);
  set_buffer(5, sixth_byte);

  // for (i = 2; i < 6; i++){
  // set_buffer(i, data[i-2]);
  // }

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(6, CRC_SIG);
  set_buffer(7, CRC_LST);
  
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