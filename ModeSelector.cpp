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
#include "ModeSelector.h"



ModeSelector::ModeSelector()
{
}


ModeSelector::~ModeSelector()
{
}


namespace {
    
    
uint8_t getSelectedValue()
{
    uint8_t result = 0;
    if (digitalRead(MODE_SELECTOR_PIN_D1) == LOW) {
        result |= B0001;
    }
    if (digitalRead(MODE_SELECTOR_PIN_D2) == LOW) {
        result |= B0010;
    }
    if (digitalRead(MODE_SELECTOR_PIN_D4) == LOW) {
        result |= B0100;
    }
    if (digitalRead(MODE_SELECTOR_PIN_D8) == LOW) {
        result |= B1000;
    }
    return result;
}
    
    
}


void ModeSelector::begin()
{
    pinMode(MODE_SELECTOR_PIN_D1, INPUT_PULLUP);
    pinMode(MODE_SELECTOR_PIN_D2, INPUT_PULLUP);
    pinMode(MODE_SELECTOR_PIN_D4, INPUT_PULLUP);
    pinMode(MODE_SELECTOR_PIN_D8, INPUT_PULLUP);
    delay(100);
    _selectedValue = getSelectedValue();
}


ModeSelector::Mode ModeSelector::getMode()
{
    if (_selectedValue < 8) {
        return Log;
    } else if (_selectedValue == 8) {
        return Read;
    } else if (_selectedValue == 9) {
        return Format;
    } else {
        return Read; // This should never happen.
    }
}


uint32_t ModeSelector::getInterval()
{
    switch (_selectedValue) {
        case 0: return 10; // 10s
        case 1: return 30; // 30s
        case 2: return 60; // 1m
        case 3: return 600; // 10m
        case 4: return 3600; // 1h
        case 5: return 14400; // 4h
        case 6: return 28800; // 8h
        case 7: return 86400; // 24h
        default: return 10;
    }
}


String ModeSelector::getIntervalText()
{
    switch (_selectedValue) {
        case 0: return String(F("10s"));
        case 1: return String(F("30s"));
        case 2: return String(F("1m"));
        case 3: return String(F("10m"));
        case 4: return String(F("1h"));
        case 5: return String(F("4h"));
        case 6: return String(F("8h"));
        case 7: return String(F("24h"));
        default: return String(F("Unknown"));
    }
}




