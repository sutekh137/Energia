// Includes
#include <LiquidCrystal.h>

// Defines
#define TRIGGER  18
#define ECHO  19
#define MS_BETWEEN_READINGS  1000

// Global variables
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

void setup() {
  // Define the number of columns and rows of the LCD.
  lcd.begin(16, 2);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
}

float GetDistanceInches() {
  int lnPulseTravelTime;
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(1000);
  digitalWrite(TRIGGER, LOW);
  lnPulseTravelTime = pulseIn(ECHO, HIGH);
  return (lnPulseTravelTime/2) / 74.07;
}

void loop() {
  float lnDistanceInches;
  lnDistanceInches = GetDistanceInches();
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("D: ");
  lcd.print(lnDistanceInches, 2);
  lcd.print((char)34);
  lcd.print(" ");
  lcd.print(lnDistanceInches * 2.54, 1);
  lcd.print("cm ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("   ");
  lcd.print(lnDistanceInches/12, 2);
  lcd.print((char)39);
  lcd.print(" ");
  lcd.print(lnDistanceInches * 0.0254, 3);
  lcd.print("m ");
  delay(MS_BETWEEN_READINGS);
}
