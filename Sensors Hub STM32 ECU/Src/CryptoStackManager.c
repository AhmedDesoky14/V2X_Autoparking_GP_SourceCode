/************************************************************************************************************************
 * Module: CryptoStackManager
 * File Name: CryptoStackManager.c
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
 *   device identifier.
 * - Crypto_RSA_Decrypt: Decrypts encrypted data using RSA decryption with the corresponding private key.
 * - Crypto_Hash_SHA256: Computes the SHA-256 hash of input data.
 * - Crypto_Check_Hash_SHA256: Verifies the SHA-256 hash of input data against an expected hash value.
 * - Crypto_Signature_Generate: Generates a digital signature for input data using the corresponding private key.
 * - Crypto_Signature_Verify: Verifies a digital signature using the corresponding public key.
 * - Crypto_AES_Encrypt: Encrypts input data using AES encryption with the appropriate symmetric key based on the specified device identifier.
 * - Crypto_AES_Decrypt: Decrypts encrypted data using AES decryption with the corresponding symmetric key
 * - Crypto_HMAC_AES: Generates a Hashed MAC for input data using the corresponding symmetric key.
 * This module abstracts the complexities of cryptographic operations and provides a simple interface for integrating
 * cryptographic functionalities into applications. It enables developers to implement secure communication, data
 * integrity verification, and authentication mechanisms with ease.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "CryptoStackManager.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
#if(AES_Algorithm == 1)
static Aes aes;
static Hmac hmac;
#endif
/***********************************************************************************************************************
*                     					        FUNCTIONS DEFINTITIONS
***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: Crypto_Init
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): None
* Return value: CryptoReturnStatus: Status of the initialization operation. CSM_OK if initialization is successful,
*                             CSM_NOT_OK otherwise.
* Description: Initializes the cryptographic module.
*
* This function initializes the cryptographic module by calling the initialization functions of the underlying
* cryptographic libraries (wolfSSL and wolfCrypt). It sets up random number generators (RNGs) for various operations
* and initializes RSA key structures for different public and private keys used in the system. It also decodes RSA
* public keys from DER format to RSA structures and validates the decoded private key.
*
************************************************************************************************************************/
CryptoReturnStatus Crypto_Init()
{
	int ret = 0;
	if( (wolfSSL_Init()!= SSL_SUCCESS) || (wolfCrypt_Init()!=0) )
	{
		return CSM_NOT_OK;
	}


#if(AES_Algorithm == 1)	/*Init For AES Algorithm*/

	  if (wc_AesInit(&aes, NULL, INVALID_DEVID) != 0)
	  {
	        return CSM_NOT_OK;
	  }
#endif

#if(RSA_Algorithm == 1)

		Define_RSA_Keys();

	   	word32 inOutIdx = 0;
	   	word32 inSz;
	   	int keySz = 0;
	   	wc_InitRng(&My_Private_Key.Key_RNG); /*Used for signature Generation*/
	   	wc_InitRng(&My_Private_Key.Key_SIGN_RNG); /*Used for signature Generation*/
	    ret = wc_InitRsaKey(&My_Private_Key.Private_Key,NULL); // not using heap hint. No custom memory
		if (ret == 0)
	    {
			wc_RsaSetRNG(&My_Private_Key.Private_Key,&My_Private_Key.Key_RNG); //setting rng as WC_RSA_BLINDING is enabled1
		}
	    else
	    {
	    	return CSM_NOT_OK;
		}

		/* Decode the RSA public key from DER format to RSA structure */
	    if((wc_RsaPrivateKeyDecode(My_Private_Key.Private_Key_der_Ptr,&My_Private_Key.idx,&My_Private_Key.Private_Key,My_Private_Key.Private_Key_der_len)!=0)){
	    	return CSM_NOT_OK;
	    }

		// Validate the decoded private key
	    inSz = My_Private_Key.Private_Key_der_len;
		int retValidate = wc_RsaPrivateKeyValidate(My_Private_Key.Private_Key_der_Ptr,&inOutIdx,&keySz,inSz);
		if (retValidate != 0)
		{ 	// Private key validation failed
			return CSM_NOT_OK;
        }

		/*Init every Public RSA key*/
		for(unsigned int k=0;k<RSA_SYSTEM_DEVICES;k++)
		{
			wc_InitRng(&Public_keys[k].Key_RNG);
			ret = wc_InitRsaKey(&Public_keys[k].RSA_Key, NULL); // not using heap hint. No custom memory
			if (ret == 0)
			{
				ret = wc_RsaSetRNG(&Public_keys[k].RSA_Key,&Public_keys[k].Key_RNG); //setting rng as WC_RSA_BLINDING is enabled1
			}
			else
			{
				return CSM_NOT_OK;
			}
			/* Decode the RSA public key from DER format to RSA structure */
			if((wc_RsaPublicKeyDecode(Public_keys[k].RSA_Key_der_Ptr,&Public_keys[k].idx,&Public_keys[k].RSA_Key,Public_keys[k].RSA_Key_der_len)!=0))
			{
				return CSM_NOT_OK;
			}
		}
#endif
	 return CSM_OK;
}

#if(RSA_Algorithm == 1)
/************************************************************************************************************************
* Function Name: Crypto_RSA_Encrypt
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in):
*     - in: Pointer to the input byte array to be encrypted.
*     - inLen: Length of the input byte array.
*     - deviceID: Identifier of the device for which the encryption is performed.
* Parameters (inout): None
* Parameters (out):
*     - out: Pointer to the output byte array where the encrypted data will be stored.
*     - outLen: Length of the output byte array.
* Return value: CryptoReturnStatus: Status of the encryption operation. CSM_OK if encryption is successful,
*                             CSM_NOT_OK otherwise.
* Description: Encrypts the input data using RSA public key encryption.
*
* This function encrypts the input data using RSA public key encryption algorithm. The specific RSA public key
* used for encryption depends on the device identifier passed as a parameter. The encrypted data is stored
* in the output byte array provided by the caller. The length of the output byte array is updated accordingly.
*
************************************************************************************************************************/
CryptoReturnStatus Crypto_RSA_Encrypt(const byte* in, word32 inLen, byte* out, word32 outLen ,unsigned int deviceID)
{

	unsigned int Device_Index = 0;
	unsigned int Index_Found = 0;
	for(unsigned int i=0;i<RSA_SYSTEM_DEVICES;i++)
	{
		if(deviceID == Public_keys[i].Device_ID)
		{
			Device_Index = i;	/*Device ID found and its index*/
			Index_Found = 1;
		}
	}
	if(Index_Found == 0)
	{
		return CSM_NOT_OK;
	}
	int retEncrypt = wc_RsaPublicEncrypt((const unsigned char *)in,inLen,(unsigned char *)out,outLen,&Public_keys[Device_Index].RSA_Key,&Public_keys[Device_Index].Key_RNG);
	if (retEncrypt < 0)
	{
		return CSM_NOT_OK; // Encryption failed
	}
	return CSM_OK; // Encryption succeeded
}
/************************************************************************************************************************
* Function Name: Crypto_RSA_Decrypt
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in):
*     - cipher: Pointer to the input byte array containing the encrypted data to be decrypted.
*     - cipherLen: Length of the input byte array containing the encrypted data.
* Parameters (inout): None
* Parameters (out):
*     - plain: Pointer to the output byte array where the decrypted data will be stored.
*     - plainLen: Length of the output byte array.
* Return value: CryptoReturnStatus: Status of the decryption operation. CSM_OK if decryption is successful,
*                             CSM_NOT_OK otherwise.
* Description: Decrypts the input data using RSA private key decryption.
*
* This function decrypts the input data using RSA private key decryption algorithm. The private key used for
* decryption is specific to the device. The decrypted data is stored in the output byte array provided by
* the caller. The length of the output byte array is updated accordingly.
*
************************************************************************************************************************/
CryptoReturnStatus Crypto_RSA_Decrypt(const byte* cipher, word32 cipherLen, byte* plain, word32 plainLen) {

    int retDecrypt = wc_RsaPrivateDecrypt((const unsigned char *)cipher,cipherLen,(unsigned char *)plain,plainLen,&My_Private_Key.Private_Key);
    if (retDecrypt < 0)
    {
       return CSM_NOT_OK; // Decryption failed
    }
    return CSM_OK; // Decryption succeeded

}
/************************************************************************************************************************
* Function Name: Crypto_Signature_Generate
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in):
*     - in: Pointer to the input byte array containing the data to be signed.
*     - inLen: Length of the input byte array containing the data.
*     - out: Pointer to the output buffer to store the generated signature.
*     - outLen: Length of the output buffer.
* Parameters (inout): None
* Parameters (out): None
* Return value: CryptoReturnStatus: Status of the signature generation operation. CSM_OK if the signature generation
*                             is successful, CSM_NOT_OK otherwise.
* Description: Generates a digital signature for the input data using the RSA private key.
*
* This function generates a digital signature for the input data using the RSA private key provided. It uses
* the RSA private key to sign the input data and stores the generated signature in the output buffer. The
* length of the signature is provided by the caller in the outLen parameter. If the signature generation
* is successful, the function returns CSM_OK; otherwise, it returns CSM_NOT_OK.
*
************************************************************************************************************************/
CryptoReturnStatus Crypto_Signature_Generate(const byte* in, word32 inLen, byte* out, word32 outLen)
{
	 int ret = wc_RsaSSL_Sign(in,inLen,out,outLen,&My_Private_Key.Private_Key,&My_Private_Key.Key_SIGN_RNG);
	 if (ret < 0)
	 {
		 return CSM_NOT_OK;
	 }
	 return CSM_OK;
}
/************************************************************************************************************************
* Function Name: Crypto_Signature_Verify
* Function ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in):
*     - cipher: Pointer to the input byte array containing the signature to be verified.
*     - cipherLen: Length of the input byte array containing the signature.
*     - plain: Pointer to the input byte array containing the plaintext data.
*     - plainLen: Length of the input byte array containing the plaintext data.
*     - deviceID: Device identifier to select the appropriate public key for verification.
* Parameters (inout): None
* Parameters (out): None
* Return value: CryptoReturnStatus: Status of the signature verification operation. CSM_OK if the signature is
*                             successfully verified, CSM_NOT_OK otherwise.
* Description: Verifies the digital signature using the corresponding public key.
*
* This function verifies the digital signature contained in the input byte array using the corresponding
* public key associated with the specified device identifier. It compares the provided signature with the
* signature generated from the plaintext data using the selected public key. If the signatures match, the
* function returns CSM_OK; otherwise, it returns CSM_NOT_OK.
* ******************************************************************************************/
CryptoReturnStatus Crypto_Signature_Verify(const byte* cipher,word32 cipherLen,byte* plain,word32 plainLen,unsigned int deviceID)
{
	unsigned int Device_Index = 0;
	unsigned int Index_Found = 0;
	for(unsigned int i=0;i<RSA_SYSTEM_DEVICES;i++)
	{
		if(deviceID == Public_keys[i].Device_ID)
		{
			Device_Index = i;	/*Device ID found and its index*/
			Index_Found = 1;
		}
	}
	if(Index_Found == 0)
	{
		return CSM_NOT_OK;
	}
	int ret = wc_RsaSSL_Verify(cipher,cipherLen,plain,plainLen,&Public_keys[Device_Index].RSA_Key);
	if (ret < 0)
	{
		return CSM_NOT_OK;
	}
	return CSM_OK;
}
#endif

#if(SHA256_HASH)
/************************************************************************************************************************
* Function Name: Crypto_Hash_SHA256
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in):
*     - in: Pointer to the input byte array containing the data to be hashed.
*     - inLen: Length of the input byte array containing the data.
* Parameters (inout): None
* Parameters (out):
*     - digest: Pointer to the output byte array where the hash digest will be stored.
* Return value: CryptoReturnStatus: Status of the hash computation operation. CSM_OK if successful,
*                             CSM_NOT_OK otherwise.
* Description: Computes the SHA-256 hash of the input data.
*
* This function computes the SHA-256 hash of the input data using the SHA-256 hashing algorithm. The resulting
* hash digest is stored in the output byte array provided by the caller. The length of the output byte array
* should be at least 32 bytes to accommodate the SHA-256 hash digest.
*
************************************************************************************************************************/
CryptoReturnStatus Crypto_Hash_SHA256(byte* in, word32 inLen , byte* digest)
{
	 enum wc_HashType hash_type = WC_HASH_TYPE_SHA256;
     int hash_len = wc_HashGetDigestSize(hash_type);
	 if (hash_len > 0)
	 {
		 int ret = wc_Hash(hash_type, in ,inLen , digest, hash_len);
	     if(ret == 0)
	     {
	       return CSM_OK;
	     }
	     else
	     {
	       return CSM_NOT_OK;
	     }
	 }
	 return CSM_NOT_OK;
}
/************************************************************************************************************************
* Function Name: Crypto_Check_Hash_SHA256
* Function ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in):
*     - in: Pointer to the input byte array containing the data to be hashed.
*     - inLen: Length of the input byte array containing the data.
*     - expected_hash: Pointer to the expected hash digest to compare with.
* Parameters (inout): None
* Parameters (out): None
* Return value: CryptoReturnStatus: Status of the hash comparison operation. CSM_OK if the computed hash matches the
*                             expected hash, CSM_NOT_OK otherwise.
* Description: Computes the SHA-256 hash of the input data and compares it with the expected hash.
*
* This function computes the SHA-256 hash of the input data using the SHA-256 hashing algorithm. It then compares
* the computed hash with the expected hash provided by the caller. If the computed hash matches the expected hash,
* the function returns CSM_OK indicating success; otherwise, it returns CSM_NOT_OK indicating a mismatch.
*
************************************************************************************************************************/
CryptoReturnStatus Crypto_Check_Hash_SHA256(const byte* in, word32 inLen, const byte* expected_hash)
{
    enum wc_HashType hash_type = WC_HASH_TYPE_SHA256;
    int hash_len = wc_HashGetDigestSize(hash_type);
    byte hash_data[hash_len];

    if (hash_len > 0)
    {
        int ret = wc_Hash(hash_type, in, inLen, hash_data, hash_len);
        if (ret != 0)
        {
            return CSM_NOT_OK; // Hash computation failed
        }
        // Compare computed hash with expected hash
        if (memcmp(hash_data, expected_hash, hash_len) != 0)
        {
            return CSM_NOT_OK; // Hashes match
        }
        return CSM_OK;
    }
    return CSM_NOT_OK;
}
#endif


#if(AES_Algorithm == 1)
/************************************************************************************************************************
* Function Name: Crypto_AES_Encrypt
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in):
*     - in: Pointer to the input byte array to be encrypted.
*     - deviceID: Identifier of the device for which the encryption is performed.
* Parameters (inout): None
* Parameters (out):
*     - out: Pointer to the output byte array where the encrypted data will be stored.
* Return value: CryptoReturnStatus: Status of the encryption operation. CSM_OK if encryption is successful,
*                             CSM_NOT_OK otherwise.
* Description: Encrypts the input data using AES Symmetric Key.
*
* This function encrypts the input data using AES encryption algorithm. The specific AES symmetric key
* used for encryption depends on the device identifier passed as a parameter. The encrypted data is stored
* in the output byte array provided by the caller. The length of the output byte is well known as 16 bytes.
*
************************************************************************************************************************/
CryptoReturnStatus Crypto_AES_Encrypt( const byte* in,byte* out,unsigned int deviceID)
{
	unsigned int Device_Index = 0;
	unsigned int Index_Found = 0;
	for(unsigned int i=0;i<AES_SYSTEM_DEVICES;i++)
	{
		if(deviceID == AES_Keys[i].Device_ID)
		{
			Device_Index = i;	/*Device ID found and its index*/
			Index_Found = 1;
		}
	}
	if(Index_Found == 0)
	{
		return CSM_NOT_OK;
	}
	int  ret = wc_AesSetKey(&aes,AES_Keys[Device_Index].Key,16,NULL,AES_ENCRYPTION);
	ret=wc_AesEncryptDirect(&aes,out,(const unsigned char*)in);
    if(ret<0)
    {
    	return CSM_NOT_OK;
    }
	return CSM_OK;
}
/************************************************************************************************************************
* Function Name: Crypto_AES_Decrypt
* Function ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in):
*     - in: Pointer to the input byte array of the encrypted data.
*     - deviceID: Identifier of the device for which the decryption is performed for.
* Parameters (inout): None
* Parameters (out):
*     - out: Pointer to the output byte array where the decrypted data will be stored.
* Return value: CryptoReturnStatus: Status of the decryption operation. CSM_OK if decryption is successful,
*                             CSM_NOT_OK otherwise.
* Description: Decrypts the encrypted data using AES Symmetric Key.
*
* This function Decrypts the input data using AES decryption algorithm. The specific AES symmetric key
* used for decryption depends on the device identifier passed as a parameter. The decrypted data is stored
* in the output byte array provided by the caller.
*
************************************************************************************************************************/
CryptoReturnStatus Crypto_AES_Decrypt(const byte* in,byte* out,unsigned int deviceID)
{
	unsigned int Device_Index = 0;
	unsigned int Index_Found = 0;
	for(unsigned int i=0;i<AES_SYSTEM_DEVICES;i++)
	{
		if(deviceID == AES_Keys[i].Device_ID)
		{
			Device_Index = i;	/*Device ID found and its index*/
			Index_Found = 1;
		}
	}
	if(Index_Found == 0)
	{
		return CSM_NOT_OK;
	}
    // AES decryption
	int ret =wc_AesSetKey(&aes,AES_Keys[Device_Index].Key,16,NULL,AES_DECRYPTION);	/*encrypted AES is 16 byte length well known*/
	ret = wc_AesDecryptDirect(&aes,out,(const unsigned char*)in);
	if(ret < 0)
	{
		return CSM_NOT_OK;
    }
	return CSM_OK;
}
/************************************************************************************************************************
* Function Name: Crypto_HMAC_AES
* Function ID[hex]: 0x07
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in):
*     - in: Pointer to the input byte array of the data to get the HMAC for.
*     - deviceID: Identifier of the device for which the HMAC is performed for.
* Parameters (inout): None
* Parameters (out):
*     - out: Pointer to the output byte array where the output data will be stored.
* Return value: CryptoReturnStatus: Status of the HMAC operation. CSM_OK if HMAC is successful,
*                             CSM_NOT_OK otherwise.
* Description: Calculate the hashed HMAC for the data.
*
* This function gets the MAC for the input data using AES HMAC algorithm. The specific AES symmetric key
* used for MAC operation depends on the device identifier passed as a parameter. The HMAC is stored
* in the output byte array provided by the caller.
************************************************************************************************************************/
CryptoReturnStatus Crypto_HMAC_AES(const byte* in,byte* digest,unsigned int deviceID)
{
	unsigned int Device_Index = 0;
	unsigned int Index_Found = 0;
	for(unsigned int i=0;i<AES_SYSTEM_DEVICES;i++)
	{
		if(deviceID == AES_Keys[i].Device_ID)
		{
			Device_Index = i;	/*Device ID found and its index*/
			Index_Found = 1;
		}
	}
	if(Index_Found == 0)
	{
		return CSM_NOT_OK;
	}
	int ret = wc_HmacSetKey(&hmac,WC_SHA256,AES_Keys[Device_Index].Key,16);	/*AES key is 16 byte*/
	if (ret != 0)
	{
		return CSM_NOT_OK;
	}
    ret = wc_HmacUpdate(&hmac,in,strlen((const char*)in));
    if (ret != 0)
    {
    	return CSM_NOT_OK;
    }
    ret = wc_HmacFinal(&hmac,digest);
    if (ret != 0)
    {
    	return CSM_NOT_OK;
    }
	return CSM_OK;
}
#endif
