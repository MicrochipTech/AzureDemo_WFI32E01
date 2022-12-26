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
#include "app.h"

extern APP_CONNECT_STATUS appConnectStatus;

void APP_STATUS_update(void)
{
    if (appConnectStatus.alarm == true) 
    {
        GPIO_RA14_Set();
#ifdef WFI32_IoT_BOARD        
        GPIO_RA13_Set();
#else
        GPIO_RK6_Set();
#endif        
        LED_RED_On();
    }
    else
    {
        if (appConnectStatus.wifi == true) 
        {
            GPIO_RA14_Clear();
#ifdef WFI32_IoT_BOARD              
            GPIO_RA13_Set();
#else
            GPIO_RK6_Set();
#endif            
        }
        if (appConnectStatus.cloud == true) 
        {
            GPIO_RA14_Set();
#ifdef WFI32_IoT_BOARD                          
            GPIO_RA13_Clear();
#else
            GPIO_RK6_Clear();
#endif            
        }
        if ( (appConnectStatus.wifi == true) && (appConnectStatus.cloud == true) )
        {
            LED_RED_Off();
        }
    }
}
