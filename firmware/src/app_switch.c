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
#include "az_util.h"

static void SWITCH1_InterruptHandler(GPIO_PIN pin, uintptr_t context)
{
    if (SWITCH1_Get() == APP_SWITCH_PRESSED)
    {
        button_press_data.sw1_press_count++;
        button_press_data.flag.sw1 = true;
        LED_RED_Toggle();
    }
}

static void SWITCH2_InterruptHandler(GPIO_PIN pin, uintptr_t context)
{
    if (SWITCH2_Get() == APP_SWITCH_PRESSED)
    {
        button_press_data.sw2_press_count++;
        button_press_data.flag.sw2 = true;
        LED_RED_Toggle();
    }
}

void APP_SWITCH_init(void)
{
    GPIO_PinInterruptCallbackRegister(SWITCH1_PIN, SWITCH1_InterruptHandler, 0); 
    SWITCH1_InterruptEnable();
    button_press_data.sw1_press_count = 0;
    GPIO_PinInterruptCallbackRegister(SWITCH2_PIN, SWITCH2_InterruptHandler, 0); 
    SWITCH2_InterruptEnable();
    button_press_data.sw2_press_count = 0;
}
