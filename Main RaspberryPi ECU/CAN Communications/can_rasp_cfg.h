/************************************************************************************************************************
 *  Module: CAN Raspberry
 *  File Name: can_cfg.h
 *  Authors: Ahmed Desoky
 *	Date: 25/4/2024
 *	*********************************************************************************************************************
 *	Description: CAN Raspberry Pi module to control CAN on Raspberry Pi using MCP2515 CAN controller interface
 *               This module only works if can-utils is installed and can interface is verified
 *               To check CAN interface, run "ip link show" shell command
 *               Before use, run "sudo ip link set (interface name) down" shell command
 *               This module supports receiving CAN frames, single frames and 
 *               contains software frames IDs filter, please check can_PBcfg.c
 *               To know how to install can-utils to enable this module functionality, please check can_cfg.h
 *               for building, link to pthread, add -lpthread option, and build as super user
 ***********************************************************************************************************************/
#ifndef CAN_CFG_H_
#define CAN_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 2.0.0*/
#define CAN_CFG_SW_MAJOR_VERSION           (2U)
#define CAN_CFG_SW_MINOR_VERSION           (0U)
#define CAN_CFG_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
#define CAN_INITIALIZED_LED_PIN         24  /*wPi PIN ID*/
#define CAN_ERROR_LED_PIN               25  /*wPi PIN ID*/
#define NUMBER_OF_RX_IDS                5   /*number of IDs accepted*/
#define CAN_FIFO_SIZE                   5   /*Rx software FIFO Size*/
/************************************************************************************************************************
 *                            	       Configurations & Installations Notes
 ***********************************************************************************************************************/
/*
 *  1- run "sudo apt-get upgrade" shell command
 *  2- run "sudo apt-get update" shell command
 *  3- run "sudo raspi-config" shell command
 *  4- enable SPI Interface from Interface Options
 *  5- you can check your kernel version using "uname -a" shell command
 *  6- run "sudo nano /boot/config.txt" or "sudo vim /boot/config.txt" shell commands, choose what you prefer
 *  7- if this file is not the one, run "sudo nano /boot/firmware/config.txt" or "sudo vim /boot/firmware/config.txt" shell commands
 *  8- add these lines in the file opened

            dtparam=spi=on
            dtoverlay=mcp2515-can0,oscillator=8000000,interrupt=25
            dtoverlay=spi-bcm2835
            dtoverlay=spi-dma

            interrupt=X -> X is the pin which the INT pin of MCP2515 connected to on the Raspberry Pi
 *  9- reboot the Raspberry Pi by running "sudo reboot" shell command
 *  10- run "sudo apt-get install autoconf autogen" shell command to install auto configuration and generation
 *  11- run "sudo apt-get install libtool" shell command to intsall libtool, a standard tool to install libraries
 *  12- choose a directory to move to
 *  13- clone can-utils repo by running "git clone https://github.com/linux-can/can-utils.git" shell command
 *  14- get into can-utils directory
 *  15- run "sudo ./autogen.sh" shell command
 *  16- run "sudo ./configure" shell command
 *  17- run "sudo make" shell command
 *  18- run "sudo make install" shell command
 *  19- or you can skip steps 12 to 18 and run this shell command ^_^ -> "sudo apt-get install can-utils"
 *  20- reboot the Raspberry Pi by running "sudo reboot" shell command
 *  21- check the CAN interface using "ip link show" shell command
 *  22- Tadaaaaa
 *  23- include "can.h"
 *  24- NJoy!
 */

#endif /*CAN_CFG_H_*/
