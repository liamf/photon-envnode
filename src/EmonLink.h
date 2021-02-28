/*
 * This app is the emoncms node for inside/outside environment monitors
 *
 * This class does the comms to emonCMS, both simple provisioning and reporting of data
 * it calls our API on the emonCMS server, and fetches some data
 * It will retrive our assigned node name, and the API key for posting, provided that our device ID is on the known list
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
 
 #ifndef emonlink_h
 #define emonline_h
 
 #include <Particle.h>
 #include <HttpClient.h>
 #include <JsonParserGeneratorRK.h>
 #include <math.h>
 
 // If we fail to report this number of times, we'll try to reprovision
 #define MAX_REPORT_RETRIES 1000

 
 class EmonLink
 {
     public:
        EmonLink(void);
        EmonLink(String hostName);
        
        // Some helper functions
        bool isProvisioned(void);
        bool attemptProvisioning(void); // Force provisioning
        
        void setCloudDeviceName(char *);
        
        String getDeviceName(void); // OUr name in the Particle console
        String getEmonName(void);
        
        bool postExternalSensorData(float temp);
        bool postInternalSensorData(float temp, float pressure, float humidity);
        
        void setDebugLogging(bool);
        
    private:

        bool getProvisioningData(void); // Call the emonCMS node and ask for some key information
        
        String formatExternalTemp(float temp);
        String formatInternalSensorData(float temp, float pressure, float humidity);
        
        bool postToEmonCMS(String jsonPayload);
        
        bool _isProvisioned;
        bool _debugLogging;
        
        String _apiKey;
        String _hostName;
        int _hostPort;
        
        String _myID;           // Device ID - unique
        String _deviceName;     // The name of this device in the particle cloud
        String _emonName;       // Name returned by emonCMS 
        
 }; 
 
 
 #endif
 