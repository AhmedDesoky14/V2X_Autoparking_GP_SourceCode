/************************************************************************************************************************
 * 	Module: WiFi Manager
 * 	File Name: wifi_manager.c
 *  Authors: Ahmed Desoky
 *	Date: 11/4/2024
 *	*********************************************************************************************************************
 *	Description: -Configurations file of WiFi Manager module to reconfigure before build and use
 *               This module is mainly based on "system" system call, you can check manual page -> man system.
 *               This system call is based on execvl and fork system calls which are mainly used to create
 *               a child process and replace this process with the desired process to invoke (execute a bash command)
 *               this system call differs from the mentioned others by being uninterruptible, so the running thread 
 *               will be pending the process to terminate.
 *               -This module uses a temporary file configured in the cfg file and used as a communication way
 *               between the executed command and the main program to parse commands returns and take decisions.
 *               -This module is designed and implemented for Unix based operating systems and can be configured for
 *               different WiFi interfaces devices as mentioned in Configurations Notes section.
 *               -It is possible to also control Hot-Spot Access Point using the same Network Manager Tool
 *               but it's not implemented in this module.
 *               -This module has a dedicated thread to continuously scan for nearby WiFi networks and store them in
                 a certain txt file configured in the cfg file
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "wifi_manager.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
static pthread_t Scan_Thread; /*Thread PID variable*/
static char WiFi_Initialized = 0; /*Global Variable to indiacate WiFi Initialization*/
static pthread_mutex_t Scan_Mutex = PTHREAD_MUTEX_INITIALIZER;
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static void *WiFi_Networks_Scan_Thread(void*);
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: WiFi_Connect
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): SSID of the access point to connect 
*                  SSID Password
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution result of this connection operation
* Description: Function used to connect using WiFi to certain SSID.
************************************************************************************************************************/
WIFI_STATE WiFi_Connect(const char* SSID,const char* Password)
{
    int exe_status = 0, connect_fd = 0, connect_buffer_size = 75;
    off_t connect_file_size = 0;
    ssize_t connect_read = 0;    
    char connect_exe[75] = {0};
    if(WiFi_Check_Availability() == WIFI_DEVICE_OFF)
    {
        return WIFI_NOT_OK;
    }
    sprintf(connect_exe,"nmcli dev wifi connect \"%s\" password %s ifname %s > %s"\
    ,SSID,Password,INTERFFACE_NAME,OPERATIONS_FILE_PATH);
    exe_status = system(connect_exe);
    if(exe_status < 0)
    {
        return WIFI_EXE_ERROR;
    }
    connect_fd = open(OPERATIONS_FILE_PATH,O_RDONLY|O_CREAT,0755);   /*open the file*/
    if(connect_fd <= 0)
    {
        return WIFI_EXE_ERROR;
    }
    char* connect_buffer = (char*)malloc((connect_buffer_size*sizeof(char))+1);
    connect_read = read(connect_fd,connect_buffer,connect_buffer_size);  /*read file*/
    if(connect_read < 0)
    {
        free(connect_buffer);
        close(connect_fd);
        return WIFI_EXE_ERROR;
    }
    if(strstr(connect_buffer,"failed") != NULL) 
    {
        free(connect_buffer);
        close(connect_fd);
        return WIFI_NOT_OK; /*failed to connect*/
    }
    free(connect_buffer);
    close(connect_fd);
    LED_ON(WIFI_CONNECT_LED_PIN);
    return WIFI_OK; /*connected successfully*/
}
/************************************************************************************************************************
* Function Name: WiFi_Disconnect
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution result of this connection operation
* Description: Function to disconnect WiFi from any connection.
************************************************************************************************************************/
WIFI_STATE WiFi_Disconnect(void)
{
    int exe_status = 0;
    char disconnect_exe[40] = "nmcli dev disconnect ";
    strcat(disconnect_exe,INTERFFACE_NAME);
    exe_status = system(disconnect_exe);
    if(exe_status < 0)
    {
        return WIFI_EXE_ERROR;
    }
    LED_OFF(WIFI_CONNECT_LED_PIN);
    return WIFI_OK; //any way a disconnection will be accomplished
}
/************************************************************************************************************************
* Function Name: WiFi_Search_AP
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): SSID to search for
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution result of this connection operation
* Description: Function to search for a given SSID.
************************************************************************************************************************/
WIFI_STATE WiFi_Search_AP(const char* SSID)
{
    int scan_results_fd = 0;
    off_t scan_results_file_size = 0;
    ssize_t results_read = 0;
    pthread_mutex_lock(&Scan_Mutex);   /*Lock mutex to start reading scan file*/
    scan_results_fd = open(NETWORKS_FILE_PATH,O_RDONLY|O_CREAT,0755);   /*open scan results file*/
    if(scan_results_fd <= 0)
    {
        return WIFI_EXE_ERROR;
    }
    while(scan_results_file_size == 0)
    {
        scan_results_file_size = lseek(scan_results_fd,0,SEEK_END); /*using lseek to get the file total size*/
    }
    lseek(scan_results_fd,0,SEEK_SET);  /*return the offset to start of the file*/
    char* Search_buffer = (char*)malloc((scan_results_file_size*sizeof(char))+1);   /*allocate buffer size as size of the file*/
    results_read = read(scan_results_fd,Search_buffer,scan_results_file_size);  /*read file*/
    pthread_mutex_unlock(&Scan_Mutex);   /*Unlock mutex*/
    if(results_read < 0)
    {
        free(Search_buffer);
        close(scan_results_fd);
        return WIFI_EXE_ERROR;
    }
    if(strstr(Search_buffer,SSID) == NULL)  /*Access Point not found*/
    {
        free(Search_buffer);
        close(scan_results_fd);
        return WIFI_NOT_OK; /*Access Point not found*/
    }
    free(Search_buffer);
    close(scan_results_fd);
    return WIFI_OK; /*Access Point found*/
}
/************************************************************************************************************************
* Function Name: WiFi_Check_AP_Connection
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution result of this connection operation
* Description: Function to check if WiFi is connected to any access point.
************************************************************************************************************************/
WIFI_STATE WiFi_Check_AP_Connection(void)
{
    int exe_status = 0, connection_status_fd = 0, connection_check_buffer_size = 75;
    off_t connection_status_file_size = 0;
    ssize_t connection_read = 0;
    char check_connection_exe[70] = {0};
    sprintf(check_connection_exe,"nmcli dev show %s | grep GENERAL.STATE > %s"\
    ,INTERFFACE_NAME,OPERATIONS_FILE_PATH);
    exe_status = system(check_connection_exe);  /*check wifi connection*/
    if(exe_status < 0)
    {
        return WIFI_EXE_ERROR;
    }
    connection_status_fd = open(OPERATIONS_FILE_PATH,O_RDONLY|O_CREAT,0755);   /*open the file*/
    if(connection_status_fd <= 0)
    {
        return WIFI_EXE_ERROR;
    }
    char* Check_connection_buffer = (char*)malloc((connection_check_buffer_size*sizeof(char))+1);
    connection_read = read(connection_status_fd,Check_connection_buffer,connection_check_buffer_size);  /*read file*/
    if(connection_read < 0)
    {
        free(Check_connection_buffer);
        close(connection_status_fd);
        return WIFI_EXE_ERROR;
    }
    if((strstr(Check_connection_buffer,"disconnected") != NULL) \
    || (strstr(Check_connection_buffer,"unavailable") != NULL))  /*wifi is disconnected*/
    {
        free(Check_connection_buffer);
        close(connection_status_fd);
        LED_OFF(WIFI_CONNECT_LED_PIN);
        return WIFI_NOT_OK; /*Not connected*/
    }
    free(Check_connection_buffer);
    close(connection_status_fd);
    LED_ON(WIFI_CONNECT_LED_PIN);
    return WIFI_OK; /*connected*/
}
/************************************************************************************************************************
* Function Name: WiFi_Get_IP_Address
* Function ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Pointer to array of characters to store the IP address
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution result of this connection operation
* Description: Function to get IP address of WiFi interface connection.
************************************************************************************************************************/
WIFI_STATE WiFi_Get_IP_Address(char* IP_Address)
{
    int exe_status = 0, ip_address_fd = 0, ip_address_buffer_size = 75;
    off_t ip_address_file_size = 0;
    ssize_t ip_address_read = 0;    
    unsigned int san_counter = 0;
    char get_ip_address_exe[75] = {0};
    char* ip_address;
    if(WiFi_Check_AP_Connection() == WIFI_NOT_OK)
    {
        return WIFI_NOT_OK; //basically if wifi connection is off, return that there's no IP address
    }
    sprintf(get_ip_address_exe,"nmcli dev show %s | grep IP4.ADDRESS > %s"\
    ,INTERFFACE_NAME,OPERATIONS_FILE_PATH);
    exe_status = system(get_ip_address_exe);  /*check wifi connection*/
    if(exe_status < 0)
    {
        return WIFI_EXE_ERROR;
    }
    ip_address_fd = open(OPERATIONS_FILE_PATH,O_RDONLY|O_CREAT,0755);   /*open the file*/
    if(ip_address_fd <= 0)
    {
        return WIFI_EXE_ERROR;
    }
    char* ip_address_buffer = (char*)malloc((ip_address_buffer_size*sizeof(char))+1);
    ip_address_read = read(ip_address_fd,ip_address_buffer,ip_address_buffer_size);  /*read file*/
    if(ip_address_read < 0)
    {
        free(ip_address_buffer);
        close(ip_address_fd);
        return WIFI_EXE_ERROR;
    }
    //Tokenizing the string
    char* token = strtok(ip_address_buffer," ");
    do
    {
        ip_address = token; //the last token from the string is the IP address
    }while(token = strtok(NULL," "));
    strcpy(IP_Address,ip_address);
    /*sanitize the output*/
    while(IP_Address[san_counter] != '/')
    {
        san_counter++;
    }
    while(IP_Address[san_counter])
    {
        IP_Address[san_counter] = '\0';
    }
    free(ip_address_buffer);
    close(ip_address_fd);
    return WIFI_OK;
}
/************************************************************************************************************************
* Function Name: WiFi_Check_Internet_Connection
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution result of this connection operation
* Description: Function to check machine Internet connection.
************************************************************************************************************************/
WIFI_STATE WiFi_Check_Internet_Connection(void)
{
    int exe_status = 0, internet_fd = 0, internet_buffer_size = 50;
    off_t internet_file_size = 0;
    ssize_t internet_read = 0;
    char internet_exe[50] = "ping google.com -c 3 > ";  /*send 5 ICMP packets*/
    strcat(internet_exe,OPERATIONS_FILE_PATH);    
    if(WiFi_Check_AP_Connection() == WIFI_NOT_OK)
    {
        return WIFI_NOT_OK; //basically if wifi connection is off, return that there's no Internet connection
    }
    exe_status = system(internet_exe);  /*check Internet connection*/
    if(exe_status < 0)
    {
        return WIFI_EXE_ERROR;
    }
    internet_fd = open(OPERATIONS_FILE_PATH,O_RDONLY|O_CREAT,0755);   /*open the file*/
    if(internet_fd <= 0)
    {
        return WIFI_EXE_ERROR;
    }
    char* internet_buffer = (char*)malloc((internet_buffer_size*sizeof(char))+1);
    internet_read = read(internet_fd,internet_buffer,internet_buffer_size);  /*read file*/
    if(internet_read < 0)
    {
        free(internet_buffer);
        close(internet_fd);
        return WIFI_EXE_ERROR;
    }
    if(strstr(internet_buffer,"PING") == NULL)  /*not connected to internet*/
    {
        free(internet_buffer);
        close(internet_fd);
        LED_OFF(INTERNET_CONNECT_LED_PIN);
        return WIFI_NOT_OK; /*Not Connected to Internet*/
    }
    free(internet_buffer);
    close(internet_fd);
    LED_ON(INTERNET_CONNECT_LED_PIN);
    return WIFI_OK; /*Connected to Internet*/
}
/************************************************************************************************************************
* Function Name: WiFi_Enable
* Function ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution result of this connection operation
* Description: Function to enable WiFi device
*              Note: This fuction relies on sleep duration for 10 seconds after enabling WiFi device,
*                    this duration is to allow the device to initialze and get ready for any connection.
************************************************************************************************************************/
WIFI_DEVICE_STATE WiFi_Enable(void)
{
    int exe_status = 0;
    char exe[50] = "nmcli radio wifi on";
    exe_status = system(exe);
    if(exe_status < 0)
    {
        return WIFI_DEVICE_ERROR;
    }
    sleep(10); //delay for 10 seconds for the device to initialze
    return WIFI_DEVICE_ON;
}
/************************************************************************************************************************
* Function Name: WiFi_Disable
* Function ID[hex]: 0x07
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution result of this connection operation
* Description: Function to disable WiFi device
************************************************************************************************************************/
WIFI_DEVICE_STATE WiFi_Disable(void)
{
    int exe_status = 0;
    char exe[50] = "nmcli radio wifi off";
    exe_status = system(exe);
    if(exe_status < 0)
    {
        return WIFI_DEVICE_ERROR;
    }
    return WIFI_DEVICE_ON;
}
/************************************************************************************************************************
* Function Name: WiFi_Check_Availability
* Function ID[hex]: 0x08
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution result of this connection operation
* Description: Function to check if WiFi device is enabled and available or not
************************************************************************************************************************/
WIFI_DEVICE_STATE WiFi_Check_Availability(void)
{
    int exe_status = 0, check_fd = 0;
    off_t ip_address_file_size = 0;
    ssize_t check_read = 0;
    char exe[50] = "nmcli radio wifi > ";
    strcat(exe,OPERATIONS_FILE_PATH);
    exe_status = system(exe);
    if(exe_status < 0)
    {
        return WIFI_DEVICE_ERROR;
    }
    check_fd = open(OPERATIONS_FILE_PATH,O_RDONLY|O_CREAT,0755);   /*open the file*/
    if(check_fd <= 0)
    {
        return WIFI_DEVICE_ERROR;
    }
    char* check_buffer = (char*)malloc((20*sizeof(char))+1); //only 20 characters are enough for such command
    check_read = read(check_fd,check_buffer,20);  /*read file*/
    if(check_read < 0)
    {
        free(check_buffer);
        close(check_fd);
        return WIFI_DEVICE_ERROR;
    }
    if(strstr(check_buffer,"disabled") != NULL)
    {
        free(check_buffer);
        close(check_fd);
        return WIFI_DEVICE_OFF; /*WiFi is disabled*/
    }
    free(check_buffer);
    close(check_fd);
    return WIFI_DEVICE_ON; /*Wifi is enabled*/
}
/************************************************************************************************************************
* Function Name: WiFi_Manager_Init
* Function ID[hex]: 0x09
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Initialization Result
* Description: Function to Initialize WiFi Manager Module and to start the networks scan thread
************************************************************************************************************************/
WIFI_STATE WiFi_Manager_Init(void)
{
    if(WiFi_Initialized == 1)
    {
        return WIFI_OK;
    }
    static pthread_attr_t Scan_Thread_Attributes; /*Thread attributes to configure*/
    // cpu_set_t CPU_Set;   /*Variable to set the dediacated core to run the thread*/
    struct sched_param Scheduling_Priority;
    pthread_attr_init(&Scan_Thread_Attributes);   /*init attributes*/
    pthread_attr_setinheritsched(&Scan_Thread_Attributes,PTHREAD_EXPLICIT_SCHED);
    // CPU_ZERO(&CPU_Set);
    // CPU_SET(2,&CPU_Set);    /*set to run on core id: 1*/
    // pthread_attr_setaffinity_np(&Scan_Thread_Attributes,sizeof(cpu_set_t),&CPU_Set);
    pthread_attr_setschedpolicy(&Scan_Thread_Attributes,SCHED_FIFO);
    Scheduling_Priority.sched_priority = 16;/*med priority for the scheduling policy meeting POSIX standard*/
    pthread_attr_setschedparam(&Scan_Thread_Attributes,&Scheduling_Priority);
    if(pthread_create(&Scan_Thread,&Scan_Thread_Attributes,&WiFi_Networks_Scan_Thread,NULL) != 0)
    {
        return WIFI_NOT_OK;
    }
    pthread_attr_destroy(&Scan_Thread_Attributes);
    WiFi_Initialized = 1;
    return WIFI_OK;
}
/************************************************************************************************************************
* Function Name: WiFi_Networks_Scan_Thread
* Function ID[hex]: 0x10
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: A thread started by initializing WiFi Manager
               This Thread is to continuously scan for nearby WiFi networks
               and store the scan results in a certain file.
************************************************************************************************************************/
static void *WiFi_Networks_Scan_Thread(void* v)
{
    char scan_exe[70] = {0};
    sprintf(scan_exe,"nmcli dev wifi list ifname %s > %s",\
    INTERFFACE_NAME,NETWORKS_FILE_PATH);
    while(1)
    {
        pthread_mutex_lock(&Scan_Mutex);   /*Lock mutex to start scaning*/
        system(scan_exe);  /*scan for existing access points*/
        pthread_mutex_unlock(&Scan_Mutex);   /*Unlock mutex*/
        sleep(1);   /*Rescan every 1 second*/
    }
}
