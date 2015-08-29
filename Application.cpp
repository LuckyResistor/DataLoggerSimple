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
#include "Application.h"


#include <avr/interrupt.h>


Application::Application()
    : dht(3, DHT22), rtc(), modeSelector(), storage(), logSystem(0, &storage)
{
}


Application::~Application()
{
}


// Create an empty interrupt for timer2 overflow.
// The interrupt is only used to wake from sleep.
EMPTY_INTERRUPT(TIMER2_OVF_vect)


namespace {

    
// constants
const char DATE_FORMAT[] PROGMEM = "%04d-%02d-%02d %02d:%02d:%02d";

    
}


void Application::signalError(uint8_t errorNumber)
{
    pinMode(SIGNAL_LED, OUTPUT);
    while (true) {
        for (uint8_t i = 0; i < errorNumber; ++i) {
            digitalWrite(SIGNAL_LED, HIGH);
            delay(300);
            digitalWrite(SIGNAL_LED, LOW);
            delay(300);
        }
        delay(1000);
    }
}


void Application::sendDateTimeToSerial(const DateTime &dateTime)
{
    char buffer[32]; //yyyy-mm-dd hh:mm:ss
    sprintf_P(buffer, DATE_FORMAT, dateTime.year(), dateTime.month(), dateTime.day(), dateTime.hour(), dateTime.minute(), dateTime.second());
    Serial.print(buffer);
}


void Application::sendDurationToSerial(uint32_t seconds)
{
    const uint8_t rtSeconds = seconds%60;
    const uint8_t rtMinutes = (seconds/60)%60;
    const uint8_t rtHours = (seconds/3600)%24;
    const uint32_t rtDays = (seconds/86400);
    if (rtDays>0) {
        Serial.print(rtDays);
        Serial.print(F(" days "));
    }
    if (rtDays>0||rtHours>0) {
        Serial.print(rtHours);
        Serial.print(F(" hours "));
    }
    if (rtDays>0||rtHours>0||rtMinutes>0) {
        Serial.print(rtMinutes);
        Serial.print(F(" minutes "));
    }
    Serial.print(rtSeconds);
    Serial.print(F(" seconds."));
}


void Application::setup()
{
    // Initialize the serial interface.
    Serial.begin(57600);

    // Initialize all libraries
    Wire.begin();
    dht.begin();
    rtc.begin();
    modeSelector.begin();
    
    // Write some initial greeting.
    Serial.println(F("Lucky Resistor's Data Logger Version 1"));
    Serial.println(F("--------------------------------------"));
    Serial.flush();

    if (!storage.begin()) {
        signalError(4);
    }
    
    // Initialize the log system.
    logSystem.begin();

    if (!rtc.isrunning()) {
        Serial.println(F("Warning! RTC is not running."));
        signalError(3);
    }
    
    // Check the mode.
    if (modeSelector.getMode() == ModeSelector::Read) {
        Serial.print(F("Read selected. Sending "));
        const uint32_t numberOfRecords = logSystem.currentNumberOfRecords();
        Serial.print(numberOfRecords);
        Serial.println(F(" records."));
        for (uint32_t i = 0; i < numberOfRecords; ++i) {
            LogRecord record = logSystem.getLogRecord(i);
            record.writeToSerial();
        }
        Serial.println(F("Finished successfully. Enter sleep mode."));
        Serial.flush();
        set_sleep_mode(B010); // Enter power-down mode.
        cli(); // no interrupts to wake the cpu again.
        sleep_mode(); // enter sleep mode.
    } else if (modeSelector.getMode() == ModeSelector::Format) {
        Serial.println(F("Format (!) selected. Format is starting in ~10 seconds."));
        // using LED on pin 13 to blink aggresively.
        pinMode(SIGNAL_LED, OUTPUT);
        digitalWrite(SIGNAL_LED, LOW);
        for (int8_t i = 1; i <= 10; ++i) {
            Serial.print(i);
            Serial.println("...");
            Serial.flush();
            for (int8_t j = 0; j < i; ++j) {
                digitalWrite(SIGNAL_LED, HIGH);
                delay(50);
                digitalWrite(SIGNAL_LED, LOW);
                delay(100);
            }
            delay(1000);
        }
        Serial.println(F("Erasing all logged records..."));
        logSystem.format();
        Serial.println(F("Format finished successfully. Enter sleep mode."));
        Serial.flush();
        set_sleep_mode(B010); // Enter power-down mode.
        cli(); // no interrupts to wake the cpu again.
        sleep_mode(); // enter sleep mode.
    } else {
        // Write about the logging mode.
        Serial.print(F("Logging selected. Interval = "));
        Serial.println(modeSelector.getIntervalText());
        Serial.print(F("Maximum records: "));
        Serial.println(logSystem.maximumNumberOfRecords());
        Serial.print(F("Current records: "));
        Serial.println(logSystem.currentNumberOfRecords());
        // calculate how long we can record data.
        const uint32_t availableRecords = logSystem.maximumNumberOfRecords()-logSystem.currentNumberOfRecords();
        Serial.print(F("Avaliable records: "));
        Serial.println(availableRecords);
        const uint32_t recordingTime = (availableRecords*modeSelector.getInterval());
        Serial.print(F("Recording time: "));
        sendDurationToSerial(recordingTime);
        Serial.println();
        Serial.print(F("Current time: "));
        _currentTime = rtc.now();
        sendDateTimeToSerial(_currentTime);
        Serial.println();
        const DateTime recordingEndTime = DateTime(_currentTime.unixtime() + recordingTime);
        Serial.print(F("Recording end time: "));
        sendDateTimeToSerial(recordingEndTime);
        Serial.println();
        
        // Enable the red led as output.
        pinMode(SIGNAL_LED, OUTPUT);
        digitalWrite(SIGNAL_LED, LOW);
        
        // Prepare the timer2 to wake from sleep.
        ASSR = 0; // Synchronous internal clock.
        TCCR2A = _BV(WGM21)|_BV(WGM20); // Normal operation. Fast PWM.
        TCCR2B |= _BV(CS22)|_BV(CS21)|_BV(CS20); // Prescaler to 1024.
        OCR2A = 0; // Ignore the compare
        OCR2B = 0; // Ignore the compare
        TIMSK2 = _BV(TOIE2); // Interrupt on overflow.
        sei(); // Allow interrupts.
        
        // Keep the sleep interval between 1s and 1m
        _sleepDelay = min(modeSelector.getInterval() / 10, 60);
        
        // Set the next record time.
        _nextRecordTime = DateTime(_currentTime.unixtime() + modeSelector.getInterval());
    }
}


void Application::loop()
{
    // Read the values from the sensor
    const float humidity = dht.readHumidity();
    const float temperature = dht.readTemperature();
    
    // Write the record
    LogRecord logRecord(_currentTime, temperature, humidity);
    if (!logSystem.appendRecord(logRecord)) {
        // storage is full
        signalError(5);
    }

#ifdef LR_APPLICATION_DEBUG
    Serial.print(F("Write log: t:"));
    Serial.print(temperature);
    Serial.print(F("C h:"));
    Serial.print(humidity);
    Serial.print(F("% time:"));
    sendDateTimeToSerial(_currentTime);
    Serial.println();
    Serial.flush();
#endif
    
    // Wait until we reached the right time.
    while (true) {
        powerSave(_sleepDelay);
        _currentTime = rtc.now();
        const int32_t secondsToNextRecord = _nextRecordTime.unixtime()-_currentTime.unixtime();
        if (secondsToNextRecord<_sleepDelay) {
            if (secondsToNextRecord > 0) {
                powerSave(secondsToNextRecord);
            }
            break;
        }
    }
    
    // Read the current time for the log entry.
    _currentTime = rtc.now();
    
    // Increase the next record time. This will keep the timing stable, even
    // if we do not wake up precise at the right time.
    _nextRecordTime = DateTime(_nextRecordTime.unixtime() + modeSelector.getInterval());
}



void Application::powerSave(uint16_t seconds)
{
    // Go to sleep (for 1/60s).
    SMCR = _BV(SM1)|_BV(SM0); // Power-save mode.
    const uint32_t waitIntervals = (seconds*61); // This is almost a second.
    for (uint32_t i = 0; i < waitIntervals; ++i) {
        TCNT2 = 0; // reset the timer.
        SMCR |= _BV(SE); // Enable sleep mode.
        sleep_cpu();
        SMCR &= ~_BV(SE); // Disable sleep mode.
    }
}










