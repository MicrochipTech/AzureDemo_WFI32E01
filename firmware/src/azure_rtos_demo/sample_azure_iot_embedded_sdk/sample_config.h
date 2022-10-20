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

#ifndef SAMPLE_CONFIG_H
#define SAMPLE_CONFIG_H

#include "app_pic32mz_w1.h"

#ifdef __cplusplus
extern   "C" {
#endif

/*** Azure IoT embedded C SDK Configuration ***/
#define MODULE_ID              ""
/* Required when DPS is used.  */
#define ENABLE_DPS_SAMPLE
#define ENDPOINT                       "global.azure-devices-provisioning.net"
#define ID_SCOPE                       default_id_scope
#define REGISTRATION_ID                default_registration_id
#define SAMPLE_MAX_BUFFER              (256)
            
    
#define DEVICE_SYMMETRIC_KEY           default_primary_key           
    
#define NX_AZURE_IOT_STACK_SIZE                (2048)
#define NX_AZURE_IOT_THREAD_PRIORITY           (4) 
#define SAMPLE_STACK_SIZE                      (2048)
#define SAMPLE_THREAD_PRIORITY                 (16)
#define MAX_PROPERTY_COUNT                     (2)

#define AZ_TELEMETRYINTERVAL_DEFAULT           5

/* Use certificate-based authentication with PKCS#11/ECC608 */
/* Comment out the following 3 definitions to use SAS token authentication */    
    
#define USE_X509_WITH_ECC608
#define USE_DEVICE_CERTIFICATE 1
#define DEVICE_KEY_TYPE NX_SECURE_X509_KEY_TYPE_HARDWARE
    
#ifdef __cplusplus
}
#endif
#endif /* SAMPLE_CONFIG_H */
