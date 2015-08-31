#pragma once
//
// Lucky Resistor's Data Logger (Simple Version)
// ---------------------------------------------------------------------------
// (c)2015 by Lucky Resistor. See LICENSE for details.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//


// Include core libraries.
#include <Arduino.h>
#include <Wire.h>

// Include additional libraries
#include <RTClib.h>
#include <RTC_DS1307.h>

// Include AVR libraries
#include <avr/sleep.h>

// Local libraries
#include "Storage.h"
#include "LogSystem.h"
#include "ModeSelector.h"
#include "DHT22.h"


// The pin for the signal LED
#define SIGNAL_LED 13


//#define LR_APPLICATION_DEBUG


/// The application
///
class Application
{
public:
    /// ctor
    ///
    Application();
    
    /// dtor
    ///
    ~Application();
    
public:
    /// Call this in the setup() method.
    ///
    void setup();
    
    /// Call this in the loop() method.
    ///
    void loop();

private:
    /// Signal an error with the signal LED
    ///
    void signalError(uint8_t errorNumber);
    
    /// Send the date/time to the serial.
    ///
    void sendDateTimeToSerial(const DateTime &dateTime);

    /// Send a duration to the serial.
    ///
    void sendDurationToSerial(uint32_t seconds);
    
    /// Enter power-safe mode.
    ///
    /// @param seconds Stay in power save mode for approx this number of seconds.
    ///
    void powerSave(uint16_t seconds);
    
private:
    DHT22 dht;
    RTC_DS1307 rtc;
    ModeSelector modeSelector;
    Storage storage;
    LogSystem logSystem;
    
    int32_t _sleepDelay;
    DateTime _currentTime;
    DateTime _nextRecordTime;
};

