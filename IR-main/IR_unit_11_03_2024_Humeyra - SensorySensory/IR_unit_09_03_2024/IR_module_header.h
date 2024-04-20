// IRModule.h

#ifndef IR_MODULE_H
#define IR_MODULE_H
#define SHIFT_REG_INPUT 8
#define SHIFT_REG_CLK_PIN 9

#define IR_RECEIVE_PIN 3
#define IR_LED 5

//delayMicroseconds() very accurately in the range 3 microseconds and up to 16383. Be careful if you are not in this range
#define K_NUMBER_OF_BURSTS 90
#define BURST_HALF_PERIOD_US 13
#define LISTEN_DURATION_MS 20
#define TRANSMIT_SLEEP_TIME 20


extern uint8_t NUMBER_OF_PACKAGE_BYTES;
extern unsigned long TRIGGER_DURATION_US;
extern uint8_t IR_module_buffer[16];

void initialize_IR_module();
uint16_t get_number_of_package_bytes();
void set_number_of_package_bytes(uint16_t new_package_size);
void set_buffer(uint16_t byte_index, uint8_t byte_value);
void reverse_buffer();
void transmit_buffer();
void transmit_zero();
void transmit_one();
uint8_t listen_IR();
uint16_t generate_CRC_16_bit();
uint16_t CRC_16_bit_for_1BYTE(uint16_t data, uint16_t last_data);

#endif // IR_MODULE_H
