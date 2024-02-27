#include <Arduino.h>
#include "HX711.h"
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// HX711 + LOAD CELL circuit 1
const int LOADCELL1_DOUT_PIN = 2;
const int LOADCELL1_SCK_PIN = 3;

// HX711 + LOAD CELL circuit 2
const int LOADCELL2_DOUT_PIN = 8;
const int LOADCELL2_SCK_PIN = 9;

// LEDs
const int LEDS1_PIN = 4;
const int LEDS2_PIN = 5;

// Light Sensor
const int LIGHT_SENSOR_PIN = 6;

// Servo Motors
const int SERVOMOTOR1_PIN = 7;
const int SERVOMOTOR2_PIN = 10;

int numberOfEmptySpaces = 10;

HX711 scale1;
HX711 scale2;

Servo ServoEntry;
Servo ServoExit;

LiquidCrystal_I2C lcd(0x27, 16, 1);

void setup() {
  Serial.begin(57600);

  //LEDs
  pinMode(LEDS1_PIN, OUTPUT);
  pinMode(LEDS2_PIN, OUTPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  
  //WEIGHT SENSOR 1
  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  scale1.set_scale(-250);
  scale1.tare();

  //WEIGHT SENSOR 2
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  scale2.set_scale(-229);
  scale2.tare();

  //SERVO MOTOR ENTRY
  ServoEntry.attach(SERVOMOTOR1_PIN);
  ServoEntry.write(0);
  
  //SERVO MOTOR EXIT
  ServoExit.attach(SERVOMOTOR2_PIN);
  ServoExit.write(0);

  //initialize lcd screen
  lcd.init();
  //turn on the backlight
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Empty spaces:");
  lcd.print(numberOfEmptySpaces);
}

int measureWeightEntry() {
  float entrance_weight = scale1.get_units();
  Serial.print("entrance weight: ");
  Serial.print(entrance_weight, 1);
  Serial.println();

  return entrance_weight;
}

int measureWeightExit() {
  float exit_weight = scale2.get_units();
  Serial.print("exit weight: ");
  Serial.print(exit_weight, 1);
  Serial.println();
  
  return exit_weight;
}

void loop() {
  //LIGHTING SYSTEM
  int light_intensity = digitalRead(6);
  Serial.print("Intensity=");
  Serial.println(light_intensity);
  delay(50);
  if (light_intensity == HIGH) {  //no light
    digitalWrite(LEDS1_PIN, HIGH); //LEDs ON        
    digitalWrite(LEDS2_PIN, HIGH);
    Serial.println("Dark, LEDs ON");
  } else { //light
    digitalWrite(LEDS1_PIN, LOW); //LEDs OFF
    digitalWrite(LEDS2_PIN, LOW);
    Serial.println("Bright ,LEDs OFF");
  }

  int entranceWeight = measureWeightEntry();
  //OPEN SERVOMOTOR ENTRY IF CAR DETECTED
  if (entranceWeight > 25) {
    if(numberOfEmptySpaces > 0) {
    ServoEntry.write(90);
    while(measureWeightEntry() > 25);
    delay(3000);
    ServoEntry.write(0); //plasez elicea servomotorului inapoi la pozitia inchisa
    numberOfEmptySpaces--;

    //afisez numarul actualizat pe serial monitor
    Serial.println("Empty spaces:");
    Serial.println(numberOfEmptySpaces);
   
    //afisez numarul actualizat pe LCD
    lcd.clear();
    lcd.backlight();
    lcd.print("Empty spaces:");
    lcd.print(numberOfEmptySpaces);
    }
  }

 //OPEN SERVOMOTOR EXIT IF CAR DETECTED
 int exitWeight = measureWeightExit();
  if (exitWeight > 25) {
    ServoExit.write(90);
    while(measureWeightExit() > 25);
    delay(3000);
    ServoExit.write(0); //plasez elicea servomotorului inapoi la pozitia inchisa
    if(numberOfEmptySpaces<10) {
    numberOfEmptySpaces++;

    //afisez numarul actualizat pe serial monitor
    Serial.println("Empty spaces:");
    Serial.println(numberOfEmptySpaces);
   
    //afisez numarul actualizat pe LCD
    lcd.clear();
    lcd.backlight();
    lcd.print("Empty spaces:");
    lcd.print(numberOfEmptySpaces);
    }
  }
}