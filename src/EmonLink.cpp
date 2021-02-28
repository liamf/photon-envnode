/*
 * This app is the emoncms node for inside/outside environment monitors
 * This class does the comms to emonCMS, both simple provisioning and reporting of data
 * it calls our API on the emonCMS server, and fetches some data
 * It will retrive our assigned node name, and the API key for posting, provided that our device ID is on the known list
 *
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
 
HttpClient http;
JsonParser parser;

http_header_t headers[] = {
        { "Accept" , "*/*"},
        { NULL, NULL } // NOTE: Always terminate headers will NULL
};

 EmonLink::EmonLink(void)
 {
     
     _isProvisioned = false;
     _debugLogging = false;
     
     // Default node hostname on the local network
     _hostName = "emonpi.oakglen.park";
     _hostPort = 5000;
     
     _myID = System.deviceID();
     
 };
 
 EmonLink::EmonLink(String host) : _hostName(host)
 {
        EmonLink();
        //_hostPort = 5000;
        //_myID = System.deviceID();
 };
 
 bool EmonLink::isProvisioned(void)
 {
     return _isProvisioned;
 }
 
 bool EmonLink::attemptProvisioning(void)
 {
    return getProvisioningData();
 }
 
 String EmonLink::getEmonName(void)
 {
     return _emonName;
 }
 
 String EmonLink::getDeviceName(void)
 {
     return _deviceName;
 }
 
 void EmonLink::setCloudDeviceName(char *name)
 {
     _deviceName = name;
 }
 
// Post data to EmonCMS: external temp if we have a DS18 sensor
bool EmonLink::postExternalSensorData(float temp)
{
    // Build a JSON payload
    JsonWriterStatic<512> jw;
    char trimmedValue[32];
    bool stat = true;
    
    // Sometimes, the sensor returns "NAN" for a reading.
    // In that case, just discard the report. It happens sufficiently rarely/randomly that we can just ignore
    
    if( temp != NAN ) 
    {
    
        {
            JsonWriterAutoObject obj(&jw);
            
    		// Add various types of data
    		snprintf(trimmedValue,sizeof(trimmedValue)-1,"%.2f", temp);
    		if( strncmp(trimmedValue,"nan", 3) != 0) {
    		    jw.insertKeyValue("extTemp", trimmedValue);
    		}
        }
        
        
        http_request_t request; 
        http_response_t response;  
        
        // EmonCMS post to the node on port 80
        request.hostname = _hostName;
        request.port = 80;
        request.path = String::format("/input/post?node=%s&fulljson=%s&apikey=%s", _deviceName.c_str(),jw.getBuffer(), _apiKey.c_str());
        
        http.get(request, response, headers);   
        
        stat = (response.status == 200);
        
        // Log this via the hardcoded webhook, if debugging is turned on
        if( _debugLogging)
        {
            Particle.publish("sensDebugLog", request.path, PRIVATE);    
        }
    }
    
    return stat;
}        
        
// Post data to EmonCMS: internal data from the BME280, if one is fitted
bool EmonLink::postInternalSensorData(float temp, float pressure, float humidity)
{
    // Build a JSON payload
    JsonWriterStatic<512> jw;
    char trimmedValue[32];
    bool stat = true;
    
    // Sometimes, the sensor returns "NAN" for a reading.
    // In that case, just discard the report. It happens sufficiently rarely/randomly that we can just ignore
    
    if( temp != NAN && pressure != NAN && humidity != NAN ) 
    {
    
        {
            JsonWriterAutoObject obj(&jw);
            
    		// Add various types of data
    		snprintf(trimmedValue,sizeof(trimmedValue)-1,"%.2f", temp);
    		if( strncmp(trimmedValue,"nan", 3) != 0) {
    		    jw.insertKeyValue("encTemp", trimmedValue);
    		}
    		
    		snprintf(trimmedValue,sizeof(trimmedValue)-1, "%.2f", pressure);
    		if( strncmp(trimmedValue,"nan", 3) != 0) {
    		    jw.insertKeyValue("pressure", trimmedValue);
    		}    		
    		
    		snprintf(trimmedValue,sizeof(trimmedValue)-1, "%.2f", humidity);
    		if( strncmp(trimmedValue,"nan", 3) != 0) {
    		    jw.insertKeyValue("humidity", trimmedValue);
    		} 
        }
        
        
        http_request_t request; 
        http_response_t response;  
        
        // EmonCMS post to the node on port 80
        request.hostname = _hostName;
        request.port = 80;
        request.path = String::format("/input/post?node=%s&fulljson=%s&apikey=%s", _deviceName.c_str(),jw.getBuffer(), _apiKey.c_str());
        
        http.get(request, response, headers);   
        
        stat = (response.status == 200);
        
        // Log this via the hardcoded webhook, if debugging is turned on
        if( _debugLogging)
        {
            Particle.publish("sensDebugLog", request.path, PRIVATE);    
        }
    }
    
    return stat;
}
 
 // Private functions
 // Call the API server on the emonCMS node to get the API key and other data
 bool EmonLink::getProvisioningData(void)
 {

    String returnedId;
    
    http_request_t request; 
    http_response_t response;  
    
    _isProvisioned = false;
    
    request.hostname = _hostName;
    request.port = _hostPort;
    request.path = String::format("/api/v1/nodes/?id=%s", _myID.c_str());  
    
    http.get(request, response, headers);
    
    parser.clear();
    parser.addString(response.body);
    
    if( !parser.parse())
    {
        // Didn't parse: not a valid response
        return false;
    }
    
    // OK we parsed the reponse
    // If we were recognised, it should have returned our ID to us ...
    if(!parser.getOuterValueByKey("id", returnedId) || !returnedId.equals(_myID))
    {
        // We weren't recognosed ... probably new device, need to added to the provisioning list
        Particle.publish("DEBUG","Device ID not recognised by emonCMS API. Check provisioning service and list");
        return false;
    }
    
    if( !parser.getOuterValueByKey("apikey", _apiKey))
    {
        Particle.publish("DEBUG","APIKEY not returned by emonCMS API. Check provisioning protocol");
        return false;
    }
    
    if( !parser.getOuterValueByKey("name", _emonName))
    {
        Particle.publish("DEBUG","Node name not returned by emonCMS API. Check provisioning protocol");
        return false;
    }   

    
    // All good, we should be good to go for publishing
    _isProvisioned = true;
    return _isProvisioned;
    
 }
 

void EmonLink::setDebugLogging(bool logging)
{
    _debugLogging = logging;
}
