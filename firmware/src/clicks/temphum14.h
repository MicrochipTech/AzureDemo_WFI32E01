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
 * @file temphum14.h
 * @brief This file contains API for Temp Hum 14 Click Driver.
 */

#ifndef TEMPHUM14_H
#define TEMPHUM14_H

#include "definitions.h"

#ifdef __cplusplus
extern "C"{
#endif

/*!
 * @addtogroup temphum14 Temp Hum 14 Click Driver
 * @brief API for configuring and manipulating Temp Hum 14 Click driver.
 * @{
 */

/**
 * @defgroup temphum14_reg Temp Hum 14 Registers List
 * @brief List of registers of Temp Hum 14 Click driver.
 */

/**
 * @addtogroup temphum14_reg
 * @{
 */

/**
 * @brief Temp Hum 14 description register.
 * @details Specified register for description of Temp Hum 14 Click driver.
 */

/**
 * @brief Temp Hum 14 device address setting.
 * @details Specified setting for device slave address selection of
 * Temp Hum 14 Click driver.
 */
#define TEMPHUM14_I2C_SLAVE_ADDR_GND                     0x40
#define TEMPHUM14_I2C_SLAVE_ADDR_VCC                     0x41

/**
 * @brief Temp Hum 14 device commands.
 * @details Specified commands of Temp Hum 14 Click driver.
 */
#define TEMPHUM14_CMD_RESET                              0x1E
#define TEMPHUM14_CMD_HEATER_ON                          0x04
#define TEMPHUM14_CMD_HEATER_OFF                         0x02
#define TEMPHUM14_CMD_READ_T_AND_RH                      0x00
#define TEMPHUM14_CMD_READ_RH                            0x10
#define TEMPHUM14_CMD_CONVERSION                         0x40
#define TEMPHUM14_CMD_READ_DIAGNOSTIC                    0x08
#define TEMPHUM14_CMD_READ_SERIAL_NUMBER                 0x0A

#define TEMPHUM14_TEMPERATURE_NUMBER_BYTES               3
#define TEMPHUM14_HUMIDITY_NUMBER_BYTES                  3    
#define TEMPHUM14_SERIAL_NUMBER_BYTES                    6
    
/**
 * @brief Temp Hum 14 device conversion settings.
 * @details Specified conversion settings of Temp Hum 14 Click driver.
 */
#define TEMPHUM14_CONVERSION_HUM_OSR_0_007               0x18
#define TEMPHUM14_CONVERSION_HUM_OSR_0_010               0x10
#define TEMPHUM14_CONVERSION_HUM_OSR_0_014               0x08
#define TEMPHUM14_CONVERSION_HUM_OSR_0_020               0x00
#define TEMPHUM14_CONVERSION_TEMP_0_012                  0x06
#define TEMPHUM14_CONVERSION_TEMP_0_016                  0x04
#define TEMPHUM14_CONVERSION_TEMP_0_025                  0x02
#define TEMPHUM14_CONVERSION_TEMP_0_040                  0x00

/**
 * @brief Temp Hum 14 device bit mask settings.
 * @details Specified bit mask settings of Temp Hum 14 Click driver.
 */
#define TEMPHUM14_BIT_MASK_HUM_OSR                       0x18
#define TEMPHUM14_BIT_MASK_TEMP_OSR                      0x06

/**
 * @brief Temp Hum 14 device heater turning settings.
 * @details Specified heater turning settings of Temp Hum 14 Click driver.
 */
#define TEMPHUM14_HEATER_DISABLE                         0x00
#define TEMPHUM14_HEATER_ENABLE                          0x01

/**
 * @brief Temp Hum 14 device status on and off.
 * @details Specified status on and off of Temp Hum 14 Click driver.
 */
#define TEMPHUM14_STATUS_OFF                             0x00
#define TEMPHUM14_STATUS_ON                              0x01

// Calculation
/**
 * @brief Temp Hum 14 device calculation settings.
 * @details Specified calculation settings of Temp Hum 14 Click driver.
 */
#define TEMPHUM14_CALC_16_BIT_DIVIDER                    65535.0
#define TEMPHUM14_CALC_TEMP_MULTI_FACT                   165.000
#define TEMPHUM14_CALC_TEMP_SUB_FACT                     40.0000
#define TEMPHUM14_CALC_HUM_MULTI_FACT                    100.000

/*! @} */ // temphum14_reg


/**
 * @defgroup temphum14_map Temp Hum 14 MikroBUS Map
 * @brief MikroBUS pin mapping of Temp Hum 14 Click driver.
 */

/**
 * @addtogroup temphum14_map
 * @{
 */

/**
 * @brief Temp Hum 14 Click return value data.
 * @details Predefined enum values for driver return values.
 */
typedef enum
{
   TEMPHUM14_OK = 0,
   TEMPHUM14_ERROR = -1

} temphum14_return_value_t;

typedef struct
{
    uint8_t nvm_error;
    uint8_t hum_un_over;
    uint8_t hum_h_err;
    uint8_t hum_l_err;
    uint8_t temp_un_over;
    uint8_t temp_h_err;
    uint8_t temp_l_err;
    uint8_t heater_on;
} temphum14_diagn_t;

/*!
 * @addtogroup temphum14 Temp Hum 14 Click Driver
 * @brief API for configuring and manipulating Temp Hum 14 Click driver.
 * @{
 */

/**
 * @brief Temp Hum 14 initialization function.
 * @details This function initializes all necessary pins and peripherals used
 * for this click board.
 * @param[out] ctx : Click context object.
 * See #temphum14_t object definition for detailed explanation.
 * @param[in] cfg : Click configuration structure.
 * See #temphum14_cfg_t object definition for detailed explanation.
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #err_t definition for detailed explanation.
 */
uint32_t TEMPHUM14_init ( uint8_t addr );

/**
 * @brief Send command function.
 * @details The function send the desired command
 * to the HTU31D RH/T SENSOR IC Digital Relative Humidity sensor with
 * Temperature output on the Temp-Hum 14 click board.
 * @param[in] ctx : Click context object.
 * See #temphum14_t object definition for detailed explanation.
 * @param[in] cmd : 8-bit command.
 * @return Nothing.
 */
void temphum14_set_cmd ( uint8_t addr, uint8_t cmd );

/**
 * @brief Set conversion function.
 * @details The function set conversion
 * a single temperature and humidity conversion and select data resolution
 * to the HTU31D RH/T SENSOR IC Digital Relative Humidity sensor with
 * Temperature output on the Temp-Hum 14 click board.
 * @param[in] ctx : Click context object.
 * See #temphum14_t object definition for detailed explanation.
 * @param[in] hum_osr : Humidity OSR
 * See #temphum14_reg object definition for detailed explanation.
 * @param[in] temp_osr : Temperature OSR
 * See #temphum14_reg object definition for detailed explanation.
 * @return Nothing.
 */
void TEMPHUM14_setConversion ( uint8_t addr, uint8_t hum_osr, uint8_t temp_osr );

/**
 * @brief Get temperature and humidity data function.
 * @details The function get temperature and humidity value
 * of the HTU31D RH/T SENSOR IC Digital Relative Humidity sensor with
 * Temperature output on the Temp-Hum 14 click board.
 * @param[in] ctx : Click context object.
 * See #temphum14_t object definition for detailed explanation.
 * @param[out] temp : Pointer to the memory location where temperature (degree Celsius) be stored.
 * @param[out] hum : Pointer to the memory location where humidity (percentage) be stored.
 * @return Nothing.
 */
void TEMPHUM14_getTemperatureHumidity ( uint8_t addr, float *temp, float *hum );

/**
 * @brief Soft reset function.
 * @details The function is performed by a software reset
 * of the HTU31D RH/T SENSOR IC Digital Relative Humidity sensor with
 * Temperature output on the Temp-Hum 14 click board.
 * @param[in] ctx : Click context object.
 * See #temphum14_t object definition for detailed explanation.
 * @return Nothing.
 */
void TEMPHUM14_softReset ( uint8_t addr );

/**
 * @brief Enable heater function.
 * @details The function set heater status ( disable or enable )
 * of the HTU31D RH/T SENSOR IC Digital Relative Humidity sensor with
 * Temperature output on the Temp-Hum 14 click board.
 * @param[in] en_heater : Heater enable and disable.
 * @param[in] ctx : Click context object.
 * See #temphum14_t object definition for detailed explanation.
 * @return Nothing.
 */
void TEMPHUM14_setHeater ( uint8_t addr, uint8_t en_heater );

/**
 * @brief Get diagnostic status function.
 * @details The function get diagnostic status
 * of the HTU31D RH/T SENSOR IC Digital Relative Humidity sensor with
 * Temperature output on the Temp-Hum 14 click board.
 * @param[in] ctx : Click context object.
 * See #temphum14_t object definition for detailed explanation.
 * @param[out] diag_data : pointer to the memory location where structure where data be stored.
 * @return Nothing.
 */
void TEMPHUM14_getDiagnostic ( uint8_t addr, temphum14_diagn_t *diag_data );

/**
 * @brief Get serial number function.
 * @details The function get the serial number
 * of the HTU31D RH/T SENSOR IC Digital Relative Humidity sensor with
 * Temperature output on the Temp-Hum 14 click board.
 * @param[in] ctx : Click context object.
 * See #temphum14_t object definition for detailed explanation.
 * @return 24-bit serial number.
 */
uint32_t TEMPHUM14_getSerialNumber ( uint8_t addr );

#ifdef __cplusplus
}
#endif
#endif // TEMPHUM14_H

/*! @} */ // temphum14

// ------------------------------------------------------------------------ END
