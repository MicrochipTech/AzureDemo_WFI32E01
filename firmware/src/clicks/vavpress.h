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
 * @file vavpress.h
 * @brief This file contains API for VAV Press Click Driver.
 */

#ifndef VAVPRESS_H
#define VAVPRESS_H

#ifdef __cplusplus
extern "C"{
#endif

#include "definitions.h"

/*!
 * @addtogroup vavpress VAV Press Click Driver
 * @brief API for configuring and manipulating VAV Press Click driver.
 * @{
 */

/**
 * @defgroup vavpress_set VAV Press Registers Settings
 * @brief Settings for registers of VAV Press Click driver.
 */

/**
 * @addtogroup vavpress_set
 * @{
 */

/**
 * @brief VAV Press description command set.
 * @details Specified command set for description of VAV Press Click driver.
 */
#define VAVPRESS_SET_CMD_RESET_FIRMWARE                 0x11
#define VAVPRESS_SET_CMD_START_PRESSURE_CONVERSION      0x21
#define VAVPRESS_SET_CMD_RETRIEVE_ELECTRONIC_SIGNATURE  0x23

/**
 * @brief VAV Press device address setting.
 * @details Specified setting for device slave address selection of
 * VAV Press Click driver.
 */
#define VAVPRESS_I2CADDR_0  0x5C // Default I2C address setting for VAV Click
#define VAVPRESS_I2CADDR_1  0x5D
#define VAVPRESS_I2CADDR_2  0x5E
#define VAVPRESS_I2CADDR_3  0x5F

/**
 * @brief VAV Press Click return value data.
 * @details Predefined enum values for driver return values.
 */
typedef enum
{
   VAVPRESS_OK = 0,
   VAVPRESS_ERROR = -1

} vavpress_return_value_t;

/**
 * @brief VAV Press Click electronic signature.
 * @details Electronic signature of VAV Press Click driver.
 */
#define EL_SIGNATURE_NUMBYTES 54

typedef struct
{
   float firmware_version;
   char part_number[ 12 ];
   char lot_number[ 7 ];
   uint16_t pressure_range;
   char output_type;
   uint16_t scale_factor;
   char calibration_id[ 2 ];
   uint8_t week_number;
   uint8_t year_number;
   uint16_t sequence_number;
   
} vavpress_el_signature_data_t;

/**
 * @brief VAV Press Click sensor parameter data.
 * @details Sensor parameter data of VAV Press Click driver.
 */
typedef struct
{
   uint16_t scale_factor_temp;
   uint16_t scale_factor_press;
   uint16_t readout_at_known_temperature;
   float known_temperature_c;

} vavpress_sensor_param_data_t;

/*!
 * @addtogroup vavpress VAV Press Click Driver
 * @brief API for configuring and manipulating VAV Press Click driver.
 * @{
 */

/**
 * @brief VAV Press initialization function.
 * @details This function initializes all necessary pins and peripherals used
 * for this click board.
 * @param[out] ctx : Click context object.
 * See #vavpress_t object definition for detailed explanation.
 * @param[in] cfg : Click configuration structure.
 * See #vavpress_cfg_t object definition for detailed explanation.
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #err_t definition for detailed explanation.
 * @note None.
 *
 * @endcode
 */
void VAVPRESS_init(void);

/**
 * @brief VAV Press default configuration function.
 * @details This function executes a default configuration of VAV Press
 * click board.
 * See #vavpress_t object definition for detailed explanation.
 * @return Nothing.
 *
 * See #err_t definition for detailed explanation.
 * @note This function can consist any necessary configuration or setting to put
 * device into operating mode.
 *
 * @endcode
 */
vavpress_return_value_t VAVPRESS_setDefaultConfig(void);
vavpress_return_value_t VAVPRESS_setDefaultSensorParams(vavpress_sensor_param_data_t *param_data);

/**
 * @brief VAV Press get data readout function.
 * @details This function get differential pressure and temperature data of the 
 * LMIS025BB3, digital low differential pressure sensors on the 
 * Vav Press click board™.
 * See #vavpress_t object definition for detailed explanation.
 * @param[out] press_data : Pressure signed 15-bit value.
 * @param[out] temp_data : Temperature signed 16-bit value.
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #err_t definition for detailed explanation.
 * @note None.
 *
 * @endcode
 */
vavpress_return_value_t VAVPRESS_getReadoutData(int16_t *press_data, int16_t *temp_data);

/**
 * @brief VAV Press get differential pressure and temperature function.
 * @details This function get differential pressure [ Pa ] and temperature [ degree Celsius ] data of the 
 * LMIS025BB3, digital low differential pressure sensors on the 
 * Vav Press click board™.
 * See #vavpress_t object definition for detailed explanation.
 * @param[in] param_data : Pointer to the memory location of the structure where data be stored.
 * @param[out] diff_press : Pressure [ Pa ].
 * @param[out] temperature : Temperature [ degree Celsius ].
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #err_t definition for detailed explanation.
 * @note None.
 *
 * @endcode
 */
vavpress_return_value_t VAVPRESS_getSensorReadings(vavpress_sensor_param_data_t *param_data, float *diff_press, float *temperature);

/**
 * @brief VAV Press retrieve electronic signature function.
 * @details This function retrieve the electronic signature data of the 
 * LMIS025BB3, digital low differential pressure sensors on the 
 * Vav Press click board™.
 * See #vavpress_t object definition for detailed explanation.
 * @param[out] vavpress_el_signature_data_t : Pointer to the memory location of the structure where data be stored.
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #err_t definition for detailed explanation.
 * @note None.
 *
 * @endcode
 */
vavpress_return_value_t VAVPRESS_getElectronicSignature(vavpress_el_signature_data_t *el_signature_data);

#ifdef __cplusplus
}
#endif

#endif // VAVPRESS_H

/*! @} */ // vavpress

// ------------------------------------------------------------------------ END
