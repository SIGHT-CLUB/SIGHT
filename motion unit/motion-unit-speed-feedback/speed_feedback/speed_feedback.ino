#define M1_ENABLE 3
#define M1_A 4
#define M1_B 5
#define M1_R 5.6  //armature resistance of the motor
#define M1_kb 0.02  // motor back emf constant (V per rpm)


float battery_voltage = 0;
void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(M1_ENABLE, OUTPUT);
  pinMode(M1_A, OUTPUT);
  pinMode(M1_B, OUTPUT);

  delay(50);
  battery_voltage = return_voltage_measurement();
  delay(50);
}

void loop() {
  drive_motor_at_constant_rpm(25);
}

float return_voltage_measurement() {
  int reading = analogRead(A0);
  float voltage = 0.00989736 * (reading);  //V
  return voltage;
}

float determine_motor_speed() {
  //turn-off motor
  digitalWrite(M1_A, HIGH);
  digitalWrite(M1_B, LOW);
  digitalWrite(M1_ENABLE, LOW);

  //wait 1ms
  delay(1);
  float voltage_before_start = return_voltage_measurement();  //V
  digitalWrite(M1_A, HIGH);
  digitalWrite(M1_B, LOW);
  digitalWrite(M1_ENABLE, HIGH);
  delay(1);

  float voltage_after_start = return_voltage_measurement();  //V

  float voltage_difference = voltage_before_start - voltage_after_start;          //V
  float motor_current = 2 * voltage_difference;                                   //A
  float armature_resistance_voltage_drop = M1_R * motor_current;                  //V
  float motor_back_emf = voltage_after_start - armature_resistance_voltage_drop;  //V
  float motor_rpm = motor_back_emf / M1_kb;

  return motor_rpm;
}

void drive_motor_at_constant_rpm(float RPM){
  uint8_t TIMEOUT_MS = 25;

  unsigned long started_at = millis();
  while(millis()-started_at<TIMEOUT_MS){
    if (determine_motor_speed() < RPM){
        digitalWrite(M1_A, HIGH);
        digitalWrite(M1_B, LOW);
        digitalWrite(M1_ENABLE, HIGH);
      }else{
        digitalWrite(M1_A, HIGH);
        digitalWrite(M1_B, LOW);
        digitalWrite(M1_ENABLE, LOW);
      }   
      delay(5);  
  } 

  //assume motor draws 0.6A when loaded. the steady state PWM is approximated as
  float back_emf_at_desired_rpm = M1_kb*RPM;
  float voltage_applied_when_on = 7.4-(0.5 + M1_R)*0.6;
  float duty = back_emf_at_desired_rpm/voltage_applied_when_on;
  uint16_t int_pwm = constrain(int(255*duty), 0, 255);
  analogWrite(M1_ENABLE,int_pwm);

}


