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
#include <RTClib.h>


class Storage;


/// A single log record.
///
class LogRecord
{
public:
    /// Create a new log record using the given values.
    ///
    /// @param dateTime The time of the record.
    /// @param temperature The temperature in celsius.
    /// @param humidity The humidity as percentage 0-100.
    ///
    LogRecord(const DateTime &dateTime, float temperature, float humidity);

    /// Create a special null record.
    ///
    /// This records are used in error situations.
    ///
    LogRecord();
    
    /// dtor
    ///
    ~LogRecord();

public:
    /// Check if this is a null record.
    ///
    bool isNull() const;
    
    /// Get the time of the record.
    ///
    inline DateTime getDateTime() const { return _dateTime; }
    
    /// Get the temperature of the record in celsius.
    ///
    inline float getTemperature() const { return _temperature; }
    
    /// Get the humidity of the record in percent 0-100.
    ///
    inline float getHumidity() const { return _humidity; }
    
    /// Write this record to the serial interface.
    ///
    /// The format is: date/time, temperature, humidity
    /// Example: 2015-08-22 12:42:21,80,25
    ///
    void writeToSerial() const;
    
private:
    DateTime _dateTime;
    float _temperature;
    float _humidity;
};


/// The log system to write and read all sensor data.
///
class LogSystem
{
public:
    /// Create a new log system instance.
    ///
    /// @param reservedForConfig The number of bytes reserved for the configuration
    ///    at the start the storage area.
    /// @param storage The storage to use for the log system.
    ///
    LogSystem(uint32_t reservedForConfig, Storage *storage);
    
    /// dtor
    ///
    ~LogSystem();
    
public:
    /// Initialize the log system
    ///
    void begin();
    
    /// Get the maximum number of records for the given storage.
    ///
    inline uint32_t maximumNumberOfRecords() const { return _maximumNumberOfRecords; }
    
    /// Get the number of records currently in the storage.
    ///
    inline uint32_t currentNumberOfRecords() const { return _currentNumberOfRecords; }
    
    /// Read a record from the storage.
    ///
    LogRecord getLogRecord(uint32_t index) const;
    
    /// Append a record to the storage.
    ///
    /// This will first zero the record (index+1) if possible, before
    /// writing the given record to (index).
    ///
    /// @param logRecord The record to append.
    /// @return true on success, false if the storage is full.
    ///
    bool appendRecord(const LogRecord &logRecord);
    
    /// Format the storage.
    ///
    /// This will set the initial two records of the storage area to zero.
    /// It is enough to initialize the storage with minimum number of writes.
    ///
    void format();
    
private:
    uint32_t _reservedForConfig;
    Storage *_storage;
    uint32_t _currentNumberOfRecords;
    uint32_t _maximumNumberOfRecords;
};



