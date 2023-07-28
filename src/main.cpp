// #include <MPU.h>
#include <Wire.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// MPU accelSensor;
TinyGPSPlus gps;

void wait(unsigned long);
void getData();
void saveData();

int16_t xAxis = 0, yAxis = 0, zAxis = 0;
float lat = 0, lng = 0;

SoftwareSerial softSerial(3, 4);  // Software Serial pins (RXPin, TXPin)

int timer = 0;
boolean filesToSave = false;

ISR(TIMER1_COMPA_vect) {
  if (timer >= 3) {
    timer = 1;
  } else {
    timer++;
  }
  Serial.println(timer);
}

int main() {
  // accelSensor.initialize();
  // accelSensor.disableTemperature();
  // accelSensor.disableGyroscope();

  OCR1A = 15624;  //timer to interrupt = ~1sec

  TCCR1A |= 0b00000000;
  TCCR1B |= 0b00001101;
  TIMSK1 |= 0b00000010;

  sei();

  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.begin(9600);
  softSerial.begin(9600);

  while (1) {
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println("No GPS detected: check wiring.");
      while (true)
        ;
    }
    while (softSerial.available() > 0){
      gps.encode(softSerial.read());
    }
    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 6, true);


    if (!filesToSave && timer == 3) {
      if (gps.location.isUpdated()){
        lat = gps.location.lat();
        lng = gps.location.lng();
      }
      xAxis = Wire.read() << 8 | Wire.read();
      yAxis = Wire.read() << 8 | Wire.read();
      zAxis = Wire.read() << 8 | Wire.read();
      getData();
    } else if (filesToSave && timer < 3) {
      saveData();
    }
  }
}


// void wait(unsigned long milliseconds)
// {
//   unsigned long currentTime = millis();
//   unsigned long previousTime = millis();

//   while (currentTime - previousTime <= milliseconds)
//   {
//     currentTime = millis();
//   }
// }

void getData() {
  Serial.println("Getting data from sensors!");
  Serial.print("X ");
  Serial.println(xAxis);
  Serial.print("Y ");
  Serial.println(yAxis);
  Serial.print("Z ");
  Serial.println(zAxis);
  Serial.print("Latitude ");
  Serial.println(lat, 6);
  Serial.print("Longitude ");
  Serial.println(lng, 6);
  filesToSave = !filesToSave;
}

void saveData() {
  Serial.println("Saving data colected!");
  filesToSave = !filesToSave;
}