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


#define MODE_SELECTOR_PIN_D1 4
#define MODE_SELECTOR_PIN_D2 5
#define MODE_SELECTOR_PIN_D4 6
#define MODE_SELECTOR_PIN_D8 8


/// This is the mode selector for the logger.
///
/// It uses the digital inputs 4, 5, 6 and 8 to detect the
/// setting of the BCD DIL.
///
/// 0 = Log values - 10s interval.
/// 1 = Log values - 30s interval.
/// 2 = Log values -  1m interval.
/// 3 = Log values - 10m interval.
/// 4 = Log values -  1h interval.
/// 5 = Log values -  4h interval.
/// 6 = Log values -  8h interval.
/// 7 = Log values - 24h interval.
/// 8 = Read records and send them to serial.
/// 9 = Format storage. All data will be lost.
///
class ModeSelector
{
public:
    enum Mode {
        Log,
        Read,
        Format
    };
    
public:
    /// ctor
    ///
    ModeSelector();
    
    /// dtor
    ///
    ~ModeSelector();
    
public:
    /// Call this method in setup() to set the inputs and read the selected value.
    ///
    void begin();
    
    /// Get the selected mode.
    ///
    Mode getMode();
    
    /// Get the selected interval in seconds.
    ///
    uint32_t getInterval();
    
    /// Get the selected interval as text.
    ///
    String getIntervalText();
    
private:
    uint8_t _selectedValue;
};