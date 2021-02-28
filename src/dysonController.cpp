/*
 * This app is the emoncms node for inside/outside environment monitors
 * 
 * This class handles the dyson AM09 heat/cool fan
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

#include "dysonController.h"

// defaults to TX pin if no pin specified, see IRsend::IRsend(int txpin) header for PWM pins available
IRsend irsend(SEND_PIN);

DysonController::DysonController(void)
{
    
};


void DysonController::powerOn(void)
{
    unsigned int buffer[4] = {0x24, 0x9E, 0xB3, 0x25};
    irsend.sendRaw(buffer, 4, 32);
    delay(1000);    
}

void DysonController::powerOff(void)
{
    
}
            
void DysonController::tempUp(void)
{
    
}

void DysonController::tempDown(void)
{
    
}
            
void DysonController::speedUp(void)
{
    
}

void DysonController::speedDown(void)
{
    
}
    
void DysonController::diffuseMode()
{
    
}

void DysonController::directMode()
{
    
}
        
void DysonController::setTemp(int newTemp)
{
    
}

