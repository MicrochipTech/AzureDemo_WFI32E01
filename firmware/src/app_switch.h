/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_switch.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_SWITCH_H
#define _APP_SWITCH_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

#include "config/pic32mz_w1/peripheral/gpio/plib_gpio.h"
#include "config/pic32mz_w1/bsp/bsp.h"
 
#define APP_SWITCH_PRESSED  0
#define APP_SWITCH_RELEASED 1
    
// *****************************************************************************
void APP_SWITCH_init(void);
// *****************************************************************************
    
#endif /* _APP_SWITCH_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

