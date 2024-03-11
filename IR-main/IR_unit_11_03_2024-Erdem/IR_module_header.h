// IRModule.h

#ifndef IR_MODULE_H
#define IR_MODULE_H

#define IR_RECEIVE_PIN 3
#define IR_LED 5  //NORMALDE 5 --- NOTENOTENOTENOTENOTENOTENOTENOTENOTE !!!!!!!!!!!!!!!!!!!!!  

//delayMicroseconds() very accurately in the range 3 microseconds and up to 16383. Be careful if you are not in this range
#define K_NUMBER_OF_BURSTS 60
#define BURST_HALF_PERIOD_US 13
#define LISTEN_DURATION_MS 50
#define TRANSMIT_SLEEP_TIME 20


extern uint8_t NUMBER_OF_PACKAGE_BYTES;
extern unsigned long TRIGGER_DURATION_US;
extern uint8_t IR_module_buffer[8];

void initialize_IR_module();
uint16_t get_number_of_package_bytes();
void set_number_of_package_bytes(uint16_t new_package_size);
void set_buffer(uint16_t byte_index, uint8_t byte_value);
void reverse_buffer();
void transmit_buffer();
void transmit_zero();
void transmit_one();
void transmit_chirping(unsigned long duration_ms);
uint8_t is_receiving_signal();
uint8_t get_buffer(uint16_t byte_index);
uint8_t listen_IR();
uint16_t generate_CRC_16_bit();
uint16_t CRC_16_bit_for_1BYTE(uint16_t data, uint16_t last_data);

#endif // IR_MODULE_H
