// IMPORTANT: Comment out unused validators to minimize memory usage, especially when dealing with memory-intensive text-heavy components. Excessive static text can lead to memory issues.
// There is no need to comment the validators in this file also. If they are not called as functions in the loop, the IDE automatically ignores them.

//===============MOTION MODULE VALIDATOR=======================
// The following function executes all of the final functions of the motion module.

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

  // Wait a little
  Serial.println("Waiting...");
  delay(2000);

  // Right and left motor backward ramp down
  Serial.println("Both motors are ramping down to pwm value of: " + String(-max_pwm_val));
  int last_pwm_left_reverse = 0;
  int last_pwm_right_reverse = 0;
  while (get_right_motor_current_pwm() > -max_pwm_val || get_left_motor_current_pwm() > -max_pwm_val) {
    if (get_right_motor_current_pwm() != last_pwm_right_reverse) {
      last_pwm_right_reverse = get_right_motor_current_pwm();
      Serial.println("PWM value| Right: " + String(get_right_motor_current_pwm()) + ", Left: " + String(get_left_motor_current_pwm()));
    }
    if (get_left_motor_current_pwm() != last_pwm_left_reverse) {
      last_pwm_left_reverse = get_left_motor_current_pwm();
      Serial.println("PWM value| Right: " + String(get_right_motor_current_pwm()) + ", Left: " + String(get_left_motor_current_pwm()));
    }
    drive_right_motor(-max_pwm_val);
    drive_left_motor(-max_pwm_val);
  }
  Serial.println("Both motors are at the full backward rated speed.");
  delay(4000);

  // Wait a little
  Serial.println("Waiting...");
  delay(2000);

  // Return to zero
  Serial.println("Both motors are returning to zero.");
  while (get_right_motor_current_pwm() != 0 || get_left_motor_current_pwm() != 0) {
    Serial.println("PWM value| Right: " + String(get_right_motor_current_pwm()) + ", Left: " + String(get_left_motor_current_pwm()));
    drive_right_motor(0);
    drive_left_motor(0);
  }
  Serial.println("Both motors are at zero.");
  Serial.println("=============================================");
  Serial.println("Throwing an error");
  throw_error();

}
