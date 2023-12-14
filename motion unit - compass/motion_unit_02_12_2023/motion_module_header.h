#define in_1 8    //IN1
#define in_2 9    //IN2
#define en_12 10  //Enable Pin (IN1 & IN2)
#define in_3 6    //IN3
#define in_4 7    //IN4
#define en_34 11  //Enable Pin (IN3 & IN4)
#define builtin_LED 13
#define voltage_reading_pin A0

// Parameters for motor operation
extern const int MOTOR_RATED_VOLTAGE;  // The voltage the motor is designed to operate at [volt]
extern float MAX_TERMINAL_VOLTAGE;     // The voltage applied to the motor terminals when H bridge is on [volt]
extern int MAX_PWM_ALLOWED;            // Ensures that the effective voltage on the motor terminals does not exceed the motor's rated voltage. Check calculate_max_pwm_allowed() function! [0,255]

// Parameters for PWM control
extern const unsigned int ABSOLUTE_MAX_STEP_SIZE_ALLOWED;
extern unsigned int PWM_STEP_SIZE;       // The allowed change of PWM after each iteration [0,255]
extern unsigned int PWM_UPDATE_PERIOD;   // The period for updating the PWM value [milliseconds]
extern int RIGHT_MOTOR_CURRENT_PWM;      //Automatically updated by the drive_right_motor();
extern int LEFT_MOTOR_CURRENT_PWM;       //Automatically updated by the drive_left_motor();

// Functions
int get_motor_rated_voltage();
float get_max_terminal_voltage();
int get_max_pwm_allowed();
int get_pwm_step_size();
int get_pwm_update_period();
int get_right_motor_current_pwm();
int get_left_motor_current_pwm();
void set_pwm_step_size(unsigned int new_pwm_step_size);
void initialize_motor_module();
void throw_error();
void haltMotorModule();
int calculate_max_pwm_allowed();
void stopped_heart_sound();
int drive_right_motor(int pwm_aimed);
void drive_left_motor(int pwm_aimed);
