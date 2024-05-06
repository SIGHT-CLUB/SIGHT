#include <QMC5883LCompass.h>

#define x_normalizer 0.73
#define y_normalizer 0.85
#define z_normalizer 0.96
#define rightMotor1 2  //right motor 240RPM
#define rightMotor2 3
#define right_pwm_normalizer 1
#define rightMotorPWM 9
#define leftMotor1 4  //right motor 320RPM
#define leftMotor2 5
#define leftMotorPWM 10
#define left_pwm_normalizer 1
uint8_t base_pwm = 70;
QMC5883LCompass compass;
float reference;
float return_angle();
void rotate_ccw();
void rotate_cw();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  compass.init();
  pinMode(rightMotor1, OUTPUT);
  pinMode(rightMotor2, OUTPUT);
  pinMode(rightMotorPWM, OUTPUT);
  pinMode(leftMotor1, OUTPUT);
  pinMode(leftMotor2, OUTPUT);
  pinMode(leftMotorPWM, OUTPUT);
  reference=return_angle();

}

void loop() {
  // put your main code here, to run repeatedly:
  
  //float angle = return_angle();
  //Serial.println(angle);

 // float  desired_angle=(angle+90)%360;
  float deviation = return_angle_deviation(reference);
  Serial.println(deviation);
  move(deviation);
  //rotate_ccw();

}


float return_angle() {
  //https://stackoverflow.com/questions/57120466/how-to-calculate-azimuth-from-x-y-z-values-from-magnetometer
  int MEAN_COUNT = 5;

  int x = 0;
  int y = 0;
  int z = 0;

  // Read compass values
  compass.read();
  // Precompute the tilt compensation parameters to improve readability.
  float phi = 0;    //TODO: needs gyro to dynamically calculate
  float theta = 0;  //TODO: needs gyro to dynamically calculate

  float bearing = 0;
  for (uint8_t i = 0; i < MEAN_COUNT; i++) {
    x = compass.getX();
    y = compass.getY();
    z = compass.getZ();


    // Precompute cos and sin of pitch and roll angles to make the calculation a little more efficient.
    float sinPhi = sin(phi);
    float cosPhi = cos(phi);
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);

    // Calculate the tilt compensated bearing, and convert to degrees.
    bearing += (360 * atan2(x * cosTheta + y * sinTheta * sinPhi + z * sinTheta * cosPhi, z * sinPhi - y * cosPhi)) / (2 * PI);
  }
  bearing = bearing / MEAN_COUNT;

  // Ensure the calculated bearing is in the 0..359 degree range.
  if (bearing < 0)
    bearing += 360.0f;
  else if (bearing > 360) {
    bearing -= 360.0f;
  }

  return bearing;
}

float return_angle_deviation(float desired_angle) {
  float angle_now = return_angle();
  // Calculate the deviation
  float angle_deviation = desired_angle - angle_now;

  // Normalize the deviation to be within the range [-180, 180)
  if (angle_deviation >= 180) {
    angle_deviation = 360 - angle_deviation;
  } else if (angle_deviation < -180) {
    angle_deviation += 360;
  }
  return angle_deviation;
}

void move(float angle_error) {

  if (-30< angle_error && angle_error < 30) {
    
    angle_error = -angle_error;
    int right_pwm = int((base_pwm+ 4 * angle_error) * right_pwm_normalizer);
    int left_pwm = int((base_pwm + 4 * angle_error) * left_pwm_normalizer);
    drive_left_motor_at(left_pwm, 25, 3);
    drive_right_motor_at(right_pwm, 25, 3);
    Serial.println("solmotorhızlandı");
  } else {
    stopmotor();

  }
}

void drive_right_motor_at(int pwm_val, int update_period_ms, uint8_t delta_pwm_per_period) {
  static unsigned long last_time_update = 0;
  static uint16_t actual_pwm = 0;
  if (millis() - last_time_update < update_period_ms) return;
  last_time_update = millis();

  int delta_actual_pwm = pwm_val - actual_pwm;

  if (delta_actual_pwm < -delta_pwm_per_period) {
    delta_actual_pwm = -delta_pwm_per_period;
  } else if (delta_actual_pwm > delta_pwm_per_period) {
    delta_actual_pwm = delta_pwm_per_period;
  }
  actual_pwm = actual_pwm + delta_actual_pwm;

  //------------

  if (actual_pwm < 0) {  //go reverse
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    analogWrite(rightMotorPWM, actual_pwm);
  } else {
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, HIGH);
    analogWrite(rightMotorPWM, actual_pwm);
  }
}

void drive_left_motor_at(int pwm_val, int update_period_ms, uint8_t delta_pwm_per_period) {
  static unsigned long last_time_update = 0;
  static uint16_t actual_pwm = 0;
  if (millis() - last_time_update < update_period_ms) return;
  last_time_update = millis();

  int delta_actual_pwm = pwm_val - actual_pwm;

  if (delta_actual_pwm < -delta_pwm_per_period) {
    delta_actual_pwm = -delta_pwm_per_period;
  } else if (delta_actual_pwm > delta_pwm_per_period) {
    delta_actual_pwm = delta_pwm_per_period;
  }
  actual_pwm = actual_pwm + delta_actual_pwm;

  //-------------
  if (actual_pwm < 0) {  //go reverse
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    analogWrite(leftMotorPWM, -actual_pwm);
  } else {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    analogWrite(leftMotorPWM, actual_pwm);
  }
}

  
  void rotate_ccw() {
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, HIGH);
  analogWrite(leftMotorPWM, base_pwm);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, HIGH);
  analogWrite(rightMotorPWM, base_pwm);
}
void rotate_cw() {
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  analogWrite(leftMotorPWM, base_pwm);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  analogWrite(rightMotorPWM, base_pwm);
}
void stopmotor() {
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  analogWrite(leftMotorPWM, base_pwm);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);
  analogWrite(rightMotorPWM, base_pwm);
}
