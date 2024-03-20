// This version sends a single ping and then switches to listening mode for a duration of 500-600 ms
// Depending on the listened message's intention, we take different actions. 
// The message size is 4 bytes for acknowledgement and ping, and 8 bytes for the actual message.)

// Response to request: "Acknowledgement"
// Intention 1: Communication Request   Intention 2: Ping Acknowledgement    Intention 3: Actual Message    Intention 4:  Message Acknowledgement


uint8_t mode_of_operation = 0; //0:listen, 1:ping, 2:ping_acknowledgement & listening for message, 3:sending message, 4:message_acknowledgment, 
unsigned long last_mode_change = 0;
uint8_t ID = 1; // MU SPECIFIC INFO bits

#include "IR_module_header.h";

void setup() {
  Serial.begin(115200);
  initialize_IR_module();
}

void loop() {

ping_test();
listen_test();

}


void ping_test() {
  
  if (mode_of_operation != 1) // If the mode of operation is NOT pinging
  return;
  
  Serial.println("\n Pinging the IR module...");

  set_number_of_package_bytes(4); // 1 byte: Location, 1 byte: Intention & ID, 2 bytes: CRC

  //INFO ---------------
  uint8_t x = 5; // 4 bits
  uint8_t y = 2; // 4 bits
  
  uint8_t intention = 1; //4 bits
  uint8_t ID = 5; //4 bits

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
  
  unsigned int current_time = millis()/1000;
  
  transmit_buffer(); 

  // Switching the mode to "listening" after one ping
  mode_of_operation = 0; 

}

void listen_test(){
  
  if (mode_of_operation != 0) // If the mode of operation is NOT listen
  return;

  unsigned long listen_duration = random(500,600);

  Serial.println("\n Listening for a ping message...");
  
  unsigned long start_time = millis();

  while(millis() - start_time < listen_duration){
    
    // We listen for 20 ms at a time
    uint8_t listening_result = listen_IR(); // 0:no package, 1:successful package, 2:corrupted package
    
    
    if (listening_result == 1){ // i.e., the message received is succesful
      
      uint8_t first_byte = get_buffer(0);
      uint8_t second_byte = get_buffer(1);

      uint8_t  x = first_byte >> 4;
      uint8_t  y = first_byte % 16;

      uint8_t  intention = second_byte >> 4;
      uint8_t  ID = second_byte % 16;

      Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID) + "\n");

      // Read intention, 1: Request for comm., 2: Acknowledgement, 3: Message
      if(intention == 1){
        
        Serial.println("Ping package is received");
        mode_of_operation = 2; // 2 corresponds to the listening for the message mode
        
        send_ping_acknowledgement();  
        listen_to_message();
        send_message_acknowledgement();
        
        return;
      }
      
      else if(intention == 2){ // Acknowledgement is received

        mode_of_operation = 3; // message sending mode
        send_message();
        uint8_t MESSAGE_DELIVERED = listen_for_message_acknowledgement();
        uint8_t message_counter = 0;

        // If we were not succesful in sending the message we retry up to 4 times
        while(MESSAGE_DELIVERED != 1 && message_counter<4){
          send_message();
          MESSAGE_DELIVERED = listen_for_message_acknowledgement();
          message_counter++;
          mode_of_operation = 1; // back to pinging
        }

        return;
      }
    }
  }

  // We switch back to pinging
  mode_of_operation = 1;

}

// mode_of_operation = 2, sending acknowledgement 
void send_ping_acknowledgement()
{
  // Sends the same acknowledgement message c times
  for(uint8_t c = 0; c<1; c++ ){

    Serial.println("\n Sending acknowledgement...");

    set_number_of_package_bytes(4);

    //INFO ---------------
    uint8_t x = 3; // 4 bits
    uint8_t y = 4; // 4 bits
    uint8_t intention = 2; //4 bits

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
    
    unsigned int current_time = millis()/1000;
    
    transmit_buffer(); 

  }

}

// mode_of_operation = 4, sending acknowledgement 
void send_message_acknowledgement()
{
  // Sends the same acknowledgement message c times
  for(uint8_t c = 0; c<1; c++ ){

    Serial.println("\n Sending acknowledgement...");

    set_number_of_package_bytes(4);

    //INFO ---------------
    uint8_t x = 3; // 4 bits
    uint8_t y = 4; // 4 bits
    uint8_t intention = 4; //4 bits

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
    
    unsigned int current_time = millis()/1000;
    
    transmit_buffer(); 

  }

}

void send_message()
{
  set_number_of_package_bytes(16);

  // INFO ---------------
  uint8_t x = 3; // 4 bits
  uint8_t y = 4; // 4 bits
  uint8_t intention = 3; //4 bits

  uint8_t first_byte = x*16 + y; //x and y position of the robot
  uint8_t second_byte = intention*16 + ID; 
  uint8_t rest_of_the_bytes = 113; 
  
  // SETING THE BUFFERS ---------------
  set_buffer(0, first_byte);
  set_buffer(1, second_byte);

  for(uint8_t i=2; i<12; i++){
    set_buffer(i, rest_of_the_bytes);
  }

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(14, CRC_SIG);
  set_buffer(15, CRC_LST);
  
  // ----------------------------------

  unsigned int current_time = millis()/1000;

  Serial.println("\n Sending message package...");    
  transmit_buffer(); 

  Serial.println("\n Transmission of message is completed, waiting for acknowledgement...");  


  mode_of_operation = 1; // Back to pinging

  delay(1000);
}

void listen_to_message(){

unsigned long listen_duration = 50;

Serial.println("\n Listening for the message...");
unsigned long start_time = millis() ;

while(millis() - start_time < listen_duration){
  uint8_t listening_result = listen_IR(); //listens for 20ms at a time. 0:no package, 1:successful package, 2:corrupted package
  if (listening_result == 1){
    Serial.println("Message packages are received");
    
    uint8_t first_byte = get_buffer(0);
    uint8_t second_byte = get_buffer(1);
    uint8_t third_byte = get_buffer(2);
    uint8_t fourth_byte = get_buffer(3);

    uint8_t  x = first_byte >> 4;
    uint8_t  y = first_byte % 16;

    uint8_t  intention = second_byte >> 4; 
    uint8_t  ID = second_byte % 16;

    Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID)); // First half of the message
    Serial.println("3rd byte: " + String(third_byte) +"4th byte: " + String(fourth_byte)); // Second half of the message

  }
}
  mode_of_operation = 1; // back to pinging

delay(1000);
}

uint8_t listen_for_message_acknowledgement(){

unsigned long listen_duration = 50;

Serial.println("\n Listening for the message acknowledgement...");
unsigned long start_time = millis() ;

while(millis() - start_time < listen_duration){
  uint8_t listening_result = listen_IR(); //listens for 20ms at a time. 0:no package, 1:successful package, 2:corrupted package
  
  if (listening_result == 1){
    
    uint8_t first_byte = get_buffer(0);
    uint8_t second_byte = get_buffer(1);

    uint8_t  x = first_byte >> 4;
    uint8_t  y = first_byte % 16;

    uint8_t  intention = second_byte >> 4;
    uint8_t  ID = second_byte % 16;

    if(intention == 4){
      Serial.println("Message acknowledgement received, transmission was succesful.");
      return 1;
      mode_of_operation = 1; // back to pinging
    }
    else{
      return 0;
    }
  }
}
delay(1000);
}

