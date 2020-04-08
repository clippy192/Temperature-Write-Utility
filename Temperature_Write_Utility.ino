#include <DS3231.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


Adafruit_BME280 bme;

File timeOut;
String date;
String time;
String dateTime;
String dateFile;
String tempFH;
float initDelay = 60000;
float readDelay = 300000;
int buzzerPin = 9;

DS3231  rtc(SDA, SCL);

void setup()
{
  Serial.begin(9600);
  rtc.begin();
  pinMode (buzzerPin, OUTPUT);
  delay(10000);

  Serial.println("Initializing BME280...");

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor.");
    tone(buzzerPin, 800, 300);
    delay(300);
    tone(buzzerPin, 500, 300);
    delay(300);
    while (1);
  }
  else {
    Serial.println("Done.");
  }

  delay(1000);

  Serial.println("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("SD initialization failed. Is your SD card inserted?");
    for (int i = 0; i <= 1; i++) {
      tone(buzzerPin, 800, 300);
      delay(300);
      tone(buzzerPin, 500, 300);
      delay(300);
    }
    while (1);
  }
  else {
    Serial.println("Done.");
  }

  delay(1000);

  if (SD.exists("temp.txt")) {
    Serial.println("Deleting old temp.txt");
    SD.remove("temp.txt");
    Serial.println("Done.");
  }
  else {
    Serial.println("Couldn't delete old temp.txt. It probably doesn't exist. Continuing anyway.");
    delay(1000);
  }

  Serial.print("Waiting ");
  Serial.print(initDelay / 60000);
  Serial.println(" minute.");
  delay(initDelay);
  Serial.println("Okay, reading now.");
}

void loop() {

  float tempf = bme.readTemperature() * 1.8 + 32;
  String tempfStr = String(tempf);

  time = rtc.getTimeStr(FORMAT_SHORT);
  date = rtc.getDateStr(FORMAT_SHORT, FORMAT_MIDDLEENDIAN);
  dateTime = date + "," + time + ",";
  tempFH = tempfStr + "," + bme.readHumidity() + "\r\n";

  if ((bme.readTemperature() < -10) || (bme.readTemperature()> 43)) {
    Serial.print(dateTime);
    Serial.print(tempFH);
    Serial.println("Temperature out of range. Check your sensor.");
    for (int i = 0; i <= 1; i++) {
      tone(buzzerPin, 800, 300);
      delay(300);
      tone(buzzerPin, 500, 300);
      delay(300);
    }
    delay(5000);
    return;
  }
  else {
    timeOut = SD.open("temp.txt", FILE_WRITE);

    if (timeOut) {
      timeOut.print(dateTime);
      Serial.print(dateTime);

      timeOut.print(tempFH);
      Serial.print(tempFH);
      Serial.println("Write succeeded.");

      timeOut.close();
    } else {
      Serial.println("Error opening file. How did this happen?");
      for (int i = 0; i <= 1; i++) {
        tone(buzzerPin, 800, 300);
        delay(300);
        tone(buzzerPin, 500, 300);
        delay(300);
      }
    }
    delay (readDelay);
  }
}
