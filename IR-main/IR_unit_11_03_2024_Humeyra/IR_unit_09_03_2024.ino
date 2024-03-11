uint8_t mode_of_operation = 0; //0:listen, 1:ping, 2:acknowledgement & waiting for message, 3:sending message
unsigned long last_mode_change = 0;

#include "IR_module_header.h";

void setup() {
  Serial.begin(115200);
  initialize_IR_module();
}

void loop() {
//communication_test();
ping_test();
listen_test();
}



void ping_test() {
  if (mode_of_operation != 1)return;
  Serial.println("\n Pinging the IR module...");

  set_number_of_package_bytes(4);

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

void listen_test(){
  if (mode_of_operation != 0)return;

  unsigned long listen_duration = random(500,600);

  // Serial.println("\nThe random number is: ");
  // Serial.println(random(1000,2000));

  Serial.println("\n  Listening for ping message...");
  unsigned long start_time = millis() ;

  while(millis() - start_time < listen_duration){
    uint8_t listening_result = listen_IR(); //listens for 20ms. 0:no package, 1:successful package, 2:corrupted package
    if (listening_result == 1){
      
      uint8_t first_byte = get_buffer(0);
      uint8_t second_byte = get_buffer(1);

      uint8_t  x = first_byte >> 4;
      uint8_t  y = first_byte % 16;

      uint8_t  intention = second_byte >> 4;
      uint8_t  ID = second_byte % 16;

      Serial.println("x: " + String(x) + " y: " + String(y) + " intention: " + String(intention) + " ID: " + String(ID));

      // Read intention, 1: Request for comm., 2: Acknowledgement, 3: Message
      if(intention == 1){
        
        Serial.println("Ping package is received");
        mode_of_operation = 2;  // 2 corresponds to the listening for the message mode
        
        send_acknowledgement();  
        listen_to_message();
        
        return;
      }
      
      else if(intention == 2){ // Now we will send the whole data

        mode_of_operation = 3;
        send_message();

        return;
      }
    }
  }
  mode_of_operation = 1;

  //check for ping (intention check)
  //if ping is received, send a ping back
}

// Response to request: "Acknowledgement"
// Intention 1: Communication Request   Intention 2: Acknowledgement 

// mode_of_operation = 2, sending acknowledgement
void send_acknowledgement()
{
  for(uint8_t c = 0; c<1; c++ ){

    Serial.println("\n Sending acknowledgement...");

    set_number_of_package_bytes(4);

    //INFO ---------------
    uint8_t x = 3; // 4 bits
    uint8_t y = 4; // 4 bits
    
    uint8_t intention = 2; //4 bits
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

  }

}

void send_message()
{
  for(uint8_t c = 0; c<1; c++ ){

    set_number_of_package_bytes(4);

    //INFO ---------------
    uint8_t x = 3; // 4 bits
    uint8_t y = 4; // 4 bits
    
    uint8_t intention = 2; //4 bits
    uint8_t ID = 5; //4 bits

    uint8_t first_byte = x*16 + y; //x and y position of the robot
    uint8_t second_byte = 3; //intention and ID of the robot
    uint8_t third_byte = 12; //intention and ID of the robot
    uint8_t fourth_byte = 13; //intention and ID of the robot

    //TRANSMIT ---------------
    set_buffer(0, first_byte);
    set_buffer(1, second_byte);

    uint16_t CRC_16 = generate_CRC_16_bit();
    uint8_t CRC_SIG = CRC_16 >> 8;
    uint8_t CRC_LST = CRC_16 % 256;
    set_buffer(2, CRC_SIG);
    set_buffer(3, CRC_LST);
    
    unsigned int current_time = millis()/1000;

    Serial.println("\n Sending message package 1...");    
    transmit_buffer(); 

    set_buffer(0, third_byte);
    set_buffer(1, fourth_byte);
  
    uint16_t CRC_16_2 = generate_CRC_16_bit();
    uint8_t CRC_SIG_2 = CRC_16_2 >> 8;
    uint8_t CRC_LST_2 = CRC_16_2 % 256;
    set_buffer(2, CRC_SIG_2);
    set_buffer(3, CRC_LST_2);
    
    unsigned int current_time_2 = millis()/1000;

    Serial.println("\n Sending message package 2...");    
    transmit_buffer(); 

    Serial.println("\n Transmission of message is completed.");  

  }

  mode_of_operation = 1;

  delay(1000);
}

void listen_to_message(){
  
  unsigned long listen_duration = 50;

  // Serial.println("\nThe random number is: ");
  // Serial.println(random(1000,2000));

  Serial.println("\n Listening for the message...");
  unsigned long start_time = millis() ;

  while(millis() - start_time < listen_duration){
    uint8_t listening_result = listen_IR(); //listens for 20ms. 0:no package, 1:successful package, 2:corrupted package
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