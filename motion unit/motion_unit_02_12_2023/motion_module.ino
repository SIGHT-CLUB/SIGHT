#define in_1 8    //IN1
#define in_2 9    //IN2
#define en_12 10  //Enable Pin (IN1 & IN2)
#define in_3 6    //IN3
#define in_4 7    //IN4
#define en_34 11  //Enable Pin (IN3 & IN4)
#define builtin_LED 13
#define voltage_reading_pin A0

// Parameters for motor operation
const int MOTOR_RATED_VOLTAGE = 5.5;  // The voltage the motor is designed to operate at [volt]
float MAX_TERMINAL_VOLTAGE = 6;       // The voltage applied to the motor terminals when H bridge is on [volt]
int MAX_PWM_ALLOWED = 255;            // Ensures that the effective voltage on the motor terminals does not exceed the motor's rated voltage. Check calculate_max_pwm_allowed() function! [0,255]

// Parameters for PWM control
const unsigned int ABSOLUTE_MAX_STEP_SIZE_ALLOWED = 10;
unsigned int PWM_STEP_SIZE = 1;       // The allowed change of PWM after each iteration [0,255]
unsigned int PWM_UPDATE_PERIOD = 15;  // The period for updating the PWM value [milliseconds]
int RIGHT_MOTOR_CURRENT_PWM = 0;      //Automatically updated by the drive_right_motor();
int LEFT_MOTOR_CURRENT_PWM = 0;       //Automatically updated by the drive_left_motor();

// ================== FINISHED FUNCTIONS ==============================
// The functions written in this section are ready to use.

//Getters & setters
int get_motor_rated_voltage() {
  return MOTOR_RATED_VOLTAGE;
}
float get_max_terminal_voltage() {
  return MAX_TERMINAL_VOLTAGE;
}
int get_max_pwm_allowed() {
  return MAX_PWM_ALLOWED;
}
int get_pwm_step_size() {
  return PWM_STEP_SIZE;
}
int get_pwm_update_period() {
  return PWM_UPDATE_PERIOD;
}
int get_right_motor_current_pwm() {
  return RIGHT_MOTOR_CURRENT_PWM;
}
int get_left_motor_current_pwm() {
  return LEFT_MOTOR_CURRENT_PWM;
}
void set_pwm_step_size(unsigned int new_pwm_step_size) {
  if (0 < new_pwm_step_size && new_pwm_step_size <= ABSOLUTE_MAX_STEP_SIZE_ALLOWED) {
    PWM_STEP_SIZE = new_pwm_step_size;
  }
}

//Single call functions
void initialize_motor_module() {
  //The LED blinking is put here so that we can detect reboots. May be removed in the future.
  pinMode(builtin_LED, OUTPUT);
  for (uint8_t i = 0; i < 0; i++) {
    digitalWrite(builtin_LED, HIGH);
    delay(100);
    digitalWrite(builtin_LED, LOW);
    delay(100);
  }
  pinMode(in_1, OUTPUT);
  pinMode(in_2, OUTPUT);
  pinMode(en_12, OUTPUT);
  pinMode(in_3, OUTPUT);
  pinMode(in_4, OUTPUT);
  pinMode(en_34, OUTPUT);
  pinMode(voltage_reading_pin, INPUT);

  calculate_max_pwm_allowed();

  delay(100);  // While not strictly necessary, it's considered good practice to include this delay after initialization.
}

void throw_error() {
  // Halts the code and indicates an unexpected result is happened
  while (true) {
    haltMotorModule();      // TODO: When combined with other modules, this function should stop all other modules to
    stopped_heart_sound();  // Annoying heart beat sound
  }
}

void haltMotorModule() {
  digitalWrite(in_1, LOW);
  digitalWrite(in_2, LOW);
  digitalWrite(en_12, LOW);
  digitalWrite(in_3, LOW);
  digitalWrite(in_4, LOW);
  digitalWrite(en_34, LOW);
}

int calculate_max_pwm_allowed() {
  MAX_PWM_ALLOWED = floor((MOTOR_RATED_VOLTAGE / MAX_TERMINAL_VOLTAGE) * 255);  //do not use round because it may round 255.X to 256. Which is not expected by the analog write
  MAX_PWM_ALLOWED = min(MAX_PWM_ALLOWED, 255);
}

void stopped_heart_sound() {
  const int dotDelay = 40;  // Time for a dot in milliseconds
  const int silenceDelay = dotDelay / 5;
  const int wordDelay = 80 * dotDelay;  // Time between words is 7 times the dot delay

  // Define arrays to store the durations of sounds and silences
  const int soundDurations[] = { 7, 3, 7, 3, 5, 3, 5, 3, 5, 2, 4, 2, 4, 2, 4, 2, 4, 2, 40 };
  const int silenceDurations[] = { 4, 4, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 10, 1 };

  for (int i = 0; i < sizeof(soundDurations) / sizeof(soundDurations[0]); i++) {
    digitalWrite(builtin_LED, HIGH);
    delay(soundDurations[i] * dotDelay);
    digitalWrite(builtin_LED, LOW);
    delay(silenceDurations[i] * dotDelay);
  }

  delay(wordDelay);  // Time between words
}

//Frequently used functions

int drive_right_motor(int pwm_aimed) {
  static unsigned long updateTime = 0;

  unsigned long currentTime = millis();                  // Get the current time since the arduino starts in miliseconds
  unsigned long elapsedTime = currentTime - updateTime;  // Calculate elapsed time in miliseconds
  if (elapsedTime >= PWM_UPDATE_PERIOD) {

    // Update pwm current ======================================
    int pwm_difference = pwm_aimed - RIGHT_MOTOR_CURRENT_PWM;
    uint8_t pwm_change = 0;
    if (pwm_difference > 0) {  //increase PWM
      RIGHT_MOTOR_CURRENT_PWM += min(PWM_STEP_SIZE, pwm_difference);
    } else if (pwm_difference < 0) {  //decrease PWM
      RIGHT_MOTOR_CURRENT_PWM -= min(PWM_STEP_SIZE, abs(pwm_difference));
    }

    // drive right motor =======================================
    if ((pwm_aimed < -MAX_PWM_ALLOWED) || (MAX_PWM_ALLOWED < pwm_aimed)) {
      // If the index exceeds the expected range, it's a critical error. Users of this function must ensure the input falls within the range of [-MAX_PWM_ALLOWED, MAX_PWM_ALLOWED] and is an integer.
      // In case of an error, the algorithm throws an error and halts the flow so that it can be detected
      throw_error();
    } else if ((0 <= RIGHT_MOTOR_CURRENT_PWM) && (RIGHT_MOTOR_CURRENT_PWM <= MAX_PWM_ALLOWED)) {
      // forward motion
      digitalWrite(in_1, HIGH);
      digitalWrite(in_2, LOW);
      analogWrite(en_12, RIGHT_MOTOR_CURRENT_PWM);

    } else if ((-MAX_PWM_ALLOWED <= RIGHT_MOTOR_CURRENT_PWM) && (RIGHT_MOTOR_CURRENT_PWM < 0)) {
      // "backward motion
      digitalWrite(in_1, LOW);
      digitalWrite(in_2, HIGH);
      analogWrite(en_12, -RIGHT_MOTOR_CURRENT_PWM);
    }

    //update the updateTime
    updateTime = currentTime;
  }
}

void drive_left_motor(int pwm_aimed) {
  static int pwm_current = 0;
  static unsigned long updateTime = 0;

  unsigned long currentTime = millis();                  // Get the current time since the arduino starts in miliseconds
  unsigned long elapsedTime = currentTime - updateTime;  // Calculate elapsed time in miliseconds
  if (elapsedTime >= PWM_UPDATE_PERIOD) {

    // Update pwm current ======================================
    int pwm_difference = pwm_aimed - LEFT_MOTOR_CURRENT_PWM;
    uint8_t pwm_change = 0;
    if (pwm_difference > 0) {  //increase PWM
      LEFT_MOTOR_CURRENT_PWM += min(PWM_STEP_SIZE, pwm_difference);
    } else if (pwm_difference < 0) {  //decrease PWM
      LEFT_MOTOR_CURRENT_PWM -= min(PWM_STEP_SIZE, abs(pwm_difference));
    }

    // drive right motor =======================================
    if ((pwm_aimed < -MAX_PWM_ALLOWED) || (MAX_PWM_ALLOWED < pwm_aimed)) {
      // If the index exceeds the expected range, it's a critical error. Users of this function must ensure the input falls within the range of [-MAX_PWM_ALLOWED, MAX_PWM_ALLOWED] and is an integer.
      // In case of an error, the algorithm is halted, and the built-in LED blinks a message in Morse code to notify the user :)
      while (true) {
        haltMotorModule();      // TODO: When combined with other modules, this function should stop all other modules to
        stopped_heart_sound();  // Annoying heart beat sound
      }
    } else if ((0 <= LEFT_MOTOR_CURRENT_PWM) && (LEFT_MOTOR_CURRENT_PWM <= MAX_PWM_ALLOWED)) {
      // forward motion
      digitalWrite(in_4, HIGH);
      digitalWrite(in_3, LOW);
      analogWrite(en_34, LEFT_MOTOR_CURRENT_PWM);

    } else if ((-MAX_PWM_ALLOWED <= LEFT_MOTOR_CURRENT_PWM) && (LEFT_MOTOR_CURRENT_PWM < 0)) {
      // "backward motion
      digitalWrite(in_4, LOW);
      digitalWrite(in_3, HIGH);
      analogWrite(en_34, -LEFT_MOTOR_CURRENT_PWM);
    }

    //update the updateTime
    updateTime = currentTime;
  }
}

//===============VALIDATOR=======================
//The function written here runs all of the final functions. It can be considered as a test function but uses real codes and prints the steps
void validator_validate_motion_module() {
  Serial.println("\n=============================================");
  Serial.println("Testing getters and setters");
  Serial.println("Motor rated voltage  (V) :" + String(get_motor_rated_voltage()));
  Serial.println("Max terminal voltage (V) :" + String(get_max_terminal_voltage()));
  Serial.println("PWM step size            :" + String(get_pwm_step_size()));
  Serial.println("Motor rated voltage      :" + String(get_pwm_update_period()));
  Serial.println("Max allowed PWM          :" + String(get_max_pwm_allowed()));
  delay(1000);
  Serial.println("=============================================");
  Serial.println("Global variables");
  Serial.println("ABSOLUTE_MAX_STEP_SIZE_ALLOWED: " + String(ABSOLUTE_MAX_STEP_SIZE_ALLOWED));
  Serial.println("PWM_UPDATE_PERIOD: " + String(PWM_UPDATE_PERIOD));
  Serial.println("MOTOR_RATED_VOLTAGE: " + String(MOTOR_RATED_VOLTAGE));
  Serial.println("MAX_TERMINAL_VOLTAGE: " + String(MAX_TERMINAL_VOLTAGE));
  Serial.println("MAX_PWM_ALLOWED: " + String(MAX_PWM_ALLOWED));
  delay(1000);
  Serial.println("=============================================");
  Serial.println("Pin definitions");
  Serial.println("in_1: " + String(in_1) + ", in_2: " + String(in_2) + ", en_12: " + String(en_12) + " ");
  Serial.println("in_3: " + String(in_3) + ", in_4: " + String(in_4) + ", en_34: " + String(en_34) + " ");
  Serial.println("builtin_LED: " + String(builtin_LED));
  Serial.println("voltage_reading_pin: " + String(voltage_reading_pin));
  delay(1000);
  Serial.println("=============================================");

  Serial.println("Adjusting PWM to ensure the effective voltage is atmost equal to rated value of the motor. If initialize_motor_module() is called before the validator, it is already set");
  calculate_max_pwm_allowed();
  Serial.println("MAX_PWM_ALLOWED:" + String(get_max_pwm_allowed()));
  Serial.println("=============================================");
  Serial.println("Testing set_pwm_step_size() function");
  delay(1000);
  for (int i = -1.5 * ABSOLUTE_MAX_STEP_SIZE_ALLOWED; i < 1.5 * ABSOLUTE_MAX_STEP_SIZE_ALLOWED; i++) {
    set_pwm_step_size(i);
    Serial.println("Testing set_pwm_step_size() function -> tried value: " + String(i) + " actual step size: " + String(get_pwm_step_size()));
    delay(100);
  }
  set_pwm_step_size(1);
  Serial.println("Step size is set to 1: Actual value:" + String(get_pwm_step_size()));
  Serial.println("=============================================");
  Serial.println("Testing error sound effect");
  delay(1000);
  stopped_heart_sound();
  Serial.println("=============================================");
  Serial.println("Testing drive functions and their responses");
  int max_pwm_val = get_max_pwm_allowed();
  int last_pwm = 0;

  //-----------------------RIGHT--------------------------
  //right motor forward ramp up
  Serial.println("Right motor is ramping up to pwm value of: " + String(max_pwm_val));
  delay(1000);
  while (get_right_motor_current_pwm() < max_pwm_val) {
    if (get_right_motor_current_pwm() != last_pwm) {
      last_pwm = get_right_motor_current_pwm();
      Serial.println("Right motor PWM value: " + String(get_right_motor_current_pwm()));
    }
    drive_right_motor(max_pwm_val);
  }
  Serial.println("Right motor is at the full forward rated speed.");
  delay(1000);

  //right motor forward ramp down
  Serial.println("Right motor is ramping down to pwm value of: 0");
  delay(1000);
  while (get_right_motor_current_pwm() != 0) {
    if (get_right_motor_current_pwm() != last_pwm) {
      last_pwm = get_right_motor_current_pwm();
      Serial.println("Right motor PWM value: " + String(get_right_motor_current_pwm()));
    }
    drive_right_motor(0);
  }
  Serial.println("Right motor is at halt.");
  delay(1000);

  //right motor backward ramp up
  Serial.println("Right motor is ramping up to pwm value of: " + String(-max_pwm_val));
  delay(1000);
  while (get_right_motor_current_pwm() > -max_pwm_val) {
    if (get_right_motor_current_pwm() != last_pwm) {
      last_pwm = get_right_motor_current_pwm();
      Serial.println("Right motor PWM value: " + String(get_right_motor_current_pwm()));
    }
    drive_right_motor(-max_pwm_val);
  }
  Serial.println("Right motor is at the full backward rated speed.");
  delay(1000);

  //right motor backward ramp down
  Serial.println("Right motor is ramping down to pwm value of: 0");
  delay(1000);
  while (get_right_motor_current_pwm() != 0) {
    if (get_right_motor_current_pwm() != last_pwm) {
      last_pwm = get_right_motor_current_pwm();
      Serial.println("Right motor PWM value: " + String(get_right_motor_current_pwm()));
    }
    drive_right_motor(0);
  }
  Serial.println("Right motor is at halt.");
  delay(1000);

  //-----------------------LEFT--------------------------
  // Left motor forward ramp up
  Serial.println("Left motor is ramping up to PWM value of: " + String(max_pwm_val));
  delay(1000);
  while (get_left_motor_current_pwm() < max_pwm_val) {
    if (get_left_motor_current_pwm() != last_pwm) {
      last_pwm = get_left_motor_current_pwm();
      Serial.println("Left motor PWM value: " + String(get_left_motor_current_pwm()));
    }
    drive_left_motor(max_pwm_val);
  }
  Serial.println("Left motor is at the full forward rated speed.");
  delay(10000);

  // Left motor forward ramp down
  Serial.println("Left motor is ramping down to PWM value of: 0");
  delay(1000);
  while (get_left_motor_current_pwm() != 0) {
    if (get_left_motor_current_pwm() != last_pwm) {
      last_pwm = get_left_motor_current_pwm();
      Serial.println("Left motor PWM value: " + String(get_left_motor_current_pwm()));
    }
    drive_left_motor(0);
  }
  Serial.println("Left motor is at halt.");
  delay(1000);

  // Left motor backward ramp up
  Serial.println("Left motor is ramping up to PWM value of: " + String(-max_pwm_val));
  delay(2500);
  while (get_left_motor_current_pwm() > -max_pwm_val) {
    if (get_left_motor_current_pwm() != last_pwm) {
      last_pwm = get_left_motor_current_pwm();
      Serial.println("Left motor PWM value: " + String(get_left_motor_current_pwm()));
    }
    drive_left_motor(-max_pwm_val);
  }
  Serial.println("Left motor is at the full backward rated speed.");
  delay(1000);

  // Left motor backward ramp down
  Serial.println("Left motor is ramping down to PWM value of: 0");
  while (get_left_motor_current_pwm() != 0) {
    if (get_left_motor_current_pwm() != last_pwm) {
      last_pwm = get_left_motor_current_pwm();
      Serial.println("Left motor PWM value: " + String(get_left_motor_current_pwm()));
    }
    drive_left_motor(0);
  }
  Serial.println("Left motor is at halt.");
  delay(1000);

  //----------------RIGHT & LEFT--------------------------
  // Right and left motor forward ramp up
  Serial.println("Both motors are ramping up to pwm value of: " + String(max_pwm_val));
  delay(2500);
  int last_pwm_left = 0;
  int last_pwm_right = 0;
  while (get_right_motor_current_pwm() < max_pwm_val || get_left_motor_current_pwm() < max_pwm_val) {
    if (get_right_motor_current_pwm() != last_pwm_right) {
      last_pwm_right = get_right_motor_current_pwm();
      Serial.println("PWM value| Right: " + String(get_right_motor_current_pwm()) + ", Left: " + String(get_left_motor_current_pwm()));
    }
    if (get_left_motor_current_pwm() != last_pwm_left) {
      last_pwm_left = get_left_motor_current_pwm();
      Serial.println("PWM value| Right: " + String(get_right_motor_current_pwm()) + ", Left: " + String(get_left_motor_current_pwm()));
    }
    drive_right_motor(max_pwm_val);
    drive_left_motor(max_pwm_val);
  }
  Serial.println("Both motors are at the full forward rated speed.");
  delay(4000);
  


  while (true) {
  }
}

//=================================================
