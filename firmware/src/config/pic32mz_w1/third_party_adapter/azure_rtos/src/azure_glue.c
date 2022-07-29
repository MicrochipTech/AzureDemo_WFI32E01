/*******************************************************************************
  Azure IoT glue Source File

  Summary: 
    Azure IoT glue file
    
  Description:
    This file provides the glue file for using
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

#include <stdio.h>
#include <stddef.h>

#include <stdarg.h>
#include <sys/types.h>
#include <string.h>

#include "configuration.h"
/*added for the SYS_CONSOLE_PRINT used in write()*/
#include "definitions.h"

#include "azure_glue.h"
#include "azure_glue_private.h"

#include "tcpip/tcpip_mac.h"
#include "tcpip/tcpip_mac_object.h"

#include "system/debug/sys_debug.h"
#include "system/int/sys_int.h"
#include "osal/osal.h"

#include "tx_api.h"
#include "nx_api.h"

// definitions
//

#define AZURE_GLUE_HDR_MESSAGE "Azure IoT Glue: "
#define AZURE_MAC_INIT_DATA_SIZE    60  // storing the MAC initialization data

#define AZURE_MAC_ALL_EVENTS          (TCPIP_MAC_EV_RX_DONE | TCPIP_MAC_EV_TX_DONE | TCPIP_MAC_EV_RXTX_ERRORS)
// MAC events used to detect that there are active RX events that need processing
// always subset of AZURE_MAC_ALL_EVENTS
#define AZURE_MAC_ACTIVE_RX_EVENTS    (TCPIP_MAC_EV_RX_DONE | TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA)

#define AZURE_MAC_TX_SYNCHRONIZE_FUNC   1   // enable the MAC driver TX synchronization function
                                            // provide a function to be called by the MAC driver

// Segment payload gap:
// sizeof the TCPIP_MAC_SEGMENT_PAYLOAD::segmentDataGap
#if defined(TCPIP_IF_PIC32WK) || defined(TCPIP_IF_PIC32MZW1) 
    #define TCPIP_MAC_DATA_SEGMENT_GAP      34   
#else
    #define TCPIP_MAC_DATA_SEGMENT_GAP      4   
#endif


// NB: this should be taken care in the nx_user.h::NX_PHYSICAL_HEADER !
// should be uintptr_t aligned, for storing the segmentPktPtr
#define _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE  ((((sizeof(TCPIP_MAC_SEGMENT_GAP_DCPT) + TCPIP_MAC_DATA_SEGMENT_GAP) + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)) * sizeof(uintptr_t))

// for TX/RX we place the segment gap in front of the packet:
#define _TCPIP_MAC_GAP_OFFSET    (int16_t)(-_TCPIP_MAC_DATA_SEGMENT_GAP_SIZE)    // offset from the segment buffer, both TX and RX

#define _AZURE_MAC_ACTION_INIT     0    // initialization value for MAC

#define _AZURE_GLUE_SLEEP_MS        1   // time to sleep when waiting for the initialization to complete

#define _AZURE_MAX_INTERFACES       32   // we have one event of a group per interface

#define _AZURE_DEBUG_MASK_BASIC         0x01
#define _AZURE_DEBUG_MASK_STATE         0x02
#define _AZURE_DEBUG_MASK_TX_ALLOC      0x04
#define _AZURE_DEBUG_MASK_RX_ALLOC      0x08
#define _AZURE_DEBUG_MASK_GAP_SIZE      0x10


// enable DHCP debugging levels
#define _AZURE_DEBUG_LEVEL  (0)

typedef enum
{
    AZURE_GLUE_STAT_IDLE    = 0,
    AZURE_GLUE_STAT_INIT,       // performing initialization
    AZURE_GLUE_WAIT_MAC_READY,  // wait MAC initialization complete
    AZURE_GLUE_STAT_RUN,        // running


}AZURE_GLUE_STATE;

static const char* _AZURE_INIT_RES_MESSAGE[] = 
{
    "Initialization Success",       // AZURE_INIT_RES_OK
    "Az Glue no Init data",         // AZURE_INIT_RES_NO_INIT_DATA
    "Az Glue too many interfaces",  // AZURE_INIT_RES_IF_NUMBER_ERROR
    "MAC Init data error",          // AZURE_INIT_RES_BAD_INIT_PARAM
    "MAC Init MAC address error",   // AZURE_INIT_RES_MAC_ADD_ERROR
    "MAC Initialization failed",    // AZURE_INIT_RES_MAC_INIT_FAIL
    "MAC Open failed",              // AZURE_INIT_RES_MAC_OPEN_FAIL
    "MAC event init failed",        // AZURE_INIT_RES_MAC_EVENT_FAIL
};

typedef enum
{
    AZURE_MAC_FLAG_NONE             = 0x0000,      // none, not initialized, etc
    AZURE_MAC_FLAG_INIT_PENDING     = 0x0001,   // initialization pending
    AZURE_MAC_FLAG_INIT_DONE        = 0x0002,   // initialized successfuly
                                              // if ! pending and !done, then failed!

    AZURE_MAC_FLAG_EVENT_PROCESS    = 0x0010,   // process needs to be called per event
}AZURE_MAC_FLAGS;

typedef struct
{
    // initialization
    TCPIP_MAC_ADDR      netMACAddr;             // MAC address
    int                 macIrqN;                // IRQ number
    
    const struct TCPIP_MAC_OBJECT_TYPE*    pMacObj;
    uint32_t macInitData[AZURE_MAC_INIT_DATA_SIZE / sizeof(uint32_t)]; // room for MAC initialization data
                                                                    // because MAC Init is delayed until the task is started
    // run time                                                
    struct _tag_AZURE_GLUE_DCPT* pParent;   // the GDcpt it belongs to 
    SYS_MODULE_OBJ      macObjHandle;   // MAC system wide handle
    TCPIP_MAC_HANDLE    hIfMac;         // quick reference to which MAC this interface belongs to 
    AZ_SINGLE_LIST      rxList;         // list of incoming packets
    uint16_t            flags;          // AZURE_MAC_FLAGS value; run time flags
    uint16_t            linkMtu;        // link MTU reported by the mac
    uint16_t            activeEvents;   // TCPIP_MAC_EVENT: current events
    uint16_t            eventCount;     // event counter
    // debug, stats
    uint32_t            maxRxProc;      // max packets processed per turn
    uint32_t            totEventCount;  // global event counter
    uint32_t            totRxEventCount;  // global RX event counter
    uint32_t            totErrEventCount; // global error event counter
    uint32_t            errorEvents;    // error event aggregation
    uint32_t            procRxPkts;     // number of processed RX packets
    uint32_t            chainedPkts;    // number of chained RX packets
    uint32_t            droppedPkts;    // number of chained RX packets
    uint32_t            txPkts;         // number of TX OK packets
    uint32_t            txRejected;     // number of rejected TX packets
    uint32_t            txAckPkts;      // number of ack-ed TX packets
    uint32_t            txBadAcks;      // number of bad ack-ed TX packets
    uint32_t            netxAckFail;    // number of failed NetX release packets
    uint32_t            txOrphans;      // orphan TX packets
}AZURE_MAC_DCPT;

// use an ordinary array to store RX/TX descriptors
#define AZURE_GLUE_PKT_SIZE         (((sizeof(TCPIP_MAC_PACKET) + 3) >> 2) << 2)  // packet size, 32 bits round up
#define AZURE_GLUE_PKT_ITEM_SIZE    (AZURE_GLUE_PKT_SIZE + sizeof(TCPIP_MAC_DATA_SEGMENT))  // complete item size
#define AZURE_GLUE_PKT_RX_ARRAY_ITEMS       NX_DEMO_NUMBER_OF_PACKETS  // (TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0 + 32)
#define AZURE_GLUE_PKT_TX_ARRAY_ITEMS       NX_DEMO_NUMBER_OF_PACKETS // (TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0 + 32)
static uint32_t azure_glue_rx_pkt_array[(AZURE_GLUE_PKT_RX_ARRAY_ITEMS * AZURE_GLUE_PKT_ITEM_SIZE) / sizeof(uint32_t)];
static uint32_t azure_glue_tx_pkt_array[(AZURE_GLUE_PKT_TX_ARRAY_ITEMS * AZURE_GLUE_PKT_ITEM_SIZE) / sizeof(uint32_t)];


// initializatin flags: keeping track of initialization outcome
typedef enum
{
    AZURE_GLUE_INI_FLAG_NONE            = 0x00,
    AZURE_GLUE_INI_FLAG_POOL_CREATED    = 0x01,

    // ...

}AZURE_GLUE_INI_FLAGS;

// run time flags
typedef enum
{
    AZURE_GLUE_ACTION_NONE            = 0x00,     // no action taken
    AZURE_GLUE_ACTION_SLEEP           = 0x01,     // sleep needed after this function

    // ...

}AZURE_GLUE_ACTIONS;


typedef struct _tag_AZURE_GLUE_DCPT
{
    uint16_t            currState;          // AZURE_GLUE_STATE: current task state
    uint16_t            nNets;              // number of interfaces in this run
    TX_BYTE_POOL*       allocH;             // pool handle for general allocation stuff
    AZ_SINGLE_LIST      rxPktList;          // list of TCPIP_MAC_PACKET packets for RX
    AZ_SINGLE_LIST      txPktList;          // list of TCPIP_MAC_PACKET packets for TX
    uint32_t            rxAllocBuffs;       // # of global RX allocated netx buffers; no info per interface!
    uint32_t            rxReleaseBuffs;     // # of global RX released netx buffers; no info per interface!
    uint32_t            rxReleaseError;     // # of global errors for RX released netx buffers
    uint32_t            txAllocPkts;        // # of global TX allocated MAC packets
    uint32_t            txReleasePkts;      // # of global TX released MAC packets
    uint32_t            rxAllocPkts;        // # of global RX allocated MAC packets
    uint32_t            rxReleasePkts;      // # of global RX released MAC packets
    uint32_t            rxAllocPktError;    // # of pkt allocation errors
    uint32_t            txAllocPktError;    // # of pkt allocation errors
    uint32_t            rxAllocNetxError;   // # of netx buffers allocation errors
    uint32_t            gapError;           // # of pkt gap errors
    // other, less important counters
    uint32_t            rxLenError;         // # of global RX length errors; no info per interface!
    uint32_t            netxLenError;       // # of netx buffers length errors
    uint32_t            netxTxIfError;      // # of netx TX requests for wrong interface index (?)

    AZURE_MAC_DCPT      macDcpt[AZURE_NET_INTERFACES];  // descriptors per interface
    uint8_t             iniFlags;           // AZURE_GLUE_INI_FLAGS value 
    uint8_t             runAction;          // AZURE_GLUE_ACTIONS value 
    uint16_t            maxRxFrame;         // maximum RX frame across all MACs/interfaces
}AZURE_GLUE_DCPT;

static AZURE_GLUE_DCPT azure_glue_dcpt;     // general descriptor
static AZURE_GLUE_DCPT* gAzureDcpt = 0;     // general descriptor

// state function
typedef void (*AZURE_STATE_FUNC)(AZURE_GLUE_DCPT* pGDcpt);

static void _Azure_Idle_Fnc(AZURE_GLUE_DCPT* pGDcpt);
static void _Azure_Init_Fnc(AZURE_GLUE_DCPT* pGDcpt);
static void _Azure_Wait_Fnc(AZURE_GLUE_DCPT* pGDcpt);
static void _Azure_Run_Fnc(AZURE_GLUE_DCPT* pGDcpt);

static const AZURE_STATE_FUNC   azure_state_func_tbl[] = 
{
    _Azure_Idle_Fnc,        // AZURE_GLUE_STAT_IDLE
    _Azure_Init_Fnc,        // AZURE_GLUE_STAT_INIT
    _Azure_Wait_Fnc,        // AZURE_GLUE_WAIT_MAC_READY
    _Azure_Run_Fnc,         // AZURE_GLUE_STAT_RUN
};

// pool of TCPIP_MAC_PACKET to be used for both RX and TX
// note that these packets are floating, they have a segment pointer
// but the segment does not have any segBuffer or segLoad set!
//      For RX, the segBuffer and segLoad are set when RX buffers allocated from netxd
//      For TX, the segLoad and segLoad will be the tx buffer passed by the netxd


// local prototypes
static AZURE_GLUE_INIT_RES _Azure_CreateResources(AZURE_GLUE_DCPT* pGDcpt);

static void     _Azure_CreatePktLists(AZURE_GLUE_DCPT* pGDcpt);
static void     _Azure_InitPktList(AZ_SINGLE_LIST *pList, uint32_t* pItemArray, size_t arrayItems, size_t itemSize);

static TCPIP_MAC_PACKET* _Azure_AllocatePkt(AZURE_GLUE_DCPT* pGDcpt, bool isTx);
static void     _Azure_ReleasePkt(AZURE_GLUE_DCPT* pGDcpt, TCPIP_MAC_PACKET* pPkt, bool isTx);

static void     _Azure_Cleanup(AZURE_GLUE_DCPT* pGDcpt);

static void     _Azure_SetMacCtrl(TCPIP_MAC_MODULE_CTRL* pMacCtrl, AZURE_GLUE_DCPT* pGDcpt, int netIx);

static AZURE_GLUE_INIT_RES   _Azure_Mac_Init(AZURE_GLUE_DCPT* pGDcpt, int netIx);
static void      _Azure_Mac_Kill(AZURE_MAC_DCPT* pMDcpt);
static void      _Azure_Mac_CheckReady(AZURE_MAC_DCPT* pMDcpt);

static void*    _Azure_Glue_Malloc(TCPIP_MAC_HEAP_HANDLE heapH, size_t bytes);
static void*    _Azure_Glue_Calloc(TCPIP_MAC_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);
static size_t   _Azure_Glue_Free(TCPIP_MAC_HEAP_HANDLE heapH, const void* ptr);

static TCPIP_MAC_PACKET* _Azure_Glue_MacRxPkt_Alloc(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags);
static void     _Azure_Glue_MacRxPkt_Free(TCPIP_MAC_PACKET* pPkt);
static void     _Azure_Glue_MacTxPkt_Ack(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId);

static void     _Azure_Glue_MacEventCB(TCPIP_MAC_EVENT event, const void* hParam);

static bool     _Azure_NetxTx_AckFunc(TCPIP_MAC_PACKET* pkt, const void* param);

#if (AZURE_MAC_TX_SYNCHRONIZE_FUNC != 0)
static bool     _Azure_Glue_SyncFunction(void* synchHandle, TCPIP_MAC_SYNCH_REQUEST req);

static bool     _Azure_Glue_SyncNone(void* synchHandle);

static bool     _Azure_Glue_SyncCritEnter(void* synchHandle);
static bool     _Azure_Glue_SyncCritLeave(void* synchHandle);

typedef bool (*_Azure_SyncF)(void* synchHandle);

static _Azure_SyncF _Azure_SyncF_Tbl[] = 
{
    _Azure_Glue_SyncNone,       // TCPIP_MAC_SYNCH_REQUEST_NONE       
    _Azure_Glue_SyncNone,       // TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE 
    _Azure_Glue_SyncNone,       // TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE 
    _Azure_Glue_SyncNone,       // TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK   
    _Azure_Glue_SyncNone,       // TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK 

    _Azure_Glue_SyncCritEnter,  // TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER     
    _Azure_Glue_SyncCritLeave,  // TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE     
};

#endif  // (AZURE_MAC_TX_SYNCHRONIZE_FUNC != 0)

static void     _Azure_Extract_MacRxPackets(AZURE_MAC_DCPT* pMDcpt);
static void     _Azure_Process_MacRxPackets(AZURE_MAC_DCPT* pMDcpt);

// basic level  debugging
#if ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_BASIC) != 0)
volatile int _AzureStayAssertLoop = 1;
static void _AzureAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("Azure Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_AzureStayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
volatile int _AzureStayCondLoop = 1;
static void _AzureDebugCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("Azure Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_AzureStayCondLoop != 0);
    }
}

#else
#define _AzureAssertCond(cond, message, lineNo)
#define _AzureDebugCond(cond, message, lineNo)
#endif  // ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_BASIC) != 0)


#if ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_STATE) != 0)
static AZURE_GLUE_STATE _azPrevState = -1;

static const char* _az_state_tbl[] = 
{
    "Idle",     // AZURE_GLUE_STAT_IDLE
    "Init",     // AZURE_GLUE_STAT_INIT
    "Wait MAC", // AZURE_GLUE_WAIT_MAC_READY
    "Run",      // AZURE_GLUE_STAT_RUN
};

void _AzureStatePrint(AZURE_GLUE_STATE newState)
{
    if(newState != _azPrevState)
    {
        _azPrevState = newState;
        SYS_CONSOLE_PRINT(AZURE_GLUE_HDR_MESSAGE "State: %s\r\n", _az_state_tbl[newState]);
    }
}
#else
#define _AzureStatePrint(newState)
#endif  // ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_STATE) != 0)

// implementation
AZURE_GLUE_INIT_RES Azure_Glue_Initialize(const AZURE_GLUE_INIT* azureInit)
{

    int netIx;
    AZURE_GLUE_DCPT* pGDcpt = &azure_glue_dcpt;
    memset(pGDcpt, 0, sizeof(*pGDcpt));
    gAzureDcpt = 0;

    AZURE_GLUE_INIT_RES initRes = AZURE_INIT_RES_OK;
    while(true)
    {
        if(azureInit == 0)
        {
            initRes = AZURE_INIT_RES_NO_INIT_DATA;
            break;
        }
        else if(azureInit->nNets > AZURE_NET_INTERFACES || azureInit->nNets > _AZURE_MAX_INTERFACES)
        {
            initRes = AZURE_INIT_RES_IF_NUMBER_ERROR;
            break;
        }


        AZURE_MAC_DCPT* pMDcpt = pGDcpt->macDcpt;
        const AZURE_GLUE_NETWORK_CONFIG* pNetConf = azureInit->pNetConf;
        for(netIx = 0; netIx < azureInit->nNets; netIx++, pMDcpt++, pNetConf++)
        {
            if(pNetConf->macInitSize > sizeof(pMDcpt->macInitData))
            {
                initRes = AZURE_INIT_RES_BAD_INIT_PARAM;
                break;
            }

            if(!_Azure_StringToMACAddress(pNetConf->macAddr, pMDcpt->netMACAddr.v))
            {
                initRes = AZURE_INIT_RES_MAC_ADD_ERROR;
                break;
            }

            // copy the configuration data
            pMDcpt->pMacObj = pNetConf->pMacObject;
            pMDcpt->macIrqN = pNetConf->macIrq;
            memcpy(pMDcpt->macInitData, pNetConf->pMacInit, pNetConf->macInitSize);
            if(pNetConf->macRxMaxFrame > pGDcpt->maxRxFrame)
            {
                pGDcpt->maxRxFrame = pNetConf->macRxMaxFrame;
            }
            // set the parent
            pMDcpt->pParent = pGDcpt;
        }

        if(initRes == AZURE_INIT_RES_OK)
        {
            initRes = _Azure_CreateResources(pGDcpt);
        }

        if(initRes == AZURE_INIT_RES_OK)
        {   // all OK 
            pGDcpt->nNets = azureInit->nNets;
            gAzureDcpt = pGDcpt;
        }

        break;
    }

    if(initRes != AZURE_INIT_RES_OK)
    {
        _Azure_Cleanup(pGDcpt);
    }

    return initRes;
}

        
// init one MAC
static AZURE_GLUE_INIT_RES _Azure_Mac_Init(AZURE_GLUE_DCPT* pGDcpt, int netIx)
{
    TCPIP_MAC_MODULE_CTRL   macCtrl;
    TCPIP_MAC_INIT          macInit;

        
    AZURE_MAC_DCPT* pMDcpt = pGDcpt->macDcpt + netIx;

    const TCPIP_MAC_OBJECT* pMacObj = pMDcpt->pMacObj;
    const void* configData = pMDcpt->macInitData;

    AZURE_GLUE_INIT_RES initRes = AZURE_INIT_RES_OK;

    while(true)
    {
        // init the MAC
        memset(&macInit, 0, sizeof(macInit));

        _Azure_SetMacCtrl(&macCtrl, pGDcpt, netIx);
        TCPIP_MAC_INIT macInit =
        {
            { 0 }, // SYS_MODULE_INIT not currently used
            &macCtrl,
            configData,
        };

        pMDcpt->macObjHandle = (*pMacObj->TCPIP_MAC_Initialize)(pMacObj->macId, &macInit.moduleInit);

        if( pMDcpt->macObjHandle == SYS_MODULE_OBJ_INVALID)
        {
            pMDcpt->macObjHandle = 0;
            initRes = AZURE_INIT_RES_MAC_INIT_FAIL;
            break;
        }

        // open the MAC
        pMDcpt->hIfMac = (*pMacObj->TCPIP_MAC_Open)(pMacObj->macId, DRV_IO_INTENT_READWRITE);
        if(pMDcpt->hIfMac == DRV_HANDLE_INVALID)
        {
            pMDcpt->hIfMac = 0;
            initRes = AZURE_INIT_RES_MAC_OPEN_FAIL;
            break;
        }


        if(!(*pMDcpt->pMacObj->TCPIP_MAC_EventMaskSet)(pMDcpt->hIfMac, AZURE_MAC_ALL_EVENTS, true))
        {
            initRes = AZURE_INIT_RES_MAC_EVENT_FAIL;
            break;
        }

        // completed the MAC initialization
        pMDcpt->flags |= AZURE_MAC_FLAG_INIT_PENDING; 
        break;
    }

    // display the initialization message
    SYS_ERROR_PRINT(SYS_ERROR_INFO, AZURE_GLUE_HDR_MESSAGE "%s: %s\r\n", _AZURE_INIT_RES_MESSAGE[initRes], pMacObj->macName);

    if(initRes != AZURE_INIT_RES_OK)
    {   // init failed
        _Azure_Mac_Kill(pMDcpt);
    }


    return initRes;
}

// checks the MAC initialization complete and updates the descriptor flags
static void _Azure_Mac_CheckReady(AZURE_MAC_DCPT* pMDcpt)
{
    const TCPIP_MAC_OBJECT* pMacObj = pMDcpt->pMacObj;
    if(pMacObj == 0 || (pMDcpt->flags & AZURE_MAC_FLAG_INIT_PENDING) == 0)
    {   // nothing here; killed or done
        return;
    }

    SYS_MODULE_OBJ objHandle = pMDcpt->macObjHandle;
    // advance the driver state machine
    pMacObj->TCPIP_MAC_Tasks(objHandle);

    SYS_STATUS macStat = pMacObj->TCPIP_MAC_Status(objHandle);
    if(macStat < 0)
    {   // failed; kill the interface
        pMDcpt->flags &= ~AZURE_MAC_FLAG_INIT_PENDING;
    }
    else if(macStat == SYS_STATUS_READY)
    {   // get the MAC address and MAC processing flags
        // set the default MTU; MAC driver will override if needed
        TCPIP_MAC_PARAMETERS macParams;
        macParams.linkMtu = TCPIP_MAC_LINK_MTU_DEFAULT; 
        TCPIP_MAC_HANDLE hIfMac = pMDcpt->hIfMac;
        pMacObj->TCPIP_MAC_ParametersGet(hIfMac, &macParams);
        memcpy(pMDcpt->netMACAddr.v, macParams.ifPhyAddress.v, sizeof(pMDcpt->netMACAddr));
        if(macParams.processFlags != TCPIP_MAC_PROCESS_FLAG_NONE)
        {
            pMDcpt->flags |= AZURE_MAC_FLAG_EVENT_PROCESS;
        }
        pMDcpt->linkMtu = macParams.linkMtu;
        // enable this interface

        pMDcpt->flags &= ~AZURE_MAC_FLAG_INIT_PENDING;
        pMDcpt->flags |= AZURE_MAC_FLAG_INIT_DONE;
    }
    // else leave pending
}

static void _Azure_Mac_Kill(AZURE_MAC_DCPT* pMDcpt)
{
    if(pMDcpt->pMacObj != 0)
    {

        if(pMDcpt->hIfMac != 0)
        {
            (*pMDcpt->pMacObj->TCPIP_MAC_Close)(pMDcpt->hIfMac);
            pMDcpt->hIfMac = 0;
        }

        if(pMDcpt->macObjHandle != 0)
        {
            (*pMDcpt->pMacObj->TCPIP_MAC_Deinitialize)(pMDcpt->macObjHandle);
            pMDcpt->macObjHandle = 0;
        }
        pMDcpt->pMacObj = 0;    // gone!
    }
}

static void _Azure_SetMacCtrl(TCPIP_MAC_MODULE_CTRL* pMacCtrl, AZURE_GLUE_DCPT* pGDcpt, int netIx)
{
    AZURE_MAC_DCPT* pMDcpt = pGDcpt->macDcpt + netIx;

    pMacCtrl->nIfs = pGDcpt->nNets;

    pMacCtrl->mallocF = _Azure_Glue_Malloc;
    pMacCtrl->callocF = _Azure_Glue_Calloc;
    pMacCtrl->freeF = _Azure_Glue_Free;
    
    pMacCtrl->memH = pGDcpt->allocH;


    pMacCtrl->pktAllocF = _Azure_Glue_MacRxPkt_Alloc;
    pMacCtrl->pktFreeF = _Azure_Glue_MacRxPkt_Free;
    pMacCtrl->pktAckF = _Azure_Glue_MacTxPkt_Ack;

#if (AZURE_MAC_TX_SYNCHRONIZE_FUNC != 0)
    pMacCtrl->synchF = _Azure_Glue_SyncFunction;
#else
    pMacCtrl->synchF = 0;
#endif  // (AZURE_MAC_TX_SYNCHRONIZE_FUNC != 0)

    // Stack can use one handler for all network interfaces, like in this case
    // Each time a notification is received, all interfaces are checked
    // Or, more efficient, use a handler per interface
    pMacCtrl->eventF = _Azure_Glue_MacEventCB;
    pMacCtrl->eventParam = pMDcpt;

    pMacCtrl->controlFlags = TCPIP_MAC_CONTROL_PAYLOAD_OFFSET_2 | TCPIP_MAC_CONTROL_NO_SMART_ALLOC | TCPIP_MAC_CONTROL_NO_LINK_CHECK;
    pMacCtrl->netIx = netIx;
    pMacCtrl->macAction = _AZURE_MAC_ACTION_INIT;
    pMacCtrl->powerMode = TCPIP_MAC_POWER_FULL;
    pMacCtrl->gapDcptSize = _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
    pMacCtrl->gapDcptOffset = _TCPIP_MAC_GAP_OFFSET;   // gap before the segmentBuffer!
    pMacCtrl->retrieveF = 0;

    memcpy(pMacCtrl->ifPhyAddress.v, pMDcpt->netMACAddr.v, sizeof(pMacCtrl->ifPhyAddress));
}

// stdio printf intercept
size_t write(int fd, const void *buffer, size_t length)
{
    if(fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        return UART3_Write((char *)buffer, length);
    }
    return -1;
}

void Azure_Glue_Tasks(void)
{
    AZURE_GLUE_DCPT* pGDcpt = gAzureDcpt;

    if(pGDcpt == 0)
    {   // uninitialized?
        return;
    }

    _AzureStatePrint(pGDcpt->currState);

    if(pGDcpt->currState < sizeof(azure_state_func_tbl) / sizeof(*azure_state_func_tbl))
    {
       azure_state_func_tbl[pGDcpt->currState](pGDcpt);
    }
    else
    {   // should NOT happen
        _AzureAssertCond(false, __func__, __LINE__);
    }

    if((pGDcpt->runAction & AZURE_GLUE_ACTION_SLEEP) != 0)
    {
        tx_thread_sleep((_AZURE_GLUE_SLEEP_MS / (TX_TICK_PERIOD_MS)) < 1 ? 1 : (_AZURE_GLUE_SLEEP_MS / (TX_TICK_PERIOD_MS)));
    }

    // clear the action for next round
    pGDcpt->runAction = AZURE_GLUE_ACTION_NONE;
}

static void _Azure_Idle_Fnc(AZURE_GLUE_DCPT* pGDcpt)
{
    // just starting the initialization
    // 1st time we run
    pGDcpt->currState = AZURE_GLUE_STAT_INIT;
    pGDcpt->runAction = AZURE_GLUE_ACTION_SLEEP;
}

static void _Azure_Init_Fnc(AZURE_GLUE_DCPT* pGDcpt)
{
    int netIx;

    // perform initialization
    for(netIx = 0; netIx < pGDcpt->nNets; netIx++)
    {
        _Azure_Mac_Init(pGDcpt, netIx);
    }

    pGDcpt->currState = AZURE_GLUE_WAIT_MAC_READY;
    pGDcpt->runAction = AZURE_GLUE_ACTION_SLEEP;
}

static void _Azure_Wait_Fnc(AZURE_GLUE_DCPT* pGDcpt)
{
    // wait for MAC initialization to complete
    int netIx, checkDoneCnt;
    AZURE_MAC_DCPT* pMDcpt;

    checkDoneCnt = 0;
    pMDcpt = pGDcpt->macDcpt;
    for(netIx = 0; netIx < pGDcpt->nNets; netIx++, pMDcpt++)
    {
        _Azure_Mac_CheckReady(pMDcpt);
        // check the outcome
        if((pMDcpt->flags & AZURE_MAC_FLAG_INIT_PENDING) == 0)
        {
            checkDoneCnt++;
        }
    }

    if(checkDoneCnt == pGDcpt->nNets)
    {   // all done
        pMDcpt = pGDcpt->macDcpt;
        for(netIx = 0; netIx < pGDcpt->nNets; netIx++, pMDcpt++)
        {
            // remove dead bodies
            if((pMDcpt->flags & AZURE_MAC_FLAG_INIT_DONE) == 0)
            {
                _Azure_Mac_Kill(pMDcpt);
            }
        }
        // next state
        pGDcpt->currState = AZURE_GLUE_STAT_RUN;
    }

    pGDcpt->runAction = AZURE_GLUE_ACTION_SLEEP;
}

static void _Azure_Run_Fnc(AZURE_GLUE_DCPT* pGDcpt)
{
    // run state
    int netIx;
    TCPIP_MAC_EVENT activeEvents;    
    TCPIP_MAC_HANDLE hIfMac;
    const struct TCPIP_MAC_OBJECT_TYPE* pMacObj;

    AZURE_MAC_DCPT* pMDcpt = pGDcpt->macDcpt;
    for(netIx = 0; netIx < pGDcpt->nNets; netIx++, pMDcpt++)
    {
        if((pMacObj = pMDcpt->pMacObj) == 0)
        {   // no MAC
            continue;
        }

        // advance the driver state machine
        pMacObj->TCPIP_MAC_Tasks(pMDcpt->macObjHandle);

        if(pMDcpt->eventCount == 0)
        {   // no MAC events
            continue;
        }
        // there are MAC events pending
        pMDcpt->eventCount = 0;
        activeEvents = pMDcpt->activeEvents;

        // get a fresh copy
        hIfMac = pMDcpt->hIfMac;
        activeEvents |= pMacObj->TCPIP_MAC_EventPendingGet(hIfMac);

        // clear processed events
        bool intStat = SYS_INT_SourceDisable(pMDcpt->macIrqN);
        pMDcpt->activeEvents &= ~activeEvents;
        SYS_INT_SourceRestore(pMDcpt->macIrqN, intStat);

        // acknowledge MAC events
        pMacObj->TCPIP_MAC_EventAcknowledge(hIfMac, activeEvents);

        if((activeEvents & (AZURE_MAC_ACTIVE_RX_EVENTS)) != 0)
        {
            _Azure_Extract_MacRxPackets(pMDcpt);
            pMDcpt->totRxEventCount++;
        }

        if((activeEvents & TCPIP_MAC_EV_RXTX_ERRORS) != 0)
        {    // some error has occurred
            pMDcpt->errorEvents |= (activeEvents & TCPIP_MAC_EV_RXTX_ERRORS);
            pMDcpt->totErrEventCount++;
        }

        if((pMDcpt->flags & AZURE_MAC_FLAG_EVENT_PROCESS) != 0)
        {   // normal MAC internal processing
            pMacObj->TCPIP_MAC_Process(hIfMac);
        }

        // process the pending packets
        _Azure_Process_MacRxPackets(pMDcpt);
    }


    // stay in AZURE_GLUE_STAT_RUN state
    pGDcpt->runAction = AZURE_GLUE_ACTION_NONE;
}

static void _Azure_Extract_MacRxPackets(AZURE_MAC_DCPT* pMDcpt)
{
    TCPIP_MAC_PACKET* pRxPkt;

    AZ_SINGLE_LIST* pRxList = &pMDcpt->rxList;
    int nPkts = 0;
    // get all the new MAC packets
    // NB: for now only this thread accesses the rxList, so no protection should be necessary!
    while((pRxPkt = pMDcpt->pMacObj->TCPIP_MAC_PacketRx(pMDcpt->hIfMac, 0, 0)) != 0)
    {
        _Azure_SingleListTailAdd(pRxList, (AZ_SGL_LIST_NODE*)pRxPkt);
        nPkts++;
    }

    if(nPkts > pMDcpt->maxRxProc)
    {
        pMDcpt->maxRxProc = nPkts;
    }
}

static inline uint16_t __attribute__((always_inline)) Azure_ntohs(uint16_t hShort)
{
       return (((hShort) << 8) | ((hShort) >> 8));
}


// NB: external function in nx_driver_harmony.c to pass packets to netxd
extern void nx_driver_receive(NX_PACKET* nxp);
static void _Azure_Process_MacRxPackets(AZURE_MAC_DCPT* pMDcpt)
{
    TCPIP_MAC_PACKET* pRxPkt;
    TCPIP_MAC_DATA_SEGMENT* pSeg;

    AZ_SINGLE_LIST* pRxList = &pMDcpt->rxList;
    int nProcPkts = 0;
    int nDroppedPkts = 0;
    // get all the pending MAC packets
    // NB: for now only this thread accesses the rxList, so no protection should be necessary!
    while((pRxPkt = (TCPIP_MAC_PACKET*)_Azure_SingleListHeadRemove(pRxList)) != 0)
    {
        // process the packet and pass the buffers to netxd
        bool dropPkt = false;
        if(pRxPkt->pDSeg->next != 0)
        {
            pMDcpt->chainedPkts++;
#if defined(NX_DISABLE_PACKET_CHAIN)
            // no support for chained packets
            dropPkt = true;
#endif  // defined(NX_DISABLE_PACKET_CHAIN)
        }

        if(dropPkt)
        {
            nDroppedPkts++;
        }
        else
        {
            // restore master packet owner
            NX_PACKET* master_nxp, *nxp;
#if !defined(NX_DISABLE_PACKET_CHAIN)
            NX_PACKET* prev_nxp = 0;
#endif  // !defined(NX_DISABLE_PACKET_CHAIN)


            int segIx;
            uint16_t totSegLen = 0;
            master_nxp = 0;
            for(pSeg = pRxPkt->pDSeg, segIx = 0; pSeg != 0; pSeg = pSeg->next, segIx++)
            {
                uint32_t segLoad = (uint32_t)pSeg->segLoad;
                uint32_t segBuffer = (uint32_t)pSeg->segBuffer;
                uint16_t segLen = pSeg->segLen;

                if(segIx == 0)
                {
                    segLen += sizeof(TCPIP_MAC_ETHERNET_HEADER);
                    // restore packet owner
                    nxp = (NX_PACKET*)pRxPkt->pTransportLayer;
                    _AzureAssertCond(nxp != 0, __func__, __LINE__); 
                    pRxPkt->pTransportLayer = 0;    // show there's no more associated nx packet
                    master_nxp = nxp;

                    _AzureAssertCond((segLoad & 0x3) == 2, __func__, __LINE__); 
                    _AzureAssertCond(segLoad == segBuffer + 2, __func__, __LINE__); 
                }
                else
                {
                    TCPIP_MAC_PACKET* pPkt;
                    TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(segBuffer + _TCPIP_MAC_GAP_OFFSET);
                    pPkt = pGap->segmentPktPtr;
                    nxp = (NX_PACKET*)pPkt->pTransportLayer;
                    _AzureAssertCond(nxp != 0, __func__, __LINE__); 
                    pPkt->pTransportLayer = 0;    // show there's no more associated nx packet

                    _AzureAssertCond(segLoad == segBuffer, __func__, __LINE__); 
                }

                // set up the nxp packet
                totSegLen += segLen;
                nxp->nx_packet_prepend_ptr = (uint8_t*)segLoad;
                nxp->nx_packet_append_ptr = nxp->nx_packet_prepend_ptr + segLen;

#if !defined(NX_DISABLE_PACKET_CHAIN)
                nxp->nx_packet_next = 0;
                if(prev_nxp)
                {
                    prev_nxp->nx_packet_next = nxp;
                }
                prev_nxp = nxp;
#endif  // !defined(NX_DISABLE_PACKET_CHAIN)
            }

            master_nxp->nx_packet_length = totSegLen;
            nx_driver_receive(master_nxp);
            nProcPkts++;
        }


        // netxd takes care of its own packets
        // we just release the MAC_PACKET
        pRxPkt->pktPriority = 0; // clear the packet priority to default
        if(pRxPkt->ackFunc)
        {
            if(pRxPkt->ackFunc(pRxPkt, pRxPkt->ackParam))
            {
                pRxPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
            }
        }
        else
        {
            _AzureDebugCond(false, __func__, __LINE__);
        }
    }

    if(nProcPkts)
    {
        pMDcpt->procRxPkts += nProcPkts;
    }
    if(nDroppedPkts)
    {
        pMDcpt->droppedPkts += nDroppedPkts;
    }
}

// Netx packet transmit acknowledge function
static bool _Azure_NetxTx_AckFunc(TCPIP_MAC_PACKET* pPkt,  const void* param)
{
    NX_PACKET* nxp = (NX_PACKET*)pPkt->pTransportLayer;

    AZURE_MAC_DCPT* pMDcpt = (AZURE_MAC_DCPT*)param;

    // release the Mac packet(s) to the pool
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    TCPIP_MAC_PACKET* linkPkt;
    int segIx;

    for(pSeg = pPkt->pDSeg, segIx = 0; pSeg != 0; pSeg = pSeg->next, segIx++)
    {
        TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(pSeg->segBuffer + _TCPIP_MAC_GAP_OFFSET);
        linkPkt = pGap->segmentPktPtr;
        if(segIx == 0)
        {
            _AzureAssertCond(linkPkt == pPkt, __func__, __LINE__); 
        }
        _Azure_ReleasePkt(pMDcpt->pParent, linkPkt, true);
    }

    // release the packet to nxp
    nxp -> nx_packet_prepend_ptr += sizeof(TCPIP_MAC_ETHERNET_HEADER);
    nxp -> nx_packet_length -= sizeof(TCPIP_MAC_ETHERNET_HEADER);
    if(nx_packet_transmit_release(nxp) != NX_SUCCESS)
    {
        pMDcpt->netxAckFail++;
    }
    
    return false;
}

AZURE_GLUE_RES Azure_Glue_Status(void)
{
    AZURE_GLUE_DCPT* pGDcpt = gAzureDcpt;

    if(pGDcpt == 0)
    {   // uninitialized?
        return AZURE_GLUE_RES_UNINITIALIZED;
    }

    return pGDcpt->currState == AZURE_GLUE_STAT_RUN ? AZURE_GLUE_RES_OK : AZURE_GLUE_RES_PENDING;
}

AZURE_GLUE_RES Azure_Glue_PacketTx(void* netxPkt)
{
    NX_PACKET* nxp;

    if((nxp = (NX_PACKET*)netxPkt) == 0)
    {
        _AzureAssertCond(false, __func__, __LINE__);
        return AZURE_GLUE_RES_PARAM_EROR; 
    }

    AZURE_GLUE_DCPT* pGDcpt = gAzureDcpt;

    if(pGDcpt == 0)
    {   // uninitialized?
        _AzureAssertCond(false, __func__, __LINE__);
        return AZURE_GLUE_RES_UNINITIALIZED;
    }

    int ifIx = nxp->nx_packet_address.nx_packet_interface_ptr->nx_interface_index;
    if(ifIx < 0 || ifIx >= AZURE_NET_INTERFACES)
    {
        pGDcpt->netxTxIfError++;
        return AZURE_GLUE_RES_IF_IX_ERROR; 
    }
    AZURE_MAC_DCPT* pMDcpt = pGDcpt->macDcpt + ifIx;

    if(pMDcpt->pMacObj == 0)
    {   // nothing here
        _AzureAssertCond(false, __func__, __LINE__);
        return AZURE_GLUE_RES_NO_IF;
    }

    if((pMDcpt->flags & AZURE_MAC_FLAG_INIT_DONE) == 0)
    {   // nothing here
        _AzureAssertCond(false, __func__, __LINE__);
        return AZURE_GLUE_RES_IF_NOT_READY;
    }
    
    // additional link check 
    if(pMDcpt->pMacObj->TCPIP_MAC_LinkCheck(pMDcpt->hIfMac) == false)
    {
        return AZURE_GLUE_RES_IF_NOT_READY;
    }

    TCPIP_MAC_SEGMENT_GAP_DCPT* pSegGap;
    uint8_t* endGap;

    uint8_t* segLoad;
    uint8_t* segBuffer;
    TCPIP_MAC_PACKET *pPkt, *masterPkt, *prevPkt, *nextPkt;
    TCPIP_MAC_DATA_SEGMENT* pSeg, *prevSeg;
    int pktIx;

    AZURE_GLUE_RES azTxRes = AZURE_GLUE_RES_OK;
    masterPkt = prevPkt = 0;
    prevSeg = 0;
    for(pktIx = 0; nxp != 0; nxp = nxp->nx_packet_next, pktIx++)
    {
        // make there's enough space to save the packet pointer expected by  the MAC driver
        segLoad = nxp->nx_packet_prepend_ptr;
        segBuffer = (uint8_t*)((uint32_t)segLoad & 0xfffffffc);   // align properly

        // search for gap space
        pSegGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(segBuffer + _TCPIP_MAC_GAP_OFFSET);
        endGap = (uint8_t*)pSegGap + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE; 
        if((uint8_t*)pSegGap < nxp->nx_packet_data_start || endGap > segBuffer)
        {
            pGDcpt->gapError++;
#if ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_GAP_SIZE) != 0)
            SYS_CONSOLE_MESSAGE("TX Packet Alloc: Gap size error!\r\n");
#endif  // ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_GAP_SIZE) != 0)
            azTxRes = AZURE_GLUE_RES_GAP_ERROR;
            break;
        }

        // grab a packet from the pool
        pPkt = _Azure_AllocatePkt(pGDcpt, true);
        if(pPkt == 0)
        {   // we're out of packets
            pGDcpt->txAllocPktError++;
#if ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_TX_ALLOC) != 0)
            SYS_CONSOLE_MESSAGE("TX Packet Alloc: out of MAC packets!\r\n");
#endif  // ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_TX_ALLOC) != 0)
            azTxRes = AZURE_GLUE_RES_PKT_POOL_EMPTY;
            break;
        }

        if(pktIx == 0)
        {
            masterPkt = pPkt;

            pPkt->ackFunc = _Azure_NetxTx_AckFunc; 
            pPkt->ackParam = pMDcpt; 
            // unused field to store the owner packet
            pPkt->pTransportLayer = (uint8_t*)nxp;
        }

        pPkt->next = 0;
        // link packets just in case there is an error and we need to free them
        if(prevPkt != 0)
        {
            prevPkt->next = pPkt;
        }
        prevPkt = pPkt;

        pSeg = pPkt->pDSeg;
        pSeg->segLoad = segLoad;
        pSeg->segBuffer = segBuffer;
        pSeg->segLen = nxp->nx_packet_append_ptr - nxp->nx_packet_prepend_ptr;
        pSeg->next = 0;
        if(prevSeg)
        {
            prevSeg->next = pSeg;
        }
        prevSeg = pSeg;

        // save the packet pointer expected by  the MAC driver
        pSegGap->segmentPktPtr = pPkt;
    }


    if(azTxRes == AZURE_GLUE_RES_OK)
    {   // all good; transmit the packet
        nextPkt = masterPkt->next;
        masterPkt->next = 0;
        TCPIP_MAC_RES macTxRes = pMDcpt->pMacObj->TCPIP_MAC_PacketTx(pMDcpt->hIfMac, masterPkt);
    
        if(macTxRes == TCPIP_MAC_RES_OK)
        {
            pMDcpt->txPkts++;
            return AZURE_GLUE_RES_OK;
        }
        masterPkt->next = nextPkt;  // restore link
        azTxRes = AZURE_GLUE_RES_TX_ERROR; 
    }


    // something went wrong

    for(pPkt = masterPkt; pPkt != 0; pPkt = pPkt->next)
    {
        _Azure_ReleasePkt(pMDcpt->pParent, pPkt, true);
    }

    // release the packet to nxp
    nxp -> nx_packet_prepend_ptr += sizeof(TCPIP_MAC_ETHERNET_HEADER);
    nxp -> nx_packet_length -= sizeof(TCPIP_MAC_ETHERNET_HEADER);
    if(nx_packet_transmit_release(nxp) != NX_SUCCESS)
    {
        pMDcpt->netxAckFail++;
    }
    
    pMDcpt->txRejected++;
    return azTxRes; 
}

const uint8_t* Azure_Glue_MACAddress(int ifIx)
{
    AZURE_GLUE_DCPT* pGDcpt = gAzureDcpt;

    if(pGDcpt != 0)
    {   // initialized
        if(ifIx >= 0 && ifIx < AZURE_NET_INTERFACES)
        {   // valid interface
            AZURE_MAC_DCPT* pMDcpt = pGDcpt->macDcpt + ifIx;
            if(pMDcpt->pMacObj != 0)
            {   // something here
                if((pMDcpt->flags & AZURE_MAC_FLAG_INIT_DONE) != 0)
                {   // initialization done
                    return pMDcpt->netMACAddr.v;
                }
            }
        }
    }

    return 0;
}

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
uint32_t Azure_Glue_IfCapability(int ifIx)
{
    AZURE_GLUE_DCPT* pGDcpt = gAzureDcpt;

    if(pGDcpt != 0)
    {   // initialized
        if(ifIx >= 0 && ifIx < AZURE_NET_INTERFACES)
        {   // valid interface
            AZURE_MAC_DCPT* pMDcpt = pGDcpt->macDcpt + ifIx;
            if(pMDcpt->pMacObj != 0)
            {   // something here
                // manually coded for now
                if(pMDcpt->pMacObj->macId == TCPIP_MODULE_MAC_PIC32C)
                {   // GMAC supports checksum offload
                    return (NX_INTERFACE_CAPABILITY_IPV4_TX_CHECKSUM | NX_INTERFACE_CAPABILITY_IPV4_RX_CHECKSUM |
                            NX_INTERFACE_CAPABILITY_TCP_TX_CHECKSUM  | NX_INTERFACE_CAPABILITY_TCP_RX_CHECKSUM  |
                            NX_INTERFACE_CAPABILITY_UDP_TX_CHECKSUM  | NX_INTERFACE_CAPABILITY_UDP_RX_CHECKSUM);
                }
            }
        }
    }

    return 0;
}
#else
uint32_t Azure_Glue_IfCapability(int ifIx)
{
    return 0;
}
#endif

// ************************************************ 
// Low Level
//
static void* _Azure_Glue_Malloc(TCPIP_MAC_HEAP_HANDLE heapH, size_t bytes)
{
    TX_BYTE_POOL* pPool = (TX_BYTE_POOL*)heapH;
    
    void* allocPtr;

    if(tx_byte_allocate(pPool, &allocPtr, bytes, TX_WAIT_FOREVER) == TX_SUCCESS)
    {
        return allocPtr;
    }
    
    return 0;
}

static void* _Azure_Glue_Calloc(TCPIP_MAC_HEAP_HANDLE heapH, size_t nElems, size_t elemSize)
{
    size_t bytes = nElems * elemSize;
    void* ptr = _Azure_Glue_Malloc(heapH, bytes);
    if(ptr)
    {
        memset(ptr, 0, bytes);
    }

    return ptr;
}

static size_t _Azure_Glue_Free(TCPIP_MAC_HEAP_HANDLE heapH, const void* ptr)
{
    return tx_byte_release((void*)ptr);
}

// NB: created in tasks.c!
extern TX_BYTE_POOL   byte_pool_0;

static AZURE_GLUE_INIT_RES _Azure_CreateResources(AZURE_GLUE_DCPT* pGDcpt)
{
    // set the byte pool for H3 driver allocation
    pGDcpt->allocH = &byte_pool_0;

    // create the packet pool
    _Azure_CreatePktLists(pGDcpt);

    return AZURE_INIT_RES_OK;
}

static void _Azure_InitPktList(AZ_SINGLE_LIST *pList, uint32_t* pItemArray, size_t arrayItems, size_t itemSize)
{
    int ix;

    _Azure_SingleListInitialize(pList);


    TCPIP_MAC_PACKET* pPkt;
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    
    uint8_t* pItem = (uint8_t*)pItemArray;
    for(ix = 0; ix < arrayItems; ix++)
    {
        pPkt = (TCPIP_MAC_PACKET*)pItem;

        // initialize the packet
        memset(pPkt, 0, itemSize);

        // set proper segment
        pSeg = (TCPIP_MAC_DATA_SEGMENT*)((uint8_t*)pPkt + AZURE_GLUE_PKT_SIZE);
        // segLoad is set at run time
        pPkt->pDSeg = pSeg;

        // other permanent fields in the packet
        // pPkt->ackFunc, pPkt->ackParam: for RX these are set by the MAC
        // for TX they'll be set when we get a buffer to TX

        _Azure_SingleListTailAdd(pList, (AZ_SGL_LIST_NODE*)pPkt);
        pItem += itemSize;
    }
}

static void _Azure_CreatePktLists(AZURE_GLUE_DCPT* pGDcpt)
{
    _Azure_InitPktList(&pGDcpt->rxPktList, azure_glue_rx_pkt_array, AZURE_GLUE_PKT_RX_ARRAY_ITEMS, AZURE_GLUE_PKT_ITEM_SIZE);
    _Azure_InitPktList(&pGDcpt->txPktList, azure_glue_tx_pkt_array, AZURE_GLUE_PKT_TX_ARRAY_ITEMS, AZURE_GLUE_PKT_ITEM_SIZE);
}


static TCPIP_MAC_PACKET* _Azure_AllocatePkt(AZURE_GLUE_DCPT* pGDcpt, bool isTx)
{
    TCPIP_MAC_PACKET* pPkt;

    if(isTx)
    {
        AZ_SINGLE_LIST *pList = &pGDcpt->txPktList;
        OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
        pPkt = (TCPIP_MAC_PACKET*)_Azure_SingleListHeadRemove(pList);
        if(pPkt)
        {
            pGDcpt->txAllocPkts++;
        }
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
    }
    else
    {
        AZ_SINGLE_LIST *pList = &pGDcpt->rxPktList;
        pPkt = (TCPIP_MAC_PACKET*)_Azure_SingleListHeadRemove(pList);
        if(pPkt)
        {
            pGDcpt->rxAllocPkts++;
        }
    }

    return pPkt;
}

static void _Azure_ReleasePkt(AZURE_GLUE_DCPT* pGDcpt, TCPIP_MAC_PACKET* pPkt, bool isTx)
{
    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(isTx)
    {
        AZ_SINGLE_LIST *pList = &pGDcpt->txPktList;
        _Azure_SingleListTailAdd(pList, (AZ_SGL_LIST_NODE*)pPkt);
        pGDcpt->txReleasePkts++;
    }
    else
    {
        AZ_SINGLE_LIST *pList = &pGDcpt->rxPktList;
        _Azure_SingleListTailAdd(pList, (AZ_SGL_LIST_NODE*)pPkt);
        pGDcpt->rxReleasePkts++;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);
}

// deallocate resources
static void _Azure_Cleanup(AZURE_GLUE_DCPT* pGDcpt)
{
}

extern NX_PACKET* nx_rx_pkt_allocate(void);

// allocate a RX packet for the MAC driver
static TCPIP_MAC_PACKET* _Azure_Glue_MacRxPkt_Alloc(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags)
{
    AZURE_GLUE_DCPT* pGDcpt = gAzureDcpt;

    // debug sanity check
    uint16_t avlblPktSize = AZURE_GLUE_PKT_ITEM_SIZE; // size of a packet in the packet array/list 
    if(pktLen > avlblPktSize)
    {
        pGDcpt->rxLenError++;
        _AzureAssertCond(false, __func__, __LINE__); 
        return 0;
    }

    // get a MAC packet first
    TCPIP_MAC_PACKET* pPkt = _Azure_AllocatePkt(pGDcpt, false);
    if(pPkt == 0)
    {   // we're out of RX packets
        pGDcpt->rxAllocPktError++;
#if ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_RX_ALLOC) != 0)
        SYS_CONSOLE_MESSAGE("RX Packet Alloc: out of MAC packets!\r\n");
#endif  // ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_RX_ALLOC) != 0)
        return 0;
    }

    // get an associated buffer
    NX_PACKET* nxp = nx_rx_pkt_allocate();
    if(nxp == 0)
    {   // we're out of RX packets
        pGDcpt->rxAllocNetxError++;
        _Azure_ReleasePkt(pGDcpt, pPkt, false);
#if ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_RX_ALLOC) != 0)
        SYS_CONSOLE_MESSAGE("RX Packet Alloc: out of RX buffers!\r\n");
#endif  // ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_RX_ALLOC) != 0)
        return 0;
    }

    uint8_t* segBuffer = nxp->nx_packet_data_start + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE;
    _AzureAssertCond(NX_PHYSICAL_HEADER >= (_TCPIP_MAC_DATA_SEGMENT_GAP_SIZE + sizeof(TCPIP_MAC_ETHERNET_HEADER)), __func__, __LINE__); 

    uint8_t* segLoad = nxp->nx_packet_prepend_ptr = segBuffer + 2; // dataOffset; make MAC header start at M2 offset

    uint16_t nxpLen = nxp->nx_packet_data_end - segBuffer;
    if(nxpLen <  pGDcpt->maxRxFrame)
    {   // wrong config ???
        pGDcpt->netxLenError++;
        _Azure_ReleasePkt(pGDcpt, pPkt, false);
        if(nx_packet_release(nxp) != 0)
        {
            gAzureDcpt->rxReleaseError++;
        }
        _AzureAssertCond(false, __func__, __LINE__); 
        return 0;
    }

    TCPIP_MAC_SEGMENT_GAP_DCPT* pGap = (TCPIP_MAC_SEGMENT_GAP_DCPT*)(segBuffer + _TCPIP_MAC_GAP_OFFSET);
    uint8_t* endGap = (uint8_t*)pGap + _TCPIP_MAC_DATA_SEGMENT_GAP_SIZE; 
    if((uint8_t*)pGap < nxp->nx_packet_data_start || endGap > segBuffer)
    {
        pGDcpt->gapError++;
        _Azure_ReleasePkt(pGDcpt, pPkt, false);
        if(nx_packet_release(nxp) != 0)
        {
            gAzureDcpt->rxReleaseError++;
        }
#if ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_GAP_SIZE) != 0)
        SYS_CONSOLE_MESSAGE("RX Packet Alloc: Gap size error!\r\n");
#endif  // ((_AZURE_DEBUG_LEVEL & _AZURE_DEBUG_MASK_GAP_SIZE) != 0)
        return 0;
    }

    // save the packet pointer expected by  the MAC driver
    pGap->segmentPktPtr = pPkt;

    // unused field to store the owner packet
    pPkt->pTransportLayer = (uint8_t*)nxp;

    // set the MAC layer pointer
    pPkt->pMacLayer = segLoad;

    // populate the segment
    TCPIP_MAC_DATA_SEGMENT* pSeg = pPkt->pDSeg;

    pSeg->segBuffer = segBuffer;
    pSeg->segLoad = segLoad;
    pSeg->segSize = nxpLen;

    pGDcpt->rxAllocBuffs++;
    return pPkt;
}

// free a previously allocated RX packet
static void  _Azure_Glue_MacRxPkt_Free(TCPIP_MAC_PACKET* pPkt)
{
    // restore packet owner
    NX_PACKET* nxp = (NX_PACKET*)pPkt->pTransportLayer;
    if(nxp != 0)
    {
        pPkt->pTransportLayer = 0;
        if(nx_packet_release(nxp) != 0)
        {
            gAzureDcpt->rxReleaseError++;
        }
    }
    gAzureDcpt->rxReleaseBuffs++;

    _Azure_ReleasePkt(gAzureDcpt, pPkt, false);
}

// callback from MAC ISR
// NB: from nx_driver_harmony.c
extern void  nx_driver_set_deferred_events(ULONG new_events);
static void _Azure_Glue_MacEventCB(TCPIP_MAC_EVENT event, const void* hParam)
{
    AZURE_MAC_DCPT* pMDcpt = (AZURE_MAC_DCPT*)hParam;

    pMDcpt->totEventCount++;
    pMDcpt->activeEvents |= event;
    pMDcpt->eventCount++;

    // set the new deferred events
    if((event & (TCPIP_MAC_EV_RX_PKTPEND | TCPIP_MAC_EV_RX_DONE | TCPIP_MAC_EV_TX_DONE)) != 0)
    {
        nx_driver_set_deferred_events(0x05);    // NX_DRIVER_DEFERRED_PACKET_RECEIVED | NX_DRIVER_DEFERRED_PACKET_TRANSMITTED
    }
    
}

// called by the MAC driver when done with TXing a packet
static void _Azure_Glue_MacTxPkt_Ack(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId)
{
    AZURE_GLUE_DCPT* pGDcpt = gAzureDcpt;
    AZURE_MAC_DCPT* pMDcpt = (AZURE_MAC_DCPT*)pPkt->ackParam;

    int macIx = pMDcpt - pGDcpt->macDcpt;
    (void)macIx;
    if(macIx < 0 || macIx >= AZURE_NET_INTERFACES)
    {
        _AzureAssertCond(false, __func__, __LINE__); 
        return;
    }

    if(ackRes != TCPIP_MAC_PKT_ACK_TX_OK)
    {
        pMDcpt->txBadAcks++;
    }

    // call the packet ackFunc
    if(pPkt->ackFunc != 0)
    {
        pPkt->ackFunc(pPkt, pMDcpt);
        pMDcpt->txAckPkts++;
    }
    else
    {   // orphan!
        _AzureAssertCond(false, __func__, __LINE__);
        pMDcpt->txOrphans++;
    }
}

#if (AZURE_MAC_TX_SYNCHRONIZE_FUNC != 0)
static bool _Azure_Glue_SyncFunction(void* synchHandle, TCPIP_MAC_SYNCH_REQUEST req)
{
    _AzureAssertCond(0 < req && req < sizeof(_Azure_SyncF_Tbl) / sizeof(*_Azure_SyncF_Tbl), __func__, __LINE__);

    return _Azure_SyncF_Tbl[req](synchHandle);
}

static bool _Azure_Glue_SyncNone(void* synchHandle)
{
    _AzureAssertCond(false, __func__, __LINE__);
    return false;
}

static bool _Azure_Glue_SyncCritEnter(void* synchHandle)
{
    *(OSAL_CRITSECT_DATA_TYPE*)synchHandle =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    return true; 
}

static bool _Azure_Glue_SyncCritLeave(void* synchHandle)
{
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, *(OSAL_CRITSECT_DATA_TYPE*)synchHandle);
    return true; 
}
#endif // (AZURE_MAC_TX_SYNCHRONIZE_FUNC != 0)

// *****************************************************************
// debug, statistics area

#if (AZURE_DEBUG_STATISTICS != 0)
typedef struct
{
    uint32_t okCnt;
    uint32_t allocErr;
}NETX_RX_ERR_COUNT;

extern NETX_RX_ERR_COUNT netx_rx_err_count; // in the nx_driver_same54.c

void Azure_Glue_Counters(void)
{
    AZURE_GLUE_DCPT* pGDcpt = gAzureDcpt;
    if(pGDcpt != 0)
    {
        AZURE_MAC_DCPT* pMDcpt = pGDcpt->macDcpt;

        TCPIP_MAC_RX_STATISTICS rxStatistics;
        pMDcpt->pMacObj->TCPIP_MAC_StatisticsGet(pMDcpt->hIfMac, &rxStatistics, 0);

        SYS_CONSOLE_PRINT("Az Glue TX counters - txPkts: %u, txAckPkts: %u, txAllocPkts: %u, txReleasePkts: %u\r\n", pMDcpt->txPkts, pMDcpt->txAckPkts, pGDcpt->txAllocPkts, pGDcpt->txReleasePkts);
        SYS_CONSOLE_PRINT("Az Glue RX counters - rxAllocBuffs: %u, rxReleaseBuffs: %u, rxAllocPkts: %u, rxReleasePkts: %u\r\n", pGDcpt->rxAllocBuffs, pGDcpt->rxReleaseBuffs, pGDcpt->rxAllocPkts, pGDcpt->rxReleasePkts);
        SYS_CONSOLE_PRINT("\tmac nRxOkPackets: %u, totEventCount: %u\r\n", rxStatistics.nRxOkPackets, pMDcpt->totEventCount);
        SYS_CONSOLE_PRINT("\tmaxRxProc: %u, totRxEventCount: %u, procRxPkts: %u, chainedPkts: %u, droppedPkts: %u\r\n",
                pMDcpt->maxRxProc, pMDcpt->totRxEventCount, pMDcpt->procRxPkts, pMDcpt->chainedPkts, pMDcpt->droppedPkts);


        SYS_CONSOLE_PRINT("Az Glue errors - rxLenError: %u, rxAllocPktError: %u, rxAllocNetxError: %u, netxLenError: %u\r\n",
                pGDcpt->rxLenError, pGDcpt->rxAllocPktError, pGDcpt->rxAllocNetxError, pGDcpt->netxLenError);
        SYS_CONSOLE_PRINT("\trxReleaseError: %u, txRejected: %u, txBadAcks: %u, totErrEventCount: %u\r\n",
                pGDcpt->rxReleaseError, pMDcpt->txRejected, pMDcpt->txBadAcks, pMDcpt->totErrEventCount);
        SYS_CONSOLE_PRINT("\ttxAllocPktError: %u, netxAckFail: %u, netxTxIfError: %u, orphans: %u, gapError: %u\r\n",
                pGDcpt->txAllocPktError, pMDcpt->netxAckFail, pGDcpt->netxTxIfError, pMDcpt->txOrphans, pGDcpt->gapError);

        SYS_CONSOLE_PRINT("RX packets list: %d, now: %d\r\n", AZURE_GLUE_PKT_RX_ARRAY_ITEMS, _Azure_SingleListCount(&pGDcpt->rxPktList));
        SYS_CONSOLE_PRINT("TX packets list: %d, now: %d\r\n", AZURE_GLUE_PKT_TX_ARRAY_ITEMS, _Azure_SingleListCount(&pGDcpt->txPktList));

        SYS_CONSOLE_PRINT("Netx err counters - okCnt: %u, allocErr: %u\r\n", netx_rx_err_count.okCnt, netx_rx_err_count.allocErr);

        extern NX_PACKET_POOL pool_0;
        SYS_CONSOLE_PRINT("netx pool - total: %u, avlbl: %u, empty req: %u, invalid release: %u\r\n", pool_0.nx_packet_pool_total, pool_0.nx_packet_pool_available, pool_0.nx_packet_pool_empty_requests, pool_0.nx_packet_pool_invalid_releases);

    }

}
#endif  // (AZURE_DEBUG_STATISTICS != 0)




#if (AZURE_DEBUG_MAC_INFO != 0)
void Azure_Mac_Info(void)
{
    TCPIP_MAC_RX_STATISTICS rxStatistics;
    TCPIP_MAC_TX_STATISTICS txStatistics;

    TCPIP_MAC_STATISTICS_REG_ENTRY  regEntries[50];
    TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntry;
    char entryName[sizeof(pRegEntry->registerName) + 1];
    int hwEntries, jx;


    AZURE_GLUE_DCPT* pGDcpt = gAzureDcpt;
    if(pGDcpt != 0)
    {
        AZURE_MAC_DCPT* pMDcpt = pGDcpt->macDcpt;

        SYS_CONSOLE_MESSAGE("MAC Driver Statistics:\r\n");
        TCPIP_MAC_RES res = pMDcpt->pMacObj->TCPIP_MAC_StatisticsGet(pMDcpt->hIfMac, &rxStatistics, &txStatistics);
        if(res == TCPIP_MAC_RES_OK)
        {
            SYS_CONSOLE_PRINT("\r\n Receive Statistics\r\n");
            SYS_CONSOLE_PRINT("\t nRxOkPackets: %d\r\n\t nRxPendBuffers: %d\r\n\t nRxSchedBuffers: %d\r\n",
                    rxStatistics.nRxOkPackets, rxStatistics.nRxPendBuffers, rxStatistics.nRxSchedBuffers);
            SYS_CONSOLE_PRINT("\t nRxErrorPackets: %d\r\n\t nRxFragmentErrors: %d\r\n\t nRxBuffNotAvailable: %d\r\n", rxStatistics.nRxErrorPackets, rxStatistics.nRxFragmentErrors, rxStatistics.nRxBuffNotAvailable);
            
            SYS_CONSOLE_PRINT("\r\n Transmit Statistics\r\n");
            SYS_CONSOLE_PRINT("\t nTxOkPackets: %d\r\n\t nTxPendBuffers: %d\r\n\t nTxErrorPackets: %d\r\n\t nTxQueueFull: %d\r\n\r\n",
                    txStatistics.nTxOkPackets, txStatistics.nTxPendBuffers, txStatistics.nTxErrorPackets, txStatistics.nTxQueueFull);
        }
        else
        {
            SYS_CONSOLE_MESSAGE("\tnot supported\r\n");
        }

        res = pMDcpt->pMacObj->TCPIP_MAC_RegisterStatisticsGet(pMDcpt->hIfMac, regEntries, sizeof(regEntries)/sizeof(*regEntries), &hwEntries);
        if(res == TCPIP_MAC_RES_OK)
        {
            SYS_CONSOLE_MESSAGE("MAC Hardware Registers:\r\n");
            entryName[sizeof(entryName) - 1] = 0;
            for(jx = 0, pRegEntry = regEntries; jx < hwEntries && jx < sizeof(regEntries)/sizeof(*regEntries); jx++, pRegEntry++)
            {
                strncpy(entryName, pRegEntry->registerName, sizeof(entryName) - 1);
                SYS_CONSOLE_PRINT("\t %s: 0x%x\r\n", entryName, pRegEntry->registerValue);
            }
        }
        else
        {
            SYS_CONSOLE_MESSAGE("Failed to get macinfo\r\n");
        }
        return;
    }

    SYS_CONSOLE_MESSAGE("not available!\r\n");
}

// GMAC statistics register access 
typedef struct
{
    char                regName[36];    // register name
    volatile const uint32_t*  regPtr;         // register address
} GMAC_HW_REG_DCPT;


static const GMAC_HW_REG_DCPT gmacHwRegDcpt[] =
{
    {"GMAC_NCR",     &GMAC_REGS->GMAC_NCR,       },
    {"GMAC_NCFGR",   &GMAC_REGS->GMAC_NCFGR,     },
    {"GMAC_NSR",     &GMAC_REGS->GMAC_NSR,       },
    {"GMAC_UR",      &GMAC_REGS->GMAC_UR,        },
    {"GMAC_DCFGR",   &GMAC_REGS->GMAC_DCFGR,     },
    {"GMAC_TSR",     &GMAC_REGS->GMAC_TSR,       },
    {"GMAC_RBQB",    &GMAC_REGS->GMAC_RBQB,      },
    {"GMAC_TBQB",    &GMAC_REGS->GMAC_TBQB,      },
    {"GMAC_RSR",     &GMAC_REGS->GMAC_RSR,       },
    {"GMAC_ISR",     &GMAC_REGS->GMAC_ISR,       },
    {"GMAC_IER",     &GMAC_REGS->GMAC_IER,       },
    {"GMAC_IDR",     &GMAC_REGS->GMAC_IDR,       },
    {"GMAC_IMR",     &GMAC_REGS->GMAC_IMR,       },
    {"GMAC_MAN",     &GMAC_REGS->GMAC_MAN,       },
    {"GMAC_RPQ",     &GMAC_REGS->GMAC_RPQ,       },
    {"GMAC_TPQ",     &GMAC_REGS->GMAC_TPQ,       },
    {"GMAC_TPSF",    &GMAC_REGS->GMAC_TPSF,      },
    {"GMAC_RPSF",    &GMAC_REGS->GMAC_RPSF,      },
    {"GMAC_RJFML",   &GMAC_REGS->GMAC_RJFML,     },
    {"GMAC_HRB",     &GMAC_REGS->GMAC_HRB,       },
    {"GMAC_HRT",     &GMAC_REGS->GMAC_HRT,       },
    {"GMAC_SAB0",    &GMAC_REGS->SA[0].GMAC_SAB,   },
    {"GMAC_SAT0",    &GMAC_REGS->SA[0].GMAC_SAT,   },
    {"GMAC_SAB1",    &GMAC_REGS->SA[1].GMAC_SAB,   },
    {"GMAC_SAT1",    &GMAC_REGS->SA[1].GMAC_SAT,   },
    {"GMAC_SAB2",    &GMAC_REGS->SA[2].GMAC_SAB,   },
    {"GMAC_SAT2",    &GMAC_REGS->SA[2].GMAC_SAT,   },
    {"GMAC_SAB3",    &GMAC_REGS->SA[3].GMAC_SAB,   },
    {"GMAC_SAT3",    &GMAC_REGS->SA[3].GMAC_SAT,   },
    {"GMAC_TIDM0",   GMAC_REGS->GMAC_TIDM + 0,     },
    {"GMAC_TIDM1",   GMAC_REGS->GMAC_TIDM + 1,     },
    {"GMAC_TIDM2",   GMAC_REGS->GMAC_TIDM + 2,     },
    {"GMAC_TIDM3",   GMAC_REGS->GMAC_TIDM + 3,     },
    {"GMAC_WOL",     &GMAC_REGS->GMAC_WOL,       },
    {"GMAC_IPGS",    &GMAC_REGS->GMAC_IPGS,      },
    {"GMAC_SVLAN",   &GMAC_REGS->GMAC_SVLAN,     },
    {"GMAC_TPFCP",   &GMAC_REGS->GMAC_TPFCP,     },
    {"GMAC_SAMB1",   &GMAC_REGS->GMAC_SAMB1,     },
    {"GMAC_SAMT1",   &GMAC_REGS->GMAC_SAMT1,     },
};


bool GMAC_RegistersGet(GMAC_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries)  
{
    const GMAC_HW_REG_DCPT* pRegDcpt = gmacHwRegDcpt;
    int dcptSize = sizeof(gmacHwRegDcpt) / sizeof(*gmacHwRegDcpt);


	if(pHwEntries)
	{
		*pHwEntries = dcptSize;
	}

	if(pRegEntries)
	{   // fill data
		unsigned int ix, regLim;

		regLim = nEntries > dcptSize ? dcptSize : nEntries;
		for(ix = 0; ix < regLim; ix++, pRegEntries++, pRegDcpt++)
		{
			strncpy(pRegEntries->registerName, pRegDcpt->regName, sizeof(pRegEntries->registerName));
			pRegEntries->registerValue = *pRegDcpt->regPtr;
		}
	}

	return true;
}
#endif  // (AZURE_DEBUG_MAC_INFO != 0)

