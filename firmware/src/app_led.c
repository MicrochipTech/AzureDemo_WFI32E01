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
#include "app_led.h"

APP_LED_CTRL appLedCtrl[APP_LED_TOTAL];
APP_CONNECT_STATUS appConnectStatus;

void APP_LED_refresh(void)
{       
    if ( (appConnectStatus.wifi == true) && (appConnectStatus.cloud == true) )
    {
        appLedCtrl[APP_LED_RED].mode = APP_LED_OFF;
        LED_RED_Off();
    }
    else
    {
        appLedCtrl[APP_LED_RED].mode = APP_LED_ON;
        LED_RED_On();        
    }
    switch (appLedCtrl[APP_LED_YELLOW].mode)
    {
        case APP_LED_OFF: LED_YELLOW_Off(); break;
        case APP_LED_ON: LED_YELLOW_On(); break;
        case APP_LED_BLINKING: LED_YELLOW_Toggle(); break;
        default: break;
    }
    if (appConnectStatus.wifi == false)
    {
        appLedCtrl[APP_LED_BLUE].mode = APP_LED_OFF;
        LED_BLUE_Off();
    }
    else
    {
        appLedCtrl[APP_LED_BLUE].mode = APP_LED_ON;
        LED_BLUE_On();        
    }
    if (appConnectStatus.cloud == false)
    {
        appLedCtrl[APP_LED_GREEN].mode = APP_LED_OFF;
        LED_GREEN_Off();
    }
    else
    {
        appLedCtrl[APP_LED_GREEN].mode = APP_LED_ON;
        LED_GREEN_On();        
    }
}

void APP_LED_init(void)
{
    for (int counter = 0; counter < APP_LED_TOTAL; counter++)
    {
        appLedCtrl[counter].mode = APP_LED_OFF;
    }
    appLedCtrl[APP_LED_YELLOW].mode = APP_LED_BLINKING;
}
