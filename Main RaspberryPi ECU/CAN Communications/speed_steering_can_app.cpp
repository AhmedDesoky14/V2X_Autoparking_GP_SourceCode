/************************************************************************************************************************
 * 	Module: Speed/Steering CAN Application
 * 	File Name: speed_steering_can_app.c
 *  Authors: Ahmed Desoky
 *	Date: 18/7/2024
 *	*********************************************************************************************************************
 *	Description: This application module is designed to be called in Qt main entry function or any entry
 *               point to the program to continously listen for CAN messages concerning read speed and
 *               steering angle values from encoders connected to another ECU in the CAN network and store
 *               them in certain files acting as pipes. and continously send current set point of vehicle
 *               speed and steering through CAN to the ECU controlling the motors.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "speed_steering_can_app.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
static unsigned int Device_ID = DEVICE_ID;
static int APP_START_Flag = 0;
static pthread_t CAN__Send_Thread; /*Thread PID variable*/
static pthread_t CAN__Receive_Thread; /*Thread PID variable*/
/***********************************************************************************************************************
*                     					      PRIVATE FUNCTIONS PROTOTYPES
***********************************************************************************************************************/
static void *Speed_Steering_App_CAN_Send_Thread(void* v);
static void *Speed_Steering_App_CAN_Receive_Thread(void* v);
static void Log_Err(const char* Error);
/***********************************************************************************************************************
*                     					        FUNCTIONS DEFINTITIONS
***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: AutoParking_CAN_APP_Start
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Status of function execution
* Description: Function to Start CAN application detecated to control CAN messages regarding vehicle
               Set speed and steering angle and the read values from encoders
************************************************************************************************************************/
SS_CAN_APP_STATUS Speed_Steering_CAN_APP_Start(void)
{
    if(APP_START_Flag == 1)
    {
        return SS_APP_OK;
    }
    /*Configure APP threads to be run*/
    pthread_attr_t Send_Thread_Attributes; /*Thread attributes to configure*/
    struct sched_param Send_Scheduling_Priority;   
    pthread_attr_t Receive_Thread_Attributes; /*Thread attributes to configure*/
    struct sched_param Receive_Scheduling_Priority;
    pthread_attr_init(&Send_Thread_Attributes);   /*init attributes*/
    pthread_attr_setinheritsched(&Send_Thread_Attributes,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&Send_Thread_Attributes,SCHED_FIFO);
    Send_Scheduling_Priority.sched_priority = 32;/*highest priority possible for the scheduling policy meeting POSIX standard*/
    pthread_attr_setschedparam(&Send_Thread_Attributes,&Send_Scheduling_Priority);
    pthread_attr_init(&Receive_Thread_Attributes);   /*init attributes*/
    pthread_attr_setinheritsched(&Receive_Thread_Attributes,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&Receive_Thread_Attributes,SCHED_FIFO);
    Receive_Scheduling_Priority.sched_priority = 32;/*highest priority possible for the scheduling policy meeting POSIX standard*/
    pthread_attr_setschedparam(&Receive_Thread_Attributes,&Receive_Scheduling_Priority);
    /*Initialize CAN*/
    LEDs_Init();
    if(CAN_Init(&CAN_Configuration) != CAN_OK)
    {
        return SS_APP_NOT_OK;
    }
    if(CAN_TP_Init() != CAN_TRANSPORT_OK)
    {
        return SS_APP_NOT_OK;
    }
    if(pthread_create(&CAN__Send_Thread,&Send_Thread_Attributes,&Speed_Steering_App_CAN_Send_Thread,NULL) != 0)
    {
        return SS_APP_NOT_OK;
    }
    if(pthread_create(&CAN__Receive_Thread,&Receive_Thread_Attributes,&Speed_Steering_App_CAN_Receive_Thread,NULL) != 0)
    {
        return SS_APP_NOT_OK;
    }
    pthread_attr_destroy(&Send_Thread_Attributes);
    pthread_attr_destroy(&Receive_Thread_Attributes);
    APP_START_Flag = 1;
    return SS_APP_OK;
}
/************************************************************************************************************************
* Function Name: APP_CAN_Send_Thread
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Status of the inbox, whether there's a message or not
* Description: Thread to take Speed and steering angle from certain files and send them through CAN
               to other system ECUs to set speed and steering angle of the vehicle
************************************************************************************************************************/
static void *Speed_Steering_App_CAN_Send_Thread(void* v)
{
    int send_fd = 0;
    char Set_Speed_str[Files_content_size] = {0};
    char Set_Steering_Str[Files_content_size] = {0};
    int read_bytes = 0;
    while(1)
    {
        /*----------------------------Set Speed Case-----------------------------*/
        send_fd = open(Set_Speed_File,O_RDWR|O_CREAT,0755);
        if(send_fd <= 0)
        {
            Log_Err("Set Speed Open File Error");
        }
        read_bytes = read(send_fd,Set_Speed_str,Files_content_size);
        if(read_bytes < 0)
        {
            Log_Err("Set Speed Read File Error");
        }
        close(send_fd);
        if(CAN_SM_Send((unsigned char*)Set_Speed_str,Set_Speed_MessageID,\
        (unsigned int)(strlen(Set_Speed_str)),AES_CIPHER_HMAC,Device_ID) != CAN_SM_OK)
        {
            Log_Err("Set Speed CAN Send Error");
        }
        memset(Set_Speed_str,0,Files_content_size);
        /*----------------------------Set Steering Case-----------------------------*/
        send_fd = open(Set_Steering_File,O_RDWR|O_CREAT,0755);
        if(send_fd <= 0)
        {
            Log_Err("Set Steering Angle Open File Error");
        }
        read_bytes = read(send_fd,Set_Steering_Str,Files_content_size);
        if(read_bytes < 0)
        {
            Log_Err("Set Steering Angle Read File Error");
        }
        close(send_fd);
        if(CAN_SM_Send((unsigned char*)Set_Steering_Str,Set_Steering_Angle_MessageID,\
            (unsigned int)(strlen(Set_Steering_Str)),AES_CIPHER_HMAC,Device_ID) != CAN_SM_OK)
        {
            Log_Err("Set Steering Angle CAN Send Error");
        }
        memset(Set_Steering_Str,0,Files_content_size);
        usleep(5000);   /*Update every 5ms*/
    }
}
/************************************************************************************************************************
* Function Name: CAN_SM_Read
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Thread to read received, secured and assembled CAN messages given a message ID
               which are Speed and steering angle from encoders.
               and it stores their values in specific files
************************************************************************************************************************/
static void *Speed_Steering_App_CAN_Receive_Thread(void* v)
{
    int receive_fd = 0;
    int write_bytes = 0;
    unsigned int Sender_DeviceID_Unused = 0;
    SecurityLevel Secur_Lvl_Unused;
    unsigned int Received_CAN_Message_Length = 0;
    unsigned char Received_CAN_Message[MAX_MESSAGE_SIZE] = {0};
    while(1)
    {
        /*---------------------------------------Read Speed Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Read_Speed_MessageID) == CAN_SM_OK)
        {
            if(CAN_SM_Read(Received_CAN_Message,Read_Speed_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused) == CAN_SM_OK)
            {
                receive_fd = open(Read_Speed_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Read Speed Open File Error");
                }
                write_bytes = write(receive_fd,Received_CAN_Message,Received_CAN_Message_Length);
                if(write_bytes < 0)
                {
                    Log_Err("Read Speed Write File Error");
                }
                close(receive_fd);
            }
            else
            {
                Log_Err("Read Speed CAN Message Error");
            }
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        /*--------------------------------------Read Steering Angle Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Read_Steering_Angle_MessageID) == CAN_SM_OK)
        {
            if(CAN_SM_Read(Received_CAN_Message,Read_Steering_Angle_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused) == CAN_SM_OK)
            {
                receive_fd = open(Read_Steering_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Read Steering Angle Open File Error");
                }
                write_bytes = write(receive_fd,Received_CAN_Message,Received_CAN_Message_Length);
                if(write_bytes < 0)
                {
                    Log_Err("Read Steering Angle Write File Error");
                }
                close(receive_fd);
            }
            else
            {
                Log_Err("Read Steering Angle CAN Message Error");
            }
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        /*--------------------------------------Speed Encoder Error Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Speed_Encoder_Error_MessageID) == CAN_SM_OK)
        {
            /*Read to just remove the message from inbox buffer*/
            CAN_SM_Read(Received_CAN_Message,Speed_Encoder_Error_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused);
            Log_Err("Speed Encoder Faliure");
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        /*--------------------------------------Steering Encoder Error Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Steering_Encoder_Error_MessageID) == CAN_SM_OK)
        {
            /*Read to just remove the message from inbox buffer*/
            CAN_SM_Read(Received_CAN_Message,Steering_Encoder_Error_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused);
            Log_Err("Steering Encoder Faliure");
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        usleep(5000);   /*Update every 5ms*/
    }
}
/************************************************************************************************************************
* Function Name: CAN_SM_Read
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Error string
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Private function to log the given error
************************************************************************************************************************/
static void Log_Err(const char* Error)
{
    int Error_fd = 0;
    time_t current_time;
    struct stat err_file_stat;
    char Error_str[Error_str_size] = {0};
    time(&current_time);
    sprintf(Error_str,"\n%s: %s",Error,ctime(&current_time));
    Error_fd = open(Err_Log,O_RDWR|O_CREAT|O_APPEND,0755);
    write(Error_fd,Error_str,Error_str_size);
    close(Error_fd);
    /*----------------------------Empty Error Log-----------------------------*/
    stat(Err_Log,&err_file_stat);
    /*Make Error Log File Empty*/
    if(err_file_stat.st_size > FILE_MAX_SIZE_BYTES)
    {
        Error_fd = open(Err_Log,O_RDWR|O_CREAT,0755);
        ftruncate(Error_fd,0);
        close(Error_fd);
    }
}