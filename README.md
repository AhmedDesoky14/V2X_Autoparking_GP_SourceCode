Repository for source code of Graduation Project: Secured V2X Enabled Autoparking system using Embedded Linux

Our Graduation Project "Secured V2X-Enabled Automatic Parking System", supported by Valeo mentorship program, comes to address critical problems related to wasted time finding free parking areas and the ability to park safely. And to introduce smart solutions by creating and implementing smart infrastructures on roads and parking areas to introduce parking finding and reservation services by secured V2X communications. Also introducing a level 3 autonomous vehicle with real auto parking scenario minimizing accidents risks.

Project Highlights:
1- Autonomous Parking System:
We developed advanced algorithms for sensor fusion, path planning, and vehicle cruise control to enable fully autonomous parking. Utilizing EKF state estimation, QR Camera Localization, Hybrid A* path planning, Stanley and PID controller. Auto parking process was successfully simulated using CARLA.

2- Intuitive User Interface:
We designed a user-friendly graphical interface using Qt QML running on custom Linux based OS, offering drivers real-time assistance and information throughout driving and the parking process.

3- Infrastructures Network using MQTT
We utilized MQTT protocol exploiting its low-latency and real-time properties for data exchange between infrastructures. Using Thingspeak MQTT free broker and implemented MQTT client based on Paho MQTT C open-source library running on STM32F4 microcontrollers.

4- V2X Communications
The project utilized V2X technology to establish seamless communication between vehicles and infrastructures. this was conducted using TCP/IP model and WiFi local network created by infrastructure's ESP8266 as Access point and vehicle's Raspberry Pi WiFi as Station.
 
5- Vehicle OnBoard Communications
To be able to exchange data between many ECUs on vehicle, OnBoard communications was required. We utilized CAN Protocol by implementing a full CAN stack to exchange vehicle control commands and sensors data between the different ECUs to ensure seamless vehicle control and real-time performance. Used sensors are JSN-SR04T Ultrasonic sensors, AS5600 Magnetic Encoders, GY271 Magnetometer and GY521 IMU.

6- Security Enhancements
Comprehensive security measures were incorporated including "AES","RSA" encryption algorithms and SHA2 hash algorithm. Securing data transmitted between vehicles and infrastructures for V2X communications and communications between vehicle OnBoard ECUs.