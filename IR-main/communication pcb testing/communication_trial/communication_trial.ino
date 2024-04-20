// This version sends a single ping and then switches to listening mode for a duration of 500-600 ms
// Depending on the listened message's intention, we take different actions. 
// The message size is 4 bytes for acknowledgement and ping, and 8 bytes for the actual message.)

// Response to request: "Acknowledgement"
// Intention 1: Communication Request   Intention 2: Ping Acknowledgement    Intention 3: Actual Message    Intention 4:  Message Acknowledgement


uint8_t mode_of_operation = 1; //0:listen, 1:ping, 2:ping_acknowledgement & listening for message, 3:sending message, 4:message_acknowledgment, 
unsigned long last_mode_change = 0;
uint8_t ID = 2; // MU SPECIFIC INFO bits

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
  uint8_t x = 7; // 4 bits
  uint8_t y = 2; // 4 bits
  
  uint8_t intention = 1; //4 bits
  //uint8_t ID = 5; //4 bits

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
  
  // unsigned int current_time = millis()/1000;
  
  transmit_buffer(); 

  // Switching the mode to "listening" after one ping
  mode_of_operation = 0; 

}

void listen_test(){
  
  //if (mode_of_operation != 0) // If the mode of operation is NOT listen
  //return;

  // unsigned long listen_duration = random(500,600);
  unsigned long listen_duration = 100;


  Serial.println("\n Listening for a ping or ping ack...");
  
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

        unsigned long measurement_start = millis();

        mode_of_operation = 2; // 2 corresponds to the listening for the message mode
        
        send_ping_acknowledgement();  
        unsigned long measurement_duration = millis() - measurement_start;
        Serial.println("Time it took for ping acknowledgement sending: " + String(measurement_duration));
              
        // LISTENING TO MESSAGE
        set_number_of_package_bytes(8);// Size of message packet is 8 BYTES
        uint8_t MESSAGE_RECEIVED = listen_to_message();
        set_number_of_package_bytes(4); // Changing back to our regular size
        
        if(MESSAGE_RECEIVED){
          send_message_acknowledgement();
        }
        
        //Back to pinging
        mode_of_operation = 1;
        
        return;
      }

      else if(intention == 2){ // Acknowledgement is received

        Serial.println("Ping acknowledgment is received, starting data transmisson.");
        mode_of_operation = 3; // message sending mode
        

        set_number_of_package_bytes(8);
        send_message();
        set_number_of_package_bytes(4);


        uint8_t MESSAGE_DELIVERED = listen_for_message_acknowledgement();
        // uint8_t message_counter = 0; 
        // uint8_t sending_limit = 0; // We try upto sending_limit times to send the message again
        
        // // If we're not succesful in sending the message we retry up to sending_limit times
        // while(MESSAGE_DELIVERED != 1 && message_counter<sending_limit){
          
        //   Serial.println("Retransmitting the message once more...");
          
        //   set_number_of_package_bytes(8);
        //   send_message();
        //   set_number_of_package_bytes(4);

        //   MESSAGE_DELIVERED = listen_for_message_acknowledgement();
        //   message_counter++;

        // }

        mode_of_operation = 1; // back to pinging
        
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
   
  Serial.println("\n Sending ping acknowledgement...");

  set_number_of_package_bytes(4);

  //INFO ---------------
  uint8_t x = 7; // 4 bits
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
  
  // unsigned int current_time = millis()/1000;
  
  transmit_buffer(); 

}

// mode_of_operation = 4, sending acknowledgement 
void send_message_acknowledgement()
{
  
  Serial.println("\n Sending message acknowledgement...");

  //INFO ---------------
  uint8_t x = 7; // 4 bits
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
  
  // unsigned int current_time = millis()/1000;
  
  transmit_buffer(); 

}

// The message is 8 bytes
void send_message()
{
  //delay(10);

  // INFO ---------------
  uint8_t x = 7; // 4 bits
  uint8_t y = 4; // 4 bits
  uint8_t intention = 3; //4 bits

  // Header Info
  uint8_t first_byte = x*16 + y; //x and y position of the robot
  uint8_t second_byte = intention*16 + ID; 
  
  uint8_t third_byte =  8; 
  uint8_t fourth_byte = 9; 
  uint8_t fifth_byte = 10; 
  uint8_t sixth_byte = 11; 
   
  
  // SETING THE BUFFERS ---------------
  set_buffer(0, first_byte);
  set_buffer(1, second_byte);
  set_buffer(2, third_byte);
  set_buffer(3, fourth_byte);
  set_buffer(4, fifth_byte);
  set_buffer(5, sixth_byte);

  uint16_t CRC_16 = generate_CRC_16_bit();
  uint8_t CRC_SIG = CRC_16 >> 8;
  uint8_t CRC_LST = CRC_16 % 256;
  set_buffer(6, CRC_SIG);
  set_buffer(7, CRC_LST);
  
  // ----------------------------------

  // unsigned int current_time = millis()/1000;

  Serial.println("\n Sending message package...");    
 
  transmit_buffer(); 

  Serial.println("\n Transmission of message is completed, waiting for acknowledgement...");  

  //delay(10); // ??
}

uint8_t listen_to_message(){

unsigned long listen_duration = 350;

Serial.println("\n Listening for the message...");
unsigned long start_time = millis() ;

while(millis() - start_time < listen_duration){
  
   
  uint8_t listening_result = listen_IR(); //listens for 20ms at a time. 0:no package, 1:successful package, 2:corrupted package
  // Serial.println("\n Number of package bytes: " + String(NUMBER_OF_PACKAGE_BYTES)); // First half of the message

  uint8_t first_byte = get_buffer(0);
  uint8_t second_byte = get_buffer(1);
  
  // Serial.println("\n Unofficial Message"); // First half of the message
  // Serial.println("\n 1st byte: " + String(first_byte) +" 2nd byte: " + String(second_byte)); // First half of the message
  uint8_t  intention = second_byte >> 4; 
  // Serial.println("\n Unofficial Intention: " + String(intention)); // First half of the message
  // Serial.println("\n Unofficial Location x: " + String(first_byte >> 4) + " y: " + String(first_byte % 16)); // First half of the message
  Serial.println("\n Unofficial Listening Result: " + String(listening_result)); // First half of the message

  uint8_t third_byte = get_buffer(2);
  uint8_t fourth_byte = get_buffer(3);
  uint8_t fifth_byte = get_buffer(4);
  uint8_t sixth_byte = get_buffer(5);

  // Serial.println("\n 3rd byte: " + String(third_byte) +" 4th byte: " + String(fourth_byte)); // First half of the message
  // Serial.println("\n 5th byte: " + String(fifth_byte) +" 6th byte: " + String(sixth_byte)); // Second half of the message

  if (listening_result == 1){
    

    uint8_t  x = first_byte >> 4;
    uint8_t  y = first_byte % 16;

    uint8_t  intention = second_byte >> 4; 
    uint8_t  ID = second_byte % 16;

   

    Serial.println("\n Message intention WAS " + String(intention));

    if(intention == 3){
    Serial.println("\n Message packages SUCCESSFULLY received");

    Serial.println("\n Intention: " + String(intention)); // First half of the message
    Serial.println("\n Location x: " + String(first_byte >> 4) + " y: " + String(first_byte % 16)); // First half of the message
    Serial.println("\n 3rd byte: " + String(third_byte) +" 4th byte: " + String(fourth_byte)); // First half of the message
    Serial.println("\n 5th byte: " + String(fifth_byte) +" 6th byte: " + String(sixth_byte)); // Second half of the message

    
    return 1;
    }
  }
}

Serial.println("Failed to listen to the message.");
//delay(10); //??
return 0;

}

uint8_t listen_for_message_acknowledgement(){

unsigned long listen_duration = 245;

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
      Serial.println("Message acknowledgement received, transmission was SUCCESSFUL.");
      return 1;
      mode_of_operation = 1; // back to pinging
    }
    else{
      
      Serial.println("Message acknowledgement NOT received, transmission FAILED.");
     
      return 0;
    }
  }
}
//delay(10);
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
