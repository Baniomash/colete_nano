#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

/*
   This sample code demonstrates the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
 */
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// The Adafruit_MPU6050 object
Adafruit_MPU6050 mpu;

void setup() {
	Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
	// Starts Software Serial with GPS Baud Rate
  ss.begin(GPSBaud);
  // Grants MPU is working properly
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  // MPU configs for acceleration, rotation and temperature
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("MPU6050 Found!");
	Serial.println(F("FullExample.ino"));
	Serial.println(F("An extensive example of many interesting TinyGPSPlus features"));
	Serial.print(F("Testing TinyGPSPlus library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
	Serial.println(F("by Mikal Hart"));
	Serial.println();
	Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum    Acceleration        Rotation   Temperature"));
	Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail       X     Y      Z     X    Y     Z     C°"));
	Serial.println(F("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"));
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
static void printFloat(float val, bool valid, int len, int prec) {
	if (!valid) {
		while (len-- > 1) {
			Serial.print('*');
		}
		Serial.print(' ');
	} else {
		Serial.print(val, prec);
		int vi = abs((int)val);
		int flen = prec + (val < 0.0 ? 2 : 1); // . and -
		flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
		for (int i=flen; i<len; ++i) {
			Serial.print(' ');
		}
	}
	smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len) {
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
	smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t) {
	if (!d.isValid()) {
		Serial.print(F("********** "));
	} else {
		char sz[32];
		sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
		Serial.print(sz);
	}

	if (!t.isValid()) {
		Serial.print(F("******** "));
	} else {
		char sz[32];
		sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
		Serial.print(sz);
	}

	printInt(d.age(), d.isValid(), 5);
	smartDelay(0);
}

static void printStr(const char *str, int len) {
	int slen = strlen(str);
	for (int i=0; i<len; ++i) {
		Serial.print(i<slen ? str[i] : ' ');
	}
	smartDelay(0);
}

void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

	static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

	printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
	printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
	printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
	printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
	printInt(gps.location.age(), gps.location.isValid(), 5);
	printDateTime(gps.date, gps.time);
	printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
	printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
	printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
	printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

	unsigned long distanceKmToLondon =
		(unsigned long)TinyGPSPlus::distanceBetween(
				gps.location.lat(),
				gps.location.lng(),
				LONDON_LAT, 
				LONDON_LON) / 1000;
	printInt(distanceKmToLondon, gps.location.isValid(), 9);

	double courseToLondon =
		TinyGPSPlus::courseTo(
				gps.location.lat(),
				gps.location.lng(),
				LONDON_LAT, 
				LONDON_LON);

	printFloat(courseToLondon, gps.location.isValid(), 7, 2);

	const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

	printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

	printInt(gps.charsProcessed(), true, 6);
	printInt(gps.sentencesWithFix(), true, 10);
	printInt(gps.failedChecksum(), true, 9);
  printFloat(a.acceleration.x, true, 6, 2);
  printFloat(a.acceleration.y, true, 6, 2);
  printFloat(a.acceleration.x, true, 6, 2);
  printFloat(a.gyro.x, true, 6, 2);
  printFloat(a.gyro.y, true, 6, 2);
  printFloat(a.gyro.x, true, 6, 2);
  printFloat(temp.temperature, true, 6, 2);
	Serial.println();

	smartDelay(3000);

	if (millis() > 5000 && gps.charsProcessed() < 10) {
		Serial.println(F("No GPS data received: check wiring"));
	}
}
