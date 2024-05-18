// This version sends a single ping and then switches to listening mode for a duration of 500-600 ms
// Depending on the listened message's intention, we take different actions. 
// The message size is 4 bytes for acknowledgement and ping, and 8 bytes for the actual message.)

// Response to request: "Acknowledgement"
// Intention 1: Communication Request   Intention 2: Ping Acknowledgement    Intention 3: Actual Message    Intention 4:  Message Acknowledgement


uint8_t mode_of_operation = 1; //0:listen, 1:ping, 2:ping_acknowledgement & listening for message, 3:sending message, 4:message_acknowledgment, 
unsigned long last_mode_change = 0;
uint8_t ID = 2; // MU SPECIFIC INFO bits
uint8_t switching_LED = 0;
uint8_t loop_iteration_wo_ping_message = 0;
uint8_t consecutive_ack_messages = 0;
uint8_t sent_ping_count = 0;
uint8_t received_ping_count = 0;
uint8_t received_ack_count = 0;
uint8_t ping_time_out = 0;
uint8_t ping_timer = millis();

#include "IR_module_header.h";

void setup() {
  Serial.begin(115200);
  initialize_IR_module();

  pinMode(SHIFT_REG_INPUT, OUTPUT);
  pinMode(SHIFT_REG_CLK_PIN, OUTPUT);
  digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
  Serial.begin(115200);
  initialize_IR_module();
  set_active_s(1);
}

uint8_t active_s = 3;

void loop() {

final_communication();

}

void final_communication(){

  // We do NOT send a ping if we haven't listened to a ping for 3 loops
  if(loop_iteration_wo_ping_message<3 || ping_time_out){

    active_s = active_s + 1;
    if(active_s == 8){active_s =0;}
    set_active_s(active_s);
    ping_test();

    // We record the last time we sent a ping
    ping_timer = millis();


  }

  if(millis()-ping_timer < 100){
    ping_time_out = 0;
  }
  // If we haven't been sending pings for 500 ms
  else{ping_time_out = 1;}


  if(active_s == 4){
    // Serial.print("Currently active transmitter is: ");
    // Serial.println(active_s);
    // Serial.println("Sent a PING from 4");  
    Serial.print("LOOP ITERATION COUNT WO PING: ");
    Serial.println(loop_iteration_wo_ping_message);
    loop_iteration_wo_ping_message++;
  }

  listen_test_3();


  // Serial.print("Consecutive ack count: ");
  // Serial.println(consecutive_ack_messages);

}

void ping_test() {
  
  // Serial.println("\n Pinging the IR module...");

  set_number_of_package_bytes(4); // 1 byte: Location, 1 byte: Intention & ID, 1 byte: Target Location, 2 bytes: CRC

  //INFO ---------------
  uint8_t x = 7; // 4 bits
  uint8_t y = 2; // 4 bits

  uint8_t target_x = 5; // 4 bits
  uint8_t target_y = 8; // 4 bits

  uint8_t intention = 1; //4 bits
  //uint8_t ID = 5; //4 bits

  uint8_t first_byte = x*16 + y; //x and y position of the robot
  uint8_t second_byte = intention*16 + ID; //intention and ID of the robot
  uint8_t third_byte = target_x*16 + target_y;

  //TRANSMIT ---------------
  set_buffer(0, first_byte);
  set_buffer(1, second_byte);
  set_buffer(2, third_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(3, CRC_SIG);
  // set_buffer(4, CRC_LST);
    
  transmit_buffer(); 

}


void listen_test_3(){
      
  // We listen for 20 ms at a time
  uint8_t listening_result = listen_IR_Erdem(); // 0:no package, 1:successful package, 2:corrupted package
  if(listening_result == 2){listening_result = listen_IR_Erdem();}
  
  if (listening_result == 1){ // i.e., the message received is succesful

    uint8_t first_byte = get_buffer(0);
    uint8_t second_byte = get_buffer(1);
    uint8_t third_byte = get_buffer(2);

    uint8_t  x = first_byte >> 4;
    uint8_t  y = first_byte % 16;

    uint8_t  intention = second_byte >> 4;
    uint8_t  ID = second_byte % 16;

    uint8_t  target_x = third_byte >> 4;
    uint8_t  target_y = third_byte % 16;      

    Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID) + " target x: " + String(target_x)+ " target y: " + String(target_y)+ "\n");


    // Read intention, 1: Request for comm., 2: Acknowledgement, 3: Message
    if(intention == 1){

      received_ping_count ++;
      loop_iteration_wo_ping_message = 0;
      consecutive_ack_messages = 0;

      
      Serial.println("Ping PACKAGE is received, sending ping ack");

      mode_of_operation = 2; // 2 corresponds to the listening for the message mode
      
      send_ping_acknowledgement();  
      send_ping_acknowledgement();  
      // delay(10);
      // listen_test_3();  
                    
      return;
    }

    else if(intention == 2){ // Acknowledgement is received

      Serial.println("Ping ACKNOWLEDGEMENT is received.");

      consecutive_ack_messages++;
      received_ack_count++;
    
      return;
    }
  }
  
}


// mode_of_operation = 2, sending acknowledgement 
void send_ping_acknowledgement()
{
   

  set_number_of_package_bytes(4);

  //INFO ---------------
  uint8_t x = 7; // 4 bits
  uint8_t y = 4; // 4 bits
  uint8_t intention = 2; //4 bits
  uint8_t target_x = 5; // 4 bits
  uint8_t target_y = 8; // 4 bits    

  uint8_t first_byte = x*16 + y; //x and y position of the robot
  uint8_t second_byte = intention*16 + ID; //intention and ID of the robot
  uint8_t third_byte = target_x*16 + target_y; //x and y position of the robot

  //TRANSMIT ---------------
  set_buffer(0, first_byte);
  set_buffer(1, second_byte);
  set_buffer(2, third_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(3, CRC_SIG);
  // set_buffer(4, CRC_LST);
  
  // unsigned int current_time = millis()/1000;
  
  transmit_buffer(); 

  // listen_test_2();


}





// A 4 byte message is sent here
void send_message()
{
  
  Serial.println("\n Sending 4 byte message");

  //INFO ---------------
  uint8_t x = 7; // 4 bits
  uint8_t y = 4; // 4 bits
  uint8_t intention = 3; //4 bits

  uint8_t first_byte = x*16 + y; //x and y position of the robot
  uint8_t second_byte = intention*16 + ID; //intention and ID of the robot

  //TRANSMIT ---------------
  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(2, CRC_SIG);
  // set_buffer(3, CRC_LST);
  
  // unsigned int current_time = millis()/1000;
  
  transmit_buffer(); 

}

