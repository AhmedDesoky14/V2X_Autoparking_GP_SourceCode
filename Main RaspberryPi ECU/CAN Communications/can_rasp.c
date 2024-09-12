/************************************************************************************************************************
 *  Module: CAN Raspberry
 *  File Name: can.c
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
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "can_rasp.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
static pthread_t Rx_Thread; /*Thread PID variable*/
static struct can_frame Rx_CAN_Frame[CAN_FIFO_SIZE];   /*Global CAN Frame*/
static int CAN_Rx_Index = 0;    /*Receive Index*/
static int CAN_Read_Index = 0;  /*Read Index*/
static int CAN_Socket;  /*CAN Socket used*/
static int CAN_Initialized = 0; /*Initialization Flag*/
int receive_ready = 0;  /*shared resource varible used for signaling upper layer threads*/
pthread_mutex_t receive_mutex = PTHREAD_MUTEX_INITIALIZER;  /*Mutex for upper layer thread signaling*/
pthread_cond_t receive_thread_condition = PTHREAD_COND_INITIALIZER; /*Condition variable for the same reason*/
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static CAN_STATUS CAN_Start_Receive(void);
static void *CAN_Receive_Thread(void*);
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: CAN_Init
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Configurations Structure object
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution Status
* Description: Function to initialize CAN module, before build and run 
*              call this shell command -> "sudo ip link set (can interface mame) down"
************************************************************************************************************************/
CAN_STATUS CAN_Init(const CAN_INIT_CONFIG* CAN_Configuration)
{
    if(CAN_Initialized == 1)
    {
        return CAN_OK;
    }
    LED_OFF(CAN_INITIALIZED_LED_PIN);
    LED_OFF(CAN_ERROR_LED_PIN);
    struct sockaddr_can can_address;
    struct ifreq interface;
    char Set_Bitrate[60] = {0}; 
    char CAN_Up[30] = {0};
    CAN_Socket = socket(PF_CAN,SOCK_RAW,CAN_RAW);   /*Create socket for CAN communications*/
    if(CAN_Socket == -1)
    {
        return CAN_ERROR;
    }
    strcpy(interface.ifr_name,CAN_Configuration->CAN_Interface_Name);
    if(ioctl(CAN_Socket,SIOCGIFINDEX,&interface) == -1)
    {
        return CAN_ERROR;
    }
    can_address.can_family = AF_CAN;
    can_address.can_ifindex = interface.ifr_ifindex;
    bind(CAN_Socket,(struct sockaddr *)&can_address,sizeof(can_address)); /*bind can_address to the created socket*/
    sprintf(Set_Bitrate,"sudo ip link set %s type can bitrate %i",CAN_Configuration->CAN_Interface_Name,CAN_Configuration->Bitrate);
    if(system(Set_Bitrate) < 0)
    {
        return CAN_ERROR;
    }
    sprintf(CAN_Up,"sudo ip link set %s up",CAN_Configuration->CAN_Interface_Name);
    if(system(CAN_Up) < 0)
    {
        return CAN_ERROR;
    }
    if(CAN_Start_Receive() != CAN_OK)
    {
        return CAN_NOT_OK;
    }
    CAN_Initialized = 1;
    LED_ON(CAN_INITIALIZED_LED_PIN);
    return CAN_OK;
}
/************************************************************************************************************************
* Function Name: CAN_Send
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Frame/Message ID
*                  Frame/Message data
*                  Frame/Message size
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution Status
* Description: Function to send CAN frames and messages
************************************************************************************************************************/
CAN_STATUS CAN_Send(unsigned int Message_ID,unsigned char* Message_Frame,unsigned short Message_Frame_Size)
{
    struct can_frame Frame;
    if(CAN_Initialized == 0)
    {
        LED_ON(CAN_ERROR_LED_PIN);
        return CAN_NOT_OK;
    }
    if((Message_Frame_Size>8) || (Message_Frame_Size<1))
    {
       LED_ON(CAN_ERROR_LED_PIN); 
        return CAN_NOT_OK;
    }
    Frame.can_id = Message_ID;
    Frame.len = Message_Frame_Size;
    memcpy(Frame.data,Message_Frame,Message_Frame_Size);
    if((write(CAN_Socket,&Frame,sizeof(struct can_frame))) != (sizeof(struct can_frame)))
    {
        LED_ON(CAN_ERROR_LED_PIN);
        return CAN_ERROR;
    }
    return CAN_OK;
}
/************************************************************************************************************************
* Function Name: CAN_Start_Receive
* Function ID[hex]: 0x02
* Sync/Async: Asynchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution Status
* Description: Function to start receiving CAN frames and messages by invoking a dedicated thread for this function
************************************************************************************************************************/
static CAN_STATUS CAN_Start_Receive(void)
{
    static pthread_attr_t Rx_Thread_Attributes; /*Thread attributes to configure*/
    struct sched_param Scheduling_Priority;
    pthread_attr_init(&Rx_Thread_Attributes);   /*init attributes*/
    pthread_attr_setinheritsched(&Rx_Thread_Attributes,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&Rx_Thread_Attributes,SCHED_FIFO);
    Scheduling_Priority.sched_priority = 32;/*highest priority possible for the scheduling policy meeting POSIX standard*/
    pthread_attr_setschedparam(&Rx_Thread_Attributes,&Scheduling_Priority);
    if(pthread_create(&Rx_Thread,&Rx_Thread_Attributes,&CAN_Receive_Thread,NULL) != 0)
    {
        LED_ON(CAN_ERROR_LED_PIN);
        return CAN_ERROR;
    }
    pthread_attr_destroy(&Rx_Thread_Attributes);
    return CAN_OK;
}
/************************************************************************************************************************
* Function Name: CAN_Receive_Thread
* Function ID[hex]: 0x03
* Sync/Async: Asynchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: A Thread that is dedicated to recieve certain frames or messages accroding to the software IDs filter
*              and runs parallel to the main thread (program) and copy the received frames in a global priavte variable
************************************************************************************************************************/
static void *CAN_Receive_Thread(void* v)
{
    struct can_frame Frame;
    unsigned int ID_Iterator = 0;
    unsigned int first_read_flag = 0; //flag to be set after first read, because first read returns -1 at system start
    while(1)
    {
        ID_Iterator = 0;
        if((read(CAN_Socket,&Frame,sizeof(struct can_frame))) < 0)
        {
            if(first_read_flag == 1)
            {
                LED_ON(CAN_ERROR_LED_PIN);
                pthread_exit(NULL); /*Exit the thread*/
            }
            first_read_flag = 1;

        }
        if(Frame.len <= 0)  /*empty frame*/
        {
            continue;
        }
        /*CAN Filter section*/
        for(ID_Iterator;ID_Iterator<NUMBER_OF_RX_IDS;ID_Iterator++)
        {
            if(Frame.can_id == RxMessageIDList[ID_Iterator])  /*if the received message is within the accepted IDs*/
            {
                Rx_CAN_Frame[CAN_Rx_Index].len = Frame.len;
                Rx_CAN_Frame[CAN_Rx_Index].can_id = Frame.can_id;
                memset(Rx_CAN_Frame[CAN_Rx_Index].data,0,MAX_PAYLOAD);
                memcpy(Rx_CAN_Frame[CAN_Rx_Index].data,Frame.data,Frame.len);
                CAN_Rx_Index++;
                if(CAN_Rx_Index == CAN_FIFO_SIZE)
                {
                    CAN_Rx_Index = 0;
                }
                /*New Frame received and stored, signal upper layer thread*/
                pthread_mutex_lock(&receive_mutex);    /*Lock Mutex to ensure cond wait before signaling*/
                receive_ready = 1;
                pthread_cond_signal(&receive_thread_condition);
                pthread_mutex_unlock(&receive_mutex);   /*Unlock mutex*/
                break;  /*break from the closed loop*/
            }
        }
    }
}
/************************************************************************************************************************
* Function Name: CAN_Check_Inbox
* Function ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution Status and Inbox status
*               if CAN_OK -> Inbox has a frame/message
*               if CAN_NOT_OK -> Inbox is empty or module is not initialized
* Description: Function to check the receiving inbox
************************************************************************************************************************/
CAN_STATUS CAN_Check_Inbox(void)
{
    if(CAN_Initialized == 0)
    {
        LED_ON(CAN_ERROR_LED_PIN);
        return CAN_NOT_OK;
    }
    if(Rx_CAN_Frame[CAN_Read_Index].len <= 0)
    {
        return CAN_NOT_OK;
    }
    return CAN_OK;
}
/************************************************************************************************************************
* Function Name: CAN_Read
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Frame/Message ID by reference
*                  Frame/Message data by reference
*                  Frame/Message size by refernce
* Parameters (inout): NONE
* Parameters (out): Frame/Message ID
*                   Frame/Message data
*                   Frame/Message size
* Return value: Execution Status
* Description: Function to read the received CAN frames or messaged from the receiving thread
************************************************************************************************************************/
CAN_STATUS CAN_Read(unsigned int* Message_ID,unsigned char* Message,unsigned short* Message_Size)
{
    if(CAN_Initialized == 0)
    {
        LED_ON(CAN_ERROR_LED_PIN);
        return CAN_NOT_OK;
    }
    if(Rx_CAN_Frame[CAN_Read_Index].len <= 0)
    {
        /*if empty buffer, return not ok and can not read*/
        return CAN_NOT_OK;
    }
    *Message_Size = Rx_CAN_Frame[CAN_Read_Index].len;
    *Message_ID = Rx_CAN_Frame[CAN_Read_Index].can_id;
    memcpy(Message,Rx_CAN_Frame[CAN_Read_Index].data,Rx_CAN_Frame[CAN_Read_Index].len);
    Rx_CAN_Frame[CAN_Read_Index].len = 0;   /*indicating that the frame is already received*/
    Rx_CAN_Frame[CAN_Read_Index].can_id = 0;  
    memset(Rx_CAN_Frame[CAN_Read_Index].data,0,MAX_PAYLOAD);
    CAN_Read_Index++;
    if(CAN_Read_Index == CAN_FIFO_SIZE)
    {
        CAN_Read_Index = 0;
    }
    return CAN_OK;
}