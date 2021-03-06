/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/io/p9_io_dmi_pre_trainadv.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2017                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
///
/// @file p9_io_dmi_pre_trainadv.H
/// @brief Pre-Training PHY Status Function.
///
///----------------------------------------------------------------------------
/// *HWP HWP Owner        : Chris Steffen <cwsteffen@us.ibm.com>
/// *HWP HWP Backup Owner : Gary Peterson <garyp@us.ibm.com>
/// *HWP FW Owner         : Jamie Knight <rjknight@us.ibm.com>
/// *HWP Team             : IO
/// *HWP Level            : 3
/// *HWP Consumed by      : FSP:HB
///----------------------------------------------------------------------------

#ifndef _P9_IO_DMI_PRE_TRAINADV_H_
#define _P9_IO_DMI_PRE_TRAINADV_H_

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include <fapi2.H>

//-----------------------------------------------------------------------------
// Structure definitions
//-----------------------------------------------------------------------------

// function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode (*p9_io_dmi_pre_trainadv_FP_t)
(const fapi2::Target < fapi2::TARGET_TYPE_DMI >&,
 const fapi2::Target < fapi2::TARGET_TYPE_MEMBUF_CHIP >&);

// ----------------------------------------------------------------------------
// Constant Definitions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function Protoypes
// ----------------------------------------------------------------------------
extern "C" {

    /**
     * @brief A simple HWP that runs prior to io_run_trainig.
     *  This function is called on every Xbus.
     * @param[in] i_target   Fapi2 Target
     * @param[in] i_ctarget  Fapi2 Connected Target
     * @retval ReturnCode
     */
    fapi2::ReturnCode p9_io_dmi_pre_trainadv(
        const fapi2::Target< fapi2::TARGET_TYPE_DMI >& i_target,
        const fapi2::Target< fapi2::TARGET_TYPE_MEMBUF_CHIP >& i_ctarget);

} // extern "C"

#endif // _P9_IO_DMI_PRE_TRAINADV_H_
