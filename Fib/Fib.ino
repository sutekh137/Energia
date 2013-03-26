int gnFibTerm = 0;
long unsigned gnFibResult;
long unsigned gnMillis = 0;
int gnDelay = 150;
int llBlinkLEDs = 1;
int llMakeTones = 0;
int lnToneFreq = 262;
String lcMsg = "";

void setup() {
  Serial.begin(9600); // msp430g2231 must use 4800
  Serial.println("Starting up!");
  pinMode(GREEN_LED, OUTPUT);     
  pinMode(RED_LED, OUTPUT);
  // Show both LEDs to start.
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, HIGH);
  delay(1000);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  delay(1000);
}

void loop() {
  gnFibTerm = gnFibTerm + 1;
  gnMillis = millis();
  gnFibResult = fib(gnFibTerm);
  gnMillis = millis() - gnMillis;
  lcMsg = "Fib term " + String(gnFibTerm) + " is " + String(gnFibResult) + ". (Took " + String(gnMillis) + " ms.)";
  Serial.println(lcMsg);
  if (llBlinkLEDs) {
    for(int i = 1; i <= gnFibResult; i++) {
      digitalWrite(GREEN_LED, HIGH);   // set the LED on
      if (llMakeTones) {
        tone(8, lnToneFreq, gnDelay);
      }
      delay(gnDelay);
      digitalWrite(GREEN_LED, LOW);   // set the LED off
      delay(gnDelay);
    }
    digitalWrite(RED_LED, HIGH);   // set the LED on
    delay(gnDelay);
    digitalWrite(RED_LED, LOW);   // set the LED off  
    delay(gnDelay);
  }
}

long unsigned fib(int n) {
  if ((n==1) || (n==2)) {
    return 1;
  }
  return fib(n-1) + fib(n-2);
}
