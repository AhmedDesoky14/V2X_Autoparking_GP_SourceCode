/************************************************************************************************************************
 * Module: Secure OnBoard Communication (SecOC)
 * File Name: SecOC.h
 * Authors: Dina Hamed Mohamed
 * Date: 2/5/2024
 * *********************************************************************************************************************
 * Description:
 This header file defines the interface for Secure OnBoard Communication (SecOC) module. SecOC provides functions for
 sending and receiving secured messages between devices. It employs various security mechanisms such as hashing, encryption,
 and digital signatures to ensure the integrity, confidentiality, and authenticity of the transmitted data.
 The SecOC module offers the following 2 functions:
* - SecOC_Send_Secured: Sends a secured message with specified security level. The message is processed according to the
specified security level before transmission.
* - SecOC_Receive_Secured: Receives a secured message and verifies its security according to the specified security level.
It checks the integrity of the message, decrypts it if necessary, and verifies digital signatures.
************************************************************************************************************************
* Security levels supported by SecOC include:
** - NO_SECURITY: No security measures are applied to the message.
** - HASH: The message is hashed to ensure integrity.
** - CIPHER: The message is encrypted to ensure confidentiality.
** - SIGNATURE: A digital signature is added to the message to ensure authenticity.
** - CIPHER_SIGNATURE_HASH: Combination of encryption, digital signature, and hashing for comprehensive (high level) security.
* Each function returns a status indicating whether the operation was successful or not. Specific details about the
* parameters and return values are provided in the function documentation.
Important Note : maximum message to send (apply cryptographic operations on) is 245 bytes.
************************************************************************************************************************/
#ifndef INC_SECOC_H_
#define INC_SECOC_H_
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "CryptoStackManager.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.1.1*/
#define SECOC_SW_MAJOR_VERSION           (1U)
#define SECOC_SW_MINOR_VERSION           (1U)
#define SECOC_SW_PATCH_VERSION           (1U)
/*Software Version checking of Dependent Module - Crypto Stack Manager - Version 2.1.0*/
#if ((CRYPTOSTACKMANAGER_SW_MAJOR_VERSION != (2U))\
 ||  (CRYPTOSTACKMANAGER_SW_MINOR_VERSION != (1U))\
 ||  (CRYPTOSTACKMANAGER_SW_PATCH_VERSION != (1U)))
  #error "The Software version of CRYPTO STACK MANAGER Module does not match the expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define ENCRYPT_SIZE 256
#define SIGN_SIZE 256
#define SEPARATOR "***"
#define SEPARATOR_LEN 3
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
typedef enum {
    SECURITY_OK,
	SECURITY_NOT_OK
} SecurityReturnStatus;

typedef enum {

    NO_SECURITY=0,HASH,AES_CIPHER,AES_CIPHER_HMAC,RSA_CIPHER,RSA_SIGNATURE,RSA_CIPHER_SIGNATURE_HASH
} SecurityLevel;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
SecurityReturnStatus SecOC_Send_Secured(byte* in, word32 inLen, byte* out, word32* outLen ,SecurityLevel Secure_level,unsigned int deviceID );
SecurityReturnStatus SecOC_Recieve_Secured(byte* in, word32 inLen, byte* out, word32* outLen, SecurityLevel Secure_level,unsigned int deviceID);

#endif /* INC_SECOC_H_ */
