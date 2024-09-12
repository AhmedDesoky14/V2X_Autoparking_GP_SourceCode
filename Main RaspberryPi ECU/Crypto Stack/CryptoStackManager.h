/************************************************************************************************************************
 * Module: CryptoStackManager
 * File Name: CryptoStackManager.h
 * Authors: Dina Hamed Mohamed
 * Date: 2/5/2024
 * *********************************************************************************************************************
 * Description:
 *
 * The CryptoStackManager module provides a set of functions for managing cryptographic operations such as encryption,
 * decryption, digital signature generation, signature verification, and hash computation. This module serves as an
 * abstraction layer for cryptographic functionalities, allowing the application to perform secure operations without
 * directly interacting with low-level cryptographic libraries.
 *
 * The functions provided by CryptoStackManager include:
 * - Crypto_Init: Initializes the cryptographic stack by initializing required cryptographic libraries, configuring
 *   cryptographic keys, and performing other initialization tasks.
 * - Crypto_RSA_Encrypt: Encrypts input data using RSA encryption with the appropriate public key based on the specified
 *   device identifier .
 * - Crypto_RSA_Decrypt: Decrypts encrypted data using RSA decryption with the corresponding private key.
 * - Crypto_Hash_SHA256: Computes the SHA-256 hash of input data.
 * - Crypto_Check_Hash_SHA256: Verifies the SHA-256 hash of input data against an expected hash value.
 * - Crypto_Signature_Generate: Generates a digital signature for input data using the corresponding private key.
 * - Crypto_Signature_Verify: Verifies a digital signature using the corresponding public key.
 *
 * The device identifier --> The device identifier is used in cryptographic operations, particularly in scenarios where
 * different devices or entities are involved in secure communication or data exchange. It helps in determining which
 * cryptographic key to use for encryption or decryption based on the identity or role of the device.

  In the context of the functions provided by the CryptoStackManager module, the device identifier parameter specifies the
  target device for encryption or decryption operations. For example, in the Crypto_RSA_Encrypt function,
  the device identifier is used to determine which public key should be used for RSA encryption. Similarly,
    in Crypto_Signature_Verify, the device identifier is used to select the appropriate public key for verifying a digital signature.
 * in my case I have 5 devices :
 * { MAIN_STM:ID0, SECONDARY_STM:ID1 , INFRASTRUCTURE1:ID2 ,INFRASTRUCTURE2:ID3 ,RASPBERRY_PI: ID4 }.
 *
 * This module abstracts the complexities of cryptographic operations and provides a simple interface for integrating
 * cryptographic functionalities into applications. It enables developers to implement secure communication, data
 * integrity verification, and authentication mechanisms with ease.
 Important Note : maximum message to send (apply cryptographic operations on using RSA) is 245 bytes.
 Important Note : maximum message to send (apply cryptographic operations on using AES) is 16 bytes.
***********************************************************************************************************************
 ***********************************************************************************************************************
 * WOLFSSL INSTALLATION STEPS on STM:
 1- Open STM32CUBEIDE choose the required stm that you'll use then install WOLFSSL library and manage from software packs the
    required version (in my case i used version 5.7).
 2- In IoC : Go to "software packs" and choose "select components" you'll find the "wolfssl I-CUBE-wolfssl" showed up in the list
    with the version you installed you'll make a check on wolfssl/core also wolfcrypt (core and test).
 3- In categories : Activate "I-CUBE WOLFSSL" from " Middleware and software packs" list and in" configurations" you'll do the following:
   - math configurations : choose "single precision ASM cortex -M3+ math".
   - wolfcrypt test/benchmark : choose " FALSE" .
   that's it, save and generate code.
 4- You'll find wolfssl files showed up, you'll go to wolfssl.I-CUBE-wolfSSL_conf.h and do the following:
   - define your hardware platform whether its stm32f1 or stm32f2 or other.
      (in my case i defined WOLFSSL_STM32F4)
   - uncomment #define NO_STM32_RNG if your hardware doesn't have RNG as my case.
   - uncomment #define WOLFSSL_AES_DIRECT if you want to use AES algorithm
   - comment every line that contains UART if you're not using UART for debuging as my case.
   - select the project properties and go to c/c++ build settings and from tool settings --> MCU GCC COMPILER
   write this command under "expert settings" --> the "command line pattern" beside {OUTPUT} you'll write this : -fomit-frame-pointer
   then apply and close .

  * Now, you're ready to use the library on the STM.
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 * WOLFSSL INSTALLATION STEPS on Linux:
 * Please be NOTED that WolfSSL is only can be installed on 64-bit Architecture Machines with 64-bit Architecture OS
   1- Download the latest version of WolfSSL from its website: https://www.wolfssl.com/download/
      latest version is 5.7.0 for the moment that Guide is being written
   2- Extract the downloaded file the move to the extracted directory
   3- run "sudo apt-get upgrade -y" shell command to upgrade 
   4- run "sudo apt-get update -y" shell command to update
   5- run "sudo apt-get install -y libtool" shell command to install libtool, a standard tool for libraries installations
   6- run "sudo apt-get install make" shell command to install "make"
   7- run "sudo ./configure" shell command
   8- run "sudo make" shell command
   9- run "sudo make install" shell command
   10-run "sudo ldconfig" to configure the install dynamic libraries of WolfSSL
   11-For building link with wolfssl, add option -lwolfssl
   *To Enable Direct AES functionality, modify "wolfSSL.I-CUBE-wolfSSL_conf" file the remake the library files*
 * Now you're ready to use the library on Linux
 * *********************************************************************************************************************/
#ifndef INC_CRYPTOSTACKMANAGER_H_
#define INC_CRYPTOSTACKMANAGER_H_
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#define HAVE_AES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include "CryptoStackManager_cfg.h"
#if(SHA256_HASH == 1)
#include <wolfssl/wolfcrypt/sha256.h>
#endif

#if(RSA_Algorithm == 1)
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/signature.h>
#endif

#if(AES_Algorithm)
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/hmac.h>
#endif
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 2.1.1*/
#define CRYPTOSTACKMANAGER_SW_MAJOR_VERSION           (2U)
#define CRYPTOSTACKMANAGER_SW_MINOR_VERSION           (1U)
#define CRYPTOSTACKMANAGER_SW_PATCH_VERSION           (1U)
/*Software Version checking between CryptoStackManager Configuration file and Header file*/
#if ((CRYPTOSTACKMANAGER_SW_MAJOR_VERSION != CRYPTOSTACKMANAGER_CFG_SW_MAJOR_VERSION)\
 ||  (CRYPTOSTACKMANAGER_SW_MINOR_VERSION != CRYPTOSTACKMANAGER_CFG_SW_MINOR_VERSION)\
 ||  (CRYPTOSTACKMANAGER_SW_PATCH_VERSION != CRYPTOSTACKMANAGER_CFG_SW_PATCH_VERSION))
  #error "The Software version of CryptoStackManager does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define DIGEST_LENGTH 32 // the length of the SHA-256 digest.
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
#if(AES_Algorithm == 1)
typedef struct
{
	byte* Key;
	unsigned int Device_ID;
}AES_Key_Struct;
#endif

#if(RSA_Algorithm == 1)
typedef struct
{
	 unsigned int Device_ID;
	 unsigned int RSA_Key_der_len;
	 unsigned char* RSA_Key_der_Ptr;
	 RsaKey RSA_Key;
	 WC_RNG Key_RNG;
	 word32 idx;
}RSA_Key_Struct;
typedef struct
{
	 unsigned int Device_ID;
	 unsigned int Private_Key_der_len;
	 unsigned char* Private_Key_der_Ptr;
	 RsaKey Private_Key;
	 WC_RNG Key_RNG;
	 WC_RNG Key_SIGN_RNG;
	 word32 idx;
}RSA_Private_Key_Struct;
#endif

typedef enum {
    CSM_NOT_OK,
	CSM_OK
} CryptoReturnStatus;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
CryptoReturnStatus Crypto_Init();

#if(RSA_Algorithm == 1)
void Define_RSA_Keys(void);
CryptoReturnStatus Crypto_RSA_Encrypt(const byte* in, word32 inLen, byte* out, word32 outLen,unsigned int deviceID);
CryptoReturnStatus Crypto_RSA_Decrypt(const byte* cipher, word32 cipherLen, byte* plain, word32 plainLen) ;
CryptoReturnStatus Crypto_Signature_Generate(const byte* in, word32 inLen, byte* out, word32 outLen);
CryptoReturnStatus Crypto_Signature_Verify(const byte* cipher, word32 cipherLen, byte* plain, word32 plainLen,unsigned int deviceID);
#endif


#if(SHA256_HASH == 1)
CryptoReturnStatus Crypto_Hash_SHA256(byte* in, word32 inLen , byte* digest);
CryptoReturnStatus Crypto_Check_Hash_SHA256(const byte* in, word32 inLen, const byte* expected_hash);
#endif


#if(AES_Algorithm == 1)
CryptoReturnStatus Crypto_AES_Encrypt( const byte* in,byte* out,unsigned int deviceID);
CryptoReturnStatus Crypto_AES_Decrypt(const byte* in,byte* out,unsigned int deviceID);
CryptoReturnStatus Crypto_HMAC_AES(const byte* in,byte* digest,unsigned int deviceID);
#endif
/************************************************************************************************************************
 *                    							   External Variables
 ***********************************************************************************************************************/
#if(RSA_Algorithm == 1)
extern RSA_Private_Key_Struct My_Private_Key;
extern RSA_Key_Struct Public_keys[RSA_SYSTEM_DEVICES];
#endif

#if(AES_Algorithm == 1)
extern AES_Key_Struct AES_Keys[AES_SYSTEM_DEVICES];
#endif

#endif /* INC_CRYPTOSTACKMANAGER_H_ */
