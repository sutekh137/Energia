//System/library includes.
#include <LCD_5110.h>

// Defines...
#define PIN_TOGGLE_BACKLIGHT  11
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
#define NO_KEY "\0"
// A higher adjustment makes the clock gain time, a lower number
// (including negative) makes the clock lose time.
#define ADJUST_MS_PER_LOOP  103
// Define our hour rollover parameters to establish 12/24-hour preference.
//#define HOUR_MAX 23
//#define HOUR_MIN 0
#define HOUR_MAX 12
#define HOUR_MIN 1

// Instantiate the Nokia 5110 LCD class (non-SPI):
// LCD_5110 lcd(Chip Select, Serial Clock, Serial Data, Data/Command, Reset, Backlight)
LCD_5110 lcd(12, 13, 14, 15, 18, 19);

volatile boolean glClockSet = false;
volatile boolean glBacklight = false;
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
unsigned long gnLastTick = 0;
unsigned long gnMS = 0;
byte gnH =0;
byte gnM = 0;
byte gnS = 0;

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
  lcd.text(0, 0, "Initializing...");
  delay(1000);
  // We always start in clock set mode.
  glClockSet = true;
}

void loop() {
  if (glClockSet) {
    // Display clock set mode.
    DisplayClockSet();
  } else {
    // Display the clock in full mode.
    DisplayClockFull();
  }
  // Check for keypresses....
  CheckForInput();
  // Whatever else the loop does, we should process time every loop.
  ProcessTime();
  unsigned long lnMS = millis();
  while (millis() - lnMS < 100) {}
}

void CheckForInput() {
  String lcKeyPressed = GetKey();
  if (glClockSet) {
    if (lcKeyPressed == "1") {
      if (gnH == HOUR_MAX) {gnH = HOUR_MIN;} else {gnH++;}
    } else if (lcKeyPressed == "4") {
      if (gnH == HOUR_MIN) {gnH = HOUR_MAX;} else {gnH--;}
    } else if (lcKeyPressed == "2") {
      if (gnM == 59) {gnM = 0;} else {gnM++;}
    } else if (lcKeyPressed == "5") {
      if (gnM == 0) {gnM = 59;} else {gnM--;}
    } else if (lcKeyPressed == "3") {
      if (gnS == 59) {gnS = 0;} else {gnS++;}
    } else if (lcKeyPressed == "6") {
      if (gnS == 0) {gnS = 59;} else {gnS--;}
    }
  }
  if (lcKeyPressed == "#") {
    lcd.clear();
    glClockSet = !glClockSet;
  }
}

void DisplayClockFull() {
  // Display clock in full mode.
  lcd.setFont(1);
  if (gnH < 10) {
    lcd.text(2, 0, "0");
    lcd.text(4, 0, String(gnH));
  } else {
    lcd.text(2, 0, String(gnH));
  }
  lcd.text(6, 0, ":");
  if (gnM < 10) {
    lcd.text(8, 0, "0");
    lcd.text(10, 0, String(gnM));
  } else {
    lcd.text(8, 0, String(gnM));
  }
  if (gnS < 10) {
    lcd.text(5, 3, "0");
    lcd.text(7, 3, String(gnS));
  } else {
    lcd.text(5, 3, String(gnS));
  }
}

void DisplayClockSet() {
  // Display clock set mode.
  lcd.setFont(0);
  lcd.text(0, 0, " 1: H+, 4: H-");
  lcd.text(0, 1, " 2: M+, 5: M-");
  lcd.text(0, 2, " 3: S+, 6: S-");
  lcd.text(0, 3, "#: Start Clock");
  if (gnH < 10) {
    lcd.text(3, 5, "0");
    lcd.text(4, 5, String(gnH));
  } else {
    lcd.text(3, 5, String(gnH));
  }
  lcd.text(5, 5, ":");
  if (gnM < 10) {
    lcd.text(6, 5, "0");
    lcd.text(7, 5, String(gnM));
  } else {
    lcd.text(6, 5, String(gnM));
  }
  lcd.text(8, 5, ":");
  if (gnS < 10) {
    lcd.text(9, 5, "0");
    lcd.text(10, 5, String(gnS));
  } else {
    lcd.text(9, 5, String(gnS));
  }
}

void ProcessTime() {
  gnMS = millis();
  if (gnMS < gnLastTick) {
    // millis() has rolled over (takes ~50 days).
    gnLastTick = 0;
  }
  if (gnMS - gnLastTick >= 1000) {
    // Increment our seconds.
    gnS++;
    // Handle roll-overs.
    if (gnS >= 60) {
      gnM++;
      gnS = 0;
    }
    if (gnM >= 60) {
      gnH++;
      gnM = 0;
    }
    if (gnH > HOUR_MAX) {
      gnH = HOUR_MIN;
    }
    gnLastTick = gnMS;
    gnLastTick = gnLastTick - ADJUST_MS_PER_LOOP;
  }
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
