// Includes
#include <LiquidCrystal.h>

// Defines.
#define TRIGGER  14
#define ECHO  15
#define MS_BETWEEN_READINGS  2000

// Global variables
LiquidCrystal Display(8, 9, 10, 11, 12, 13);

void setup() {
  // Define the number of columns and rows of the LCD.
  Display.begin(16, 2);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
}

void ResetLCDLine(byte pnLineIndex) {
  Display.setCursor(0, pnLineIndex);
  Display.print("                ");
  Display.setCursor(0, pnLineIndex);
}

float GetDistanceInches() {
  int lnPulseTravelTime;
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  lnPulseTravelTime = pulseIn(ECHO, HIGH);
  return (lnPulseTravelTime/2) / 74.07;
}

void RangeInfo() {
  byte lnDecFormat = 1;
  float lnDistanceInches;
  lnDistanceInches = GetDistanceInches();
  ResetLCDLine(0);
  Display.print("D: ");
  Display.print(lnDistanceInches, 2);
  Display.print((char)34);
  Display.print(" ");
  if (lnDistanceInches * 2.54 > 100) {lnDecFormat = 0;}
  Display.print(lnDistanceInches * 2.54, lnDecFormat);
  Display.print("cm ");
  ResetLCDLine(1);
  Display.print("   ");
  Display.print(lnDistanceInches/12, 2);
  Display.print((char)39);
  Display.print(" ");
  Display.print(lnDistanceInches * 0.0254, 3);
  Display.print("m ");
}

void loop() {
  RangeInfo();
  delay(MS_BETWEEN_READINGS);
}
