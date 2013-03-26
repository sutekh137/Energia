int lnRedBtn = P2_2;
int lnGreenBtn = P2_3;
int lnHoldBtn = PUSH2;
volatile int lnLit = -999;

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(lnRedBtn, INPUT_PULLUP);
  pinMode(lnGreenBtn, INPUT_PULLUP);
  pinMode(lnHoldBtn, INPUT_PULLUP);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  attachInterrupt(lnRedBtn, red, FALLING);
  attachInterrupt(lnGreenBtn, green, FALLING);
  attachInterrupt(lnHoldBtn, hold, FALLING);
}

void loop() {
}

void red() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  lnLit = RED_LED;
}

void green() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  lnLit = GREEN_LED;
}

void hold() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  switch (lnLit) {
    case GREEN_LED:
      while (digitalRead(lnHoldBtn) == LOW) {
        digitalWrite(RED_LED, HIGH);
      }
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      lnLit = GREEN_LED;
      break;
    case RED_LED:   
      while (digitalRead(lnHoldBtn) == LOW) {
        digitalWrite(GREEN_LED, HIGH);
      }
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      lnLit = RED_LED;
      break;
  }
}
