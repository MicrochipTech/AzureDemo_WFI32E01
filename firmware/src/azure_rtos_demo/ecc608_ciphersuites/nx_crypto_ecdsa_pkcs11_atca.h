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


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** NetX Crypto Component                                                 */
/**                                                                       */
/**   Elliptic Curve Digital Signature Algorithm (ECDSA)                  */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    NX_CRYPTO_ECDSA_PKCS11_ATCA.h                        PORTABLE C     */
/*                                                           6.x          */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the basic Application Interface (API) to the      */
/*    NetX ECDSA module.                                                  */
/*                                                                        */
/*    It is assumed that nx_api.h and nx_port.h have already been         */
/*    included.                                                           */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Timothy Stapko           Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/

#ifndef  NX_CRYPTO_ECDSA_PKCS11_ATCA_H
#define  NX_CRYPTO_ECDSA_PKCS11_ATCA_H

/* Determine if a C++ compiler is being used.  If so, ensure that standard
   C is used to process the API information.  */
#ifdef __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {

#endif

#include "cryptoki.h"
#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <string.h>
#include <stdlib.h>
#include "definitions.h"                // SYS function prototypes
#include "cryptoauthlib.h"              //must be manually added for now

#include "nx_crypto.h"
#include "nx_crypto_ec.h"


#ifndef NX_CRYPTO_ECDSA_PKCS11_ATCA_MAX_KEY_SIZE
#define NX_CRYPTO_ECDSA_PKCS11_ATCA_MAX_KEY_SIZE     (32)
#endif /* NX_CRYPTO_ECDSA_PKCS11_ATCA_MAX_KEY_SIZE */

#define BUFFERSIZE	64
#define PKCS11_ATCA_ECDSA_SHA256_XYRS_LENGTH 32U
#define PKCS11_ATCA_ECDSA_SHA384_XYRS_LENGTH 48U

/* ECDSA signature structure. */
typedef struct NX_CRYPTO_ECDSA_PKCS11_ATCA
{
    NX_CRYPTO_EC *nx_crypto_ecdsa_curve;
    NX_CRYPTO_METHOD *nx_crypto_ecdsa_hash_method;
    CK_SESSION_HANDLE session_handle;
    CK_OBJECT_HANDLE privatekey_objhandle;
    CK_OBJECT_HANDLE publickey_objhandle;
    CK_MECHANISM genmech;
    CK_MECHANISM smech;
    
    UCHAR nx_crypto_ecdsa_signature_rs[ATCA_SIG_SIZE];
    UINT nx_crypto_ecdsa_signature_rs_length;
    UCHAR nx_crypto_ecdsa_signature_asn1[PKCS11_ATCA_ECDSA_SHA384_XYRS_LENGTH];
    UINT nx_crypto_ecdsa_signature_asn1_length;

} NX_CRYPTO_ECDSA_PKCS11_ATCA;

uint32_t _nx_crypto_convert_signRS_to_asn1(UCHAR *signRS, UINT signRS_size,
                                                          UCHAR *signature, UINT signature_len, UINT *actual_signature_length);

uint32_t _nx_crypto_convert_asn1_to_signRS(UCHAR *signature, UINT signature_length,
                            UCHAR *signRS, UINT signRS_size);

/* Define the function prototypes for ECDSA.  */
UINT _nx_crypto_ecdsa_sign_pkcs11_atca(NX_CRYPTO_ECDSA_PKCS11_ATCA *ecdsa_ptr,
                               UCHAR *hash,
                               UINT hash_length,
                               UCHAR *private_key,
                               UINT private_key_length,
                               UCHAR *signature,
                               UINT signature_length,
                               UINT *actual_signature_length);

UINT _nx_crypto_ecdsa_verify_pkcs11_atca(NX_CRYPTO_ECDSA_PKCS11_ATCA *ecdsa_ptr,
                                 UCHAR *hash,
                                 UINT hash_length,
                                 UCHAR *public_key,
                                 UINT public_key_length,
                                 UCHAR *signature,
                                 UINT signature_length);

UINT _nx_crypto_method_ecdsa_pkcs11_atca_init(struct  NX_CRYPTO_METHOD_STRUCT *method,
                                      UCHAR *key, NX_CRYPTO_KEY_SIZE key_size_in_bits,
                                      VOID  **handle,
                                      VOID  *crypto_metadata,
                                      ULONG crypto_metadata_size);

UINT _nx_crypto_method_ecdsa_pkcs11_atca_cleanup(VOID *crypto_metadata);

UINT _nx_crypto_method_ecdsa_pkcs11_atca_operation(UINT op,
                                           VOID *handle,
                                           struct NX_CRYPTO_METHOD_STRUCT *method,
                                           UCHAR *key, NX_CRYPTO_KEY_SIZE key_size_in_bits,
                                           UCHAR *input, ULONG input_length_in_byte,
                                           UCHAR *iv_ptr,
                                           UCHAR *output, ULONG output_length_in_byte,
                                           VOID *crypto_metadata, ULONG crypto_metadata_size,
                                           VOID *packet_ptr,
                                           VOID (*nx_crypto_hw_process_callback)(VOID *, UINT));

#ifdef __cplusplus
}
#endif

#endif /* NX_CRYPTO_ECDSA_STSAFE_A110_H */
