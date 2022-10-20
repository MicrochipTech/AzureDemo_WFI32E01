/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_pic32mz_w1.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_PIC32MZ_W1_Initialize" and "APP_PIC32MZ_W1_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_PIC32MZ_W1_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_PIC32MZ_W1_H
#define _APP_PIC32MZ_W1_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
#define APP_MOUNT_NAME          "/mnt/myDrive1"
#define APP_DEVICE_NAME         "/dev/mtda1"
#define APP_FS_TYPE             FAT
 
#define AZURE_DEVICE_CRT_FILE_NAME_FMT              "sn%s.der"
    
    
#define AZURE_CLOUD_CFG_FILE_NAME               "CLOUD.CFG"
#define AZURE_WIFI_CFG_FILE_NAME                "WIFI.CFG"    

#define AZURE_CLOUD_CFG_FILE                    APP_MOUNT_NAME"/"AZURE_CLOUD_CFG_FILE_NAME   
#define AZURE_WIFI_CFG_FILE                     APP_MOUNT_NAME"/"AZURE_WIFI_CFG_FILE_NAME   
    
    
#define AZURE_CLOUD_IDSCOPE_JSON_TAG        "ID_SCOPE"
#define AZURE_CLOUD_DEVICEID_JSON_TAG       "REGISTRATION_ID"
#define AZURE_CLOUD_PRIMARY_KEY_JSON_TAG    "PRIMARY_KEY" 

/* Config/Web files' contents */
#define APP_USB_MSD_WIFI_CONFIG_ID              "CMD:SEND_UART=wifi"
#define APP_USB_MSD_WIFI_CONFIG_DATA_TEMPLATE   APP_USB_MSD_WIFI_CONFIG_ID" %s,%s,%d"
#define APP_USB_MSD_WIFI_CONFIG_MIN_LEN         (strlen(APP_USB_MSD_WIFI_CONFIG_ID) \
                                                + 1 \
                                                + sizeof(WIFI_AUTH))

#define PEM_HEADER "-----BEGIN CERTIFICATE-----\n"
#define PEM_FOOTER "\n-----END CERTIFICATE-----\n"

// TODO: how many bytes is the max size?
#define CERT_MAX_SIZE 600
    
extern char default_id_scope[];
extern char default_registration_id[];
extern char default_primary_key[];
// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_PIC32MZ_W1_STATE_MOUNT_DISK=0,
    APP_PIC32MZ_W1_STATE_MSD_CONNECT,
    APP_PIC32MZ_W1_STATE_FORMAT_DISK,
    APP_PIC32MZ_W1_STATE_CHECK_CERT_FILES,            
    APP_PIC32MZ_W1_STATE_CHECK_AZURE_CFG_FILE,
    APP_PIC32MZ_W1_STATE_CHECK_WIFI_CFG_FILE,    
    APP_PIC32MZ_W1_STATE_READ_CERT_WIFI_CLOUD_CFG_FILE,    
    APP_PIC32MZ_W1_STATE_FLUSH_FILE,
    APP_PIC32MZ_W1_STATE_UNMOUNT_DISK,
    APP_PIC32MZ_W1_STATE_INIT,
    APP_PIC32MZ_W1_STATE_INIT_READY,
    APP_PIC32MZ_W1_STATE_CHECK_CREDENTIALS,
    APP_PIC32MZ_W1_STATE_CONFIG,
    APP_PIC32MZ_W1_STATE_RECONNECT,
    APP_PIC32MZ_W1_STATE_IDLE,
    APP_PIC32MZ_W1_STATE_ERROR,
    APP_PIC32MZ_W1_STATE_SERVICE_TASKS,
    /* TODO: Define states used by the application state machine. */

} APP_PIC32MZ_W1_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_PIC32MZ_W1_STATES appPic32mzW1State;
    USB_DEVICE_HANDLE usbDeviceHandle;     
    /* SYS_FS File handle */
    SYS_FS_HANDLE fileHandle;
    SYS_FS_FSTAT fileStatus;
    
    SYS_CONSOLE_HANDLE consoleHandle;
    DRV_HANDLE wdrvHandle;
    uint8_t ValidCrednetials;
    
    /* Serial number in hex string format; 9 bytes binary */
    char ecc608SerialNum[27];

    char ecc608DeviceCert[CERT_MAX_SIZE];
    size_t certSize;
    
} APP_PIC32MZ_W1_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_PIC32MZ_W1_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_PIC32MZ_W1_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_PIC32MZ_W1_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_PIC32MZ_W1_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_PIC32MZ_W1_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_PIC32MZ_W1_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_PIC32MZ_W1_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_PIC32MZ_W1_H */

/*******************************************************************************
 End of File
 */

