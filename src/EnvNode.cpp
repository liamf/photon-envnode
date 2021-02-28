/*
 * This app is the emoncms node for inside/outside environment monitors
 * 
 * This class handles the bme280 and 18b20 sensors
 *
 * Liam Friel
 *
 * Copyright (c) 2020 Liam Friel
 *
 * Permission is hereby granted, free of charge, 
 * to any person obtaining a copy of this software and 
 * associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom 
 * the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice 
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "EnvNode.h"
        
DS18B20 extDS18(dsData, true);
Adafruit_BME280 bmeSensor;        
            
EnvNode::EnvNode(void)
{
    _ds18Found = false;
    _bmeFound = false;
    
};

// Find the bme280 and ds18b20 sensors (if present)
// We can use these later to read internal/external team and the other environment values
// The caller might call this multiple times (since finding the DS18 does not seem to be 100% reliable) so we make it safe to call multiple times
void EnvNode::initSensors(void)
{
    // Find the BME280
    if(!_bmeFound) {
        _bmeFound = bmeSensor.begin();
    }
    
    // See if we can find an external sensor
    if( !_ds18Found) {
        _ds18Found = extDS18.search();
    }

}

float EnvNode::getEnclosureTemp(void)
{
    if( !_bmeFound)
    {
        // Caller should not really have called us ... but let's return a totally impossible answer anyway
        return 255.0;
    }
    
    return bmeSensor.readTemperature();
    
}

float EnvNode::getExternalTemp(void)
{
    if( !_ds18Found )
    {
        // Caller should not really have called us ... but let;s return a totally impossible answer anyway
        return 255.0;
    }
    
    float temp;
    int   i = 0;

    do 
    {
        temp = extDS18.getTemperature();
    } while (!extDS18.crcCheck() && MAX_DS18_RETRY > i++);

    if (i >= MAX_DS18_RETRY) {
        // No valid reading .. use our spurious reading again
        temp = 255.0;
    }

    return temp;
}

// Returns pressure in hPa
float EnvNode::getPressure(void)
{
    if( !_bmeFound)
    {
        // Caller should not really have called us ... but let;s return a totally impossible answer anyway
        return 255.0;        
    }
    
    return (bmeSensor.readPressure()/100.0F);
}

float EnvNode::getHumidity(void)
{
    if( !_bmeFound)
    {
        // Caller should not really have called us ... but let;s return a totally impossible answer anyway
        return 255.0;        
    }
    
    return bmeSensor.readHumidity();
}


bool EnvNode::bmeFound(void)
{
    return _bmeFound;
}

bool EnvNode::ds18Found(void)
{
    return _ds18Found;
}

