/// 
/// @mainpage	DHT22 temperature and humidity sensor library
/// @details	DHT22 on LaunchPad
/// @n
/// @n		2012-06-15 First release
/// @n		2012-06-17 Arduino-related code wiped-out
/// @n		2012-06-17 int32_t only to avoid float and math.h
/// @n		2012-07-02 LaunchPad release
///
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
/// 
/// @author	Rei VILO
/// @author	http://embeddedcomputing.weebly.com
/// @date	Jul 02, 2012
/// @version	2.01
/// 
/// @copyright	© Rei VILO, 2012
/// @copyright	CC = BY NC SA
///
/// @see	
/// @n		Based on http://www.ladyada.net/learn/sensors/dht.html
/// @n		written by Adafruit Industries, MIT license
/// @n		 
/// @b		LaunchPad implementation 
/// @n		by energia » Tue Jun 26, 2012 9:24 pm
/// @n		http://www.43oh.com/forum/viewtopic.php?p=20821#p20821
/// @n		As LaunchPad is faster than Arduino,
/// *		1. replace delayMicroseconds(1) with delayMicroseconds(3)
/// @n 	or
/// *		2. compare counter to a higher number
///

///
/// @file	DHT22_430_main.pde 
/// @brief	Main sketch
/// @details	DHT22 on LaunchPad
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
/// 
/// @author	Rei VILO
/// @author	http://embeddedcomputing.weebly.com
/// @date	Jul 02, 2012
/// @version	2.01
/// 
/// @copyright	© Rei VILO, 2012
/// @copyright	CC = BY NC SA
///


// Core library - MCU-based
#if defined(__MSP430G2452__) || defined(__MSP430G2553__) || defined(__MSP430G2231__) // LaunchPad specific
#include "Energia.h"
#else // error
#error Platform not supported
#endif

// Include application, user and local libraries
#include "DHT22_430.h"


///
/// @brief	Pin for DHT22 signal
/// @n 		Connect 
/// *		pin 1 (on the left) of the sensor to +5V
/// *		pin 2 of the sensor to DHTPIN 
/// *		pin 4 (on the right) of the sensor to GROUND
/// @n		Place a 10k resistor between pin 2 (data) to pin 1 (power) of the sensor
///
#define DHTPIN P1_4

DHT22 mySensor(DHTPIN);
boolean flag;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\n\n*** DHT22 test starts"); 
  Serial.println("PUSH2 to end"); 
  pinMode(PUSH2, INPUT_PULLUP);     
  
  mySensor.begin();
}

void loop() {
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  flag = mySensor.get();
  int32_t h = mySensor.humidityX10();
  int32_t t = mySensor.temperatureX10();
  
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (!flag) {
    Serial.println("Failed to read from DHT");
  } 
  else {
    Serial.print("RH% \t");
    Serial.print(h/10);
    Serial.print(".");
    Serial.print(h%10);
    Serial.println(" %\t");
    
    Serial.print("oC \t");
    Serial.print(t/10);
    Serial.print(".");
    Serial.print(t%10);
    Serial.println(" *C");    
  }
  
  if (digitalRead(PUSH2)==LOW) {
    Serial.println("\n\n*** End"); 
    Serial.end();
    while(true); // endless loop
  }
}

