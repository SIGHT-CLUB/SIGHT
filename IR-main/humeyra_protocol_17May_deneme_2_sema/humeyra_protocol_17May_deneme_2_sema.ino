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

  uint16_t CRC_8 = generate_CRC_8_bit();
  set_buffer(3, CRC_8);
    
  transmit_buffer(); 

}

void listen_test(){
  
  //if (mode_of_operation != 0) // If the mode of operation is NOT listen
  //return;

  // unsigned long listen_duration = random(500,600);
  unsigned long listen_duration = random(200,250);


  Serial.println("\n Listening for a ping or ping ack...");
  
  unsigned long start_time = millis();

  while(millis() - start_time < listen_duration){
    
    // We listen for 20 ms at a time
    uint8_t listening_result = listen_IR(); // 0:no package, 1:successful package, 2:corrupted package
    
    
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

      Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID) + "target x: " + String(target_x)+ "target y: " + String(target_y)+ "\n");

      // Read intention, 1: Request for comm., 2: Acknowledgement, 3: Message
      if(intention == 1){
        
        Serial.println("Ping package is received");

        unsigned long measurement_start = millis();

        mode_of_operation = 2; // 2 corresponds to the listening for the message mode
        
        send_ping_acknowledgement();  
        
        unsigned long measurement_duration = millis() - measurement_start;
        // Serial.println("Time it took for ping acknowledgement sending: " + String(measurement_duration));
              
        // LISTENING TO MESSAGE
        set_number_of_package_bytes(4);// Size of message packet is 8 BYTES
        uint8_t MESSAGE_RECEIVED = listen_to_message();
        set_number_of_package_bytes(4); // Changing back to our regular size
        
        if(MESSAGE_RECEIVED){
          send_message_acknowledgement();
          // delay(20);
        }
        
        //Back to pinging
        mode_of_operation = 1;
        
        return;
      }

      else if(intention == 2){ // Acknowledgement is received

        Serial.println("Ping acknowledgment is received, starting data transmisson.");
        mode_of_operation = 3; // message sending mode
        

        set_number_of_package_bytes(4);
        send_message();
        set_number_of_package_bytes(4);


        uint8_t MESSAGE_DELIVERED = listen_for_message_acknowledgement();
    
        mode_of_operation = 1; // back to pinging
        
        return;
      }
    }
  }

  // We switch back to pinging
  mode_of_operation = 1;

}

void listen_test_2(){
  

  // unsigned long listen_duration = random(100,150);
  unsigned long listen_duration = 20;

  // Serial.println("\n Listening fo0r a ping or ping ack...");
  
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
        
        Serial.println("Ping PACKAGE is received, sending ping ack");

        mode_of_operation = 2; // 2 corresponds to the listening for the message mode
        
        send_ping_acknowledgement();  
        send_ping_acknowledgement();  
        send_ping_acknowledgement();  
                     
        return;
      }

      else if(intention == 2){ // Acknowledgement is received

        Serial.println("Ping ACKNOWLEDGEMENT is received.");
      
        return;
      }
    }
  }
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

  uint16_t CRC_8 = generate_CRC_8_bit();
  set_buffer(3, CRC_8);
  
  // unsigned int current_time = millis()/1000;
  
  transmit_buffer(); 

  // listen_test_2();


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

  uint16_t CRC_8 = generate_CRC_8_bit();
  set_buffer(2, CRC_8);
  
  // unsigned int current_time = millis()/1000;
  
  transmit_buffer(); 


}

// The message is 8 bytes
void send_message_8_bytes()
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

  uint16_t CRC_8 = generate_CRC_8_bit();
  set_buffer(6, CRC_8);
  
  // ----------------------------------

  // unsigned int current_time = millis()/1000;

  Serial.println("\n Sending message package...");    
 
  transmit_buffer(); 

  Serial.println("\n Transmission of message is completed, waiting for acknowledgement...");  

  //delay(10); // ??
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

  uint16_t CRC_8 = generate_CRC_8_bit();
  set_buffer(2, CRC_8);
  
  // unsigned int current_time = millis()/1000;
  
  transmit_buffer(); 

}

// Listening to message 4 bytes
uint8_t listen_to_message(){

  unsigned long listen_duration = 245;

  Serial.println("\n Listening for the message...");
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

      if(intention == 3){
        Serial.println("Message is received, transmission was SUCCESSFUL.");
        return 1;
        mode_of_operation = 1; // back to pinging
      }
      else{
        
        Serial.println("Message NOT received, transmission FAILED.");
      
        return 0;
      }
    }
  }
  //delay(10);
}

// Listen to message of 8 bytes
uint8_t listen_to_message_8_bytes(){

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

