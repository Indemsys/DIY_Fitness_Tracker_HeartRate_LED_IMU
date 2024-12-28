/***********************************************************************************************************************
 * Copyright [2015-2023] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : sf_crypto_key.h
 * Description  : Interface definition for SSP Crypto Key Framework module instance.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup SF_Library
 * @defgroup SF_CRYPTO_KEY SSP Crypto Key Framework
 * @brief RTOS-integrated Crypto Key Framework Module.
 *
 * @{
 **********************************************************************************************************************/

#ifndef SF_CRYPTO_KEY_H
#define SF_CRYPTO_KEY_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "sf_crypto_key_api.h"
#include "sf_crypto_key_cfg.h"
#include "sf_crypto.h"
#include "r_crypto_api.h"

/* Includes Cryptography algorithms. */
#include "r_rsa_api.h"
#include "r_dsa_api.h"
#include "r_aes_api.h"
#include "r_tdes_api.h"
#include "r_ecc_api.h"

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** The API version of SSP Crypto Framework */
#define SF_CRYPTO_KEY_CODE_VERSION_MAJOR       (2U)
#define SF_CRYPTO_KEY_CODE_VERSION_MINOR       (0U)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** SSP Crypto Key Framework instance control block */
typedef struct st_sf_crypto_key_instance_ctrl
{
    sf_crypto_key_state_t       status;                 ///< Module status
    sf_crypto_key_type_t        key_type;               ///< Key type
    sf_crypto_key_size_t        key_size;               ///< Key size
    sf_crypto_data_handle_t     domain_params;          ///< Domain parameters structure with pointer to data and length.
    sf_crypto_data_handle_t     generator_point;        ///< Generator Point structure with pointer to data and length.
    sf_crypto_instance_ctrl_t * p_fwk_common_ctrl;      ///< Pointer to the Crypto Framework Common instance
    sf_crypto_api_t           * p_fwk_common_api;       ///< Pointer to the Crypto Framework Common instance
    void                      * p_hal_ctrl;             ///< pointer to Crypto module control structure
    void                      * p_hal_api;              ///< pointer to Crypto module API structure
} sf_crypto_key_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const sf_crypto_key_api_t    g_sf_crypto_key_api;
/** @endcond */

/** @} (end defgroup SF_CRYPTO_KEY) */

/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif /* SF_CRYPTO_KEY_H */
