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


#define LR_STORAGE_FRAM


/// A storage class
///
/// This storage class provide a simple abstraction to the hardware layer.
/// The software can use either the EEPROM to any attached memory to
/// store the data.
///
/// In a desktop application, this would be implemented using a virtual abstract
/// interface, but here we just replace the implementation of the class
/// depening on the used memory area.
///
class Storage
{
public:
    /// ctor
    ///
    Storage();
    
    /// dtor
    ///
    ~Storage();
    
public:
    /// Initialize the storage.
    ///
    /// @return true on success, false if the storage could not be initialized.
    ///   Expects an error message on serial.
    ///
    bool begin();
    
    /// Get the size of the available memory.
    ///
    uint32_t size();
    
    /// Read a byte from this memory.
    ///
    uint8_t readByte(uint32_t index);
    
    /// Read multiple bytes from this memory.
    ///
    /// @param firstIndex The index for the first byte.
    /// @param data A pointer to the target buffer.
    /// @param size The number of bytes to read into the target buffer.
    ///
    void readBytes(uint32_t firstIndex, uint8_t *data, uint32_t size);
    
    /// Write a byte to this memory.
    ///
    void writeByte(uint32_t index, uint8_t data);
    
    /// Write multiple bytes to this memory.
    ///
    /// @param startIndex The index for the first byte.
    /// @param data A pointer to the data to write into memory.
    /// @param size The number of bytes to write to the memory.
    ///
    void writeBytes(uint32_t firstIndex, const uint8_t *data, uint32_t size);
};

