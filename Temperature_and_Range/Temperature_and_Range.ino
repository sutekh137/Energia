
// Includes
#include <LiquidCrystal.h>
#include <DHT22_430.h>
#include <math.h>

// Defines.
// We will use empirically determined reference voltages, as even with a 3.3V
// regulator (and so using 3.3V for the external sensor Vref), the temperature
// was off. Calibrated against a very accurate food thermometer and another digital
// thermometer. (Even when I used the exact voltage measured at the tempersture
// sensor as the reference voltage, temperature read too low.)
#define DHT22_INPUT  15
#define EXTERNAL_REF_VOLTAGE  3.35
#define INTERNAL_REF_VOLTAGE  2.06
#define EXTERNAL_TEMP_SENSOR  A5
#define DEGREE_SYMBOL  (char)223
#define TRIGGER  18
#define ECHO  19
#define MS_BETWEEN_READINGS  2000
#define PIN_CHANGE_MODE  14
#define MIN_MODE  1
#define MAX_MODE  4

// Global variables
LiquidCrystal Display(8, 9, 10, 11, 12, 13);
DHT22 THSensor(DHT22_INPUT);
float gnTempF, gnTempC;
boolean glModeChanged;
byte gnMode = 0;  // [0=Intro], 1=Full Temp > 2=Full Range > 3=TR > 4=Temp/RH > 1=Full Temp...

void setup() {
  // Define the number of columns and rows of the LCD.
  Display.begin(16, 2);
  THSensor.begin();
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  // Use button on an interrupt to change mode.
  pinMode(PIN_CHANGE_MODE, INPUT_PULLUP);
  attachInterrupt(PIN_CHANGE_MODE, ISR_ChangeMode, FALLING);
  analogRead(EXTERNAL_TEMP_SENSOR);
  analogRead(TEMPSENSOR);
  glModeChanged = false;
  ResetLCDLine(0);
  Display.print("*PRESS TO START*");
  while (gnMode == 0) {
    if (gnMode == 0) {
      ResetLCDLine(1);
      Display.print("  T> D> TD> TH  ");
      delay(750);
    }
    if (gnMode == 0) {
      ResetLCDLine(1);
      Display.print("  T >D >TD >TH  ");
      delay(750);
    }  
  }
  gnMode = MIN_MODE;
}

void ISR_ChangeMode() {
  delayMicroseconds(1000);
  if (digitalRead(PIN_CHANGE_MODE) != LOW) {
    return;
  }
  gnMode++;
  if (gnMode > MAX_MODE) {gnMode = 1;}
  glModeChanged = true;
}

void ResetLCDLine(byte pnLineIndex) {
  Display.setCursor(0, pnLineIndex);
  Display.print("                ");
  Display.setCursor(0, pnLineIndex);
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
  Display.print(pcIntro);
  Display.print(gnTempF, 1);
  if (gnTempF >= 0.00 && gnTempF < 100.00) {
    Display.print(DEGREE_SYMBOL);
  }
  Display.print("F ");
  Display.print(gnTempC, 1);
  if (gnTempC >= 0.00 && gnTempC < 100.00) {
    Display.print(DEGREE_SYMBOL);
  }
  Display.print("C");
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
  Display.print("D: ");
  Display.print(lnDistanceInches, 2);
  Display.print((char)34);
  Display.print(" ");
  if (lnDistanceInches * 2.54 > 100) {lnDecFormat = 0;}
  Display.print(lnDistanceInches * 2.54, lnDecFormat);
  Display.print("cm ");
}

void TempRH() {
  boolean llSuccess = THSensor.get();
  if (llSuccess) {
    // Sensor reading was sucessful. We can check object properties then display.
    float lnHumidity = THSensor.humidityX10()/10.0;
    // Display tempeature.
    gnTempC = THSensor.temperatureX10()/10.0;
    gnTempF = (gnTempC * 9/5) + 32;
    DisplayTempOutputLine(0, "T: ");
    // Display Relative Humidity.
    ResetLCDLine(1);
    Display.print("H: ");
    Display.print(lnHumidity, 0);
    Display.print("%");
    // Calculate dew point and display it as well.
    float lnDewPointC = CalcDP(gnTempC, lnHumidity);
    float lnDewPointF = (lnDewPointC * 9/5) + 32;
    Display.print(" DP: ");
    Display.print(lnDewPointF, 0);
    Display.print(DEGREE_SYMBOL);    
    Display.print("F");    
  } 
  else {
    ResetLCDLine(1);
    ResetLCDLine(0);
    Display.print("(No T/RH info.)");
  }
}

float CalcDP(float pnT, float pnRH)
{
  // Approximate dewpoint using the formula from Wikipedia's article on dewpoint

  float lnDP = 0.0;
  float gTRH = 0.0;
  float a = 17.271;
  float b = 237.7;

  gTRH = ((a * pnT)/(b + pnT)) + logf(pnRH/100);
  lnDP = (b * gTRH)/(a - gTRH);

  return lnDP;
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
    case 4:
      TempRH();
      break;
  }
  unsigned long lnElapsed = 0;
  unsigned long lnStart = millis();
  while ((lnElapsed < MS_BETWEEN_READINGS) && !glModeChanged) {
    lnElapsed = millis() - lnStart;
  }
  glModeChanged = false;
}
