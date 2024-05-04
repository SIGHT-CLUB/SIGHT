/*
===============================================================================================================
QMC5883LCompass.h Library XYZ Example Sketch
Learn more at [https://github.com/mprograms/QMC5883LCompass]

This example shows how to get the XYZ values from the sensor.

===============================================================================================================
Release under the GNU General Public License v3
[https://www.gnu.org/licenses/gpl-3.0.en.html]
===============================================================================================================
*/
#include <QMC5883LCompass.h>

#define x_normalizer 0.73
#define y_normalizer 0.85
#define z_normalizer 0.96
QMC5883LCompass compass;

void setup() {
  Serial.begin(9600);
  compass.init();
}


void loop() {
  // float angle = return_angle();
  // Serial.println(angle);

  float deviation = return_angle_deviation(58);
  Serial.println(deviation);
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
