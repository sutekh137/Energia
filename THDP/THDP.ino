
// Includes
#include <LiquidCrystal.h>
#include <DHT22_430.h>
#include <math.h>

// Defines.
#define DHT22_INPUT  15
#define DEGREE_SYMBOL  (char)223
#define MS_BETWEEN_READINGS  5000

// Global variables
LiquidCrystal Display(8, 9, 10, 11, 12, 13);
DHT22 THSensor(DHT22_INPUT);
float gnTempF, gnTempC;

void setup() {
  // Define the number of columns and rows of the LCD.
  Display.begin(16, 2);
  THSensor.begin();
  // Kick things off with a shorter, known pause so that the display 
  // fires up as soon as possible. Readings take a while, so don't make the
  // delay after this initial reading too short...
  TempRH();
  delay(2000);
}

void loop() {
  TempRH();
  delay(MS_BETWEEN_READINGS);
}

void ResetLCDLine(byte pnLineIndex) {
  Display.setCursor(0, pnLineIndex);
  Display.print("                ");
  Display.setCursor(0, pnLineIndex);
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

void TempRH() {
  boolean llSuccess = THSensor.get();
  if (llSuccess) {
    // Sensor reading was sucessful. We can check object properties then display.
    float lnHumidity = THSensor.humidityX10()/10.0;
    // Display temperature.
    gnTempC = THSensor.temperatureX10()/10.0;
    gnTempF = (gnTempC * 9/5) + 32;
    DisplayTempOutputLine(0, "T: ");
    // Display relative humidity.
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
