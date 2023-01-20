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
 * @file pht.c
 * @brief PHT Click Driver.
 */

#include "pht.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

PHT_RETVAL PHT_status;

// -------------------------------------------- PRIVATE FUNCTION DECLARATIONS 

/**
 * @brief Command Send function
 * @details The function performs the desired command 
 * by determined communication.
 * @param[out] cfg : Click configuration structure.
 * See #pht_cfg_t object definition for detailed explanation.
 * @param[in] cmd_byte : Command which be performed
 */
static void dev_send_cmd ( uint8_t addr, uint8_t cmd_byte );

/**
 * @brief Send Command With Device Response function
 * @details The function performs the desired command 
 * by determined communication and reads response from the device.
 * @param[in] cfg : Click configuration structure.
 * See #pht_cfg_t object definition for detailed explanation.
 * @param[in] num_bytes : Number of bytes which be read
 * @param[in] tx_data : Buffer where data (device response) be stored
 * @param[in] cmd_byte : Command which be performed
 */
static void dev_send_cmd_resp ( PHT_Data *ctx, uint8_t addr, uint8_t cmd_byte, uint8_t num_bytes, uint32_t *output_data );

/**
 * @brief Conversion Command Make function
 * @details The function makes the conversion command byte for desired ratio.
 * @param[out] cmd_temp : Stores the conversion command for desired temperature ratio
 * @param[out] cmd_press : Stores the conversion command for desired pressure ratio
 */
static void dev_make_conv_cmd ( PHT_Data *ctx, uint8_t *cmd_temp, uint8_t *cmd_press );

// --------------------------------------------------------- PRIVATE FUNCTIONS 

static void dev_send_cmd ( uint8_t addr, uint8_t cmd_byte )
{
    APP_SENSORS_writeByte(addr, cmd_byte);
}

static void dev_send_cmd_resp ( PHT_Data *ctx, uint8_t addr, uint8_t cmd_byte, uint8_t num_bytes, uint32_t *output_data )
{
    uint8_t tmp[ 3 ];
    uint8_t cnt;
    uint32_t pom = 0;

    tx_thread_sleep(10);
    APP_SENSORS_writeReadBytes(addr, cmd_byte, num_bytes);

    for( cnt = 0; cnt < num_bytes; cnt++ )
    {
        tmp[ cnt ] = APP_SENSORS_data.i2c.rxBuffBytes[ cnt ];
        pom = pom << 8;
        pom = pom | tmp[ cnt ];
        
    }
    *output_data = pom;
}

static void dev_make_conv_cmd ( PHT_Data *ctx, uint8_t *cmd_temp, uint8_t *cmd_press )
{
    uint8_t cmd_byte;
    uint8_t ratio_cnt;
    
    cmd_byte = PHT_PT_CMD_CONVERT_D2;
    ratio_cnt = 0;

    while ( ctx->ratio_temp != ratio_cnt )
    {
        cmd_byte += 2;
        ratio_cnt++;
    }
    
    *cmd_temp = cmd_byte;

    cmd_byte = PHT_PT_CMD_CONVERT_D1;
    ratio_cnt = 0;
    
    while ( ctx->ratio_press != ratio_cnt )
    {
        cmd_byte += 2;
        ratio_cnt++;
    }
    
    *cmd_press = cmd_byte;
}

// --------------------------------------------------------- PUBLIC FUNCTIONS 

PHT_RETVAL PHT_init ( PHT_Data *ctx )
{
    for (int index = 0; index < PHT_COEFFS_MAX; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[0] = 0;
    }

    pht_reset( ctx );
    pht_set_ratio( ctx, PHT_PT_CMD_RATIO_2048, PHT_PT_CMD_RATIO_2048);

    if ( (ctx->data_prom[0] == ctx->data_prom[1]) &&
         (ctx->data_prom[1] == ctx->data_prom[2]) && 
         (ctx->data_prom[2] == ctx->data_prom[3]) &&
         (ctx->data_prom[3] == ctx->data_prom[4]) && 
         (ctx->data_prom[4] == ctx->data_prom[5])
       )
    {
        return PHT_ERROR;
    }
    else
    {
        return PHT_OK;
    }
}

PHT_RETVAL pht_get_prom ( PHT_Data *ctx, uint8_t sel_data, uint32_t *tx_data )
{
    uint8_t tmp_data;

    if ( sel_data > (PHT_COEFFS_MAX+1) )
    {
        return PHT_ERROR;
    }

    tmp_data = PHT_PT_CMD_PROM_READ_P_T_START | ( sel_data << 1 );
    dev_send_cmd_resp( ctx, PHT_I2C_SLAVE_ADDR_P_AND_T, tmp_data, 2, tx_data );

    return PHT_OK;
}

PHT_RETVAL pht_set_ratio ( PHT_Data *ctx, uint8_t temp_ratio, uint8_t press_ratio )
{
    if ( temp_ratio > PHT_PT_CMD_RATIO_8192 )
    {
        return PHT_ERROR;
    }

    if ( temp_ratio > PHT_PT_CMD_RATIO_8192 )
    {
        return PHT_ERROR;
    }

    ctx->ratio_temp = temp_ratio;
    ctx->ratio_press = press_ratio;

    return PHT_OK;
}

void pht_reset ( PHT_Data *ctx )
{
    uint8_t n_cnt;

    dev_send_cmd( PHT_I2C_SLAVE_ADDR_P_AND_T, PHT_PT_CMD_RESET );

    for ( n_cnt = 1; n_cnt < (PHT_COEFFS_MAX+1); n_cnt++ ) {
        pht_get_prom( ctx, n_cnt, &ctx->data_prom[ n_cnt - 1 ] );
    }
}

void PHT_getTemperaturePressure ( PHT_Data *ctx, float *temperature, float *pressure )
{
    uint32_t buff_data[ 2 ];
    uint8_t temp_cmd;
    uint8_t press_cmd;
    float res_data[ 4 ];
    
    dev_make_conv_cmd( ctx, &temp_cmd, &press_cmd );
    dev_send_cmd( PHT_I2C_SLAVE_ADDR_P_AND_T, temp_cmd );
    dev_send_cmd_resp( ctx, PHT_I2C_SLAVE_ADDR_P_AND_T, PHT_PT_CMD_ADC_READ, 3, &buff_data[ 0 ] );
    dev_send_cmd( PHT_I2C_SLAVE_ADDR_P_AND_T, press_cmd );
    dev_send_cmd_resp( ctx, PHT_I2C_SLAVE_ADDR_P_AND_T, PHT_PT_CMD_ADC_READ, 3, &buff_data[ 1 ] );

    res_data[ 0 ] = ctx->data_prom[ 4 ] * 256.0;
    res_data[ 0 ] = buff_data[ 0 ] - res_data[ 0 ];
    res_data[ 1 ] = res_data[ 0 ] / 8.0;
    res_data[ 1 ] = res_data[ 1 ] * ( ctx->data_prom[ 5 ] / 1048576.0 );
    res_data[ 1 ] = res_data[ 1 ] + 2000.0;
    res_data[ 1 ] /= 100.0;
    *temperature = res_data[ 1 ];

    res_data[ 2 ] = res_data[ 0 ] / 128.0;
    res_data[ 2 ] = res_data[ 2 ] * ( ctx->data_prom[ 2 ] / 2097152.0 );
    res_data[ 2 ] = res_data[ 2 ] + ( ctx->data_prom[ 0 ] / 32.0 );
    res_data[ 3 ] = res_data[ 0 ] / 32.0;
    res_data[ 3 ] = res_data[ 3 ] * ( ctx->data_prom[ 3 ] / 65536.0 );
    res_data[ 3 ] = res_data[ 3 ] + ( ctx->data_prom[ 1 ] * 4.0 );
    res_data[ 1 ] = buff_data[ 1 ] / 32768.0;
    res_data[ 1 ] = res_data[ 1 ] * res_data[ 2 ];
    res_data[ 1 ] = res_data[ 1 ] - res_data[ 3 ];
    res_data[ 1 ] /= 100.0;
    *pressure = res_data[ 1 ];
}

void PHT_getRelativeHumidity ( float *humidity )
{
    uint16_t rh_val;
    float rh;
   
    APP_SENSORS_writeByte(PHT_I2C_SLAVE_ADDR_RH, PHT_RH_MEASURE_RH_HOLD);
    tx_thread_sleep(20);
    APP_SENSORS_justRead(PHT_I2C_SLAVE_ADDR_RH, 2);
        
    rh_val = APP_SENSORS_data.i2c.rxBuffBytes[ 0 ];
    rh_val <<= 8;
    rh_val |= APP_SENSORS_data.i2c.rxBuffBytes[ 1 ];

    rh = ( float ) rh_val;
    rh *= 12500.0;
    rh /= 65536.0;
    rh -= 600.0;
    rh /= 100.0;

    *humidity = rh;
}

// ------------------------------------------------------------------------- END
