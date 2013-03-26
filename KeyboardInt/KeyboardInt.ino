#include "pitches.h"
 
byte gnNote1 = P1_1;
byte gnNote2 = P1_2;
byte gnNote3 = P1_4;
byte gnNote4 = P1_5;
byte gnNote5 = P2_0;
byte gnNote6 = P2_1;
byte gnNote7 = P2_2;
byte gnNote8 = P2_3;
byte gnChangeRange = PUSH2;
byte gnAudioOut = P2_5;
unsigned int gaNotes[8];

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(gnNote1, INPUT_PULLUP);
  pinMode(gnNote2, INPUT_PULLUP);
  pinMode(gnNote3, INPUT_PULLUP);
  pinMode(gnNote4, INPUT_PULLUP);
  pinMode(gnNote5, INPUT_PULLUP);
  pinMode(gnNote6, INPUT_PULLUP);
  pinMode(gnNote7, INPUT_PULLUP);
  pinMode(gnNote8, INPUT_PULLUP);
  pinMode(gnChangeRange, INPUT_PULLUP);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  attachInterrupt(gnNote1, PlayNote1, FALLING);
  attachInterrupt(gnNote2, PlayNote2, FALLING);
  attachInterrupt(gnNote3, PlayNote3, FALLING);
  attachInterrupt(gnNote4, PlayNote4, FALLING);
  attachInterrupt(gnNote5, PlayNote5, FALLING);
  attachInterrupt(gnNote6, PlayNote6, FALLING);
  attachInterrupt(gnNote7, PlayNote7, FALLING);
  attachInterrupt(gnNote8, PlayNote8, FALLING);
  attachInterrupt(gnChangeRange, ChangeRange, FALLING);
  TestTones();
}

void loop() {
  // All we do here is check to see if NO keys are pressed. If so, turn off tone.
  /**if (digitalRead(gnNote1) == HIGH &&
      digitalRead(gnNote2) == HIGH &&
      digitalRead(gnNote3) == HIGH &&
      digitalRead(gnNote4) == HIGH &&
      digitalRead(gnNote5) == HIGH &&
      digitalRead(gnNote6) == HIGH &&
      digitalRead(gnNote7) == HIGH &&
      digitalRead(gnNote8) == HIGH) {
    noTone(gnAudioOut);
  }**/
}

void TestTones() {
  // Test the full ranges. Remember that top note of one range is lowest note of next higher range.
  byte i, j;
  for (i = 1; i <= 4; i++) {
    SetRange(i);
    for (j = 0; j <= 6; j++) {
      tone(gnAudioOut, gaNotes[j], 1000);
      delay(125);
    }
  }
  // One last tone for highest note! (Range will still be set to highest).
  tone(gnAudioOut, gaNotes[7], 2000);
  delay(1000);
  SetRange(1);
}

byte GetNoteIndexFromButton(byte pnButton) {
  if (pnButton == gnNote1) {return 1;}
  if (pnButton == gnNote2) {return 2;}
  if (pnButton == gnNote3) {return 3;}
  if (pnButton == gnNote4) {return 4;}
  if (pnButton == gnNote5) {return 5;}
  if (pnButton == gnNote6) {return 6;}
  if (pnButton == gnNote7) {return 7;}
  if (pnButton == gnNote8) {return 8;}
}

void PlayNote(byte pnButton) {
  byte lnNoteIndex = GetNoteIndexFromButton(pnButton);
  tone(gnAudioOut, gaNotes[lnNoteIndex - 1]);
}

void PlayNote1() {
  PlayNote(gnNote1);
}
void PlayNote2() {
  PlayNote(gnNote2);
}
void PlayNote3() {
  PlayNote(gnNote3);
}
void PlayNote4() {
  PlayNote(gnNote4);
}
void PlayNote5() {
  PlayNote(gnNote5);
}
void PlayNote6() {
  PlayNote(gnNote6);
}
void PlayNote7() {
  PlayNote(gnNote7);
}
void PlayNote8() {
  PlayNote(gnNote8);
}

void ChangeRange() {
  noTone(gnAudioOut);
  byte lnCurrRange = GetRange();
  if (lnCurrRange == 4) {
    // Wrap back around.
    lnCurrRange = 0;
  }
  SetRange(lnCurrRange + 1);
  delayMicroseconds(100);
}

void SetRange(byte pnRange) {
  switch (pnRange) {
    case 1:
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, LOW);
      gaNotes[0] = NOTE_C3;
      gaNotes[1] = NOTE_D3;
      gaNotes[2] = NOTE_E3;
      gaNotes[3] = NOTE_F3;
      gaNotes[4] = NOTE_G3;
      gaNotes[5] = NOTE_A3;
      gaNotes[6] = NOTE_B3;
      gaNotes[7] = NOTE_C4;
      break;
    case 2:
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      gaNotes[0] = NOTE_C4;
      gaNotes[1] = NOTE_D4;
      gaNotes[2] = NOTE_E4;
      gaNotes[3] = NOTE_F4;
      gaNotes[4] = NOTE_G4;
      gaNotes[5] = NOTE_A4;
      gaNotes[6] = NOTE_B4;
      gaNotes[7] = NOTE_C5;
      break;
    case 3:
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      gaNotes[0] = NOTE_C5;
      gaNotes[1] = NOTE_D5;
      gaNotes[2] = NOTE_E5;
      gaNotes[3] = NOTE_F5;
      gaNotes[4] = NOTE_G5;
      gaNotes[5] = NOTE_A5;
      gaNotes[6] = NOTE_B5;
      gaNotes[7] = NOTE_C6;
      break;
    case 4:
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, HIGH);
      gaNotes[0] = NOTE_C6;
      gaNotes[1] = NOTE_D6;
      gaNotes[2] = NOTE_E6;
      gaNotes[3] = NOTE_F6;
      gaNotes[4] = NOTE_G6;
      gaNotes[5] = NOTE_A6;
      gaNotes[6] = NOTE_B6;
      gaNotes[7] = NOTE_C7;
      break;
  }
}

byte GetRange() {
  if (digitalRead(GREEN_LED) == LOW && digitalRead(RED_LED) == LOW) {
    // No lights, this is the lowest range state.
    return 1;
  } else {
    if (digitalRead(GREEN_LED) == HIGH && digitalRead(RED_LED) == LOW) {
      // Just green, this is state 2.
      return 2;
    } else {
      if (digitalRead(GREEN_LED) == LOW && digitalRead(RED_LED) == HIGH) {
        // Just red, this is state 3.
        return 3;
      } else {
        // This must mean both green and red are on, this is state 4.
        return 4;
      }
    }
  }
  
}

