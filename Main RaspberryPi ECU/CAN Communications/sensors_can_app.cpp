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
#include "sensors_can_app.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
static unsigned int Device_ID = DEVICE_ID;
static int APP_START_Flag = 0;
static pthread_t CAN__Receive_Thread; /*Thread PID variable*/
/***********************************************************************************************************************
*                     					      PRIVATE FUNCTIONS PROTOTYPES
***********************************************************************************************************************/
static void *Sensors_App_CAN_Receive_Thread(void* v);
static void Log_Err(const char* Error);
/***********************************************************************************************************************
*                     					        FUNCTIONS DEFINTITIONS
***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: Sensors_CAN_APP_Start
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Status of function execution
* Description: Function to Start CAN application detecated to receive CAN messages regarding vehicle sensors
************************************************************************************************************************/
SENSORS_CAN_APP_STATUS Sensors_CAN_APP_Start(void)
{
    if(APP_START_Flag == 1)
    {
        return SENSORS_APP_OK;
    }
    /*Configure APP threads to be run*/  
    pthread_attr_t Receive_Thread_Attributes; /*Thread attributes to configure*/
    struct sched_param Receive_Scheduling_Priority;
    pthread_attr_init(&Receive_Thread_Attributes);   /*init attributes*/
    pthread_attr_setinheritsched(&Receive_Thread_Attributes,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&Receive_Thread_Attributes,SCHED_FIFO);
    Receive_Scheduling_Priority.sched_priority = 32;/*highest priority possible for the scheduling policy meeting POSIX standard*/
    pthread_attr_setschedparam(&Receive_Thread_Attributes,&Receive_Scheduling_Priority);
    /*Initialize CAN*/
    LEDs_Init();
    if(CAN_Init(&CAN_Configuration) != CAN_OK)
    {
        return SENSORS_APP_NOT_OK;
    }
    if(CAN_TP_Init() != CAN_TRANSPORT_OK)
    {
        return SENSORS_APP_NOT_OK;
    }
    if(pthread_create(&CAN__Receive_Thread,&Receive_Thread_Attributes,&Sensors_App_CAN_Receive_Thread,NULL) != 0)
    {
        return SENSORS_APP_NOT_OK;
    }
    pthread_attr_destroy(&Receive_Thread_Attributes);
    APP_START_Flag = 1;
    return SENSORS_APP_OK;
}
/************************************************************************************************************************
* Function Name: APP_CAN_Receive_Thread
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Thread to read received, secured and assembled CAN messages given a message ID
               which are Ultrasonic sensors values and IMU values and shock sensor state.
               and it stores their values in specific files
************************************************************************************************************************/
static void *Sensors_App_CAN_Receive_Thread(void* v)
{
    int receive_fd = 0;
    int write_bytes = 0;
    unsigned int Sender_DeviceID_Unused = 0;
    SecurityLevel Secur_Lvl_Unused;
    unsigned int Received_CAN_Message_Length = 0;
    unsigned char Received_CAN_Message[MAX_MESSAGE_SIZE] = {0};
    while(1)
    {
        /*---------------------------------------US 0,1 Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Ultrasonic_01_MessageID) == CAN_SM_OK)
        {
            if(CAN_SM_Read(Received_CAN_Message,Ultrasonic_01_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused) == CAN_SM_OK)
            {
                int i = 0;
                char Ultrasonic0_str[Files_content_size] = {0};
                char Ultrasonic1_str[Files_content_size] = {0};
                for(;Received_CAN_Message[i] != ',';i++);
                memcpy(Ultrasonic0_str,(char*)(Received_CAN_Message),i);
                memcpy(Ultrasonic1_str,(char*)(Received_CAN_Message+i+1),Received_CAN_Message_Length-i-1);
                /*Store Ultrasonic0 reading*/
                receive_fd = open(US0_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Ultrasonic0 Open File Error");
                }
                write_bytes = write(receive_fd,Ultrasonic0_str,strlen(Ultrasonic0_str));
                if(write_bytes < 0)
                {
                    Log_Err("Ultrasonic0 Write File Error");
                }
                close(receive_fd);
                /*Store Ultrasonic1 reading*/
                receive_fd = open(US1_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Ultrasonic1 Open File Error");
                }
                write_bytes = write(receive_fd,Ultrasonic1_str,strlen(Ultrasonic1_str));
                if(write_bytes < 0)
                {
                    Log_Err("Ultrasonic1 Write File Error");
                }
                close(receive_fd);
                /*Store Ultrasonic0 State*/
                int Ultrasonic0 = atoi(Ultrasonic0_str);
                if(Ultrasonic0<30 && Ultrasonic0>0)
                {
                    receive_fd = open(US0_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic0 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"1",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic0 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic0>=30 && Ultrasonic0<50)
                {
                    receive_fd = open(US0_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic0 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"2",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic0 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic0>=50 && Ultrasonic0<=80)
                {
                    receive_fd = open(US0_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic0 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"3",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic0 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic0>80)
                {
                    receive_fd = open(US0_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic0 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"4",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic0 Write State File Error");
                    }
                    close(receive_fd);
                }
                else
                {
                    Log_Err("Ultrasonic0 Value Error");
                }
                /*Store Ultrasonic1 State*/
                int Ultrasonic1 = atoi(Ultrasonic1_str);
                if(Ultrasonic1<30 && Ultrasonic1>0)
                {
                    receive_fd = open(US1_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic1 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"1",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic1 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic1>=30 && Ultrasonic1<50)
                {
                    receive_fd = open(US1_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic1 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"2",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic1 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic1>=50 && Ultrasonic1<=80)
                {
                    receive_fd = open(US1_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic1 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"3",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic1 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic1>80)
                {
                    receive_fd = open(US1_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic1 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"4",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic1 Write State File Error");
                    }
                    close(receive_fd);
                }
                else
                {
                    Log_Err("Ultrasonic1 Value Error");
                }
            }
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        /*---------------------------------------US 2,3 Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Ultrasonic_23_MessageID) == CAN_SM_OK)
        {
            if(CAN_SM_Read(Received_CAN_Message,Ultrasonic_23_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused) == CAN_SM_OK)
            {
                int i = 0;
                char Ultrasonic2_str[Files_content_size] = {0};
                char Ultrasonic3_str[Files_content_size] = {0};
                for(;Received_CAN_Message[i] != ',';i++);
                memcpy(Ultrasonic2_str,(char*)(Received_CAN_Message),i);
                memcpy(Ultrasonic3_str,(char*)(Received_CAN_Message+i+1),Received_CAN_Message_Length-i-1);
                /*Store Ultrasonic2 reading*/
                receive_fd = open(US2_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Ultrasonic2 Open File Error");
                }
                write_bytes = write(receive_fd,Ultrasonic2_str,strlen(Ultrasonic2_str));
                if(write_bytes < 0)
                {
                    Log_Err("Ultrasonic2 Write File Error");
                }
                close(receive_fd);
                /*Store Ultrasonic3 reading*/
                receive_fd = open(US3_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Ultrasonic3 Open File Error");
                }
                write_bytes = write(receive_fd,Ultrasonic3_str,strlen(Ultrasonic3_str));
                if(write_bytes < 0)
                {
                    Log_Err("Ultrasonic3 Write File Error");
                }
                close(receive_fd);
                /*Store Ultrasonic2 State*/
                int Ultrasonic2 = atoi(Ultrasonic2_str);
                if(Ultrasonic2<30 && Ultrasonic2>0)
                {
                    receive_fd = open(US2_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic2 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"1",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic2 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic2>=30 && Ultrasonic2<50)
                {
                    receive_fd = open(US2_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic2 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"2",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic2 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic2>=50 && Ultrasonic2<=80)
                {
                    receive_fd = open(US2_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic2 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"3",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic2 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic2>80)
                {
                    receive_fd = open(US2_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic2 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"4",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic2 Write State File Error");
                    }
                    close(receive_fd);
                }
                else
                {
                    Log_Err("Ultrasonic2 Value Error");
                }
                /*Store Ultrasonic3 State*/
                int Ultrasonic3 = atoi(Ultrasonic3_str);
                if(Ultrasonic3<30 && Ultrasonic3>0)
                {
                    receive_fd = open(US3_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic3 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"1",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic3 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic3>=30 && Ultrasonic3<50)
                {
                    receive_fd = open(US3_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic3 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"2",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic3 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic3>=50 && Ultrasonic3<=80)
                {
                    receive_fd = open(US3_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic3 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"3",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic3 Write State File Error");
                    }
                    close(receive_fd);
                }
                else if(Ultrasonic3>80)
                {
                    receive_fd = open(US3_State_File,O_RDWR|O_CREAT,0755);
                    if(receive_fd <= 0)
                    {
                        Log_Err("Ultrasonic3 Open State File Error");
                    }
                    write_bytes = write(receive_fd,"4",1);
                    if(write_bytes < 0)
                    {
                        Log_Err("Ultrasonic3 Write State File Error");
                    }
                    close(receive_fd);
                }
                else
                {
                    Log_Err("Ultrasonic3 Value Error");
                }
            }
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        /*---------------------------------------Gyroscope Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Gyroscope_MessageID) == CAN_SM_OK)
        {
            if(CAN_SM_Read(Received_CAN_Message,Gyroscope_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused) == CAN_SM_OK)
            {
                receive_fd = open(Gyroscope_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Gyroscope Open File Error");
                }
                write_bytes = write(receive_fd,Received_CAN_Message,Received_CAN_Message_Length);
                if(write_bytes < 0)
                {
                    Log_Err("Gyroscope Write File Error");
                }
                close(receive_fd);
            }
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        /*---------------------------------------Accelerometer Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Accelerometer_MessageID) == CAN_SM_OK)
        {
            if(CAN_SM_Read(Received_CAN_Message,Accelerometer_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused) == CAN_SM_OK)
            {
                receive_fd = open(Accelerometer_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Accelerometer Open File Error");
                }
                write_bytes = write(receive_fd,Received_CAN_Message,Received_CAN_Message_Length);
                if(write_bytes < 0)
                {
                    Log_Err("Accelerometer Write File Error");
                }
                close(receive_fd);
            }
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        /*---------------------------------------Magnetometer Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Magnetometer_MessageID) == CAN_SM_OK)
        {
            if(CAN_SM_Read(Received_CAN_Message,Magnetometer_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused) == CAN_SM_OK)
            {
                receive_fd = open(Magnetometer_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Magnetometer Open File Error");
                }
                write_bytes = write(receive_fd,Received_CAN_Message,Received_CAN_Message_Length);
                if(write_bytes < 0)
                {
                    Log_Err("Magnetometer Write File Error");
                }
                close(receive_fd);
            }
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        /*---------------------------------------Temperature Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Temperature_MessageID) == CAN_SM_OK)
        {
            if(CAN_SM_Read(Received_CAN_Message,Temperature_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused) == CAN_SM_OK)
            {
                receive_fd = open(Temperature_File,O_RDWR|O_CREAT,0755);
                if(receive_fd <= 0)
                {
                    Log_Err("Temperature Open File Error");
                }
                write_bytes = write(receive_fd,Received_CAN_Message,Received_CAN_Message_Length);
                if(write_bytes < 0)
                {
                    Log_Err("Temperature Write File Error");
                }
                close(receive_fd);
            }
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        /*---------------------------------------Shock Sensor Case------------------------------------------------------*/
        if(CAN_SM_Check_Inbox(Shock_MessageID) == CAN_SM_OK)
        {
            /*Read to just remove the message from inbox buffer*/
            CAN_SM_Read(Received_CAN_Message,Shock_MessageID,&Received_CAN_Message_Length,\
                        &Secur_Lvl_Unused,&Sender_DeviceID_Unused);
            receive_fd = open(Shock_State_File,O_RDWR|O_CREAT,0755);
            if(receive_fd <= 0)
            {
                Log_Err("Shock Sensor Open File Error");
            }
            write_bytes = write(receive_fd,"1",1);   /*Raise the flag*/
            if(write_bytes < 0)
            {
                Log_Err("Shock Sensor Write File Error");
            }
            Log_Err("CRUSH DETECTED");
            close(receive_fd);
        }
        /*Clear Receive Buffer*/
        Received_CAN_Message_Length = 0;
        memset(Received_CAN_Message,0,MAX_MESSAGE_SIZE);
        usleep(5000);   /*Update every 5ms*/
    }
}
/************************************************************************************************************************
* Function Name: CAN_SM_Read
* Function ID[hex]: 0x02
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