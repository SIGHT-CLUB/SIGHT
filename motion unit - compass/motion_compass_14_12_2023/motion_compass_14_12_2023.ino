#include "motion_module_header.h"
#include <QMC5883LCompass.h>

QMC5883LCompass compass;
int x, y, z, a, b;
int azimuth_offset;
char myArray[3];
int angle_dif;

void setup() {
  Serial.begin(115200);
  initialize_motor_module();
  compass.init();


  compass.read();

  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();
  a = compass.getAzimuth();
  azimuth_offset = a;
  b = compass.getBearing(a);
  compass.getDirection(myArray, a);
}

void loop() {
  //validator_validate_motion_module();

  //Serial.println(get_max_pwm_allowed());
  //drive_right_motor(212);
  //drive_left_motor(212);
  //Serial.println(get_right_motor_current_pwm());

  compass.read();
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();
  a = compass.getAzimuth();
  b = compass.getBearing(a);
  compass.getDirection(myArray, a);
  angle_dif = a - azimuth_offset;
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.print(z);
  Serial.print(" Azimuth: ");
  Serial.print(a);
  Serial.print("  Azimuth_start: ");
  Serial.print(azimuth_offset);
  Serial.print(" Bearing: ");
  Serial.print(b);
  Serial.print(" Direction: ");
  Serial.print(myArray[0]);
  Serial.print(myArray[1]);
  Serial.print(myArray[2]);
  Serial.println();
  Serial.print("Angle_Difference: ");
  Serial.println(angle_dif);

  Serial.print(" ");
  Serial.println();

  if (angle_dif < -9) {
    angle_dif = -9;
  } else if (angle_dif > 9) {
    angle_dif = 9;
  }

  drive_right_motor(110 - (angle_dif * 10));
  drive_left_motor(110 + (angle_dif * 10));
  Serial.print(get_right_motor_current_pwm());
  Serial.print(" ");
  Serial.println(get_left_motor_current_pwm());

}
