/*
 * This app is the emoncms node for inside/outside environment monitors
 *
 * This class handles the bme280 and 18b20 sensors
 * It also supports an IR blaster used for customised device remote control
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

#ifndef envnode_h
#define envnode_h

#include <Particle.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DS18B20.h>

// The pin the DS18B20 is connected to (if mounted)
const int16_t dsData = D2;

#define MAX_DS18_RETRY  4

class EnvNode
{
        public:
            EnvNode(void);
            
            void initSensors(void);  
            
            float getEnclosureTemp(void);
            float getPressure(void);
            float getHumidity(void);
            
            float getExternalTemp(void);
            
            bool bmeFound(void);
            bool ds18Found(void);
        
        private:
        
          
            bool _bmeFound;
            bool _ds18Found;
            
            
};

#endif