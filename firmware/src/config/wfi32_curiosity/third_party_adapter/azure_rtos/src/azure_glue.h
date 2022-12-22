/*******************************************************************************
  Azure IoT glue header File

  Summary: 
    Azure IoT glue file
    
  Description:
    This file provides the Azure IoT glue file for using
    Azure IoT with the Harmony framework
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

#ifndef _AZURE_IOT_GLUE_H_
#define _AZURE_IOT_GLUE_H_

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

// Azure IoT glue init operation result
typedef enum
{
    AZURE_INIT_RES_OK       = 0,    // all good
    // errors
    //
    AZURE_INIT_RES_NO_INIT_DATA,    // missing initialization data
    AZURE_INIT_RES_IF_NUMBER_ERROR, // too many interfaces
    AZURE_INIT_RES_BAD_INIT_PARAM,  // MAC initialization data error
    AZURE_INIT_RES_MAC_ADD_ERROR,   // MAC address error
    AZURE_INIT_RES_MAC_INIT_FAIL,   // MAC initialization failed
    AZURE_INIT_RES_MAC_OPEN_FAIL,   // MAC open failed
    AZURE_INIT_RES_MAC_EVENT_FAIL,  // MAC event notification failed
}AZURE_GLUE_INIT_RES;

// Azure IoT glue run operation result
typedef enum
{
    AZURE_GLUE_RES_OK       = 0,    // all good
    AZURE_GLUE_RES_PENDING,         // ongoing
    // errors
    //
    AZURE_GLUE_RES_UNINITIALIZED    = -1,   // driver not up and running
    AZURE_GLUE_RES_IF_IX_ERROR      = -2,   // bad interface index
    AZURE_GLUE_RES_NO_IF            = -3,   // no such interface
    AZURE_GLUE_RES_IF_NOT_READY     = -4,   // interface not up and running
    AZURE_GLUE_RES_PKT_POOL_EMPTY   = -5,   // could not allocate a packet because th epacket pool is empty
    AZURE_GLUE_RES_TX_ERROR         = -6,   // MAC driver rejected the packet
    AZURE_GLUE_RES_GAP_ERROR        = -7,   // nx packet too small to fit the MAC driver gap space
    AZURE_GLUE_RES_PARAM_EROR       = -8,   // bad parameter call

}AZURE_GLUE_RES;


// Azure glue configuration per interface    
typedef struct
{
    /* MAC address to use for this interface. */
    /*   Use "00:04:a3:00:00:00" or 0 for the factory preprogrammed address*/
    const char*     macAddr;


    /* Non-volatile pointer to the MAC driver object associated with this network interface */
    /* This is the MAC driver that this interface will use */
    /* Note: This object has to be valid for the whole life of the interface! */
    /*       The TCP/IP stack does not make a private copy of this object */
    const struct TCPIP_MAC_OBJECT_TYPE*    pMacObject;   

    /* Pointer to the MAC driver initialization data */
    /* Its type is MAC dependant */
    const void*     pMacInit;

    /* size of the initialization data (*pMacInit) */
    size_t          macInitSize;

    /* Interrupt vector used by the MAC
       This is used to disable/enable the MAC interrupts */
    int             macIrq;

    /* RX max frame size */
    uint16_t        macRxMaxFrame;

}AZURE_GLUE_NETWORK_CONFIG;


typedef struct
{
    /* number of networks in the configuration array */
    int                                 nNets;      
    /* pointer to array of network configurations */
    const AZURE_GLUE_NETWORK_CONFIG*    pNetConf;   
}AZURE_GLUE_INIT;




//*****************************************************************************
// Azure IoT initialization
AZURE_GLUE_INIT_RES    Azure_Glue_Initialize(const AZURE_GLUE_INIT* azureInit);

//*****************************************************************************
// Azure IoT glue task
void Azure_Glue_Tasks(void);

//*****************************************************************************
// Azure IoT glue status
// returns:
//      AZURE_GLUE_RES_OK       - if ready
//      AZURE_GLUE_RES_PENDING  - if still waiting for initialization to complete
//      < 0                     - if some error occurred
AZURE_GLUE_RES Azure_Glue_Status(void);


//*****************************************************************************
// Azure IoT function to TX a fully formatted NX_PACKET packet
AZURE_GLUE_RES Azure_Glue_PacketTx(void* netxPkt);

//*****************************************************************************
// Azure IoT function to get the MAC address of an interface
// returns a valid pointer if success
// otherwise 0
const uint8_t* Azure_Glue_MACAddress(int ifIx);

//*****************************************************************************
// Azure IoT function to get the MAC driver capabilities (IPv4 + UDP + TCP  RX/TX checksum offload) 
// returns an or-ed NX_ENABLE_INTERFACE_CAPABILITY mask:
//      NX_INTERFACE_CAPABILITY_IPV4_TX_CHECKSUM, NX_INTERFACE_CAPABILITY_IPV4_RX_CHECKSUM, etc. 
uint32_t Azure_Glue_IfCapability(int ifIx);
//*****************************************************************************
// mapping of the printf to the system console

size_t write(int fd, const void *buffer, size_t length);


//*********************************************************
// test and debug

// enable/disable statistics
#define AZURE_DEBUG_STATISTICS      0

void Azure_Glue_Counters(void);


// enable/disable MAC info
#define AZURE_DEBUG_MAC_INFO      0

void Azure_Mac_Info(void);

typedef struct
{
    /* name of the hardware register */
    char        registerName[36];           

    /*  value of the hardware register */
    uint32_t    registerValue;
}GMAC_REG_ENTRY;

bool GMAC_RegistersGet(GMAC_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);  

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _AZURE_IOT_GLUE_H_ */

