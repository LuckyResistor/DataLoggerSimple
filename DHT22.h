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


#include <Arduino.h>


//#define LR_DHT22_DEBUG 1


/// A own compact implementation of the DHT22 sensor library.
///
/// Using some timing values from the DHT library by Adafruit.
///
class DHT22
{
    /// The result for measuring a pulse.
    ///
    enum PulseResult : uint8_t {
        PulseHigh,
        PulseLow,
        Timeout
    };

public:
    /// One single measurement from the sensor.
    ///
    struct Measurement {
        float temperature;
        float humidity;
    };
    
public:
    /// ctor
    ///
    DHT22(uint8_t pin);
    
    /// dtor
    ///
    ~DHT22();
    
public:
    /// Initialize the library
    ///
    void begin();
    
    /// Read the temperature and humidity
    ///
    /// The temperature is read in celsius.
    ///
    Measurement readTemperatureAndHumidity();
    
private:
    /// Get the result for a single pulse
    ///
    PulseResult getPulse();
    
private:
    uint8_t _pin; ///< The pin to read from.
    uint8_t _pinMask; ///< The bit for the pin.
    uint8_t _pinPort; ///< The port for the pin.
    uint32_t _pulseTimeout; ///< The maximum count to wait for a pulse.
};