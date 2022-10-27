/*
 * MikroSDK - MikroE Software Development Kit
 * Copyright© 2020 MikroElektronika d.o.o.
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
 * OR OTHER DEALINGS IN THE SOFTWARE. 
 */

/*!
 * \file
 *
 * \brief This file contains API for Altitude 2 Click driver.
 *
 * \addtogroup altitude2 Altitude 2 Click Driver
 * @{
 */
// ----------------------------------------------------------------------------

#ifndef ALTITUDE2_H
#define ALTITUDE2_H

#include <math.h>
#include "definitions.h"

// -------------------------------------------------------------- PUBLIC MACROS 
/**
 * \defgroup macros Macros
 * \{
 */


/**
 * \defgroup error_code Error Code
 * \{
 */
#define ALTITUDE2_RETVAL  uint8_t

#define ALTITUDE2_OK                                  0x00
#define ALTITUDE2_INIT_ERROR                          0xFF
/** \} */

/**
 * \defgroup device_adr Device addresses
 * \{
 */
#define ALTITUDE2_DEVICE_ADDR_0                       0x76
#define ALTITUDE2_DEVICE_ADDR_1                       0x77
/** \} */


/**
 * \defgroup o_sampling_ratio_values Oversampling ratio values
 * \{
 */
#define ALTITUDE2_RATIO_256                           0x00       
#define ALTITUDE2_RATIO_512                           0x01
#define ALTITUDE2_RATIO_1024                          0x02
#define ALTITUDE2_RATIO_2048                          0x03
#define ALTITUDE2_RATIO_4096                          0x04
/** \} */


/**
 * \defgroup calibration_data Calibration data - coefficients
 * \{
 */
#define ALTITUDE2_MANUFACTURER_RESERVED               0x00
#define ALTITUDE2_C1_SENS                             0x01
#define ALTITUDE2_C2_OFF                              0x02
#define ALTITUDE2_C3_TCS                              0x03
#define ALTITUDE2_C4_TCO                              0x04
#define ALTITUDE2_C5_TREF                             0x05 
#define ALTITUDE2_C6_TEMPSENS                         0x06
#define ALTITUDE2_CRC                                 0x07
#define ALTITUDE2_COEFFS_MAX                          6
/** \} */



/** \} */ // End group macro 
// --------------------------------------------------------------- PUBLIC TYPES
/**
 * \defgroup type Types
 * \{
 */


/**
 * @brief Click ctx object definition.
 */
typedef struct altitude2_s
{
   uint8_t ratio_temp;
   uint8_t ratio_press;
   volatile uint32_t data_prom[ 6 ];   
} ALTITUDE2_Data;


/** \} */ // End types group
// ------------------------------------------------------------------ CONSTANTS
/**
 * \defgroup constants Constants
 * \{
 */
 
 

/** \} */ // End constants group
// ------------------------------------------------------------------ VARIABLES
/**
 * \defgroup variable Variable
 * \{
 */


/** \} */ // End variable group
// ----------------------------------------------- PUBLIC FUNCTION DECLARATIONS
/**
 * \defgroup public_function Public function
 * \{
 */

#ifdef __cplusplus
extern "C"{
#endif


/**
 * @brief Initialization function.
 * @param altitude2 Click object.
 * @param cfg Click configuration structure.
 * 
 * @description This function initializes all necessary pins and peripherals used for this click.
 */
ALTITUDE2_RETVAL ALTITUDE2_init ( ALTITUDE2_Data *ctx );

/**
 * @brief Calibration data read function.
 * @param ctx              Click object.
 * @param select_data            Select coefficient from 0 to 7.
 * @param data_out               Coefficient returned by function.
 * 
 * @returns                      0-Ok, 1 - Wrong select_data parameter.
 * @description This function reads calibration data from PROM.
 */
uint8_t altitude2_read_prom ( ALTITUDE2_Data *ctx, uint8_t select_data, uint32_t *data_out );

/**
 * @brief Reset function.
 * @param ctx              Click object.
 * 
 * @description This function resets the device and reads calibration coefficients after reset.
 */
void altitude2_reset( ALTITUDE2_Data *ctx );


/**
 * @brief Set ratio function.
 * @param ctx                    Click object.
 * @param temp_ratio             Determines oversampling ratio for temperature measurement.
 * @param press_ratio            Determines oversampling ration for pressure measurement.
 * 
 * @returns                      0-Ok, 1 - Wrong temp_ratio parameter, 2 - Wrong press_ratio parameter.
 * @description This function determines oversampling ratio for temperature and pressure measurement.
 */
uint8_t altitude2_set_ratio( ALTITUDE2_Data *ctx, uint8_t temp_ratio, uint8_t press_ratio );


/**
 * @brief Data read function.
 *
 * @param ctx                    Click object.
 * @param temp_data              Stores temperature data in celsius.
 * @param press_data             Stores pressure data in mbar.
 * @param altitude_data          Stores altitude data in meters.
 *
 * @description This function performs pressure and temperature measurements
 * and calculates temperature data in celsius and pressure data in mbar. Depending
 * on the temperature and pressure data, function calculates altitude in meters.
 */
void ALTITUDE2_readData( ALTITUDE2_Data *ctx, float *temp_data, float *press_data, float *altitude_data );


#ifdef __cplusplus
}
#endif
#endif  // _ALTITUDE2_H_

/** \} */ // End public_function group
/// \}    // End click Driver group  
/*! @} */
// ------------------------------------------------------------------------- END
