#include <Arduino.h>
#include <MPU.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

MPU accelSensor;
TinyGPSPlus gps;

void wait(unsigned long);
void getData();
void saveData();

short int xAxis = 0;
short int yAxis = 0;
short int zAxis = 0;
double lat = 0;
double lng = 0;

// short int data[5];

SoftwareSerial softSerial(4, 3); // Software Serial pins (RXPin, TXPin)

int timer = 0;
boolean filesToSave = false;

ISR(TIMER1_COMPA_vect){
  if(timer >= 3){
    timer = 1;
  } else{
    timer++;
  }
  Serial.println(timer);
}

int main() {
  accelSensor.initialize();
  accelSensor.disableTemperature();
  accelSensor.disableGyroscope();

  OCR1A = 15624; //timer to interrupt = ~1sec
  
  TCCR1A |= 0b00000000;
  TCCR1B |= 0b00001101;
  TIMSK1 |= 0b00000010;

  sei();

  Serial.begin(9600);
  softSerial.begin(9600);

  while(1)
	{
    if(softSerial.available()> 0){
      
      if (millis() > 5000 && gps.charsProcessed() < 10){
        Serial.println("No GPS detected: check wiring.");
        while (true);      
      }

      if(!filesToSave && timer == 3){
        accelSensor.readAccelerometer(&xAxis, &yAxis, &zAxis);
        if (gps.encode(softSerial.read())){
          lat = gps.location.lat();
          lng = gps.location.lng();
        }
        getData();
      } else if(filesToSave && timer < 3){
        saveData();
      }

    }else{
      Serial.println("Software Serial not working");
      while (true);
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

void getData(){
  Serial.println("Getting data from sensors!");
  Serial.print("X ");
  Serial.println(xAxis);
  Serial.print("Y ");
  Serial.println(yAxis);
  Serial.print("Z ");
  Serial.println(zAxis);
  Serial.print("Latitude ");
  Serial.println(lat);
  Serial.print("Longitude ");
  Serial.println(lng);
  filesToSave = !filesToSave;
}

void saveData(){
  Serial.println("Saving data colected!");
  filesToSave = !filesToSave;
}