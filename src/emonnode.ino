/*
 * This app is the emoncms node for inside/outside environment monitors
 * It uses bme280 and 18b20 sensors
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

#include "EmonLink.h"
#include "EnvNode.h"
#include "dysonController.h"

EnvNode envNode;
EmonLink emonLink;
DysonController dysonController;


// Simple variable output from our devices
double temperature;
double enclosureTemperature;
double pressure;
double humidity;

Timer sensorInitTimer(20000, searchForSensors);
Timer provisioningTimer(10000, provisionEmonCMSNode);
Timer measurementTimer(30000, refreshSensorReadings);

// Some globals: take off the stack
String myCloudName;
char dev_name[32] = "";
bool publishName = false;
bool debugLogging = false;
bool dysonControl = false;

// Some control flags
bool attemptCMSProvisioning = false;
bool takeSensorMeasurement = false;
bool attemptSensorInit = false;
bool resetFlag = false;

int  reportFailureCount = 0;

#define DELAY_BEFORE_REBOOT 2000
unsigned int rebootDelayMillis = DELAY_BEFORE_REBOOT;
unsigned long rebootSync = millis();


// Our init function
void setup() {

    Serial.begin(9600);
    
    //  Remote Reset Particle Function
    Particle.function("reset", cloudResetFunction);
    Particle.function("debug", toggleDebugFunction);
    Particle.function("dyson", setDysonControl);
    
    // Publish some variables to play with in the console
    Particle.variable("temperature", temperature);  
    Particle.variable("enctemp", enclosureTemperature); 
    Particle.variable("pressure", pressure); 
    Particle.variable("humidity", humidity); 
    
    // Initialise our shared variabled
    temperature = 255.0;
    enclosureTemperature = 255.0;
    humidity = 255.0;
    pressure = 0.0;

    // Initialise the sensor handler first time
    // If it doesn't work, we'll retry on a timer
    envNode.initSensors();
    
    // Setup some timers to trigger provisioning, and temperature measurement
    // They provision our device to emonCMS
    provisioningTimer.start();
    measurementTimer.start();
    
    // If both sensors not found, start the rettry timer
    if( !envNode.bmeFound() || !envNode.ds18Found() ) {
        sensorInitTimer.start();
    }
    
    // Get my device name
    Particle.subscribe("particle/device/name", nameEventHandler);
    Particle.publish("particle/device/name");  // <-- ask the cloud for the name to be sent to you
}

void loop() {
    // Let the emonLink know what our name is
    if( publishName )
    {
        emonLink.setCloudDeviceName(dev_name);
        myCloudName = String(dev_name);
        Particle.publish("INFO","My assigned device name is " + myCloudName);
        publishName = false;
    }
    
    // Now check for timer triggered actions
    
    if( attemptSensorInit) 
    {
        // The timer will turn this back on, if running
        attemptSensorInit = false;
        
        Particle.publish("INFO", "Retrying searching for sensors");
        
        envNode.initSensors();
        
        String bmeMsg = "I will ";
        if( !envNode.bmeFound())
        {
            bmeMsg.concat("NOT currently be able to ");
        }
        bmeMsg.concat("report BME280 sensor data");
        Particle.publish("INFO",bmeMsg);
            
        String ds18Msg = "I will ";
        if( !envNode.ds18Found())
        {
            ds18Msg.concat("NOT currently be able to ");    
        }
        ds18Msg.concat("report DS18B20 sensor data");
        Particle.publish("INFO",ds18Msg);
            
        // If we found both sensors, we're done. Kill time timer.
        if( envNode.bmeFound() && envNode.ds18Found()) {
            sensorInitTimer.stop();       // No need to search again
        }        
    }
    
    
    if(attemptCMSProvisioning)
    {
        // The timer will turn this back on, if running
        attemptCMSProvisioning = false;

        if( emonLink.attemptProvisioning())
        {
            Particle.publish("INFO","Provisioned and ready to talk to emonpi");
            // What will we publish?
            
            String bmeMsg = "I will ";
            if( !envNode.bmeFound())
            {
                bmeMsg.concat("NOT currently be able to ");
            }
            bmeMsg.concat("report BME280 sensor data");
            Particle.publish("INFO",bmeMsg);
            
            String ds18Msg = "I will ";
            if( !envNode.ds18Found())
            {
                ds18Msg.concat("NOT currently be able to ");    
            }
            ds18Msg.concat("report DS18B20 sensor data");
            Particle.publish("INFO",ds18Msg);
            
            provisioningTimer.stop();       // Kill the timer. To reprovision a node, force a reboot via the Particle console.
        }
        else
        {
            // Set a timer to try provisioning again, in 30 seconds
            Particle.publish("DEBUG","Not provisioned to emonpi. Will retry ...");
        }

    }
    
    // Measurement time
    if(takeSensorMeasurement)
    {
        takeSensorMeasurement = false;
        
        // We do two separate data posts to emonCMS to make this code very simple
        if( envNode.bmeFound() )
        {
            enclosureTemperature = envNode.getEnclosureTemp();
            pressure = envNode.getPressure();
            humidity = envNode.getHumidity();
        
            // Publish on the event stream as an attidition way of getting them
            Particle.publish("ENCTEMP", String::format("%.2f", enclosureTemperature));
            Particle.publish("PRESSURE", String::format("%.2f", pressure));
            Particle.publish("HUMIDITY", String::format("%.2f", humidity));
            
            // Post to emoncms
            if( emonLink.isProvisioned())
            {
                if( !emonLink.postInternalSensorData(enclosureTemperature, pressure, humidity))
                {
                    // Somethihg wrong: emonCMS node might be offline
                    // We start a counter - if it fails enough times, we'll trigger a reprovisioning
                    reportFailureCount++;
                }
                else
                {
                    reportFailureCount = 0;
                }
            }
            
        }

    
        if( envNode.ds18Found() )
        {
            temperature = envNode.getExternalTemp();
            
            // Publish on the event stream
            Particle.publish("EXTTEMP", String::format("%.2f", temperature));   
            
            // Post to emoncms
            if( emonLink.isProvisioned())
            {
                if( !emonLink.postExternalSensorData(temperature))
                {
                    // Somethihg wrong: emonCMS node might be offline
                    // We start a counter - if it fails enough times, we'll trigger a reprovisioning
                    reportFailureCount++;
                }
                else
                {
                    reportFailureCount = 0;
                }              
            }
        }
        
        // Do we want to try provisioning again?
        if( reportFailureCount > MAX_REPORT_RETRIES)
        {
            reportFailureCount = 0;
            provisioningTimer.start(); 
        }
    }
    
    //  Remote Reset Function
    if ((resetFlag) && (millis() - rebootSync >=  rebootDelayMillis)) {
        // do things here  before reset and then push the button
        Particle.publish("Debug", "Remote Reset Initiated", 300, PRIVATE);
        System.reset();
    }
    

}


// Attempts provisioning from emonCMS service
void provisionEmonCMSNode(void)
{
    attemptCMSProvisioning = true;
}

void searchForSensors(void)
{
    // If either sensor is not detected, try again
    // All our nodes should have both sensors, but sometimes finding the DS18 is not reliable
    if( !envNode.bmeFound() || !envNode.ds18Found() ) {
        attemptSensorInit = true;    
    }
}

// Handler which gets our Sensor readings
// For now, we just update out variables and publish to emonCMS
void refreshSensorReadings(void)
{
    takeSensorMeasurement = true;      // debug: never update
}

 // Handler to out our Cloud device nane
 void nameEventHandler(const char *topic, const char *data) {
    strncpy(dev_name, data, sizeof(dev_name)-1);
    publishName = true;
}

//  Remote Reset Function, in case we want to rename/change the device and get it to restart without a reflash
int cloudResetFunction(String command) {
    resetFlag = true;
    rebootSync = millis();
    return 0;
}

// For now just a dumb function that toggles debug state
// Debug state writes to google sheets via our configured webhook
int toggleDebugFunction(String command) {
    debugLogging = !debugLogging;  
    emonLink.setDebugLogging(debugLogging);
    
    return 0;
}

// Turn of/off controlling the Dyson remote control
int setDysonControl(String command) {
   
   dysonController.powerOn();
   return 0; 
}
