#include <Arduino.h>
#include <MPU.h>


MPU accelSensor;

void wait(unsigned long);

short int xAxis = 0;
short int yAxis = 0;
short int zAxis = 0;

void setup() {
  accelSensor.initialize();

  accelSensor.disableTemperature();
  accelSensor.disableGyroscope();

  Serial.begin(9600);
}

void loop() {
  accelSensor.readAccelerometer(&xAxis, &yAxis, &zAxis);
  Serial.println("");
  Serial.print(" X = ");
  Serial.print(xAxis);
  Serial.print(" | Y = ");
  Serial.print(yAxis);
  Serial.print(" | Z = ");
  Serial.println(zAxis);
  wait(5000);
}

void wait(unsigned long milliseconds)
{
  unsigned long currentTime = millis();
  unsigned long previousTime = millis();

  while (currentTime - previousTime <= milliseconds)
  {
    currentTime = millis();
  }
}