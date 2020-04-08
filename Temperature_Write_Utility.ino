#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>

Adafruit_BME280 bme;
LiquidCrystal_I2C lcd(0x27, 16, 2);

File timeOut;
String date;
String time;
String dateTime;
String dateFile;
String tempFH;
float initDelay = 60000;
float readDelay = 60000;
int buzzerPin = 9;

DS3231  rtc(SDA, SCL);

void setup()
{
  Serial.begin(9600);
  rtc.begin();
  lcd.init();
  lcd.noBacklight();
  pinMode (buzzerPin, OUTPUT);
  delay(10000);

  Serial.println("Initializing BME280...");
  lcd.clear();
  lcd.print("Init BME280");
  delay(1000);

  //check if BME280 is working
  if (!bme.begin(0x76)) {
    lcd.clear();
    lcd.print("BME280 issue.");
    Serial.println("Could not find a BME280 sensor.");
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
    lcd.clear();
    lcd.print("Done.");
    delay(1000);
  }

  delay(1000);

  Serial.println("Initializing SD card...");
  lcd.clear();
  lcd.print("Init SD");
  delay(1000);

  //check if SD card is working
  if (!SD.begin(4)) {
    Serial.println("SD initialization failed. Is your SD card inserted?");
    for (int i = 0; i <= 1; i++) {
      tone(buzzerPin, 800, 300);
      delay(300);
      tone(buzzerPin, 500, 300);
      delay(300);
      lcd.clear();
      lcd.print("SD issue");
      lcd.setCursor(0, 1);
      lcd.print("Check card.");
    }
    while (1);
  }
  else {
    Serial.println("Done.");
    lcd.clear();
    lcd.print("Done.");
    delay(1000);
  }

  delay(1000);

  /*it's annoying to have to delete temp.txt
    every time you disconnect the
    Arduino, so delete it on boot.*/
  if (SD.exists("temp.txt")) {
    Serial.println("Deleting old temp.txt");
    lcd.clear();
    lcd.print("Delete temp.txt");
    SD.remove("temp.txt");
    delay(1000);
    Serial.println("Done.");
    lcd.clear();
    lcd.print("Done.");
    delay(1000);
  }

  /*doesn't matter if it can't delete
    temp.txt, it'll either continue writing
    or error later on in the script.*/
  else {
    Serial.println("Couldn't delete old temp.txt. It probably doesn't exist. Continuing anyway.");
    lcd.clear();
    lcd.print("Cant delete temp");
    lcd.setCursor(0, 1);
    lcd.print("Continuing..");
    delay(2000);
  }

  lcd.clear();
  lcd.print("Waiting 1 min.");
  Serial.print("Waiting ");
  Serial.print(initDelay / 60000);
  Serial.println(" minute.");
  delay(initDelay);
  Serial.println("Okay, reading now.");
}

void loop() {

  //fahrenheit is superior to celsius
  float tempf = bme.readTemperature() * 1.8 + 32;

  //converting tempf to a string is needed to concatenate everything into tempFH below.
  String tempfStr = String(tempf);

  //get time and date with the 'murica date format.
  time = rtc.getTimeStr(FORMAT_SHORT);
  date = rtc.getDateStr(FORMAT_SHORT, FORMAT_MIDDLEENDIAN);

  //concatenate date and time; concatenate temp (in F) and humidity (in %).
  dateTime = date + "," + time + ",";
  tempFH = tempfStr + "," + bme.readHumidity() + "\r\n";

  /*if temp is too high or low, don't write
    until it's back into a normal range
   (or just beep forever)*/
  if ((bme.readTemperature() < -10) || (bme.readTemperature() > 43)) {
    Serial.print(dateTime);
    Serial.print(tempFH);
    Serial.println("Temperature out of range. Check your sensor.");
    lcd.clear();
    lcd.print("Bad temp");
    lcd.setCursor(0, 1);
    lcd.print("Check sensor");
    for (int i = 0; i <= 1; i++) {
      tone(buzzerPin, 800, 300);
      delay(300);
      tone(buzzerPin, 500, 300);
      delay(300);
    }
    delay(5000);
    return;
  }

  //write the date, time, temp, and humidity to a file on the SD card.
  else {
    timeOut = SD.open("temp.txt", FILE_WRITE);

    if (timeOut) {
      timeOut.print(dateTime);
      Serial.print(dateTime);

      timeOut.print(tempFH);
      Serial.print(tempFH);
      Serial.println("Write succeeded.");

      timeOut.close();
      lcd.clear();
      lcd.print("Temp: ");
      lcd.print(tempfStr);
      lcd.setCursor(11, 0);
      lcd.print("F");
      lcd.setCursor(0, 1);
      lcd.print("Hum: ");
      lcd.print(bme.readHumidity());
      lcd.setCursor(10, 1);
      lcd.print("%");

      //or error and try again once the script restarts.
    } else {
      Serial.println("Error opening file. How did this happen?");
      lcd.clear();
      lcd.print("File error.");
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
