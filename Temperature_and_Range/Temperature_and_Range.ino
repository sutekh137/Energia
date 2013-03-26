// Includes
#include <LiquidCrystal.h>

// Defines.
// We will use empirically determined reference voltages, as even with a 3.3V
// regulator (and so using 3.3V for the external sensor Vref), the temperature
// was off. Calibrated against a very accurate food thermometer and another digital
// thermometer. (Even when I used the exact voltage measured at the tempersture
// sensor as the reference voltage, temperature read too low.)
#define EXTERNAL_REF_VOLTAGE  3.35
#define INTERNAL_REF_VOLTAGE  2.06
#define EXTERNAL_TEMP_SENSOR  A5
#define DEGREE_SYMBOL  (char)223
#define TRIGGER  18
#define ECHO  19
#define MS_BETWEEN_READINGS  2000
#define PIN_CHANGE_MODE  14
#define DEFAULT_MODE  1

// Global variables
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
float gnTempF, gnTempC;
boolean glModeChanged;
byte gnMode = 0;  // [0=Intro], 1=Full Temp > 2=Full Range > 3=Both > 1=Full Temp...

void setup() {
  // Define the number of columns and rows of the LCD.
  lcd.begin(16, 2);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  // Use button on an interrupt to change mode.
  pinMode(PIN_CHANGE_MODE, INPUT_PULLUP);
  attachInterrupt(PIN_CHANGE_MODE, ISR_ChangeMode, FALLING);
  analogRead(EXTERNAL_TEMP_SENSOR);
  analogRead(TEMPSENSOR);
  glModeChanged = false;
  ResetLCDLine(0);
  lcd.print("*PRESS TO START*");
  while (gnMode == 0) {
    if (gnMode == 0) {
      ResetLCDLine(1);
      lcd.print("Temp> Dist> Both");
      delay(750);
    }
    if (gnMode == 0) {
      ResetLCDLine(1);
      lcd.print("Temp >Dist >Both");
      delay(750);
    }  
  }
  gnMode = DEFAULT_MODE;
}

void ISR_ChangeMode() {
  delayMicroseconds(1000);
  if (digitalRead(PIN_CHANGE_MODE) != LOW) {
    return;
  }
  gnMode++;
  if (gnMode > 3) {gnMode = 1;}
  glModeChanged = true;
}

void ResetLCDLine(byte pnLineIndex) {
  lcd.setCursor(0, pnLineIndex);
  lcd.print("                ");
  lcd.setCursor(0, pnLineIndex);
}

float GetTempC(byte pnReadFrom, float pnRefVoltage) {
  float lnVoltage = (analogRead(pnReadFrom) * pnRefVoltage)/1023;
  return (lnVoltage - 0.5) * 100;
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

void DisplayTempOutputLine(byte pnDisplayLine, char* pcIntro) {
  ResetLCDLine(pnDisplayLine);
  lcd.print(pcIntro);
  lcd.print(gnTempF, 1);
  if (gnTempF >= 0.00) {
    lcd.print(DEGREE_SYMBOL);
  }
  lcd.print("F ");
  lcd.print(gnTempC, 1);
  if (gnTempC >= 0.00) {
    lcd.print(DEGREE_SYMBOL);
  }
  lcd.print("C");
  return;
}

void FullTempInfo() {
  // External temperature reading.
  gnTempC = GetTempC(EXTERNAL_TEMP_SENSOR, EXTERNAL_REF_VOLTAGE);
  gnTempF = (gnTempC * 9/5) + 32;
  DisplayTempOutputLine(0, "E: ");
  // Internal temperature reading.
  gnTempC = GetTempC(TEMPSENSOR, INTERNAL_REF_VOLTAGE);
  gnTempF = (gnTempC * 9/5) + 32;
  DisplayTempOutputLine(1, "I: ");
}

void FullRangeInfo() {
  byte lnDecFormat = 1;
  float lnDistanceInches;
  lnDistanceInches = GetDistanceInches();
  ResetLCDLine(0);
  lcd.print("D: ");
  lcd.print(lnDistanceInches, 2);
  lcd.print((char)34);
  lcd.print(" ");
  if (lnDistanceInches * 2.54 > 100) {lnDecFormat = 0;}
  lcd.print(lnDistanceInches * 2.54, lnDecFormat);
  lcd.print("cm ");
  ResetLCDLine(1);
  lcd.print("   ");
  lcd.print(lnDistanceInches/12, 2);
  lcd.print((char)39);
  lcd.print(" ");
  lcd.print(lnDistanceInches * 0.0254, 3);
  lcd.print("m ");
}

void BothInfo() {
  byte lnDecFormat = 1;
  // External temperature reading.
  gnTempC = GetTempC(EXTERNAL_TEMP_SENSOR, EXTERNAL_REF_VOLTAGE);
  gnTempF = (gnTempC * 9/5) + 32;
  DisplayTempOutputLine(0, "T: ");
  // Range reading.
  float lnDistanceInches;
  lnDistanceInches = GetDistanceInches();
  ResetLCDLine(1);
  lcd.print("D: ");
  lcd.print(lnDistanceInches, 2);
  lcd.print((char)34);
  lcd.print(" ");
  if (lnDistanceInches * 2.54 > 100) {lnDecFormat = 0;}
  lcd.print(lnDistanceInches * 2.54, lnDecFormat);
  lcd.print("cm ");
}

void loop() {
  switch (gnMode) {
    case 1:
      FullTempInfo();
      break;
    case 2:
      FullRangeInfo();
      break;
    case 3:
      BothInfo();
      break;
  }
  unsigned long lnElapsed = 0;
  unsigned long lnStart = millis();
  while ((lnElapsed < MS_BETWEEN_READINGS) && !glModeChanged) {
    lnElapsed = millis() - lnStart;
  }
  glModeChanged = false;
}
