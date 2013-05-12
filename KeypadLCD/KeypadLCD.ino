//System/library includes.
#include <LCD_5110.h>

// Defines...
#define TOGGLE_BACKLIGHT  PUSH2

// Instantiate the Nokia 5110 LCD class (non-SPI):
// LCD_5110 lcd(Chip Select, Serial Clock, Serial Data, Data/Command, Reset, Backlight, getButton() Trigger)
LCD_5110 lcd(12, 13, 14, 15, 18, 19, 0);
             
boolean	glBacklight = false;

void setup() {
  // Button will toggle back light using an ISR.
  pinMode(TOGGLE_BACKLIGHT, INPUT_PULLUP);
  attachInterrupt(TOGGLE_BACKLIGHT, ISR_ToggleBacklight, FALLING);
  lcd.begin();
  lcd.setBacklight(glBacklight);
  lcd.text(0, 0, "Hello!");
  delay(1000);
  lcd.clear();
  lcd.text(0, 5, "Light off");
}

void loop() {
  lcd.setFont(0);
  lcd.text(0, 5, glBacklight ? "Light on " : "Light off");
  lcd.setFont(1);
  lcd.text(0, 2, " MSP430");
  delay(200);
}

void ISR_ToggleBacklight() {
  delayMicroseconds(1000);
  if (digitalRead(TOGGLE_BACKLIGHT) != LOW) {
    return;
  }
  glBacklight = (glBacklight == 0);
  lcd.setBacklight(glBacklight);
  return;
}
