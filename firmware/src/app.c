/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


#if (AZURE_DEBUG_STATISTICS != 0)
static void _Command_GlueCounts(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif // (AZURE_DEBUG_STATISTICS != 0)

#if (AZURE_DEBUG_MAC_INFO != 0)
static void _Command_MacInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void _Command_MacRegs(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
#endif // (AZURE_DEBUG_MAC_INFO != 0)

static const SYS_CMD_DESCRIPTOR    appCmdTbl[]=
{
#if (AZURE_DEBUG_STATISTICS != 0)
    {"scount",  _Command_GlueCounts,   ": Az Glue Stats"},
#endif // (AZURE_DEBUG_STATISTICS != 0)
#if (AZURE_DEBUG_MAC_INFO != 0)
    {"macinfo", _Command_MacInfo,      ": Show MAC info"},
    {"macregs", _Command_MacRegs,      ": Show MAC Regs"},
#endif // (AZURE_DEBUG_MAC_INFO != 0)
};

#if (AZURE_DEBUG_MAC_INFO != 0)
static void _Command_MacInfo(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // macinfo
    //
    Azure_Mac_Info();
}

static void _Command_MacRegs(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // macregs
    //
    GMAC_REG_ENTRY  regEntries[50];
    GMAC_REG_ENTRY* pRegEntry;
    const char* msg = "GMAC Registers";
    int   jx, hwEntries;
    char  entryName[sizeof(pRegEntry->registerName) + 1];

    const void* cmdIoParam = pCmdIO->cmdIoParam;


    (*pCmdIO->pCmdApi->print)(cmdIoParam, "%s:\r\n", msg);
    if(GMAC_RegistersGet(regEntries, sizeof(regEntries)/sizeof(*regEntries), &hwEntries))
    {
        entryName[sizeof(entryName) - 1] = 0;
        for(jx = 0, pRegEntry = regEntries; jx < hwEntries && jx < sizeof(regEntries)/sizeof(*regEntries); jx++, pRegEntry++)
        {
            strncpy(entryName, pRegEntry->registerName, sizeof(entryName) - 1);
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\t %s: 0x%x\r\n", entryName, pRegEntry->registerValue);
        }
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\tnot supported\r\n");
    }
}
#endif // (AZURE_DEBUG_MAC_INFO != 0)

#if (AZURE_DEBUG_STATISTICS != 0)
static void _Command_GlueCounts(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    Azure_Glue_Counters();
}
#endif // (AZURE_DEBUG_STATISTICS != 0)

static bool APP_Commands_Init()
{
    if(sizeof(appCmdTbl)/sizeof(*appCmdTbl) != 0)
    {
        if (!SYS_CMD_ADDGRP(appCmdTbl, sizeof(appCmdTbl)/sizeof(*appCmdTbl), "app", ": app commands"))
        {
            SYS_ERROR(SYS_ERROR_ERROR, "Failed to create App Commands\r\n");
            return false;
        }

        SYS_CONSOLE_MESSAGE("Created App Commands\r\n");
    }

    return true;
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
    APP_Commands_Init();

}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
            nx_azure_init();
            
            appData.state = APP_TCPIP_WAITING_FOR_COMMAND;            

            break;

        case APP_TCPIP_WAITING_FOR_COMMAND:

            break;


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */

