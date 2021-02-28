/* This app is the emoncms node for inside/outside environment monitors
 * and IR control of a dyson heater 
 * 
 * This class handles the Dyson AM09 heater
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

#ifndef __dysonController_h
#define __dysonController_h
        
#include <IRremoteLearn.h>

// These are the learned IR commands for the Dyson AM09

// Our notes are built to use TX as the send pin
const int SEND_PIN = TX;

class DysonController
{
        public:
            DysonController(void);
            
            void powerOn(void);
            void powerOff(void);
            
            void tempUp(void);
            void tempDown(void);
            
            void speedUp(void);
            void speedDown(void);
            
            void diffuseMode();
            void directMode();
            
            void setTemp(int);
            
        private:
        
            
};

#endif

