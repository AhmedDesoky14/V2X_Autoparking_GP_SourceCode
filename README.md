## Graduation Project: Secured V2X Enabled Autoparking system using Embedded Linux Repository

Our Graduation Project "Secured V2X-Enabled Automatic Parking System", supported by Valeo mentorship program, comes to address critical problems related to wasted time finding free parking areas and the ability to park safely. And to introduce smart solutions by creating and implementing smart infrastructures on roads and parking areas to introduce parking finding and reservation services. Also introducing a level 3 autonomous vehicle with real auto parking scenario minimizing accidents risks.

## Objective

Project's objective is to utilize V2I Services provided by smart infrastructures via Wi-Fi communication module and using TCP/IP. 
Combined with Hybrid A* path planning algorithm, EKF (Extended Kalman Filter) state estimation, Stanley and PID algorithms for controlling.
To implement a real auto parking scenario. Visualized using interactive GUI powered by Qt Quick running on custom Linux based OS. 
Vehicle components are distributed on group of ECUs all connected via secured CAN network. **Agile methodology applied in this project.**

## =================================================================================================================================================== ##

## Project Highlights:

1- Autonomous Parking System:
We developed advanced algorithms for sensor fusion, path planning, and vehicle cruise control to enable fully autonomous parking. Utilizing EKF state estimation, QR Camera Localization, Hybrid A* path planning, Stanley and PID controller. Auto parking process was successfully simulated using CARLA.

- Work and results are in "Main RaspberryPi ECU" folder

**Contributors:**
    - Omar Adel
    - Omar Tolba

## =================================================================================================================================================== ##

2- Intuitive Graphical User Interface:
We designed a user-friendly graphical interface using Qt QML running on custom Linux based OS, offering drivers real-time assistance and information throughout driving and the parking process.

- Work and results are in another repository, link: https://github.com/AhmedDesoky14/V2X_Autoparking_GP_GUI.git

**Contributors:**
    - Ziad Emad
    - Esraa Fawzy

## =================================================================================================================================================== ##

3- MQTT-Network for Infrastructures
We utilized MQTT protocol exploiting its low-latency and real-time properties for data exchange between infrastructures.
This is a Client application for MQTT communications between many IoT nodes represnted and run by STM32F4x micrcontrollers connected to ESP8266 WiFi modules using UART-based serial communications. This application is Client side. MQTT broker used is "Mathworks Thingspeak" free MQTT broker.

## Features
- Seamless communications with other MQTT clients connected to the same MQTT broker.
- Subscribing to more than 1 topic at MQTT broker.
- Publishing topics to MQTT broker.
- Handle all exchanged messages asynchronously.

## Components
- ESP8266 WiFi hardware module, based on STM HAL libraries.
- Paho MQTT C Packet external library for requests serialization and deserialization.
- cJSON external library for handling requests in JSON using C langauge.
- MQTT Client module. to deal in general with different types of MQTT brokers. publish topics and subscribe to topics.
- Thingspeak MQTT module. wrapper application for MQTT Client module to handle requests with Thingspeak Broker using JSON.

## External Dependencies
- Paho MQTT C Packet library.
- cJSON library.
- Established Broker.

- Work and results are shared in "Street Infrastructure ECU" and "Garage Infrastructure ECU" folders

**Contributors:**
    - Ahmed Desoky

## =================================================================================================================================================== ##

4- Security Enhancements
Comprehensive security measures were incorporated including "AES 128","RSA 2048" encryption algorithms and "SHA2-256" hash algorithm. Securing data transmitted between vehicles and infrastructures for V2X communications and communications between vehicle OnBoard ECUs.

## Components
- Crypto Stack Manager (CSM), a wrapper for necessary functions needed by WolfSSL crypto library.
- SecuredOnBoard Communications module (SecOC). Acts as interface between different communications stacks and the Cryptography stack represnted by the CSM              and the crypto sofware library 

## Features
- RSA2048 encryption/decryption
- AES128 encryption/decryption
- SHA256 Hash generation and verification
- Sigantures creation and verification

## External Dependencies
- WolfSSL open-source crypto library

- Work and results are shared in all folders

**Contributors:**
    - Dina Hamed
    - Ahmed Desoky

## =================================================================================================================================================== ##

5- V2X Communications
The project utilized V2X technology to establish seamless communication between vehicles and infrastructures. this was conducted using TCP/IP protocol and WiFi local network created by infrastructure's ESP8266 as Access point and vehicle's Raspberry Pi WiFi as Station.
communications are encrypted secured by "RSA 2048" and "SHA2-256" algorithms.
This is two-way application to apply V2I communications technology using TCP/IP model and WiFi available technoolgy, designed and developed into 2 parts.

First part, **V2I Server - Infrastructure** is run on STM32F4x micrcontroller based on STM HAL libraries. 

## Features
- Act as WiFi Access Point for vehiceles WiFi stations, using ESP8266 hardware module connected to STM32Fx via UART-based communications.
- Manage all incoming clients requests connected to the WiFi local network and exchange data with clients.
- Provide connected clients with different types of services.
- Support high level of security by using RSA cryptosystem and SHA256 hash algorithm for the exchanged messages.

## Components
- Cryptography stack files utilizing WolfSSL open-source crypto library functions.
- ESP8266 hardware module driver.
- V2I Server Manager -> Application module.

## External Dependencies
- WolfSSL open-source crypto library
- STM HAL libraries
- Work and results are shared in "Street Infrastructure ECU", "Garage Infrastructure ECU" and "Main RaspberryPi ECU" folders

Second part, **V2I Client - Vehicle** is installed and run on Raspbian, a Linux Debian distribution. running on Raspberry Pi microcontroller.

## Features
- Act as WiFi station using WiFi hardware on Raspberry Pi microcontroller to connect to Infrastructures Access Points.
- Request needded services such as parking requests and provide neccessary data.
- Support high level of security by using RSA cryptosystem and SHA256 hash algorithm for the exchanged messages.

## Components
- Cryptography stack files utilizing WolfSSL open-source crypto library functions.
- WiFi manager module to control Raspberry Pi WiFi hardware.
- TCP Client module that utilizes Unix sockets libraries for TCP.
- V2I Client -> Application module.

## External Dependencies
- WolfSSL open-source crypto library
- network manager for Linux

**Contributors:**
    - Ahmed Desoky
    - Omar Tolba
 
 ## =================================================================================================================================================== ##


6- Vehicle OnBoard Communications using Secured-CAN Network
To be able to exchange data between many ECUs on vehicle, OnBoard communications was required. We utilized CAN Protocol by implementing a full CAN stack to exchange vehicle control commands and sensors data between the different ECUs to ensure seamless vehicle control and real-time performance. Used sensors are JSN-SR04T Ultrasonic sensors, AS5600 Magnetic Encoders, GY271 Magnetometer and GY521 IMU. CAN stac

Designed and developed for 2 targets
First, **Raspberry Pi with MCP2515 CAN controller**. 
Second, **STM32F1x with MCP2551 CAN transceiver**

## Features
- Real-time communications between different micrcontrollers via CAN bus.
- Support messages up to size of 116 bytes.
- Support high level of security by using AES cryptosystem and SHA256 hash algorithm for the exchanged messages.
- Handle all exchanged messages asynchronously.

## Components
*1- For STM32F1x*
	- CAN handler, based on STM HAL libraries, acts as a wrapper for CAN STM sHAL library.
 	- CAN Tansport Protocol. Implementation of CAN TP (ISO 15765-2) for CAN segmentation and assembly. **Tareget independent.**
  	- CAN Service Manager. CAN application APIs with the ability and option to encrypt the messages using the implemented Cryptography stack. **Tareget independent.**
  
*2- For Raspberry Pi*
	- CAN handler, based on can-utils library, acts as a wrapper for it.
 	- CAN Tansport Protocol. Implementation of CAN TP (ISO 15765-2) for CAN segmentation and assembly. **Tareget independent.**
  	- CAN Service Manager. CAN application APIs with the ability and option to encrypt the messages using the implemented Cryptography stack. **Tareget independent.**

## External Dependencies
- WolfSSL open-source crypto library
- STM HAL libraries
- can-utils library

- Work and results are shared in "Sensors Hub STM32 ECU", "Ultrasonic Hub STM32 ECU" and "Main RaspberryPi ECU"

**Contributors:**
    - Ahmed Desoky
    - Ziad Emad


Emails:
    - ahmed0201150@gmail.com - Ahmed Desoky
    - ziademadh7@gmail.com  - Ziad Emad
    - ef44529@gmail.com - Esraa Fawzy
    - omartolba004@gmail.com - Omar Tolba
    - dina.hamed001@gmail.com - Dina Hamed
    - patchslawhere@gmail.com - Omar Adel
