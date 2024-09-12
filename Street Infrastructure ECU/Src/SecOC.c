/************************************************************************************************************************
 * Module: Secure OnBoard Communication (SecOC)
 * File Name: SecOC.c
 * Authors: Dina Hamed Mohamed
 * Date: 2/5/2024
 * *********************************************************************************************************************
 * Description:
This source file implements the functionality of the Secure OnBoard Communication (SecOC) module. SecOC provides
functions for sending and receiving secured messages between devices. It employs various security mechanisms such as
hashing, encryption, and digital signatures to ensure the integrity, confidentiality, and authenticity of the transmitted
data.
The SecOC module offers the following 2 functions:
* -  SecOC_Send_Secured: Sends a secured message with specified security level. The message is processed according to the
specified security level before transmission.
* -  SecOC_Receive_Secured: Receives a secured message and verifies its security according to the specified security level.
It checks the integrity of the message, decrypts it if necessary, and verifies digital signatures.
***********************************************************************************************************************
* Security levels supported by SecOC include:
** - NO_SECURITY: No security measures are applied to the message.
** - HASH: The message is hashed to ensure integrity.
** - CIPHER: The message is encrypted to ensure confidentiality.
** - SIGNATURE: A digital signature is added to the message to ensure authenticity.
** - CIPHER_SIGNATURE_HASH: Combination of encryption, digital signature, and hashing for comprehensive (high level) security.
Each function returns a status indicating whether the operation was successful or not. Specific details about the
parameters and return values are provided in the function documentation.
 ********************************************************************************************************************/
 /************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
 #include "SecOC.h"
/************************************************************************************************************************
* Function Name: SecOC_Send_Secured
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in): in (input data), inLen (length of input data), Secure_level (security level), deviceID (device identifier)
* Parameters (inout): out (output buffer), outLen (length of output data)
* Parameters (out):
* Return value: SecurityReturnStatus
* Description:
* SecOC_Send_Secured function is responsible for sending secured messages over an onboard communication channel. It provides
* different levels of security based on the specified security level and device identifier.
*
* Parameters:
* - in: Input data to be secured.
* - inLen: Length of the input data.
* - out: Output buffer to store the secured message.
* - outLen: Pointer to the length of the output data.
* - Secure_level: Security level to be applied to the message (NO_SECURITY, HASH, CIPHER, SIGNATURE, or CIPHER_SIGNATURE_HASH).
* - deviceID: Identifier of the device used for cryptographic operations.
*
* Return Values:
* - SECURITY_OK: The secured message was generated successfully.
* - SECURITY_NOT_OK: Failed to generate the secured message.
*
* Description of Security Levels:
* - NO_SECURITY: No security is applied to the message. It is sent as plain text.
* - HASH: The message is hashed using SHA-256, and the hash value is appended to the message.
* - CIPHER: The message is encrypted using RSA encryption with the specified device's public key.
* - SIGNATURE: A digital signature is generated for the message using the specified device's private key.
* - CIPHER_SIGNATURE_HASH: The message is encrypted, and a digital signature is generated for the hash of the message.
*   Both the encrypted message and the signature are included in the output.
*
* Note: This function internally utilizes functions from the CryptoStackManager module for cryptographic operations.
************************************************************************************************************************/
SecurityReturnStatus SecOC_Send_Secured(byte* in, word32 inLen, byte* out, word32* outLen, SecurityLevel Secure_level,unsigned int deviceID)
{
	byte Encrypted[ENCRYPT_SIZE] ={0};
	byte hash_digest[DIGEST_LENGTH] = {0};
	byte sign[SIGN_SIZE] ={0};
	CryptoReturnStatus result;
	switch(Secure_level)
	{
	case NO_SECURITY:
		memcpy(out,in,inLen);
		*outLen = inLen;
		return SECURITY_OK;
#if(SHA256_HASH == 1)
	case HASH:
		result = Crypto_Hash_SHA256(in,inLen,hash_digest);

		memcpy(out,in,inLen);
		memcpy(out+inLen,SEPARATOR,SEPARATOR_LEN);
		memcpy(out+inLen+SEPARATOR_LEN,hash_digest,DIGEST_LENGTH);

		if (result == CSM_OK )
		{
			*outLen = DIGEST_LENGTH + SEPARATOR_LEN + inLen;
			return SECURITY_OK;
		}
		return SECURITY_NOT_OK;

#endif
#if(RSA_Algorithm == 1)
	case RSA_CIPHER:
		result=Crypto_RSA_Encrypt((const byte*)in,inLen,out,ENCRYPT_SIZE,deviceID);
		*outLen = ENCRYPT_SIZE;
		if (result == CSM_OK )
		{
			return SECURITY_OK;
		}
		return SECURITY_NOT_OK;


	case RSA_SIGNATURE:


		result=Crypto_Signature_Generate((const byte*)in,inLen,out,SIGN_SIZE);
		*outLen = SIGN_SIZE;
		if (result == CSM_OK )
		{
			return SECURITY_OK;
		}
		return SECURITY_NOT_OK;

#endif
#if((RSA_Algorithm == 1) && (SHA256_HASH == 1))
	case RSA_CIPHER_SIGNATURE_HASH:

		hash_digest[DIGEST_LENGTH];
		if( (Crypto_Hash_SHA256(in,inLen,hash_digest) !=CSM_OK) || (Crypto_RSA_Encrypt((const byte*)in,inLen,Encrypted,ENCRYPT_SIZE,deviceID)!= CSM_OK )){
			return SECURITY_NOT_OK;
		}
		if(Crypto_Signature_Generate(hash_digest,DIGEST_LENGTH,sign,SIGN_SIZE)!=CSM_OK){
			return SECURITY_NOT_OK;
		}
		memcpy(out,Encrypted,ENCRYPT_SIZE);
		memcpy(out+ENCRYPT_SIZE,SEPARATOR,SEPARATOR_LEN);
		memcpy(out+ENCRYPT_SIZE+SEPARATOR_LEN,sign,SIGN_SIZE);
		*outLen = SIGN_SIZE + SEPARATOR_LEN + ENCRYPT_SIZE;
		return SECURITY_OK;
#endif

#if(AES_Algorithm == 1)

	case AES_CIPHER:
		result=Crypto_AES_Encrypt((const byte*)in,out,deviceID);
		*outLen = strlen((char*)out);
		if (result == CSM_OK )
		{
			return SECURITY_OK;
		}
		return SECURITY_NOT_OK;


	case AES_CIPHER_HMAC:

		hash_digest[DIGEST_LENGTH];
		if(( Crypto_HMAC_AES((const byte*)in,hash_digest,deviceID) !=CSM_OK) || (Crypto_AES_Encrypt(in,Encrypted,deviceID)!= CSM_OK )){
			return SECURITY_NOT_OK;
		}
		memcpy(out,Encrypted,AES_BLOCK_SIZE);
		memcpy(out+AES_BLOCK_SIZE,SEPARATOR,SEPARATOR_LEN);
		memcpy(out+AES_BLOCK_SIZE+SEPARATOR_LEN,hash_digest,DIGEST_LENGTH);
		*outLen = DIGEST_LENGTH + SEPARATOR_LEN + AES_BLOCK_SIZE;
		return SECURITY_OK;
#endif

	default:
		return SECURITY_NOT_OK;

   }
}
/************************************************************************************************************************
* Function Name: SecOC_Recieve_Secured
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Non-reentrant
* Parameters (in): in (input data), inLen (length of input data), Secure_level (security level), deviceID (device identifier)
* Parameters (inout): out (output buffer), outLen (length of output data)
* Parameters (out):
* Return value: SecurityReturnStatus
* Description:
* SecOC_Recieve_Secured function is responsible for receiving secured messages over an onboard communication channel. It
* validates the security of the received message based on the specified security level and device identifier.
*
* Parameters:
* - in: Input data representing the received secured message.
* - inLen: Length of the input data.
* - out: Output buffer to store the received plain text message.
* - outLen: Pointer to the length of the output data.
* - Secure_level: Security level applied to the received message (NO_SECURITY, HASH, CIPHER, SIGNATURE, or CIPHER_SIGNATURE_HASH).
* - deviceID: Identifier of the device used for cryptographic operations.
*
* Return Values:
* - SECURITY_OK: The received secured message was validated successfully.
* - SECURITY_NOT_OK: Failed to validate the received secured message.
*
* Description of Security Levels:
* - NO_SECURITY: No security is applied to the message. It is received as plain text.
* - HASH: The received message is hashed using SHA-256, and the hash value is compared with the appended hash.
* - CIPHER: The received message is decrypted using RSA decryption with the specified device's private key.
* - SIGNATURE: The received message's signature is verified using the specified device's public key.
* - CIPHER_SIGNATURE_HASH: The received message is decrypted, and its hash is compared with the verified signature's hash.
*   The decrypted message (plain text) is returned in the output in case of operation success .
*
* Note: This function internally utilizes functions from the CryptoStackManager module for cryptographic operations.
************************************************************************************************************************/
SecurityReturnStatus SecOC_Recieve_Secured(byte* in, word32 inLen, byte* out, word32* outLen, SecurityLevel Secure_level,unsigned int deviceID)
{
	static CryptoReturnStatus result;
 	unsigned int i = 0;
 	int lendec = 0;
 	byte input_message[ENCRYPT_SIZE*3] = {0};
	byte encrypted_text[ENCRYPT_SIZE] = {0};
	byte decrypted_text[ENCRYPT_SIZE] = {0};
	byte* hash_start;
	byte plain_text[ENCRYPT_SIZE*2] = {0};
	byte message_hash[DIGEST_LENGTH] = {0};
	byte digest_text[DIGEST_LENGTH] = {0};
    byte hash_digest [DIGEST_LENGTH] = {0};
	byte signature_text[SIGN_SIZE] = {0};
	byte verified_digest_text[SIGN_SIZE] = {0};
	char* hash_start_ptr;
	word32 Decrypt_len;
	word32 Verified_len;
 switch(Secure_level)
 {
	 case NO_SECURITY:
		 memcpy(out,in,inLen);
		 *outLen = inLen;
		return SECURITY_OK;
#if(SHA256_HASH == 1)
	 case HASH:
		 hash_start_ptr = strstr((char*)in,SEPARATOR);
		 hash_start = (byte*)hash_start_ptr;
		 memcpy(message_hash,hash_start+SEPARATOR_LEN,DIGEST_LENGTH);
		 while(!((in[i] == '*') && (in[i+1] == '*') && (in[i+2] == '*')))
		 {
			 i++;
		 }
		 memcpy(plain_text,in,i);
    	 result = Crypto_Check_Hash_SHA256(plain_text,i,message_hash);
    	 if (result != CSM_OK )
    	 {
             return SECURITY_NOT_OK;
    	 }
    	 memcpy(out,plain_text,i);
    	 *outLen = i;
 	    return SECURITY_OK;
#endif

#if(RSA_Algorithm == 1)
	 case RSA_CIPHER:
		 Decrypt_len = ENCRYPT_SIZE;
	     result=Crypto_RSA_Decrypt((const byte*)in,inLen,out,Decrypt_len);
	     *outLen = strlen((char*)out);
	     if (result == CSM_OK )
	     {
	       return SECURITY_OK;
	     }
	       return SECURITY_NOT_OK;

	 case RSA_SIGNATURE:

		  Verified_len = SIGN_SIZE;
		  result=Crypto_Signature_Verify((const byte*)in,inLen,out,Verified_len,deviceID);
		  *outLen = strlen((char*)out);
		  if (result == CSM_OK )
		  {
			 return SECURITY_OK;
		 }
		   return SECURITY_NOT_OK;
#endif
#if((RSA_Algorithm == 1)&&(SHA256_HASH == 1))
	 case RSA_CIPHER_SIGNATURE_HASH :
		         i = 0;
				 memcpy(input_message,in,inLen);
				 while(!((in[i] == '*') && (in[i+1] == '*') && (in[i+2] == '*')))
				 {
					 i++;
				 }
				 memcpy(signature_text,in+i+SEPARATOR_LEN,SIGN_SIZE);
				 memcpy(encrypted_text,in,i);
				 if((Crypto_Signature_Verify(signature_text,SIGN_SIZE,verified_digest_text,sizeof(signature_text),deviceID) != CSM_OK ))
				 {
					 return SECURITY_NOT_OK;
				 }
				 if( (Crypto_RSA_Decrypt(encrypted_text,ENCRYPT_SIZE,decrypted_text,ENCRYPT_SIZE) !=CSM_OK))
				 {
					 return SECURITY_NOT_OK;
				 }
				 lendec = strlen((char*)decrypted_text);
		    	 result =   Crypto_Check_Hash_SHA256(decrypted_text,strlen((char*)decrypted_text),verified_digest_text);
		    	 if (result != CSM_OK )
		    	 {
		             return SECURITY_NOT_OK;
		    	 }
		    	 memcpy(out,decrypted_text,sizeof(decrypted_text));
		    	 *outLen = strlen((char*)decrypted_text);
		 	     return SECURITY_OK;
#endif

#if(AES_Algorithm == 1)
	 case AES_CIPHER:

	     result=Crypto_AES_Decrypt((const byte*)in,out,deviceID);
	     *outLen = strlen((char*)out);
	     if (result == CSM_OK )
	     {
	       return SECURITY_OK;
	     }
	       return SECURITY_NOT_OK;

	 case AES_CIPHER_HMAC:
         i = 0;
		 memcpy(input_message,in,inLen);
		 while(!((in[i] == '*') && (in[i+1] == '*') && (in[i+2] == '*')))
		 {
			 i++;
		 }
		 memcpy(digest_text,in+i+SEPARATOR_LEN,DIGEST_LENGTH);
		 memcpy(encrypted_text,in,i);
		 i++;
		 if( (Crypto_AES_Decrypt(encrypted_text,decrypted_text,deviceID) !=CSM_OK))
		 {
			 return SECURITY_NOT_OK;
		 }

		 if((Crypto_HMAC_AES((const byte*)decrypted_text,hash_digest,deviceID) != CSM_OK ))
		 {
			 return SECURITY_NOT_OK;
		 }
		 lendec = strlen((char*)decrypted_text);

    	 if (memcmp(digest_text, hash_digest,32) != 0)
    	 {
             return SECURITY_NOT_OK;
    	 }
    	 memcpy(out,decrypted_text,sizeof(decrypted_text));
    	 *outLen = strlen((char*)decrypted_text);
 	     return SECURITY_OK;
#endif
	  default:
		   return SECURITY_NOT_OK;
	}
}
