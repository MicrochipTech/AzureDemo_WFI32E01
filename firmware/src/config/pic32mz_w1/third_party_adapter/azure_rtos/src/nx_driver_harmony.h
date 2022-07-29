/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** NetX Component                                                        */
/**                                                                       */
/**   Ethernet driver for ATMEL SAME54 family of microprocessors          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef NX_DRIVER_HARMONY_H
#define NX_DRIVER_HARMONY_H


#ifdef   __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {
#endif


/* Include ThreadX header file, if not already.  */  
  
#ifndef TX_API_H
#include "tx_api.h"
#endif


/* Include NetX header file, if not already.  */

#ifndef NX_API_H
#include "nx_api.h"
#endif


/* Determine if the driver's source file is being compiled. The constants and typdefs are only valid within
   the driver's source file compilation.  */

#ifdef NX_DRIVER_SOURCE


/****** DRIVER SPECIFIC ****** Start of part/vendor specific include area.  Include any such files here!  */
    
/****** DRIVER SPECIFIC ****** End of part/vendor specific include file area!  */


/* Define generic constants and macros for all NetX Ethernet drivers.  */

#define NX_DRIVER_ETHERNET_IP                   0x0800
#define NX_DRIVER_ETHERNET_IPV6                 0x86dd
#define NX_DRIVER_ETHERNET_ARP                  0x0806
#define NX_DRIVER_ETHERNET_RARP                 0x8035

#define NX_DRIVER_ETHERNET_MTU                  1514
#define NX_DRIVER_ETHERNET_FRAME_SIZE           14

#define NX_DRIVER_DEFERRED_PACKET_RECEIVED      1
#define NX_DRIVER_DEFERRED_DEVICE_RESET         2
#define NX_DRIVER_DEFERRED_PACKET_TRANSMITTED   4
    
#define NX_DRIVER_STATE_NOT_INITIALIZED         1
#define NX_DRIVER_STATE_INITIALIZE_FAILED       2
#define NX_DRIVER_STATE_INITIALIZED             3    
#define NX_DRIVER_STATE_LINK_ENABLED            4    

#define NX_DRIVER_ERROR                         90


#define NX_DRIVER_ETHERNET_HEADER_REMOVE(p)   \
{   \
    p -> nx_packet_prepend_ptr =  p -> nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE;  \
    p -> nx_packet_length =  p -> nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;    \
}    


/****** DRIVER SPECIFIC ****** Start of part/vendor specific constants area.  Include any such constants here!  */
    
#define NX_DRIVER_CAPABILITY ( NX_INTERFACE_CAPABILITY_IPV4_TX_CHECKSUM   | \
                               NX_INTERFACE_CAPABILITY_IPV4_RX_CHECKSUM   | \
                               NX_INTERFACE_CAPABILITY_TCP_TX_CHECKSUM    | \
                               NX_INTERFACE_CAPABILITY_TCP_RX_CHECKSUM    | \
                               NX_INTERFACE_CAPABILITY_UDP_TX_CHECKSUM    | \
                               NX_INTERFACE_CAPABILITY_UDP_RX_CHECKSUM)    


/****** DRIVER SPECIFIC ****** End of part/vendor specific constant area!  */


/* Define basic Ethernet driver information typedef. Note that this typedefs is designed to be used only
   in the driver's C file. */

typedef struct NX_DRIVER_INFORMATION_STRUCT
{
    /* NetX IP instance that this driver is attached to.  */
    NX_IP               *nx_driver_information_ip_ptr;            

    /* Driver's current state.  */
    ULONG               nx_driver_information_state ;  

    /* Packet pool used for receiving packets. */
    NX_PACKET_POOL      *nx_driver_information_packet_pool_ptr;        

    /* Define the driver interface association.  */
    NX_INTERFACE        *nx_driver_information_interface;

    /* Define the deferred event field. This will contain bits representing events 
       deferred from the ISR for processing in the thread context.  */
    ULONG               nx_driver_information_deferred_events;

    /****** DRIVER SPECIFIC ****** Start of part/vendor specific driver information area.  Include any such constants here!  */

    /* Define the multicast packets count */
    ULONG               nx_driver_information_multicast_count;

    /****** DRIVER SPECIFIC ****** End of part/vendor specific driver information area.  */

}   NX_DRIVER_INFORMATION;

#endif


/****** DRIVER SPECIFIC ****** Start of part/vendor specific external function prototypes.  A typical NetX Ethernet driver
                               should expose its entry function as well as its interrupt handling function(s) here. All other
                               functions in the driver should have local scope, i.e., defined as static.  */
    
/* Define global driver entry function. */

VOID  nx_driver_harmony(NX_IP_DRIVER *driver_req_ptr);


/* Packet allocation function for the Harmony MAC driver */
NX_PACKET* nx_rx_pkt_allocate(void);

/* Passes a Harmony MAC packet to the netxd stack */ 
void nx_driver_receive(NX_PACKET* nxp);

/* Sets new available events */
void  nx_driver_set_deferred_events(ULONG new_events);


/****** DRIVER SPECIFIC ****** End of part/vendor specific external function prototypes.  */


#ifdef   __cplusplus
/* Yes, C++ compiler is present.  Use standard C.  */
    }
#endif
#endif  // NX_DRIVER_HARMONY_H


