// Includes
#include <LiquidCrystal.h>

// Defines
#define EXTERNAL_TEMP_SENSOR  A5
#define EXTERNAL_REF_VOLTAGE  3.3
#define INTERNAL_REF_VOLTAGE  2.1    // Empirical determination.
#define DEGREE_SYMBOL  (char)223
#define MS_BETWEEN_READINGS  2000

// Global variables
float gnTempF, gnTempC;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

void setup() {
  // Define the number of columns and rows of the LCD
  lcd.begin(16, 2);
}

void DisplayTempOutputLine(byte pnDisplayLine, char* pcIntro, float pnF, float pnC) {
  lcd.setCursor(0, pnDisplayLine);
  lcd.print("                ");
  lcd.setCursor(0, pnDisplayLine);
  lcd.print(pcIntro);
  lcd.print(pnF, 1);
  if (pnF >= 0.00) {
    lcd.print(DEGREE_SYMBOL);
  }
  lcd.print("F ");
  lcd.print(pnC, 1);
  if (pnC >= 0.00) {
    lcd.print(DEGREE_SYMBOL);
  }
  lcd.print("C");
  return;
}

void loop() {
  // External temperature reading.
  gnTempC = GetTempC(EXTERNAL_TEMP_SENSOR, EXTERNAL_REF_VOLTAGE);
  gnTempF = (gnTempC * 9/5) + 32;
  DisplayTempOutputLine(0, "E: ", gnTempF, gnTempC);
  // Internal temperature reading.
  gnTempC = GetTempC(TEMPSENSOR, INTERNAL_REF_VOLTAGE);
  gnTempF = (gnTempC * 9/5) + 32;
  DisplayTempOutputLine(1, "I: ", gnTempF, gnTempC);
  delay(MS_BETWEEN_READINGS);
}

float GetTempC(byte pnReadFrom, float pnRefVoltage) {
  float lnVoltage = (analogRead(pnReadFrom) * pnRefVoltage)/1023;
  return (lnVoltage - 0.5) * 100;
}
