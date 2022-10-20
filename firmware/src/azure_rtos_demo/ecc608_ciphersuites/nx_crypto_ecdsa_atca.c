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

#include "nx_crypto_ecdsa_pkcs11_atca.h"

static char pkcs11_trust_device_private_key_label[] = "device private";
static char pkcs11_trust_device_public_key_label[] = "device public";

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_crypto_convert_signRS_to_asn1                   PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function converts the R and S values to ASN.1 formatted ECDSA  */
/*    signature.                                                          */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    signRS                                R and S values of signature   */
/*    signRS_size                           R and S integers length       */
/*    signature                             Pointer to ECDSA signature    */
/*    signature_length                      Length of signature buffer    */
/*    actual_signature_length               Pointer to actual signature   */
/*                                          length                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Timothy Stapko           Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
NX_CRYPTO_KEEP uint32_t _nx_crypto_convert_signRS_to_asn1(UCHAR *signRS, UINT signRS_size,
                                                          UCHAR *signature, UINT signature_len, UINT *actual_signature_length)
{
  UINT                  r_size = signRS_size/2;
  UINT                  s_size = signRS_size/2;
  UCHAR                 pad_zero_r;
  UCHAR                 pad_zero_s;
  UINT                  sequence_size;
  UCHAR                *signature_r = signRS;
  UCHAR                *signature_s = signRS + r_size;
  UINT                   i;
  
  signature[0] = 0x30;    /* SEQUENCE */
  
  /* Trim prefix zeros. */
  for (i = 0; i < r_size; i++)
  {
       if (signature_r[i])
       {

           /* Loop until none zero byte. */
           break;
       }
  }
  signature_r += i;
  r_size -= i;
  
  /* The most significant bit must be zero to indicate positive integer. */
  /* Pad zero at the front if necessary. */
  pad_zero_r = (signature_r[0] & 0x80) ? 1 : 0;
  
  /* Trim prefix zeros. */
  for (i = 0; i < s_size; i++)
  {
    if (signature_s[i])
    {

      /* Loop until none zero byte. */
      break;
    }
  }
  signature_s += i;
  s_size -= i;
  
  /* The most significant bit must be zero to indicate positive integer. */
  /* Pad zero at the front if necessary. */
  pad_zero_s = (signature_s[0] & 0x80) ? 1 : 0;

    /* update length */
  sequence_size = r_size + s_size + pad_zero_r + pad_zero_s + 4;

  if (sequence_size < 0x80)
  {
    signature[1] = (UCHAR)sequence_size;
    signature += 2;
    *actual_signature_length = sequence_size + 2;
  }
  else
  {
    signature[1] = 0x81;
    signature[2] = (UCHAR)sequence_size;
    signature += 3;
    *actual_signature_length = sequence_size + 3;
  }

  if ((*actual_signature_length) > signature_len)
  {
    return(NX_CRYPTO_INVALID_BUFFER_SIZE);
  }
  
  /* Setup r. */
  NX_CRYPTO_MEMMOVE(&signature[2 + pad_zero_r], signature_r, r_size); /* Use case of memmove is verified. */
  signature[0] = 0x02;    /* Integer */
  signature[1] = (UCHAR)(r_size + pad_zero_r);
  if (pad_zero_r)
  {
        signature[2] = 0;
  }
  signature += (2u + pad_zero_r + r_size);

  /* Setup s. */
  NX_CRYPTO_MEMMOVE(&signature[2 + pad_zero_s], signature_s, s_size); /* Use case of memmove is verified. */
  signature[0] = 0x02;    /* Integer */
  signature[1] = (UCHAR)(s_size + pad_zero_s);
  if (pad_zero_s)
  {
    signature[2] = 0;
  }
  
  return(NX_CRYPTO_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_crypto_convert_asn1_to_signRS                   PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function converts the ASN.1 formatted ECDSA signature to R and */
/*    S values.                                                            */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    signature                             Pointer to ECDSA signature    */
/*    signature_length                      Length of signature           */
/*    signRS                                R and S values of signature   */
/*    signRS_size                           R and S integers length       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Timothy Stapko           Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
NX_CRYPTO_KEEP uint32_t _nx_crypto_convert_asn1_to_signRS(UCHAR *signature, UINT signature_length,
                            UCHAR *signRS, UINT signRS_size)
{
    /* Buffer should contain the signature data sequence. */
    if (signature[0] != 0x30)
    {
        printf("signature[0] = 0x%x\n", signature[0]);
        return(NX_CRYPTO_AUTHENTICATION_FAILED);
    }

    /* Check the size in SEQUENCE.  */
    if (signature[1] & 0x80)
    {
        if (signature_length < (signature[2] + 3u))
        {
            printf("signature[2] = 0x%x\n", signature[2]);
            return(NX_CRYPTO_SIZE_ERROR);
        }
        signature_length = signature[2];
        signature += 3;
    }
    else
    {
        if (signature_length < (signature[1] + 2u))
        {
            printf("signature[1] = 0x%x\n", signature[1]);
            return(NX_CRYPTO_SIZE_ERROR);
        }
        signature_length = signature[1];
        signature += 2;
    }

    /* Get r value from input signature. */
    UINT signR_size = signature[1];
    UCHAR pad_zero_r = (signR_size == 0x21)? 1 : 0;

    if (signature_length < 2 + pad_zero_r + PKCS11_ATCA_ECDSA_SHA256_XYRS_LENGTH)
    {
        printf("signR size is not correct, signature_length = 0x%x, pad_zero_r = %x\n",
                signature_length, pad_zero_r);
        return(NX_CRYPTO_SIZE_ERROR);
    }
    NX_CRYPTO_MEMCPY(signRS, signature + 2u + pad_zero_r, PKCS11_ATCA_ECDSA_SHA256_XYRS_LENGTH);

    signature_length -= (signature[1] + 2u);
    signature += signature[1] + 2u;
    signRS += PKCS11_ATCA_ECDSA_SHA256_XYRS_LENGTH;

    /* Get s value from input signature. */
    UINT signS_size =  signature[1];
    UCHAR pad_zero_s = (signS_size == 0x21)? 1 : 0;
    if (signature_length < 2 + pad_zero_s + PKCS11_ATCA_ECDSA_SHA256_XYRS_LENGTH)
    {
        printf("signS size is not correct, signature_length = 0x%x, pad_zero_s = %x\n",
                signature_length, pad_zero_s);
        return(NX_CRYPTO_SIZE_ERROR);
    }
    NX_CRYPTO_MEMCPY(signRS, signature + 2u + pad_zero_s, PKCS11_ATCA_ECDSA_SHA256_XYRS_LENGTH);

    return(NX_CRYPTO_SUCCESS);
}
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_crypto_ecdsa_sign_pkcs11_atca                   PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function computes a signature of the hash data using the       */
/*    private key.                                                        */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ecdsa_ptr                             ECDSA context                 */
/*    hash                                  Hash data to be signed        */
/*    hash_length                           Length of hash data           */
/*    private_key                           Pointer to EC private key     */
/*    signature                             Pointer to signature output   */
/*    signature_length                      Signature length              */
/*    actual_signature_length               Pointer to actual sign length */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Timothy Stapko           Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
UINT _nx_crypto_ecdsa_sign_pkcs11_atca(NX_CRYPTO_ECDSA_PKCS11_ATCA *ecdsa_ptr, UCHAR *hash, UINT hash_length,
                                              UCHAR *private_key, UINT private_key_length,
                                              UCHAR *signature, UINT signature_length,
                                              UINT *actual_signature_length)
{

NX_CRYPTO_EC *curve = ecdsa_ptr->nx_crypto_ecdsa_curve;
UINT status = NX_CRYPTO_SUCCESS;
CK_RV rv;

    if (curve->nx_crypto_ec_id != NX_CRYPTO_EC_SECP256R1)
	{
		printf("Unsupported ecc curve 0x%x\n", curve->nx_crypto_ec_id);
		return(NX_CRYTPO_MISSING_ECC_CURVE);
	}

    rv = C_SignInit(ecdsa_ptr->session_handle, &ecdsa_ptr->smech, ecdsa_ptr->privatekey_objhandle);

	if (rv != CKR_OK)
    {
		printf("C_SignInit: rv = 0x%.8lX\n", rv);
		return(NX_NOT_SUCCESSFUL);
	}

	CK_BYTE sign[ATCA_SIG_SIZE];
	CK_ULONG slen = ATCA_SIG_SIZE;
    CK_BYTE_PTR message = (CK_BYTE_PTR)hash;
    CK_ULONG messagelen = hash_length; //ATCA_SHA256_DIGEST_SIZE;

    /* 
     * Signature are R and S integers in big-endian format.
     * 64 bytes for P256 curve
     */
	rv = C_Sign(ecdsa_ptr->session_handle, message, messagelen,
                    (CK_BYTE_PTR)sign, &slen);

	if (rv != CKR_OK || slen != ATCA_SIG_SIZE)
    {
		printf("C_Sign: rv = 0x%.8lX, slen = %lu\n", rv, slen);
		return(NX_NOT_SUCCESSFUL);
	}

    NX_CRYPTO_MEMCPY(ecdsa_ptr->nx_crypto_ecdsa_signature_rs, sign, slen);
    ecdsa_ptr->nx_crypto_ecdsa_signature_rs_length = slen;
    
    /* convert R, S of ECDSA signature to ASN.1 format */ 
    NX_CRYPTO_MEMSET(signature, 0, signature_length);
    
    status = _nx_crypto_convert_signRS_to_asn1(sign, slen,
                                    signature, signature_length,
                                    (UINT *)actual_signature_length);
    if (status)
    {
      return(status);
    }
    ecdsa_ptr->nx_crypto_ecdsa_signature_asn1_length = (*actual_signature_length);
    UINT len = ecdsa_ptr->nx_crypto_ecdsa_signature_asn1_length;
    NX_CRYPTO_MEMCPY(ecdsa_ptr->nx_crypto_ecdsa_signature_asn1, signature, len);
    
    return NX_CRYPTO_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_crypto_ecdsa_verify_pkcs11_atca                 PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function verifies the signature of the hash data using the     */
/*    public key.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ecdsa_ptr                             ECDSA context                 */
/*    hash                                  Hash data to be verified      */
/*    hash_length                           Length of hash data           */
/*    public_key                            Pointer to EC public key      */
/*    signature                             Signature to be verified      */
/*    signature_length                      Signature length              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Timothy Stapko           Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
UINT _nx_crypto_ecdsa_verify_pkcs11_atca(NX_CRYPTO_ECDSA_PKCS11_ATCA *ecdsa_ptr, UCHAR *hash, UINT hash_length,
                                                UCHAR *public_key, UINT public_key_length,
                                                UCHAR *signature, UINT signature_length)
{

NX_CRYPTO_EC *curve = ecdsa_ptr->nx_crypto_ecdsa_curve;
UINT status = NX_CRYPTO_SUCCESS;
CK_RV rv;
UCHAR signRS [ATCA_ECCP256_SIG_SIZE] = {0};
UINT signRS_size = ATCA_ECCP256_SIG_SIZE;

	if (curve->nx_crypto_ec_id != NX_CRYPTO_EC_SECP256R1)
	{
		printf("Unsupported ecc curve 0x%x\n", curve->nx_crypto_ec_id);
		return(NX_CRYTPO_MISSING_ECC_CURVE);
	}

    status = _nx_crypto_convert_asn1_to_signRS(signature, signature_length, signRS, signRS_size);
    if (status)
    {
        return(status);
    }

    rv = C_VerifyInit(ecdsa_ptr->session_handle, &ecdsa_ptr->smech, ecdsa_ptr->publickey_objhandle);

	if (rv != CKR_OK)
    {
		printf("C_VerifyInit: rv = 0x%.8lX\n", rv);
		return(NX_CRYPTO_NOT_SUCCESSFUL);
	}

	rv = C_Verify(ecdsa_ptr->session_handle, (CK_BYTE_PTR)hash, hash_length,
                (CK_BYTE_PTR)signRS, signRS_size);

	if (rv != CKR_OK)
    {
		printf("C_Verify: rv = 0x%.8lX\n", rv);
		return(NX_CRYPTO_NOT_SUCCESSFUL);
	}

    return NX_CRYPTO_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    nx_crypto_find_pkcs11_object                       PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function finds PKCS#11 object based on object attribute        */
/*    template for a given session.                                       */
/*    public key.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    session_handle                        PKCS#11 session               */
/*    template                              Pointer to attribute template */
/*    obj_handle_ptr                        Pointer to object handle      */
/*    public_key                            Pointer to EC public key      */
/*    signature                             Signature to be verified      */
/*    signature_length                      Signature length              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_crypto_method_ecdsa_pkcs11_atca_init                            */
/*    pkcs11_get_device_cert                                              */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Timothy Stapko           Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
CK_RV nx_crypto_find_pkcs11_object(CK_SESSION_HANDLE session_handle, CK_ATTRIBUTE_PTR template,
                CK_OBJECT_HANDLE_PTR obj_handle_ptr)
{
    
    CK_RV rv;
    CK_ULONG max_object_count = 1;
    CK_ULONG obj_count;
    //CK_ATTRIBUTE objectMask[] = { {CKA_LABEL, label, labelLen} };

    rv = C_FindObjectsInit(session_handle, template, 1);
    if (rv != CKR_OK)
    {
        printf("Error FindObjectsInit: 0x%lX\n", rv);
        return rv;
    }
  
    rv = C_FindObjects(session_handle, obj_handle_ptr, max_object_count, &obj_count);
    if (rv != CKR_OK)
    {
    	printf("Error FindObjects: 0x%lX\n", rv); return rv;
    }
  
    rv = C_FindObjectsFinal(session_handle);
    if (rv != CKR_OK)
    {
        printf("Error FindObjectsFinal: 0x%lX\n", rv); return rv;
    }
    return CKR_OK;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_crypto_method_ecdsa_pkcs11_atca_init            PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the common crypto method init callback for         */
/*    Express Logic supported ECDSA cryptograhic algorithm.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    method                                Pointer to crypto method      */
/*    key                                   Pointer to key                */
/*    key_size_in_bits                      Length of key size in bits    */
/*    handler                               Returned crypto handler       */
/*    crypto_metadata                       Metadata area                 */
/*    crypto_metadata_size                  Size of the metadata area     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Timothy Stapko           Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
NX_CRYPTO_KEEP UINT  _nx_crypto_method_ecdsa_pkcs11_atca_init(struct  NX_CRYPTO_METHOD_STRUCT *method,
                                                      UCHAR *key, NX_CRYPTO_KEY_SIZE key_size_in_bits,
                                                      VOID  **handle,
                                                      VOID  *crypto_metadata,
                                                      ULONG crypto_metadata_size)
{

    NX_PARAMETER_NOT_USED(key);
    NX_PARAMETER_NOT_USED(key_size_in_bits);
    NX_PARAMETER_NOT_USED(handle);

    NX_CRYPTO_STATE_CHECK

    if ((method == NX_NULL) || (crypto_metadata == NX_NULL))
    {
        return(NX_PTR_ERROR);
    }

    /* Verify the metadata addrsss is 4-byte aligned. */
    if((((ULONG)crypto_metadata) & 0x3) != 0)
    {
        return(NX_PTR_ERROR);
    }

    if(crypto_metadata_size < sizeof(NX_CRYPTO_ECDSA_PKCS11_ATCA))
    {
        return(NX_PTR_ERROR);
    }

    CK_RV   rv;
    CK_C_INITIALIZE_ARGS  cinit_args;

    NX_CRYPTO_ECDSA_PKCS11_ATCA *ecdsa = (NX_CRYPTO_ECDSA_PKCS11_ATCA *)crypto_metadata;

    memset(&cinit_args, 0x0, sizeof(cinit_args));
    cinit_args.flags = CKF_OS_LOCKING_OK;

    rv = C_Initialize(&cinit_args);
    if (rv != CKR_OK && rv != CKR_CRYPTOKI_ALREADY_INITIALIZED)
    {
        printf("C_Initialize failed with rv = %lu", rv );
        return(NX_NOT_SUCCESSFUL);
    }

    CK_MECHANISM genmech, smech;
    /* Set up mechanism for generating key pair */
	genmech.mechanism = CKM_EC_KEY_PAIR_GEN; //CKM_RSA_PKCS_KEY_PAIR_GEN;
	genmech.pParameter = NULL_PTR;
	genmech.ulParameterLen = 0;

	/* Set up the signing mechanism */
	smech.mechanism = CKM_ECDSA; //CKM_RSA_PKCS;
	smech.pParameter = NULL_PTR;
	smech.ulParameterLen = 0;

    /* There is only one device slotID 0 on atecc608B */
    CK_SLOT_ID slotID = 0;

    /* Open a session on the slot found */
    CK_SESSION_HANDLE session_handle;

	rv = C_OpenSession(slotID, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR,
	    &session_handle);

	if (rv != CKR_OK)
    {
		printf("C_OpenSession: rv = 0x%.8lX\n", rv);
		C_Finalize(NULL_PTR);
        return(NX_NOT_SUCCESSFUL);
	}
    
    /* create a privateKey handle for slot 0 */
    CK_OBJECT_CLASS class_pri_val = CKO_PRIVATE_KEY;
    CK_OBJECT_CLASS class_pub_val = CKO_PUBLIC_KEY;
    
    CK_ATTRIBUTE publickey_template[] = {
            {CKA_CLASS,     &class_pub_val, sizeof(CK_OBJECT_CLASS)},
            {CKA_LABEL, pkcs11_trust_device_public_key_label, strlen(pkcs11_trust_device_public_key_label)}
    };

    CK_ATTRIBUTE privatekey_template[] = {
            {CKA_CLASS, &class_pri_val, sizeof(CK_OBJECT_CLASS)},
            {CKA_LABEL, pkcs11_trust_device_private_key_label, strlen(pkcs11_trust_device_private_key_label)}
    };
    
    CK_OBJECT_HANDLE privatekey_objhandle, publickey_objhandle;

    rv = nx_crypto_find_pkcs11_object(session_handle, privatekey_template, &privatekey_objhandle);
    if (rv != CKR_OK)
    {
		printf("find_object for privatekey: rv = 0x%.8lX\n", rv);
        C_CloseSession(session_handle);
		C_Finalize(NULL_PTR);
        return(NX_NOT_SUCCESSFUL);
	}

    rv = nx_crypto_find_pkcs11_object(session_handle, publickey_template, &publickey_objhandle);
    if (rv != CKR_OK)
    {
		printf("find_object for publickey: rv = 0x%.8lX\n", rv);
		C_CloseSession(session_handle);
		C_Finalize(NULL_PTR);
        return(NX_NOT_SUCCESSFUL);
	}
    
    ecdsa->session_handle = session_handle;
    ecdsa->privatekey_objhandle = privatekey_objhandle;
    ecdsa->publickey_objhandle = publickey_objhandle;
    ecdsa->genmech = genmech;
    ecdsa->smech = smech;
    return(NX_CRYPTO_SUCCESS);

}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_crypto_method_ecdsa_pkcs11_atca_cleanup         PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function cleans up the crypto metadata.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    crypto_metadata                       Crypto metadata               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    NX_CRYPTO_MEMSET                      Set the memory                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Timothy Stapko           Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
NX_CRYPTO_KEEP UINT  _nx_crypto_method_ecdsa_pkcs11_atca_cleanup(VOID *crypto_metadata)
{

    NX_CRYPTO_STATE_CHECK

#ifdef NX_SECURE_KEY_CLEAR
    if (!crypto_metadata)
        return (NX_CRYPTO_SUCCESS);

    /* Clean up the crypto metadata.  */
    NX_CRYPTO_MEMSET(crypto_metadata, 0, sizeof(NX_CRYPTO_ECDSA_PKCS11_ATCA));
#else
    NX_PARAMETER_NOT_USED(crypto_metadata);
#endif/* NX_SECURE_KEY_CLEAR  */

    NX_CRYPTO_ECDSA_PKCS11_ATCA *ecdsa = (NX_CRYPTO_ECDSA_PKCS11_ATCA *)crypto_metadata;
    C_CloseSession(ecdsa->session_handle);
	//C_Finalize(NULL_PTR);
    return(NX_CRYPTO_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_crypto_method_ecdsa_pkcs11_atca_operation       PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Timothy Stapko, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function performs an ECDSA operation.                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    op                                    ECDSA operation               */
/*    handle                                Crypto handle                 */
/*    method                                Cryption Method Object        */
/*    key                                   Encryption Key                */
/*    key_size_in_bits                      Key size in bits              */
/*    input                                 Input data                    */
/*    input_length_in_byte                  Input data size               */
/*    iv_ptr                                Initial vector                */
/*    output                                Output buffer                 */
/*    output_length_in_byte                 Output buffer size            */
/*    crypto_metadata                       Metadata area                 */
/*    crypto_metadata_size                  Metadata area size            */
/*    packet_ptr                            Pointer to packet             */
/*    nx_crypto_hw_process_callback         Callback function pointer     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_crypto_ecdsa_sign_pkcs11_atca     Sign using ECDSA              */
/*    _nx_crypto_ecdsa_verify_pkcs11_atca   Verify ECDSA signature        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Timothy Stapko           Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
NX_CRYPTO_KEEP UINT _nx_crypto_method_ecdsa_pkcs11_atca_operation(UINT op,
                                                          VOID *handle,
                                                          struct NX_CRYPTO_METHOD_STRUCT *method,
                                                          UCHAR *key, NX_CRYPTO_KEY_SIZE key_size_in_bits,
                                                          UCHAR *input, ULONG input_length_in_byte,
                                                          UCHAR *iv_ptr,
                                                          UCHAR *output, ULONG output_length_in_byte,
                                                          VOID *crypto_metadata, ULONG crypto_metadata_size,
                                                          VOID *packet_ptr,
                                                          VOID (*nx_crypto_hw_process_callback)(VOID *, UINT))
{
NX_CRYPTO_ECDSA_PKCS11_ATCA *ecdsa;
UINT                 status = NX_CRYPTO_SUCCESS;
NX_CRYPTO_EXTENDED_OUTPUT
                    *extended_output;

    NX_PARAMETER_NOT_USED(handle);
    NX_PARAMETER_NOT_USED(iv_ptr);
    NX_PARAMETER_NOT_USED(packet_ptr);
    NX_PARAMETER_NOT_USED(nx_crypto_hw_process_callback);
    NX_PARAMETER_NOT_USED(key);

    NX_CRYPTO_STATE_CHECK

    /* Verify the metadata addrsss is 4-byte aligned. */
    if((method == NX_NULL) || (crypto_metadata == NX_NULL) || ((((ULONG)crypto_metadata) & 0x3) != 0))
    {
        return(NX_PTR_ERROR);
    }

    if(crypto_metadata_size < sizeof(NX_CRYPTO_ECDSA_PKCS11_ATCA))
    {
        return(NX_PTR_ERROR);
    }

    ecdsa = (NX_CRYPTO_ECDSA_PKCS11_ATCA *)crypto_metadata;


    if (op == NX_CRYPTO_EC_CURVE_SET)
    {
        /* Set curve to be used in the ECDSA. */
        status = ((NX_CRYPTO_METHOD *)input) -> nx_crypto_operation(NX_CRYPTO_EC_CURVE_GET,
                                                                    NX_NULL,
                                                                    (NX_CRYPTO_METHOD *)input,
                                                                    NX_NULL, 0,
                                                                    NX_NULL, 0,
                                                                    NX_NULL,
                                                                    (UCHAR *)&ecdsa -> nx_crypto_ecdsa_curve,
                                                                    sizeof(NX_CRYPTO_EC *),
                                                                    NX_NULL, 0,
                                                                    NX_NULL, NX_NULL);
    }
    else if (op == NX_CRYPTO_AUTHENTICATE)
    {

        extended_output = (NX_CRYPTO_EXTENDED_OUTPUT *)output;

        status = _nx_crypto_ecdsa_sign_pkcs11_atca(ecdsa,
                                           input,
                                           input_length_in_byte,
                                           key,
                                           key_size_in_bits >> 3,
                                           extended_output -> nx_crypto_extended_output_data,
                                           extended_output -> nx_crypto_extended_output_length_in_byte,
                                           (UINT *)&extended_output -> nx_crypto_extended_output_actual_size);
    }
    else if (op == NX_CRYPTO_VERIFY)
    {

        status = _nx_crypto_ecdsa_verify_pkcs11_atca(ecdsa,
                                             input,
                                             input_length_in_byte,
                                             key,
                                             key_size_in_bits >> 3,
                                             output, output_length_in_byte);
    }
    else
    {
        status = NX_NOT_SUCCESSFUL;
    }

    return(status);
}
