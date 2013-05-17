//System/library includes.
#include <LCD_5110.h>

// Defines...
#define PIN_TOGGLE_BACKLIGHT  11
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
#define NO_KEY "\0"

// Instantiate the Nokia 5110 LCD class (non-SPI):
// LCD_5110 lcd(Chip Select, Serial Clock, Serial Data, Data/Command, Reset, Backlight)
LCD_5110 lcd(12, 13, 14, 15, 18, 19);
             
boolean	glBacklight = false;
byte gnRow;
byte gnCol;
byte gaRow[4] = {10, 9, 8, 7};
byte gaCol[4] = { 6, 5, 4, 3};
String gaKey[KEYPAD_ROWS][KEYPAD_COLS] = {
 {"1", "2", "3", "A"},
 {"4", "5", "6", "B"},
 {"7", "8", "9", "C"},
 {"*", "0", "#", "D"}
};

void setup() {
  lcd.begin();
  // Button will toggle back light using an ISR.
  pinMode(PIN_TOGGLE_BACKLIGHT, INPUT_PULLUP);
  attachInterrupt(PIN_TOGGLE_BACKLIGHT, ISR_ToggleBacklight, FALLING);
  // Initialize keypad rows and columns. Rows will be what we send signals across
  // to capture key presses, that is why they all get set to HIGH.
  for (gnCol = 0; gnCol < KEYPAD_COLS; gnCol++) {
    pinMode(gaCol[gnCol], INPUT_PULLUP);
  }
  for (gnRow = 0; gnRow < KEYPAD_ROWS; gnRow++) {
    pinMode(gaRow[gnRow], OUTPUT);
    digitalWrite(gaRow[gnRow], HIGH);
  }
  lcd.setBacklight(glBacklight);
  lcd.text(0, 0, "Hello!");
  delay(1000);
}

void loop() {
  lcd.setFont(1);
  lcd.text(0, 0, "HI!");
  lcd.setFont(0);
  lcd.text(6, 0, "  JoeK");
  lcd.text(6, 1, " Rules!");
  lcd.text(0, 2, "<press key to");
  lcd.text(0, 3, " see below...>");
  lcd.setFont(1);
  lcd.text(6, 4, GetKey());  
  //lcd.text(0, 5, glBacklight ? "Light on " : "Light off");
  //lcd.setFont(1);
  //lcd.text(0, 2, " MSP430");
  delay(100);
}

void ISR_ToggleBacklight() {
  delayMicroseconds(1000);
  if (digitalRead(PIN_TOGGLE_BACKLIGHT) != LOW) {
    return;
  }
  glBacklight = (!glBacklight);
  lcd.setBacklight(glBacklight);
  return;
}

String GetKey() {
  String lcPressed = NO_KEY;
  boolean llGotKey = false;
  for(gnRow = 0; gnRow < KEYPAD_ROWS && !llGotKey; gnRow++) {
    digitalWrite(gaRow[gnRow], LOW);
    for(gnCol = 0; gnCol < KEYPAD_COLS && !llGotKey; gnCol++) {
      if (digitalRead(gaCol[gnCol]) == LOW) {
        lcPressed = gaKey[gnRow][gnCol];
      }
    }
    digitalWrite(gaRow[gnRow], HIGH);
  }
  return lcPressed;
}
