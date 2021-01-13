# photon-envnode
Particle Photon-powered emonCMs environment monitoring nodes

The nodes use BME280 and/or DS18B20 sensors to read temperature, humidity and pressure.

Nodes publish sensors readings in a few ways
* by exposting Particle variables
* by publishing Particle events
* by logging to a local emonCMS server

The nodes provision themselves by calling a small daemon running on the emonCMS server
