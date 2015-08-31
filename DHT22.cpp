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


#include "DHT22.h"


DHT22::DHT22(uint8_t pin)
    : _pin(pin), _pinMask(0), _pinPort(0)
{
    _pinMask = digitalPinToBitMask(_pin);
    _pinPort = digitalPinToPort(_pin);
    _pulseTimeout = microsecondsToClockCycles(1000); // > 1ms timeout.
}


DHT22::~DHT22()
{
}


void DHT22::begin()
{
    pinMode(_pin, INPUT);
    digitalWrite(_pin, HIGH);
}


DHT22::PulseResult DHT22::getPulse()
{
    uint32_t lowCount = 0;
    uint32_t highCount = 0;
    while ((*portInputRegister(_pinPort) & _pinMask) == 0) {
        if (++lowCount > _pulseTimeout) {
            return Timeout;
        }
    }
    while ((*portInputRegister(_pinPort) & _pinMask) != 0) {
        if (++highCount > _pulseTimeout) {
            return Timeout;
        }
    }
    return (highCount>lowCount) ? PulseHigh : PulseLow;
}


DHT22::Measurement DHT22::readTemperatureAndHumidity()
{
    Measurement measurement = {NAN, NAN};
    
    // 5 bytes of read data.
    uint8_t readData[5];
    memset(readData, 0, 5);
    
    // Start time critical code
    noInterrupts();
    
    // Trigger the start of a new read.
    digitalWrite(_pin, HIGH);
    delay(250);
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(20); // Data low for 20ms.
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);
    // Back observing the line for the data.
    pinMode(_pin, INPUT);
    delayMicroseconds(10);
    
    // Read and ignore the initial pulse from the sensor.
    if (getPulse() == Timeout) {
#ifdef LR_DHT22_DEBUG
        Serial.println(F("Timeout for initial pulse."));
#endif
        goto END_READ;
    }
    
    // Now read all 40 bits.
    for (uint8_t i = 0; i < 5; ++i) {
        for (uint8_t j = 0; j < 8; ++j) {
            switch (getPulse()) {
                case PulseHigh:
                    readData[i] <<= 1;
                    readData[i] |= 1;
                    break;
                case PulseLow:
                    readData[i] <<= 1;
                    break;
                case Timeout:
#ifdef LR_DHT22_DEBUG
                    Serial.print(F("Timeout for reading byte "));
                    Serial.print(i);
                    Serial.print(F(" bit "));
                    Serial.println(j);
#endif
                    goto END_READ;
            }
        }
    }

#ifdef LR_DHT22_DEBUG
    Serial.print(F("Read bytes: 0x"));
    Serial.print(readData[0], HEX);
    Serial.print(F(", 0x"));
    Serial.print(readData[1], HEX);
    Serial.print(F(", 0x"));
    Serial.print(readData[2], HEX);
    Serial.print(F(", 0x"));
    Serial.print(readData[3], HEX);
    Serial.print(F(", 0x"));
    Serial.print(readData[4], HEX);
#endif
    
    // Check the checksum
    if (readData[4] != ((readData[0]+readData[1]+readData[2]+readData[3])&0xff)) {
#ifdef LR_DHT22_DEBUG
        Serial.println(F("Checksum does not match."));
#endif
        goto END_READ;
    }
    
    // Convert the read bits into temperature and humidity
    measurement.temperature = (static_cast<uint16_t>(readData[2]&0x7f) << 8) + readData[3];
    measurement.temperature /= 10.0f;
    if ((readData[2] & 0x80) != 0) {
        measurement.temperature *= -1.0f;
    }
    measurement.humidity = (static_cast<uint16_t>(readData[0]&0x7f) << 8) + readData[1];
    measurement.humidity /= 10.0f;
    if ((readData[0] & 0x80) != 0) {
        measurement.humidity *= -1.0f;
    }
    
END_READ:
    // End time critical code
    interrupts();
    
    return measurement;
}
