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
 * @file pht.h
 * @brief This file contains API for PHT Click Driver.
 */

#ifndef PHT_H
#define PHT_H

#ifdef __cplusplus
extern "C"{
#endif

#include "definitions.h"

    /*!
 * @addtogroup pht PHT Click Driver
 * @brief API for configuring and manipulating PHT Click driver.
 * @{
 */

    /**
 * \defgroup error_code Error Code
 * \{
 */
#define PHT_RETVAL  uint8_t

#define PHT_OK      0x00
#define PHT_ERROR   0xFF

/** \} */
    
/**
 * @defgroup pht_set PHT Registers Settings
 * @brief Settings for registers of PHT Click driver.
 */

/**
 * @addtogroup pht_set
 * @{
 */

/**
 * @brief PHT Command for relative humidity.
 * @details Specified commands for relative humidity of PHT Click driver.
 */
#define PHT_RH_CMD_RESET                                           0xFE
#define PHT_RH_CMD_WRITE_USER_REG                                  0xE6
#define PHT_RH_CMD_READ_USER_REG                                   0xE7
#define PHT_RH_MEASURE_RH_HOLD                                     0xE5
#define PHT_RH_MEASURE_RH_NO_HOLD                                  0xF5

/**
 * @brief PHT Command for pressure and temperature.
 * @details Specified commands for pressure and temperature of PHT Click driver.
 */
#define PHT_PT_CMD_RESET                                           0x1E
#define PHT_PT_CMD_ADC_READ                                        0x00
#define PHT_PT_CMD_CONVERT_D1                                      0x40
#define PHT_PT_CMD_CONVERT_D2                                      0x50    
#define PHT_PT_CMD_PROM_READ_P_T_START                             0xA0
#define PHT_PT_CMD_PROM_READ_P_T_END                               0xAE

#define PHT_PT_CMD_RATIO_256                                       0x00
#define PHT_PT_CMD_RATIO_512                                       0x01
#define PHT_PT_CMD_RATIO_1024                                      0x02
#define PHT_PT_CMD_RATIO_2048                                      0x03
#define PHT_PT_CMD_RATIO_4096                                      0x04
#define PHT_PT_CMD_RATIO_8192                                      0x05

/**
 * @brief PHT coefficient for pressure and temperature.
 * @details Specified coefficient for calculation pressure and temperature of PHT Click driver.
 */
#define PHT_PT_MANUFACTURER_RESERVED                               0x00
#define PHT_PT_C1_SENS                                             0x01
#define PHT_PT_C2_OFF                                              0x02
#define PHT_PT_C3_TCS                                              0x03
#define PHT_PT_C4_TCO                                              0x04
#define PHT_PT_C5_TREF                                             0x05
#define PHT_PT_C6_TEMPSENS                                         0x06
#define PHT_PT_CRC                                                 0x07
#define PHT_COEFFS_MAX                                             6

/**
 * @brief PHT Select output data.
 * @details Select output data of PHT Click driver.
 */
#define PHT_SENSOR_TYPE_RH                                         0x00
#define PHT_SENSOR_TYPE_PT                                         0x01

/**
 * @brief PHT device address setting.
 * @details Specified setting for device slave address selection of
 * PHT Click driver.
 */
#define PHT_I2C_SLAVE_ADDR_RH                                      0x40
#define PHT_I2C_SLAVE_ADDR_P_AND_T                                 0x76

/**
 * @brief Click ctx object definition.
 */
typedef struct pht_s
{
   uint8_t ratio_temp;
   uint8_t ratio_press;
   uint32_t data_prom[ PHT_COEFFS_MAX ];   
} PHT_Data;

/*!
 * @addtogroup pht PHT Click Driver
 * @brief API for configuring and manipulating PHT Click driver.
 * @{
 */

/**
 * @brief PHT initialization function.
 * @details This function initializes all necessary pins and peripherals used
 * for this click board.
 * @param[out] ctx : Click context object.
 * See #pht_t object definition for detailed explanation.
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #err_t definition for detailed explanation.
 */
PHT_RETVAL PHT_init ( PHT_Data *ctx );

/**
 * @brief Calibration Data Read function.
 * @details The function reads calibration data from PROM
 * of MS8607 PHT ( Pressure, Humidity and Temperature ) Combination Sensor
 * on the PHT click board.
 * @param[in] ctx : Click context object.
 * See #pht_t object definition for detailed explanation.
 * @param[in] sel_data : Select coefficient from 0 to 7 which to be read;
 * @param[in] tx_data : Buffer where coefficient be stored;
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #err_t definition for detailed explanation.
 */
PHT_RETVAL pht_get_prom ( PHT_Data *ctx, uint8_t sel_data, uint32_t *tx_data );

/**
 * @brief Set Ratio function.
 * @details The function determines the oversampling ratio value 
 * for temperature and pressure measurements 
 * of MS8607 PHT ( Pressure, Humidity and Temperature ) Combination Sensor
 * on the PHT click board.
 * @param[in] ctx : Click context object.
 * See #pht_t object definition for detailed explanation.
 * @param[in] temp_ratio : Determines the oversampling ratio for temperature measurement:
 * @param[in] press_ratio : Determines the oversampling ratio for pressure measurement:
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #err_t definition for detailed explanation.
 */
PHT_RETVAL pht_set_ratio ( PHT_Data *ctx, uint8_t temp_ratio, uint8_t press_ratio );

/**
 * @brief Reset function.
 * @details The function performs the device reset and 
 * reads calibration coefficients after reset,
 * which are necessary for temperature and pressure calculation
 * of MS8607 PHT ( Pressure, Humidity and Temperature ) Combination Sensor
 * on the PHT click board.
 * @param[in] ctx : Click context object.
 * See #pht_t object definition for detailed explanation.
 * @return Nothing.
 */
void pht_reset ( PHT_Data *ctx );

/**
 * @brief Get temperature and pressure function.
 * @details The function performs temperature and pressure measurements 
 * with desired oversampling ratio and performs
 * the calculations that converts temperature data in celsius value 
 * and pressure data in mbar value 
 * of MS8607 PHT ( Pressure, Humidity and Temperature ) Combination Sensor
 * on the PHT click board.
 * @param[in] ctx : Click context object.
 * See #pht_t object definition for detailed explanation.
 * @param[out] temperature : Pointer to the memory location where temperature (degree Celsius) be stored.
 * @param[out] pressure : Pointer to the memory location where pressure (mBar) be stored.
 * @return Nothing.
 */
void PHT_getTemperaturePressure ( PHT_Data *ctx, float *temperature, float *pressure );

/**
 * @brief Get humidity function.
 * @details The function performs humidity measurements
 * with desired oversampling ratio and performs
 * the calculations that converts humidity data in mBar
 * of MS8607 PHT ( Pressure, Humidity and Temperature ) Combination Sensor
 * on the PHT click board.
 * @param[out] humidity : Pointer to the memory location where humidity (percentage) be stored.
 * @return Nothing.
 */
void PHT_getRelativeHumidity ( float *humidity );

#ifdef __cplusplus
}
#endif
#endif // PHT_H

/*! @} */ // pht

// ------------------------------------------------------------------------ END
