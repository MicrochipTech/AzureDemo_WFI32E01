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
        GPIO_RA13_Set();
        LED_RED_On();
    }
    else
    {
        if (appConnectStatus.wifi == true) 
        {
            GPIO_RA14_Clear();
            GPIO_RA13_Set();
        }
        if (appConnectStatus.cloud == true) 
        {
            GPIO_RA14_Set();
            GPIO_RA13_Clear();
        }
        if ( (appConnectStatus.wifi == true) && (appConnectStatus.cloud == true) )
        {
            LED_RED_Off();
        }
    }
}
