#include "pitches.h"

#define AUDIO_OUT  19
#define TONES_IN_RANGE  13
#define NOTE_START_PIN  2
#define NOTE_END_PIN  14
#define MIN_RANGE  1
#define MAX_RANGE  7
#define PIN_RANGE_UP  15
#define PIN_RANGE_DOWN  18
#define START_RANGE  4      // C4 is "middle C"
#define SPECIAL_BASE_1  3
#define SPECIAL_BASE_2  5
#define SPECIAL_TRIGGER_1  NOTE_END_PIN
#define SPECIAL_TRIGGER_2  NOTE_END_PIN - 1

#define USE_LOW_POWER  true

// Variables used in interrupts should be "volatile" so they
// don't get optimized out by the compiler.
boolean volatile glISRFree = true;
boolean volatile glReplay = false;
boolean volatile glSpecial1 = false;
boolean volatile glSpecial2 = false;
byte volatile gnRange = 0;
unsigned int gaNotes[TONES_IN_RANGE];

unsigned long volatile gnLoopsElapsed = 0;
// It takes ~38 seconds for 1,000,000 loops to run, a frequency of
// 26.315 kHz. That means one second of time passes for the loop() here to
// run 26,315 times. E.g. let's say we want an idle time of ~30 seconds to trigger
// low power mode. That would mean 30 x 26315 = 789,450 loops. (We could use 800,000.)
// (NOTE: even when loops to idle is zero, things work pretty darn well.
//#define LOOPS_TO_IDLE  800000    // About 30 seconds.
#define LOOPS_TO_IDLE  130000    // About 5 Seconds.


void setup() {
  // All note buttons and interrupt triggers should be pulled up so that taking them
  // LOW (or using FALLING, for IRQs) is cleaner. So, LOW state is what plays notes,
  // and IRQs are triggered via FALLING (and checking for LOW in ISR as a quick debounce).
  for (byte i = NOTE_START_PIN; i <= NOTE_END_PIN; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  // Range buttons are also pulled up.
  pinMode(PIN_RANGE_UP, INPUT_PULLUP);
  pinMode(PIN_RANGE_DOWN, INPUT_PULLUP);
  // Attach any persistent interrupts. Normal notes will have interrupts attached and
  // detached depending on power mode.
  attachInterrupt(SPECIAL_TRIGGER_1, ISR_Special1, FALLING);
  attachInterrupt(SPECIAL_TRIGGER_2, ISR_Special2, FALLING);
  attachInterrupt(PIN_RANGE_UP, ISR_ChangeRangeUp, FALLING);
  attachInterrupt(PIN_RANGE_DOWN, ISR_ChangeRangeDown, FALLING);
  // Quick start-up tones. Sound slike the "line disconnected" phone trio.
  tone(AUDIO_OUT, 950);
  delay(500);
  tone(AUDIO_OUT, 1400);
  delay(500);
  tone(AUDIO_OUT, 1875);
  delay(1000);
  noTone(AUDIO_OUT);
  SetRange(START_RANGE);
}

void loop() {
  // ncrement global loop counter (used for LPM entry after set idle time at end of loop).
  gnLoopsElapsed++;
  // Before we check the keyboard, see if there are any special actions we should take.
  // Special actions should be mutually-exclusive, so no need to "else" anything.
  if (glSpecial1) {
    PlayTestTones();
    glSpecial1 = false;
    return;
  }
  if (glSpecial2) {
    PlayQuadrophenia();
    glSpecial2 = false;
    return;
  }
  // Because of the way we have set up the note pins, we can simply go incrementally from
  // first to last and play whatever first note we find as LOW (pressed). The PlayNote()
  // subroutine loops until key is released.
  for (byte i = NOTE_START_PIN; i <= NOTE_END_PIN; i++) {
    if (digitalRead(i) == LOW) {
      PlayNote(i);
      return;
    }
  }
  // We have made it here because no buttons are pressed or the last-pressed button was
  // just released. We can now check to see if we want to drop into low power mode.
  if (USE_LOW_POWER && (gnLoopsElapsed > LOOPS_TO_IDLE)) {
    // Switch to LPM4 with interrupts enabled. ISRs will wake things up, then another idle period
    // will put things back to sleep. Attach/Detach interrupts to prevent any stuttering in played
    // notes. When in active mode, it will be as if LPM was never initiated. But in LPM, interrupts
    // will be in place to wake things up.
    // 1.  Get ready for sleep. Make sure all interrupts are attached so that playing a note
    // immediately wakes up the MCU. We also have to attach the "special key", as those keys
    // are also just notes. If we don't re-attach then after cycling in and out of LPM, the 
    // special key combinations will not work. (Apparently attachInterrupt() is smart enough 
    // to replace the interrups as first established by the for loop).
    for (byte i = NOTE_START_PIN; i <= NOTE_END_PIN; i++) {
      attachInterrupt(i, ISR_ButtonPressed, FALLING);
    }
    attachInterrupt(SPECIAL_TRIGGER_1, ISR_Special1, FALLING);
    attachInterrupt(SPECIAL_TRIGGER_2, ISR_Special2, FALLING);
    // 2. Sleep, baby. Hush now.
    _BIS_SR(LPM4_bits | GIE);
    // 3. We're back and awake. We don't need interrupts on normal, playable pins (notes) any more, 
    // except for the "special" keys, as that will just add stuttering/warbling to notes played in sequence.
    for (byte i = NOTE_START_PIN; i <= NOTE_END_PIN; i++) {
      if (i != SPECIAL_TRIGGER_1 && i != SPECIAL_TRIGGER_2) {
        detachInterrupt(i);
      }
    }
    gnLoopsElapsed = 0;
  }
}

void LockInt() {
  glISRFree = false;
  delayMicroseconds(100);
}

void UnlockInt() {
  glISRFree = true;
  delayMicroseconds(100);
}

void ISR_ButtonPressed() {
  // Just here to wake up from low power mode (if even attached).
  // It does not appear that any live code needs to occur.
}

void ISR_Special1() {
  if (glISRFree) {
    LockInt();
    if (SpecialState()) {
      glSpecial1 = true;
    }
    UnlockInt();
  }
  return;
}

void ISR_Special2() {
  if (glISRFree) {
    LockInt();
    if (SpecialState()) {
      glSpecial2 = true;
    }
    UnlockInt();
  }
  return;
}

void ISR_ChangeRangeUp() {
  if (glISRFree) {
    LockInt();
    delayMicroseconds(1000);
    if (digitalRead(PIN_RANGE_UP) != LOW) {
      UnlockInt();
      return;
    }
    SetRange(gnRange + 1);
    glReplay = true;
    UnlockInt();
  }
  return;
}

void ISR_ChangeRangeDown() {
  if (glISRFree) {
    LockInt();
    delayMicroseconds(1000);
    if (digitalRead(PIN_RANGE_DOWN) != LOW) {
      UnlockInt();
      return;
    }
    SetRange(gnRange - 1);
    glReplay = true;
    UnlockInt();
  }
  return;
}

void PlayNote(byte pnButton) {
  if (digitalRead(pnButton) == LOW) {
    byte lnNoteIndex = pnButton - 2;    // Buttons start at 2, note array is zero-indexed.
    tone(AUDIO_OUT, gaNotes[lnNoteIndex]);
    while (true) {
      if (digitalRead(pnButton) == HIGH) {
        noTone(AUDIO_OUT);
        return;
      } else {
        if (glReplay) {
          tone(AUDIO_OUT, gaNotes[lnNoteIndex]);
          glReplay = false;
        }
      }
    }
  }
}

boolean SpecialState() {
  return (digitalRead(SPECIAL_BASE_1) == LOW & digitalRead(SPECIAL_BASE_2) == LOW);
}

void SetRange(byte pnRange) {
  gnRange = pnRange;
  // Make sure we are not going out of bounds in terms of range.
  if (gnRange < MIN_RANGE) {gnRange = MIN_RANGE;}
  if (gnRange > MAX_RANGE) {gnRange = MAX_RANGE;}
  switch (gnRange) {
    case 1:
      gaNotes[0] = NOTE_C1;
      gaNotes[1] = NOTE_CS1;
      gaNotes[2] = NOTE_D1;
      gaNotes[3] = NOTE_DS1;
      gaNotes[4] = NOTE_E1;
      gaNotes[5] = NOTE_F1;
      gaNotes[6] = NOTE_FS1;
      gaNotes[7] = NOTE_G1;
      gaNotes[8] = NOTE_GS1;
      gaNotes[9] = NOTE_A1;
      gaNotes[10] = NOTE_AS1;
      gaNotes[11] = NOTE_B1;
      gaNotes[12] = NOTE_C2;
      break;
    case 2:
      gaNotes[0] = NOTE_C2;
      gaNotes[1] = NOTE_CS2;
      gaNotes[2] = NOTE_D2;
      gaNotes[3] = NOTE_DS2;
      gaNotes[4] = NOTE_E2;
      gaNotes[5] = NOTE_F2;
      gaNotes[6] = NOTE_FS2;
      gaNotes[7] = NOTE_G2;
      gaNotes[8] = NOTE_GS2;
      gaNotes[9] = NOTE_A2;
      gaNotes[10] = NOTE_AS2;
      gaNotes[11] = NOTE_B2;
      gaNotes[12] = NOTE_C3;
      break;
    case 3:
      gaNotes[0] = NOTE_C3;
      gaNotes[1] = NOTE_CS3;
      gaNotes[2] = NOTE_D3;
      gaNotes[3] = NOTE_DS3;
      gaNotes[4] = NOTE_E3;
      gaNotes[5] = NOTE_F3;
      gaNotes[6] = NOTE_FS3;
      gaNotes[7] = NOTE_G3;
      gaNotes[8] = NOTE_GS3;
      gaNotes[9] = NOTE_A3;
      gaNotes[10] = NOTE_AS3;
      gaNotes[11] = NOTE_B3;
      gaNotes[12] = NOTE_C4;
      break;
    case 4:
      gaNotes[0] = NOTE_C4;
      gaNotes[1] = NOTE_CS4;
      gaNotes[2] = NOTE_D4;
      gaNotes[3] = NOTE_DS4;
      gaNotes[4] = NOTE_E4;
      gaNotes[5] = NOTE_F4;
      gaNotes[6] = NOTE_FS4;
      gaNotes[7] = NOTE_G4;
      gaNotes[8] = NOTE_GS4;
      gaNotes[9] = NOTE_A4;
      gaNotes[10] = NOTE_AS4;
      gaNotes[11] = NOTE_B4;
      gaNotes[12] = NOTE_C5;
      break;
    case 5:
      gaNotes[0] = NOTE_C5;
      gaNotes[1] = NOTE_CS5;
      gaNotes[2] = NOTE_D5;
      gaNotes[3] = NOTE_DS5;
      gaNotes[4] = NOTE_E5;
      gaNotes[5] = NOTE_F5;
      gaNotes[6] = NOTE_FS5;
      gaNotes[7] = NOTE_G5;
      gaNotes[8] = NOTE_GS5;
      gaNotes[9] = NOTE_A5;
      gaNotes[10] = NOTE_AS5;
      gaNotes[11] = NOTE_B5;
      gaNotes[12] = NOTE_C6;
      break;
    case 6:
      gaNotes[0] = NOTE_C6;
      gaNotes[1] = NOTE_CS6;
      gaNotes[2] = NOTE_D6;
      gaNotes[3] = NOTE_DS6;
      gaNotes[4] = NOTE_E6;
      gaNotes[5] = NOTE_F6;
      gaNotes[6] = NOTE_FS6;
      gaNotes[7] = NOTE_G6;
      gaNotes[8] = NOTE_GS6;
      gaNotes[9] = NOTE_A6;
      gaNotes[10] = NOTE_AS6;
      gaNotes[11] = NOTE_B6;
      gaNotes[12] = NOTE_C7;
      break;
    case 7:
      gaNotes[0] = NOTE_C7;
      gaNotes[1] = NOTE_CS7;
      gaNotes[2] = NOTE_D7;
      gaNotes[3] = NOTE_DS7;
      gaNotes[4] = NOTE_E7;
      gaNotes[5] = NOTE_F7;
      gaNotes[6] = NOTE_FS7;
      gaNotes[7] = NOTE_G7;
      gaNotes[8] = NOTE_GS7;
      gaNotes[9] = NOTE_A7;
      gaNotes[10] = NOTE_AS7;
      gaNotes[11] = NOTE_B7;
      gaNotes[12] = NOTE_C8;
      break;
  }
}

void PlayTestTones() {
  // Test the full ranges.
  byte lnCurrRange = gnRange;
  for (byte i = MIN_RANGE; i <= MAX_RANGE; i++) {
    SetRange(i);
    for (byte j = 0; j <= TONES_IN_RANGE - 1; j++) {
      tone(AUDIO_OUT, gaNotes[j], 1000);
      delay(100);
    }
  }
  // One last tone for next highest note (a C)! (Range will still be set to highest).
  tone(AUDIO_OUT, gaNotes[TONES_IN_RANGE - 1], 2000);
  delay(1000);
  SetRange(lnCurrRange);
}

void PlayQuadrophenia() {
  int laQuadNotes[] = {NOTE_E4, 
                        NOTE_D4,
                        NOTE_E4,
                        NOTE_F4,
                        NOTE_G4,
                        NOTE_F4,
                        NOTE_G4,
                        NOTE_C4,
                        NOTE_GS4,
                        NOTE_G4,
                        NOTE_GS4,
                        NOTE_G4,
                        NOTE_F4,
                        NOTE_E4,
                        NOTE_F4,
                        NOTE_D4};
  for (byte i = 0; i <= 15; i++) {
    tone(AUDIO_OUT, laQuadNotes[i]);
    delay(200);
  }
  noTone(AUDIO_OUT);
}
