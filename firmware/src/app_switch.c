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
#include "app_switch.h"

uint16_t APP_SWITCH1_counter = 0;
uint16_t APP_SWITCH2_counter = 0;

static void SWITCH1_InterruptHandler(GPIO_PIN pin, uintptr_t context)
{
    if (SWITCH1_Get() == APP_SWITCH_PRESSED)
    {
        APP_SWITCH1_counter++;
        LED_RED_Toggle();
    }
}

static void SWITCH2_InterruptHandler(GPIO_PIN pin, uintptr_t context)
{
    if (SWITCH2_Get() == APP_SWITCH_PRESSED)
    {
        APP_SWITCH2_counter++;
        LED_RED_Toggle();
    }
}

void APP_SWITCH_init(void)
{
    GPIO_PinInterruptCallbackRegister(SWITCH1_PIN, SWITCH1_InterruptHandler, 0); 
    SWITCH1_InterruptEnable();
    GPIO_PinInterruptCallbackRegister(SWITCH2_PIN, SWITCH2_InterruptHandler, 0); 
    SWITCH2_InterruptEnable();
}
