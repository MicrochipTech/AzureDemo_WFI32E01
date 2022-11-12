/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/
#include "app_sensors.h"

APP_SENSORS_DATA APP_SENSORS_data;

void APP_SENSORS_writeByte(uint8_t addr, uint8_t val)
{
    APP_SENSORS_data.i2c.txBuffer[0] = (uint8_t)val;
    
    DRV_I2C_WriteTransfer(APP_SENSORS_data.i2c.i2cHandle,
            addr, (void*)APP_SENSORS_data.i2c.txBuffer, 1);
}

void APP_SENSORS_write(uint8_t addr, uint8_t *buffer, uint8_t size)
{
    memcpy(APP_SENSORS_data.i2c.txBuffer, buffer, size);
    
    DRV_I2C_WriteTransfer(APP_SENSORS_data.i2c.i2cHandle,
            addr, (void*)APP_SENSORS_data.i2c.txBuffer, size);
}

void APP_SENSORS_writeWord_MSB_b4_LSB(uint8_t addr, uint16_t reg, uint16_t val)
{
    APP_SENSORS_data.i2c.txBuffer[0] = (uint8_t)reg;
    APP_SENSORS_data.i2c.txBuffer[1] = (uint8_t)(val >> 8);
    APP_SENSORS_data.i2c.txBuffer[2] = (uint8_t)(val & 0x00FF);
    
    DRV_I2C_WriteTransfer(APP_SENSORS_data.i2c.i2cHandle,
            addr, (void*)APP_SENSORS_data.i2c.txBuffer, 3);
}

void APP_SENSORS_writeWord_LSB_b4_MSB(uint8_t addr, uint16_t reg, uint16_t val)
{
    APP_SENSORS_data.i2c.txBuffer[0] = (uint8_t)reg;
    APP_SENSORS_data.i2c.txBuffer[1] = (uint8_t)(val & 0x00FF);
    APP_SENSORS_data.i2c.txBuffer[2] = (uint8_t)(val >> 8);
    
    DRV_I2C_WriteTransfer(APP_SENSORS_data.i2c.i2cHandle,
            addr, (void*)APP_SENSORS_data.i2c.txBuffer, 3);
}

void APP_SENSORS_justRead(uint8_t addr, uint8_t size)
{
    DRV_I2C_ReadTransfer(APP_SENSORS_data.i2c.i2cHandle, 
            addr, (void*)&APP_SENSORS_data.i2c.rxBuffBytes, size);
}

void APP_SENSORS_writeReadBytes(uint8_t addr, uint16_t reg, uint8_t size)
{
    APP_SENSORS_data.i2c.txBuffer[0] = (uint8_t)reg;
    
    DRV_I2C_WriteReadTransfer(APP_SENSORS_data.i2c.i2cHandle, 
            addr, 
            (void*)APP_SENSORS_data.i2c.txBuffer, 1, 
            (void*)&APP_SENSORS_data.i2c.rxBuffBytes, size);
}

void APP_SENSORS_writeReadWords(uint8_t addr, uint16_t reg, uint8_t size)
{
    APP_SENSORS_data.i2c.txBuffer[0] = (uint8_t)reg;
    
    DRV_I2C_WriteReadTransfer(APP_SENSORS_data.i2c.i2cHandle, 
            addr, 
            (void*)APP_SENSORS_data.i2c.txBuffer, 1, 
            (void*)&APP_SENSORS_data.i2c.rxBuffWords, size);
}

void APP_SENSORS_process(uint8_t addr, uint8_t reg)
{       
    APP_SENSORS_data.i2c.rxBuffWords[0] = (APP_SENSORS_data.i2c.rxBuffWords[0] << 8) | (APP_SENSORS_data.i2c.rxBuffWords[0] >> 8);
    //APP_CTRL_DBG(SYS_ERROR_DEBUG, "I2C read complete - periph addr %x val %x\r\n", addr, sensorsData.i2c.rxBuffWords);
    switch(addr)
    {   
        case MCP9808_I2C_ADDRESS:
            if (reg == MCP9808_REG_TAMBIENT)
            {
                uint8_t upperByte = (uint8_t)(APP_SENSORS_data.i2c.rxBuffWords[0] >> 8);
                uint8_t lowerByte = ((uint8_t)(APP_SENSORS_data.i2c.rxBuffWords[0] & 0x00FF));
                upperByte = upperByte & 0x1F;
                if ((upperByte & 0x10) == 0x10)
                {   // Ta < 0 degC
                    upperByte = upperByte & 0x0F;       // Clear sign bit
                    APP_SENSORS_data.mcp9808.temperature = 256 - ((upperByte * 16) + lowerByte/16);
                }
                else
                {
                    APP_SENSORS_data.mcp9808.temperature = ((upperByte * 16) + lowerByte/16);
                }
                //APP_CTRL_DBG(SYS_ERROR_INFO, "MCP9808 Temperature %d (C)\r\n", sensorsData.mcp9808.temperature);                
            }
            else if (reg == MCP9808_REG_DEVICE_ID)
            {
                APP_SENSORS_data.mcp9808.deviceID = APP_SENSORS_data.i2c.rxBuffWords[0];
                //APP_CTRL_DBG(SYS_ERROR_INFO, "MCP9808 Device ID %x\r\n", sensorsData.mcp9808.deviceID);                
            }
            break;
        case OPT3001_I2C_ADDRESS:
            if (reg == OPT3001_REG_RESULT)
            {
                uint16_t m = APP_SENSORS_data.i2c.rxBuffWords[0] & 0x0FFF;
                uint16_t e = (APP_SENSORS_data.i2c.rxBuffWords[0] & 0xF000) >> 12;
                APP_SENSORS_data.opt3001.light = (m*pow(2,e))/100;
                //APP_CTRL_DBG(SYS_ERROR_INFO, "OPT3001 Light %d (lux)\r\n", sensorsData.opt3001.light); 
            }
            else if (reg == OPT3001_REG_DEVICE_ID)
            {
                APP_SENSORS_data.opt3001.deviceID = APP_SENSORS_data.i2c.rxBuffWords[0];
                //APP_CTRL_DBG(SYS_ERROR_INFO, "OPT3001 Device ID %x\r\n", sensorsData.opt3001.deviceID);                
            }
            break;
        default:
            break;
    }
}

void APP_SENSORS_init(void)
{
    memset(&APP_SENSORS_data.mcp9808, 0, sizeof(APP_SENSORS_data.mcp9808));
    memset(&APP_SENSORS_data.opt3001, 0, sizeof(APP_SENSORS_data.opt3001));
    memset(&APP_SENSORS_data.i2c, 0, sizeof(APP_SENSORS_data.i2c));

    APP_SENSORS_data.i2c.i2cHandle = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);
    if (APP_SENSORS_data.i2c.i2cHandle == DRV_HANDLE_INVALID)
    {
        APP_SENSORS_DBG(SYS_ERROR_ERROR, "Failed to open I2C driver for reading sensors!\r\n");
    }
#ifdef WFI32IOT_SENSORS
    APP_SENSORS_writeWord_MSB_b4_LSB(MCP9808_I2C_ADDRESS, MCP9808_REG_CONFIG, MCP9808_CONFIG_DEFAULT);
    APP_SENSORS_writeWord_MSB_b4_LSB(OPT3001_I2C_ADDRESS, OPT3001_REG_CONFIG, OPT3001_CONFIG_CONT_CONVERSION);
#endif /* WFI32IOT_SENSORS */
}

int16_t APP_SENSORS_readTemperature(void)
{
    APP_SENSORS_writeReadWords(MCP9808_I2C_ADDRESS, MCP9808_REG_TAMBIENT, 2);
    APP_SENSORS_process(MCP9808_I2C_ADDRESS, MCP9808_REG_TAMBIENT);

    return (APP_SENSORS_data.mcp9808.temperature);
}

uint32_t APP_SENSORS_readLight(void)
{
    APP_SENSORS_writeReadWords(OPT3001_I2C_ADDRESS, OPT3001_REG_RESULT, 2);
    APP_SENSORS_process(OPT3001_I2C_ADDRESS, OPT3001_REG_RESULT);
         
    return (APP_SENSORS_data.opt3001.light);
}

