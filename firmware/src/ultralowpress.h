/****************************************************************************
** Copyright (C) 2020 MikroElektronika d.o.o.
** Contact: https://www.mikroe.com/contact
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
**  USE OR OTHER DEALINGS IN THE SOFTWARE.
****************************************************************************/

/*!
 * @file ultralowpress.h
 * @brief This file contains API for Ultra-Low Press Click Driver.
 */

#ifndef ULTRALOWPRESS_H
#define ULTRALOWPRESS_H

#ifdef __cplusplus
extern "C"{
#endif

#include "definitions.h"
#include "app_sensors.h"

/**
 * @brief Ultra-Low Press device I2C address setting.
 * @details Specified setting for I2C slave address selection of
 * Ultra-Low Press Click driver.
 */
#define ULTRALOWPRESS_I2CADDR      0x6C
    
/**
 * @brief Ultra-Low Press description register.
 * @details Specified register for description of Ultra-Low Press Click driver.
 */
#define ULTRALOWPRESS_REG_CMD           0x22
#define ULTRALOWPRESS_REG_TEMP          0x2E
#define ULTRALOWPRESS_REG_PRESS         0x30
#define ULTRALOWPRESS_REG_STATUS_SYNC   0x32
#define ULTRALOWPRESS_REG_STATUS        0x36
#define ULTRALOWPRESS_REG_SERIAL_NUM_L  0x50
#define ULTRALOWPRESS_REG_SERIAL_NUM_H  0x52

/**
 * @brief Ultra-Low Press description setting.
 * @details Specified setting for description of Ultra-Low Press Click driver.
 */
#define ULTRALOWPRESS_REG_CMD_SLEEP     0x6C32
#define ULTRALOWPRESS_REG_CMD_RESET     0xB169
#define ULTRALOWPRESS_STATUS_CLEAR      0xFFFF

/**
 * @brief Temperature sensitivity/offset macros
 * @details Temperature sensitivity and offset macros for converting data to Celcius.
 */
#define ULTRALOWPRESS_B1                397.2
#define ULTRALOWPRESS_B0                -16881

/**
 * @brief Pressure MIN/MAX macros
 * @details Pressure MIN/MAX macros for converting data to Pascal.
 */
#define ULTRALOWPRESS_P_MIN             -20.0
#define ULTRALOWPRESS_P_MAX             500.0
#define ULTRALOWPRESS_OUT_MIN           -26215.0
#define ULTRALOWPRESS_OUT_MAX           26214.0

/**
 * @brief Status masks
 * @details Mask for temperature and pressure data ready to read.
 */
#define ULTRALOWPRESS_STATUS_TEMP_MASK  0x0010
#define ULTRALOWPRESS_STATUS_PRESS_MASK 0x0008
/**
 * @brief Ultra-Low Press Click return value data.
 * @details Predefined enum values for driver return values.
 */
typedef enum
{
   ULTRALOWPRESS_OK = 0,
   ULTRALOWPRESS_ERROR = -1

} ultralowpress_return_value_t;

/*!
 * @addtogroup ultralowpress Ultra-Low Press Click Driver
 * @brief API for configuring and manipulating Ultra-Low Press Click driver.
 * @{
 */

/**
 * @brief Ultra-Low Press initialization function
 * @return Serial number read from the SM8436
 */
uint32_t ULTRALOWPRESS_init(void);

/**
 * @brief Clear status
 * @details Clears status register by writing 0xFFFF to it.
 * @return Nothing.
 */
void ULTRALOWPRESS_clearStatus(void);

/**
 * @brief Ready to read?
 * @details Reads status and checks if status bits for temperature and pressure data is ready.
 * @return @li @c 1 - Temperature and pressure data ready to read.
 *         @li @c 0 - Temperature and pressure data *not* ready to read.
 */
bool ULTRALOWPRESS_isReady(void);

/**
 * @brief Read pressure
 * @details Reads pressure from register and calculates and converts data to Pascal data.
 * @return Pressure data in Pascal.
 */
float ULTRALOWPRESS_getPressure(void);

/**
 * @brief Read temperature
 * @details Reads temperature from register and calculates and converts data to Celsius data.
 * @return Temperature data in Celsius.
 */
float ULTRALOWPRESS_getTemperature(void);

#ifdef __cplusplus
}
#endif

#endif // ULTRALOWPRESS_H

/*! @} */ // ultralowpress

// ------------------------------------------------------------------------ END
