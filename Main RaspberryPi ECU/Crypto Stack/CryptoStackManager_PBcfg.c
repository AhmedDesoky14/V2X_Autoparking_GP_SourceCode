/************************************************************************************************************************
 * 	Module: CryptoStackManager
 * 	File Name: CryptoStackManager_PBcfg.c
 *  Authors: Ahmed Desoky
 *	Date:	23/6/2024
 *	*********************************************************************************************************************
 *	Description: Crypto Stack Manager Post Build Configurations File
 *				 Please go ahead to configurations notes section in "CryptoStackManager_cfg.h" file
 *				 to understand how to confiure the crypto stack for your use case.
 ***********************************************************************************************************************/
/************************************************************************************************************************
 *                     							         INCLUDES
 ***********************************************************************************************************************/
#include "CryptoStackManager.h"
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 2.1.1*/
#define CRYPTOSTACKMANAGER_PBCFG_SW_MAJOR_VERSION           (2U)
#define CRYPTOSTACKMANAGER_PBCFG_SW_MINOR_VERSION           (1U)
#define CRYPTOSTACKMANAGER_PBCFG_SW_PATCH_VERSION           (1U)
/*Software Version checking between Module Post Build Configuration file and Header file*/
#if ((CRYPTOSTACKMANAGER_SW_MAJOR_VERSION != CRYPTOSTACKMANAGER_PBCFG_SW_MAJOR_VERSION)\
 ||  (CRYPTOSTACKMANAGER_SW_MINOR_VERSION != CRYPTOSTACKMANAGER_PBCFG_SW_MINOR_VERSION)\
 ||  (CRYPTOSTACKMANAGER_SW_PATCH_VERSION != CRYPTOSTACKMANAGER_PBCFG_SW_PATCH_VERSION))
  #error "The Software version of CryptoStackManager does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                            			          CONFIGURATIONS
 ***********************************************************************************************************************/
#if(RSA_Algorithm == 1)
/*Define or Include RSA Keys here*/
/* ALL PUBLIC KEYS  */
#include "Inf2_PrivKey.h"
///* MY PRIVATE KEY */
#include "Valeo_Vehicle_PublicKey.h"
#include "Inf2_PublicKey.h"
/*Configure Private Key der_Ptr and der_len and device ID*/
RSA_Private_Key_Struct My_Private_Key;
//My_Private_Key.Private_Key_der_Ptr = Infrastucture2_PrivKey_der;

/*Configure Keys der_Ptr and der_len and device ID*/

RSA_Key_Struct Public_keys[RSA_SYSTEM_DEVICES];

/*Function to define the configurations*/
void Define_RSA_Keys(void)
{
	My_Private_Key.Device_ID = 123;
	My_Private_Key.Private_Key_der_Ptr = Infrastucture2_PrivKey_der;
	My_Private_Key.Private_Key_der_len = Infrastucture2_PrivKey_der_len;
	Public_keys[0].Device_ID = 123;
	Public_keys[0].RSA_Key_der_Ptr = Infrastucture2_PublicKey_der;
	Public_keys[0].RSA_Key_der_len = Infrastucture2_PublicKey_der_len;
}
#endif

#if(AES_Algorithm == 1)
/*Define or Include AES Keys here*/
byte aes_key0[16] = {
    0xc3, 0x56, 0x37, 0x00,
    0xb2, 0x46, 0x93, 0xba,
    0x95, 0xc5, 0xfb, 0xd1,
    0xcc, 0xcd, 0x47, 0xed
};
byte aes_key1[16] = {
    0x1a, 0x3f, 0x28, 0x99,
    0xbd, 0x7c, 0x6e, 0x4b,
    0x3e, 0x2a, 0x59, 0xf1,
    0xa4, 0xc9, 0x8b, 0x2d
};
byte aes_key2[16] = {
    0xfe, 0xa1, 0x73, 0x52,
    0x4c, 0x88, 0xda, 0x9e,
    0x77, 0xb0, 0x3d, 0x65,
    0x13, 0xf2, 0x94, 0x06
};
/*Configure Keys pointers and device ID*/

AES_Key_Struct AES_Keys[AES_SYSTEM_DEVICES] = {aes_key0,99};

#endif


