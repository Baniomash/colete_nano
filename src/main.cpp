#include <Arduino.h>
#include <MPU.h>
#include <avr/io.h>
#include <avr/interrupt.h>


MPU accelSensor;

void wait(unsigned long);
void getData();
void saveData();

short int xAxis = 0;
short int yAxis = 0;
short int zAxis = 0;

short int data[5];

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

  while(1)
	{
    if(!filesToSave && timer == 3){
    accelSensor.readAccelerometer(&xAxis, &yAxis, &zAxis);
      getData();
    } else if(filesToSave && timer < 3){
      saveData();
    }
	}
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

void getData(){
  Serial.println("Getting data from sensors!");
  data[0] = xAxis;
  data[1] = yAxis;
  data[2] = zAxis;
  Serial.print("X ");
  Serial.println(data[0]);
  Serial.print("Y ");
  Serial.println(data[1]);
  Serial.print("Z ");
  Serial.println(data[2]);
  filesToSave = !filesToSave;
}

void saveData(){
  Serial.println("Saving data colected!");
  filesToSave = !filesToSave;
}