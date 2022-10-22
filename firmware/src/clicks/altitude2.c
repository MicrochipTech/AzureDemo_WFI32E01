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
 */

#include "altitude2.h"

// ---------------------------------------------- PRIVATE FUNCTION DECLARATIONS 

static void altitude2_make_conv_comm ( ALTITUDE2_Data *ctx, uint8_t *comm_temp, uint8_t *comm_press );
static void altitude2_i2c_send_comm ( ALTITUDE2_Data *ctx, uint8_t comm_byte, uint32_t *input_data, uint8_t num_bytes );
static void altitude2_i2c_send_comm_resp ( ALTITUDE2_Data *ctx, uint8_t comm_byte, uint32_t *output_data, uint8_t num_bytes );

// ------------------------------------------------ PUBLIC FUNCTION DEFINITIONS

ALTITUDE2_RETVAL ALTITUDE2_init ( ALTITUDE2_Data *ctx )
{
    altitude2_reset( ctx );
    altitude2_set_ratio ( ctx, ALTITUDE2_RATIO_2048, ALTITUDE2_RATIO_2048 );

    return ALTITUDE2_OK;
}

uint8_t altitude2_read_prom( ALTITUDE2_Data *ctx, uint8_t select_data, uint32_t *data_out )
{
    uint8_t tmp_data;

    if( select_data > 7 )
    {
        return 1;
    }

    tmp_data = 0xA0 | ( select_data << 1 );
    altitude2_i2c_send_comm_resp( ctx, tmp_data, data_out, 2 );

    return 0;
}

void altitude2_reset( ALTITUDE2_Data *ctx )
{
    uint8_t comm_data = 0x1E;
    uint8_t cnt;

    altitude2_i2c_send_comm( ctx, comm_data, 0, 0);
    
    for( cnt = 1; cnt < 7; cnt++ )
    {
        altitude2_read_prom( ctx, cnt, &ctx->data_prom[ cnt - 1 ] );
    } 
}

uint8_t altitude2_set_ratio( ALTITUDE2_Data *ctx, uint8_t temp_ratio, uint8_t press_ratio )
{
    if ( temp_ratio > 4 )
    {
        return 1;
    }
    if ( press_ratio > 4 )
    {
        return 2;
    }

    ctx->ratio_temp = temp_ratio;
    ctx->ratio_press = press_ratio;

    return 0;
}

void ALTITUDE2_readData( ALTITUDE2_Data *ctx, float *temp_data, float *press_data, float *altitude_data )
{
    uint32_t buff_data[ 2 ];
    uint8_t temp_comm;
    uint8_t press_comm;
    float res_data[ 4 ];
    float  volatile tmp_var;
        
    altitude2_make_conv_comm( ctx, &temp_comm, &press_comm );
    altitude2_i2c_send_comm( ctx, temp_comm, 0, 0 );
    altitude2_i2c_send_comm_resp( ctx, 0x00, &buff_data[ 0 ], 3 );
    altitude2_i2c_send_comm( ctx, press_comm, 0, 0 );
    altitude2_i2c_send_comm_resp( ctx, 0x00, &buff_data[ 1 ], 3 );
    
    res_data[ 0 ] = ( float )ctx->data_prom[ 4 ] * 256.0;
    res_data[ 0 ] = ( float )buff_data[ 0 ] - res_data[ 0 ];
    res_data[ 1 ] = res_data[ 0 ] / 8.0;
    res_data[ 1 ] = res_data[ 1 ] * ( ( float )ctx->data_prom[ 5 ] / 1048576.0 );
    res_data[ 1 ] = res_data[ 1 ] + 2000.0;
    res_data[ 1 ] = res_data[ 1 ] / 100.0;
    *temp_data = res_data[ 1 ];
    
    res_data[ 2 ] = res_data[ 0 ] / 128.0;
    res_data[ 2 ] = res_data[ 2 ] * ( ctx->data_prom[ 2 ] / 2097152.0 );
    res_data[ 2 ] = res_data[ 2 ] + ( ctx->data_prom[ 0 ] / 32.0 );
    res_data[ 3 ] = res_data[ 0 ] / 32.0;
    res_data[ 3 ] = res_data[ 3 ] * ( ctx->data_prom[ 3 ] / 65536.0 ); 
    res_data[ 3 ] = res_data[ 3 ] + ( ctx->data_prom[ 1 ] * 4.0 );
    res_data[ 1 ] = buff_data[ 1 ] / 32768.0; 
    res_data[ 1 ] = res_data[ 1 ] * res_data[ 2 ];
    res_data[ 1 ] = res_data[ 1 ] - res_data[ 3 ];
    res_data[ 1 ] = res_data[ 1 ] / 100.0;
    *press_data = res_data[ 1 ];
    
    tmp_var = 1013.25 / *press_data;
    *altitude_data = pow( tmp_var, 0.19022256 );
    *altitude_data = *altitude_data - 1;
    tmp_var = *temp_data + 273.15;
    *altitude_data = *altitude_data *  tmp_var;
    *altitude_data = *altitude_data / 0.0065;
}


// ----------------------------------------------- PRIVATE FUNCTION DEFINITIONS

static void altitude2_i2c_send_comm ( ALTITUDE2_Data *ctx, uint8_t comm_byte, uint32_t *input_data, uint8_t num_bytes )
{
    //i2c_master_write( &ctx->i2c, &comm_byte, 1 );
    APP_SENSORS_writeByte(ALTITUDE2_DEVICE_ADDR_1 , comm_byte);
    //Delay_10ms( );
}

static void altitude2_i2c_send_comm_resp ( ALTITUDE2_Data *ctx, uint8_t comm_byte, uint32_t *output_data, uint8_t num_bytes )
{
    uint8_t tmp[ 3 ];
    uint8_t cnt;
    uint32_t pom = 0;

    //i2c_master_write_then_read( &ctx->i2c, &comm_byte, 1, tmp, num_bytes );
    APP_SENSORS_writeRead(ALTITUDE2_DEVICE_ADDR_1, comm_byte, num_bytes);

    for( cnt = 0; cnt < num_bytes; cnt++ )
    {
        pom = pom << 8;
        pom = pom | tmp[ cnt ];
        
    }
    *output_data = pom;
}

static void altitude2_make_conv_comm( ALTITUDE2_Data *ctx, uint8_t *comm_temp, uint8_t *comm_press )
{
    uint8_t comm_byte = 0x50;
    uint8_t ratio_cnt = 0;

    while ( ctx->ratio_temp != ratio_cnt )
    {
        comm_byte += 2;
        ratio_cnt++;
    }

    *comm_temp = comm_byte;
    comm_byte = 0x40;
    ratio_cnt = 0;
    while ( ctx->ratio_press != ratio_cnt )
    {
        comm_byte += 2;
        ratio_cnt++;
    }

    *comm_press = comm_byte;
}

// ------------------------------------------------------------------------- END

