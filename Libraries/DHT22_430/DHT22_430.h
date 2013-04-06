///
/// @file	DHT22_430.h 
/// @brief	Library header
/// @details	DHT22 temperature and humidity sensor library
/// @n	
/// @n @b	Project DHT22_430_main
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


// Core library - MCU-based
#if defined(__MSP430G2452__) || defined(__MSP430G2553__) || defined(__MSP430G2231__) // LaunchPad specific
#include "Energia.h"
#else // error
#error Platform not supported
#endif

#ifndef DHT22_430_Library_h
#define DHT22_430_Library_h

////
/// @brief	Number of timing transitions needed to keep track of 2 * number bits + extra
///
#define MAXTIMINGS 85
///
/// @brief	Uncomment for debug print
///
#define DEBUG

///
/// @brief	Class for DHT22 sensor
///
class DHT22 {
private:
  uint8_t data[6];
  uint8_t _pin;
  uint32_t _lastMillis;
  boolean _lastResult;
  uint32_t _temperature, _humidity;
  
public:
  ///
  /// @brief	Constructor
  /// @param	pin pin for the signal
  ///
  DHT22(uint8_t pin);
  
  ///
  /// @brief	Initialise
  ///
  void begin();
  
  ///
  /// @brief	Acquire temperature and humidity
  /// @return	true=success, false=error
  /// @note	sensor frequency = 0.5 Hz
  ///
  boolean get();
  
  ///
  /// @brief	Temperature in Celsius
  /// @return	return temperature x 10 to avoid float
  /// @code {.cpp}
  /// Serial.print(temperatureX10()/10, DEC); // integer part
  /// Serial.print(".");                      // decimal separator
  /// Serial.print(temperatureX10()%10, DEC); // decimal part
  /// @endcode
  ///
  int32_t temperatureX10();
  
  ///
  /// @brief	Relative humidity in %
  /// @return	return relative humidity x 10 to avoid float
  /// @code {.cpp}
  /// Serial.print(humidityX10()/10, DEC); // integer part
  /// Serial.print(".");                   // decimal separator
  /// Serial.print(humidityX10()%10, DEC); // decimal part
  /// @endcode
  ///
  int32_t humidityX10();
};

#endif
