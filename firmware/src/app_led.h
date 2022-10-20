/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_led.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_LED_H
#define _APP_LED_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************

#define APP_LED_DBG(level,fmt,...) SYS_DEBUG_PRINT(level,"[APP_CTRL] "fmt, ##__VA_ARGS__)
#define APP_LED_PRNT(fmt,...) SYS_CONSOLE_PRINT("[APP_CTRL] "fmt, ##__VA_ARGS__)

#define APP_LED_TOTAL 4

// *****************************************************************************
    
// *****************************************************************************

/* LED Colors */
typedef enum 
{
    APP_LED_BLUE = 0,
    APP_LED_GREEN,
    APP_LED_YELLOW,
    APP_LED_RED,
    APP_LED_COLOR_INVALID,
} APP_LED_COLOR;

/* LED Modes */
typedef enum 
{
    APP_LED_OFF = 0,
    APP_LED_ON,
    APP_LED_BLINKING,
    APP_LED_MODE_INVALID,
} APP_LED_MODE;

/* LED Controller Structure */
typedef struct
{
    APP_LED_MODE mode;    
} APP_LED_CTRL;

typedef struct
{
    bool wifi;
    bool cloud;
    bool alarm;
} APP_CONNECT_STATUS;

// *****************************************************************************

void APP_LED_init(void);
void APP_LED_refresh(void);

#endif /* _APP_LED_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

