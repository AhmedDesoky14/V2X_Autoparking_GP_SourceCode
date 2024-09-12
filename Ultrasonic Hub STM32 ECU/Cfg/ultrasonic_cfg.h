/************************************************************************************************************************
 * 	Module:ultrasonic
 * 	File Name:ultrasonic_cfg.h
 *  Authors:Esraa Fawzy
 *	Date:Feb 20, 2024
 *	*********************************************************************************************************************
 *	Description:The ultrasonic module's configuration parameters are contained in this file.
 *	 It specifies pre-build configurations for the ultrasonic sensors as well as vendor and
 *	 software version control macros.
 ***********************************************************************************************************************/
#ifndef INC_ULTRASONIC_CFG_H_
#define INC_ULTRASONIC_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 1.1.0*/
#define ultrasonic_CFG_SW_MAJOR_VERSION           (1U)
#define ultrasonic_CFG_SW_MINOR_VERSION           (1U)
#define ultrasonic_CFG_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
#define ULTRASONIC_NUMBERS  2
#define Trigger_Timeout_ms	100
#endif /* INC_ULTRASONIC_CFG_H_ */
