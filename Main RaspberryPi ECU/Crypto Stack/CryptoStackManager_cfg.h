/************************************************************************************************************************
 * 	Module: CryptoStackManager
 * 	File Name: CryptoStackManager_cfg.h
 *  Authors: Ahmed Desoky
 *	Date:	4/5/2024
 *	*********************************************************************************************************************
 *	Description: Crypto Stack Manager Configurations File
 *				 Please go ahead to configurations notes section to understand how to confiure
 *				 the crypto stack for your use case.
 ***********************************************************************************************************************/
#ifndef INC_CRYPTOSTACKMANAGER_CFG_H_
#define INC_CRYPTOSTACKMANAGER_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 2.1.1*/
#define CRYPTOSTACKMANAGER_CFG_SW_MAJOR_VERSION           (2U)
#define CRYPTOSTACKMANAGER_CFG_SW_MINOR_VERSION           (1U)
#define CRYPTOSTACKMANAGER_CFG_SW_PATCH_VERSION           (1U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
 *                            			    	  TRIGGERS
 ***********************************************************************************************************************/
#define RSA_Algorithm		0
#define AES_Algorithm		1
#define SHA256_HASH			1
/************************************************************************************************************************
 *                     				    	DEVICES CONFIGURATIONS
 ***********************************************************************************************************************/
#if(RSA_Algorithm == 1)
#define RSA_SYSTEM_DEVICES	1		/*Connected Devices using RSA security (Excluding this device)*/
#endif

#if(AES_Algorithm == 1)
#define AES_SYSTEM_DEVICES	1		/*Connected Devices using AES security (Excluding this device)*/
#endif
/************************************************************************************************************************
 *                            	                 Configurations Notes
 ***********************************************************************************************************************/
/*
 *	RSA Configurations:
 *		- Determine in this file if you want to use RSA or not, by using "RSA_Algorithm" Trigger.
 *		If you decided to use RSA
 *		- Determine how many devices to communicate with using RSA security, by using RSA_SYSTEM_DEVICES
 *		- Include your keys at CryptoStackManager_PBcfg.c file.
 *		- Configure Keys der_Ptr, der_len and device ID related to that key in CryptoStackManager_PBcfg.c file.
 *	AES Configurations:
 *		- Determine in this file if you want to use AES or not, by using "AES_Algorithm" Trigger.
 *		If you decided to use RSA
 *		- Determine how many devices to communicate with using AES security, by using AES_SYSTEM_DEVICES
 *		- Include your keys at CryptoStackManager_PBcfg.c file.
 *		- Configure Keys and device ID related to that key in CryptoStackManager_PBcfg.c file.
 *	SHA_HASH Configurations:
 *		- Determine only if you want to use HASH or not using "SHA256_HASH" Trigger in this file.
 */

#endif /*INC_CRYPTOSTACKMANAGER_CFG_H_*/
