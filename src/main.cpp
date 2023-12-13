#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

/*
   This sample code demonstrates the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
 */
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// Chip select pin for microSD module
const int chipSelect = 10; 

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// The Adafruit_MPU6050 address
#define MPU6050_ADDR 0x68
int16_t accel_x, accel_y, accel_z;

// MicroSD file
// File dataFile;

void setup() {
	Serial.begin(115200);
 	while (!Serial){
		Serial.println("Serial problema"); //###
		delay(10);
	} // will pause Zero, Leonardo, etc until serial console opens
	// Starts Software Serial with GPS Baud Rate
  	ss.begin(GPSBaud);
	// Initialize the microSD card
	//   if (!SD.begin(chipSelect)) {
	//     Serial.println("MicroSD initialization failed. Check your connections or card.");
	//     return;
	//   }
	// Begin MPU transmittion
	Wire.begin();
	Wire.beginTransmission(MPU6050_ADDR);
  	Wire.write(0x6B);
  	Wire.write(0);
  	Wire.endTransmission(true);
  
	Serial.println();
	Serial.println(F("  Latitude   Longitude     Hour    Acceleration"));
	Serial.println(F("    (deg)      (deg)                 X     Y      Z"));
	Serial.println(F("-----------------------------------------------------"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) {
	unsigned long start = millis();
	do {
		while (ss.available()) {
			gps.encode(ss.read());
		}
	} while (millis() - start < ms);
}
// Functions
static void printFloat(float val, bool valid, short int len, short int prec) {
	if (!valid) {
		while (len-- > 1) {
			Serial.print('*');
			// dataFile.print('*');
		}
		Serial.print(' ');
		// dataFile.print(' ');
	} else {
		Serial.print(val, prec);
		// dataFile.print(val, prec);
		int vi = abs((int)val);
		int flen = prec + (val < 0.0 ? 2 : 1); // . and -
		flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
		for (int i=flen; i<len; ++i) {
			Serial.print(' ');
			// dataFile.print(' ');
		}
	}
	smartDelay(0);
}

static void printInt(unsigned long val, bool valid, short int len) {
	char sz[32] = "*****************";
	if (valid) {
		sprintf(sz, "%ld", val);
	}
	sz[len] = 0;
	for (int i=strlen(sz); i<len; ++i) {
		sz[i] = ' ';
	}
	if (len > 0) {
		sz[len-1] = ' ';
	}
	Serial.print(sz);
	// dataFile.print(sz);
	smartDelay(0);
}

static void printTime(TinyGPSTime &t) {
	if (!t.isValid()) {
		Serial.print(F("******** "));
		// dataFile.print(F("******** "));
	} else {
		char sz[32];
		sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
		Serial.print(sz);
		// dataFile.print(sz);
	}
	smartDelay(0);
}

// static void printStr(const char *str, short int len) {
// 	short int slen = strlen(str);
// 	for (short int i=0; i<len; ++i) {
// 		Serial.print(i<slen ? str[i] : ' ');
// 		dataFile.print(i<slen ? str[i] : ' ');
// 	}
// 	smartDelay(0);
// }

void loop() {
  	/* Get new sensor events with the readings */
	Wire.beginTransmission(MPU6050_ADDR);
  	Wire.write(0x3B);
  	Wire.endTransmission(false);
  	Wire.requestFrom(MPU6050_ADDR, 14, true);
	
	accel_x = (Wire.read() << 8 | Wire.read());
  	accel_y = (Wire.read() << 8 | Wire.read());
  	accel_z = (Wire.read() << 8 | Wire.read());

	// Create a new file
  	// dataFile = SD.open("trainingVestData.txt", FILE_WRITE);

	// printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
	printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
	printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
	// printInt(gps.location.age(), gps.location.isValid(), 5);
	printTime(gps.time);
	// printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
	// printInt(gps.failedChecksum(), true, 9);
 	printInt(accel_x, true, 10);
  	printInt(accel_y, true, 10);
  	printInt(accel_z, true, 10);
	Serial.println();
	// dataFile.println();
	smartDelay(1000);
	// dataFile.close();
	smartDelay(2000);

	if (millis() > 5000 && gps.charsProcessed() < 10) {
		Serial.println(F("No GPS data received: check wiring"));
	}
}
