/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_sensors.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_SENSORS_H
#define _APP_SENSORS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************

#define APP_SENSORS_DBG(level,fmt,...) SYS_DEBUG_PRINT(level,"[APP_CTRL] "fmt, ##__VA_ARGS__)
#define APP_SENSORS_PRNT(fmt,...) SYS_CONSOLE_PRINT("[APP_CTRL] "fmt, ##__VA_ARGS__)

// *****************************************************************************
    
// *****************************************************************************

/* MCP9808 registers */
#define MCP9808_I2C_ADDRESS         0x18 
#define MCP9808_REG_CONFIG          0x01
#define MCP9808_REG_TAMBIENT		0x05
#define MCP9808_REG_MANUF_ID		0x06
#define MCP9808_REG_DEVICE_ID		0x07
#define MCP9808_REG_RESOLUTION		0x08

/* MCP9808 settings */
#define MCP9808_CONFIG_DEFAULT		0x00
#define MCP9808_CONFIG_SHUTDOWN		0x0100
#define MCP9808_RES_DEFAULT         62500
#define MCP9808_MANUF_ID            0x54
#define MCP9808_DEVICE_ID           0x0400
#define MCP9808_DEVICE_ID_MASK		0xFF00

/* OPT3001 registers */
#define OPT3001_I2C_ADDRESS             0x44
#define OPT3001_REG_RESULT              0x00
#define OPT3001_REG_CONFIG              0x01
#define OPT3001_REG_LOW_LIMIT           0x02
#define OPT3001_REG_HIGH_LIMIT          0x03
#define OPT3001_REG_MANUFACTURER_ID     0x7E
#define OPT3001_REG_DEVICE_ID           0x7F

/* MCP9808 settings */
#define OPT3001_CONFIG_SHUTDOWN         0x00
#define OPT3001_CONFIG_CONT_CONVERSION  0xCE10
#define OPT3001_MANUF_ID                0x5449
#define OPT3001_DEVICE_ID               0x3001

// *****************************************************************************
/* I2C Operations */
typedef struct
{
    DRV_HANDLE i2cHandle;
    uint8_t  txBuffer[64];
    uint8_t  rxBuffBytes[64];
    uint16_t rxBuffWords[64];
} APP_SENSORS_I2C;

/* MCP9808 Structure */
typedef struct
{
    bool IsShutdown;
    int16_t temperature;
    uint16_t deviceID;
} APP_SENSORS_MCP9808;

/* OPT3001 Structure */
typedef struct
{
    bool IsShutdown;
    uint32_t light;
    uint16_t deviceID;
} APP_SENSORS_OPT3001;

/* APP SENSORS module data structure */
typedef struct
{   
    APP_SENSORS_I2C i2c;   
    APP_SENSORS_MCP9808 mcp9808;
    APP_SENSORS_OPT3001 opt3001;
} APP_SENSORS_DATA;

// *****************************************************************************

void APP_SENSORS_init(void);
void APP_SENSORS_writeByte(uint8_t addr, uint8_t val);
void APP_SENSORS_write(uint8_t addr, uint8_t *buffer, uint8_t size);
void APP_SENSORS_writeWord_MSB_b4_LSB(uint8_t addr, uint16_t reg, uint16_t val);
void APP_SENSORS_writeWord_LSB_b4_MSB(uint8_t addr, uint16_t reg, uint16_t val);
void APP_SENSORS_justRead(uint8_t addr, uint8_t size);
void APP_SENSORS_writeReadBytes(uint8_t addr, uint16_t reg, uint8_t size);
void APP_SENSORS_writeReadWords(uint8_t addr, uint16_t reg, uint8_t size);
int16_t APP_SENSORS_readTemperature(void);
uint32_t APP_SENSORS_readLight(void);

#endif /* _APP_SENSORS_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

