/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/pm/p9_pm_ocb_init.C $      */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2018                        */
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
/// @file  p9_pm_ocb_init.C
/// @brief Setup and configure OCB channels
///
// *HWP HWP Owner       : Amit Kumar <akumar3@us.ibm.com>
// *HWP HWP Backup Owner: Greg Still <stillgs@us.ibm.com>
// *HWP FW Owner        : Sangeetha T S <sangeet2@in.ibm.com>
// *HWP Team            : PM
// *HWP Level           : 3
// *HWP Consumed by     : SBE:HS

///   High-level procedure flow:
///
///   - if mode = PM_INIT
///   - placeholder - currently do nothing
///   - if mode = PM_RESET
///     - reset each register in each OCB channel to its scan0-flush state
///   - if mode = PM_SETUP_PIB or PM_SETUP_ALL
///     - process parameters passed to procedure
///     - Set up channel control/status register based on passed parameters
///       (OCBCSRn)
///     - Set Base Address Register
///       - linear streaming & non-streaming => OCBARn
///       - push queue                       => OCBSHBRn  (only if PM_SETUP_ALL)
///       - pull queue                       => OCBSLBRn  (only if PM_SETUP_ALL)
///     - Set up queue control and status register (only if PM_SETUP_ALL)
///       - push queue  => OCBSHCSn
///       - pull queue  => OCBSLCSn
///
///   Procedure Prerequisite:
///   - System clocks are running
///

//------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------

#include <p9_pm_ocb_init.H>

//------------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------------
enum PM_OCB_CONST
{
    MAX_OCB_QUE_LEN = 31, // Max length of PULL/PUSH queue
    INTERRUPT_SRC_MASK_REG = 0xFFFFFFFF, // Mask for interrupt source register
    MAX_OCB_CHANNELS = 3 // Max no. of OCB channels
};

// channel register arrrays
const uint64_t OCBARn[4]       = {PU_OCB_PIB_OCBAR0,
                                  PU_OCB_PIB_OCBAR1,
                                  PU_OCB_PIB_OCBAR2,
                                  PU_OCB_PIB_OCBAR3
                                 };

const uint64_t OCBCSRn_CLEAR[4]  = {PU_OCB_PIB_OCBCSR0_CLEAR,
                                    PU_OCB_PIB_OCBCSR1_CLEAR,
                                    PU_OCB_PIB_OCBCSR2_CLEAR,
                                    PU_OCB_PIB_OCBCSR3_CLEAR
                                   };

const uint64_t OCBCSRn_OR[4]   = {PU_OCB_PIB_OCBCSR0_OR,
                                  PU_OCB_PIB_OCBCSR1_OR,
                                  PU_OCB_PIB_OCBCSR2_OR,
                                  PU_OCB_PIB_OCBCSR3_OR
                                 };

const uint64_t OCBESRn[4]      = {PU_OCB_PIB_OCBESR0,
                                  PU_OCB_PIB_OCBESR1,
                                  PU_OCB_PIB_OCBESR2,
                                  PU_OCB_PIB_OCBESR3
                                 };

const uint64_t OCBSLBRn[4]     = {PU_OCB_OCI_OCBSLBR0_SCOM,
                                  PU_OCB_OCI_OCBSLBR1_SCOM,
                                  PU_OCB_OCI_OCBSLBR2_SCOM,
                                  PU_OCB_OCI_OCBSLBR3_SCOM
                                 };

const uint64_t OCBSHBRn[4]     = {PU_OCB_OCI_OCBSHBR0_SCOM,
                                  PU_OCB_OCI_OCBSHBR1_SCOM,
                                  PU_OCB_OCI_OCBSHBR2_SCOM,
                                  PU_OCB_OCI_OCBSHBR3_SCOM
                                 };

const uint64_t OCBSLCSn[4]     = {PU_OCB_OCI_OCBSLCS0_SCOM,
                                  PU_OCB_OCI_OCBSLCS1_SCOM,
                                  PU_OCB_OCI_OCBSLCS2_SCOM,
                                  PU_OCB_OCI_OCBSLCS3_SCOM
                                 };

const uint64_t OCBSHCSn[4]     = {PU_OCB_OCI_OCBSHCS0_SCOM,
                                  PU_OCB_OCI_OCBSHCS1_SCOM,
                                  PU_OCB_OCI_OCBSHCS2_SCOM,
                                  PU_OCB_OCI_OCBSHCS3_SCOM
                                 };

const uint64_t OCBSESn[4]      = {PU_OCB_OCI_OCBSES0_SCOM,
                                  PU_OCB_OCI_OCBSES1_SCOM,
                                  PU_OCB_OCI_OCBSES2_SCOM,
                                  PU_OCB_OCI_OCBSES3_SCOM
                                 };

const uint64_t OCBLWCRn[4]     = {PU_OCB_OCI_OCBLWCR0_SCOM,
                                  PU_OCB_OCI_OCBLWCR1_SCOM,
                                  PU_OCB_OCI_OCBLWCR2_SCOM,
                                  PU_OCB_OCI_OCBLWCR3_SCOM
                                 };

const uint64_t OCBLWSBRn[4]    = {PU_OCB_OCI_OCBLWSBR0_SCOM,
                                  PU_OCB_OCI_OCBLWSBR1_SCOM,
                                  PU_OCB_OCI_OCBLWSBR2_SCOM,
                                  PU_OCB_OCI_OCBLWSBR3_SCOM
                                 };

const uint32_t PU_OCB_OCI_OIMR0_OR = PU_OCB_OCI_OIMR0_SCOM2;
const uint32_t PU_OCB_OCI_OIMR1_OR = PU_OCB_OCI_OIMR1_SCOM2;

const uint32_t PU_OCB_OCI_OITR0_CLEAR = PU_OCB_OCI_OITR0_SCOM1;
const uint32_t PU_OCB_OCI_OITR1_CLEAR = PU_OCB_OCI_OITR1_SCOM1;
const uint32_t PU_OCB_OCI_OIEPR0_CLEAR = PU_OCB_OCI_OIEPR0_SCOM1;
const uint32_t PU_OCB_OCI_OIEPR1_CLEAR = PU_OCB_OCI_OIEPR1_SCOM1;
const uint32_t PU_OCB_OCI_OISR0_CLEAR = PU_OCB_OCI_OISR0_SCOM1;
const uint32_t PU_OCB_OCI_OISR1_CLEAR = PU_OCB_OCI_OISR1_SCOM1;

//------------------------------------------------------------------------------
//  Function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///
/// @brief Reset OCB Channels to default state (ie. scan-0 flush state)
///
/// @param [in]   i_target          Chip Target
///
/// @return FAPI2_RC_SUCCESS on success, else error.
///
fapi2::ReturnCode pm_ocb_reset(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target);

//------------------------------------------------------------------------------
///
/// @brief Init specified channel to type specified
///
/// @param [in]   i_target         Chip Target
///
/// @param [in]   i_ocb_chan       Channel to setup from enum PM_OCB_CHAN_NUM.
///                                OCB_CHAN0 : OCB  Channel 0
///                                OCB_CHAN1 : OCB  Channel 1
///                                OCB_CHAN2 : OCB  Channel 2
///                                OCB_CHAN3 : OCB  Channel 3
///
/// @param [in]   i_ocb_type       Type of channel from PM_OCB_CHAN_TYPE.
///                                OCB_TYPE_LIN:Linear w/o address increment
///                                OCB_TYPE_LINSTR:Linear with address increment
///                                OCB_TYPE_CIRC:Circular mode
///                                OCB_TYPE_PUSHQ:Circular Push Queue
///                                OCB_TYPE_PULLQ:Circular Pull Queue
///
/// @param [in]   i_ocb_bar        32-bit channel base address(29 bits + "000")
///
/// @param [in]   i_ocb_upd_reg    Type of register to init 'PM_OCB_CHAN_REG'
///                                OCB_UPD_PIB_REG:Update PIB Register
///                                OCB_UPD_PIB_OCI_REG:Update OCI+PIB Registers
///
/// @param [in]   i_ocb_q_len      0-31 length of push or pull queue in
///                                (queue_length + 1) * 8B
///
/// @param [in]   i_ocb_ouflow_en  Channel flow control from PM_OCB_CHAN_OUFLOW
///                                OCB_Q_OUFLOW_EN:Overflow/Underflow Enable
///                                OCB_Q_OUFLOW_DIS:Overflow/Underflow Disable
///
/// @param [in]   i_ocb_itp_type   Channel interrupt control from PM_OCB_ITPTYPE
///                                OCB_Q_ITPTYPE_FULL:Interrupt on Full
///                                OCB_Q_ITPTYPE_NOTFULL:Interrupt on Not Full
///                                OCB_Q_ITPTYPE_EMPTY:Interrupt on Empty
///                                OCB_Q_ITPTYPE_NOTEMPTY:Interrupt on Not Empty
/// @return FAPI2_RC_SUCCESS on success, else error.
///
fapi2::ReturnCode pm_ocb_setup(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
    const p9ocb::PM_OCB_CHAN_NUM    i_ocb_chan,
    const p9ocb::PM_OCB_CHAN_TYPE   i_ocb_type,
    const uint32_t                  i_ocb_bar,
    const p9ocb::PM_OCB_CHAN_REG    i_ocb_upd_reg,
    const uint8_t                   i_ocb_q_len,
    const p9ocb::PM_OCB_CHAN_OUFLOW i_ocb_ouflow_en,
    const p9ocb::PM_OCB_ITPTYPE     i_ocb_itp_type);

//------------------------------------------------------------------------------
//  Function definitions
//------------------------------------------------------------------------------

fapi2::ReturnCode p9_pm_ocb_init(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
    const p9pm::PM_FLOW_MODE      i_mode,
    const p9ocb::PM_OCB_CHAN_NUM     i_ocb_chan,
    const p9ocb::PM_OCB_CHAN_TYPE    i_ocb_type,
    const uint32_t                   i_ocb_bar,
    const uint8_t                    i_ocb_q_len,
    const p9ocb::PM_OCB_CHAN_OUFLOW  i_ocb_ouflow_en,
    const p9ocb::PM_OCB_ITPTYPE      i_ocb_itp_type)
{
    FAPI_INF("p9_pm_ocb_init Enter");

    // -------------------------------------------------------------------------
    // INIT mode: Placeholder; NOOP at present
    // -------------------------------------------------------------------------
    if (i_mode == p9pm::PM_INIT)
    {
        FAPI_DBG(" Channel initialization is a no-op.");
    }
    // -------------------------------------------------------------------------
    // RESET mode: Change the OCB channel registers to scan-0 flush state
    // -------------------------------------------------------------------------
    else if (i_mode == p9pm::PM_RESET)
    {
        FAPI_DBG(" *** Resetting OCB Indirect Channels");
        FAPI_TRY(pm_ocb_reset(i_target), "ERROR: OCB Reset failed.");
    }
    // -------------------------------------------------------------------------
    // SETUP mode: Perform user setup of an indirect channel
    // -------------------------------------------------------------------------
    else if (i_mode == p9pm::PM_SETUP_ALL || i_mode == p9pm::PM_SETUP_PIB)
    {
        FAPI_INF("*** Setup OCB Indirect Channel %d ", i_ocb_chan);
        p9ocb::PM_OCB_CHAN_REG l_upd_reg = p9ocb::OCB_UPD_PIB_REG;

        if (i_mode == p9pm::PM_SETUP_ALL)
        {
            l_upd_reg = p9ocb::OCB_UPD_PIB_OCI_REG;
        }

        FAPI_TRY(pm_ocb_setup(i_target, i_ocb_chan, i_ocb_type, i_ocb_bar,
                              l_upd_reg, i_ocb_q_len, i_ocb_ouflow_en,
                              i_ocb_itp_type),
                 "ERROR: OCB Setup failed.");
    }
    // Invalid Mode
    else
    {
        FAPI_ASSERT(false, fapi2::PM_OCBINIT_BAD_MODE().set_BADMODE(i_mode),
                    "ERROR; Unknown mode passed to proc_ocb_init. Mode %x",
                    i_mode);
    }

fapi_try_exit:
    FAPI_INF("p9_pm_ocb_init EXIT");
    return fapi2::current_err;
}


fapi2::ReturnCode pm_ocb_setup(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
    const p9ocb::PM_OCB_CHAN_NUM    i_ocb_chan,
    const p9ocb::PM_OCB_CHAN_TYPE   i_ocb_type,
    const uint32_t                  i_ocb_bar,
    const p9ocb::PM_OCB_CHAN_REG    i_ocb_upd_reg,
    const uint8_t                   i_ocb_q_len,
    const p9ocb::PM_OCB_CHAN_OUFLOW i_ocb_ouflow_en,
    const p9ocb::PM_OCB_ITPTYPE     i_ocb_itp_type)
{
    FAPI_INF("pm_ocb_setup Enter");

    uint32_t l_ocbase = 0x0;
    fapi2::buffer<uint64_t> l_mask_or(0);
    fapi2::buffer<uint64_t> l_mask_clear(0);
    fapi2::buffer<uint64_t> l_data64;

    // Verify input queue length is valid
    if ((i_ocb_type == p9ocb::OCB_TYPE_PUSHQ) ||
        (i_ocb_type == p9ocb::OCB_TYPE_PULLQ))
    {
        // check queue_len
        if (i_ocb_q_len > MAX_OCB_QUE_LEN)
        {
            FAPI_ASSERT(
                false,
                fapi2::PM_OCBINIT_BAD_Q_LENGTH_PARM().
                set_BADQLENGTH(i_ocb_q_len).
                set_CHANNEL(i_ocb_chan).
                set_TYPE(i_ocb_type),
                "ERROR: Bad Queue Length Passed to Procedure => %d",
                i_ocb_q_len);
        }
    }

    // -------------------------------------------------------------------------
    // Init Status and Control Register (OCBCSRn, OCBCSRn_CLEAR, OCBCSRn_OR)
    //    bit 2 => pull_read_underflow_en (0=disabled 1=enabled)
    //    bit 3 => push_write_overflow_en (0=disabled 1=enabled)
    //    bit 4 => ocb_stream_mode        (0=disabled 1=enabled)
    //    bit 5 => ocb_stream_type        (0=linear   1=circular)
    // -------------------------------------------------------------------------

    if (i_ocb_type == p9ocb::OCB_TYPE_LIN) // linear non-streaming
    {
        l_mask_clear.setBit<4, 2>();
    }
    else if (i_ocb_type == p9ocb::OCB_TYPE_LINSTR) // linear streaming
    {
        l_mask_or.setBit<4>();
        l_mask_clear.setBit<5>();
    }
    else if (i_ocb_type == p9ocb::OCB_TYPE_CIRC) // circular
    {
        l_mask_or.setBit<4, 2>();
    }
    else if (i_ocb_type == p9ocb::OCB_TYPE_PUSHQ) // push queue
    {
        l_mask_or.setBit<4, 2>();

        if (i_ocb_ouflow_en == p9ocb::OCB_Q_OUFLOW_EN)
        {
            l_mask_or.setBit<3>();
        }
        else if (i_ocb_ouflow_en == p9ocb::OCB_Q_OUFLOW_DIS)
        {
            l_mask_clear.setBit<3>();
        }
    }
    else if (i_ocb_type == p9ocb::OCB_TYPE_PULLQ) // pull queue
    {
        l_mask_or.setBit<4, 2>();

        if (i_ocb_ouflow_en == p9ocb::OCB_Q_OUFLOW_EN)
        {
            l_mask_or.setBit<2>();
        }
        else if (i_ocb_ouflow_en == p9ocb::OCB_Q_OUFLOW_DIS)
        {
            l_mask_clear.setBit<2>();
        }
    }

    FAPI_DBG("Writing to Channel %d Register : OCB Channel Status & Control",
             i_ocb_chan);

    // write using OR mask
    FAPI_TRY(fapi2::putScom(i_target, OCBCSRn_OR[i_ocb_chan], l_mask_or),
             "ERROR: Unexpected error encountered in write to OCB Channel "
             "Status & Control using OR mask");
    // write using AND mask
    FAPI_TRY(fapi2::putScom(i_target, OCBCSRn_CLEAR[i_ocb_chan], l_mask_clear),
             "ERROR: Unexpected error encountered in write to OCB Channel "
             "Status & Control using and mask");

    //--------------------------------------------------------------------------
    // set address base register for linear, pull queue or push queue
    //--------------------------------------------------------------------------
    //don't update bar if type null or circular
    if (!(i_ocb_type == p9ocb::OCB_TYPE_NULL ||
          i_ocb_type == p9ocb::OCB_TYPE_CIRC))
    {
        // BAR for linear (streaming / non-streaming)
        if ((i_ocb_type == p9ocb::OCB_TYPE_LIN) ||
            (i_ocb_type == p9ocb::OCB_TYPE_LINSTR))
        {
            l_ocbase = OCBARn[i_ocb_chan];
        }
        // BAR for push queue
        else if (i_ocb_type == p9ocb::OCB_TYPE_PUSHQ)
        {
            l_ocbase = OCBSHBRn[i_ocb_chan];
        }
        // BAR for pull queue
        else
        {
            l_ocbase = OCBSLBRn[i_ocb_chan];
        }

        l_data64.flush<0>().insertFromRight<0, 32>(i_ocb_bar);

        FAPI_DBG("Writing to Channel %d Register : OCB Channel Base Address",
                 i_ocb_chan);

        FAPI_TRY(fapi2::putScom(i_target, l_ocbase, l_data64),
                 "ERROR: Unexpected encountered in write to OCB Channel "
                 "Base Address");

    }

    // -------------------------------------------------------------------------
    // set up push queue control register
    //    bits 4:5  => push interrupt action
    //                   00=full
    //                   01=not full
    //                   10=empty
    //                   11=not empty
    //    bits 6:10 => push queue length
    //    bit  31   => push queue enable
    // -------------------------------------------------------------------------
    if ((i_ocb_type == p9ocb::OCB_TYPE_PUSHQ) &&
        (i_ocb_upd_reg == p9ocb::OCB_UPD_PIB_OCI_REG))
    {
        l_data64.flush<0>().insertFromRight<6, 5>(i_ocb_q_len);
        l_data64.insertFromRight<4, 2>(i_ocb_itp_type);
        l_data64.setBit<31>();

        FAPI_DBG("Writing to Channel %d Register : OCB Channel Push "
                 "Control/Status Address", i_ocb_chan);
        FAPI_TRY(fapi2::putScom(i_target, OCBSHCSn[i_ocb_chan], l_data64),
                 "ERROR : Unexpected error encountered in write to OCB "
                 "Channel Push Address");
    }

    // -------------------------------------------------------------------------
    // set up pull queue control register
    //    bits 4:5  => pull interrupt action
    //                   00=full
    //                   01=not full
    //                   10=empty
    //                   11=not empty
    //    bits 6:10 => pull queue length
    //    bit  31   => pull queue enable
    // -------------------------------------------------------------------------
    if ((i_ocb_type == p9ocb::OCB_TYPE_PULLQ) &&
        (i_ocb_upd_reg == p9ocb::OCB_UPD_PIB_OCI_REG))
    {
        l_data64.flush<0>().insertFromRight<6, 5>(i_ocb_q_len);
        l_data64.insertFromRight<4, 2>(i_ocb_itp_type);
        l_data64.setBit<31>();

        FAPI_DBG("Writing to Channel %d ,"
                 "Register : OCB Channel Pull Control/Status Address",
                 i_ocb_chan);
        FAPI_TRY(fapi2::putScom(i_target, OCBSLCSn[i_ocb_chan], l_data64),
                 "ERROR : Unexpected error encountered in write to OCB "
                 "Channel Pull Address");
    }

    // -------------------------------------------------------------------------
    // Print Channel Configuration Info
    // -------------------------------------------------------------------------
    FAPI_INF("-----------------------------------------------------");
    FAPI_INF("OCB Channel Configuration                            ");
    FAPI_INF("-----------------------------------------------------");
    FAPI_INF("  channel number             => %d ", i_ocb_chan);
    FAPI_INF("  channel type               => %d ", i_ocb_type);

    if ((i_ocb_type == p9ocb::OCB_TYPE_PUSHQ) ||
        (i_ocb_type == p9ocb::OCB_TYPE_PULLQ))
    {
        FAPI_INF("  queue length               => %d ", i_ocb_q_len);
        FAPI_INF("  interrupt type             => %d ", i_ocb_itp_type);

        if (i_ocb_type == p9ocb::OCB_TYPE_PUSHQ)
        {
            FAPI_INF("  push write overflow enable => %d ", i_ocb_ouflow_en);
        }
        else
        {
            FAPI_INF("  pull write underflow enable => %d ", i_ocb_ouflow_en);
        }
    }

    FAPI_INF("  channel base address       => 0x%08X ", i_ocb_bar);
    FAPI_INF("-----------------------------------------------------");

fapi_try_exit:
    return fapi2::current_err;
}


fapi2::ReturnCode pm_ocb_reset(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target)
{
    FAPI_INF("p9_pm_ocb_reset Enter");
    fapi2::buffer<uint64_t> l_buf64;

    // vector of reset channels
    std::vector<uint8_t> v_reset_chan;
    v_reset_chan.push_back(1);

    // -------------------------------------------------------------------------
    // Loop over PIB Registers
    // -------------------------------------------------------------------------
    for (auto& chan : v_reset_chan)
    {
        FAPI_INF(" Reset OCB channel : %d", chan);

        fapi2::buffer<uint64_t> l_data64;
        // Clear out OCB Channel BAR registers
        FAPI_TRY(fapi2::putScom(i_target, OCBARn[chan], 0),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d BAR Register", chan);

        // Clear out OCB Channel control and status registers
        l_data64.flush<1>();
        FAPI_TRY(fapi2::putScom(i_target, OCBCSRn_CLEAR[chan], l_data64),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Control & Status Register Clear", chan);

        // Put channels in Circular mode
        //  - set bits 4,5 (circular mode) using OR
        l_data64.flush<0>().setBit<4>().setBit<5>();
        FAPI_TRY(fapi2::putScom(i_target, OCBCSRn_OR[chan], l_data64),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Control & Status OR Register Set", chan);

        // Clear out OCB Channel Error Status registers
        FAPI_TRY(fapi2::putScom(i_target, OCBESRn[chan], 0),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Error Status Register", chan);
    }

    // -------------------------------------------------------------------------
    // Loop over OCI Registers
    // -------------------------------------------------------------------------
    for (auto& chan : v_reset_chan)
    {
        FAPI_INF(" Reset OCB channel : %d", chan);

        fapi2::buffer<uint64_t> l_data64;
        // Clear out Pull Base
        FAPI_TRY(fapi2::putScom(i_target, OCBSLBRn[chan], 0),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Pull Base Register", chan);

        // Clear out Push Base
        FAPI_TRY(fapi2::putScom(i_target, OCBSHBRn[chan], 0),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Push Base Register", chan);

        // Clear out Pull Control & Status
        FAPI_TRY(fapi2::putScom(i_target, OCBSLCSn[chan], 0),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Pull Control & Status Register", chan);

        // Clear out Push Control & Status
        FAPI_TRY(fapi2::putScom(i_target, OCBSHCSn[chan], 0),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Push Control & Status Register", chan);

        // Clear out Stream Error Status
        FAPI_TRY(fapi2::putScom(i_target, OCBSESn[chan], 0),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Stream Error Status Register", chan);

        // Clear out Linear Window Control
        FAPI_TRY(fapi2::putScom(i_target, OCBLWCRn[chan], 0),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Linear Window Control Register", chan);

        // Clear out Linear Window Base
        //  - set bits 3:9
        l_data64.setBit<3, 7>();
        FAPI_TRY(fapi2::putScom(i_target, OCBLWSBRn[chan], l_data64),
                 "**** ERROR : Unexpected error encountered in write to OCB "
                 "Channel %d Linear Window Base Register", chan);
    }

    // Set Interrupt Source Mask Registers 0 & 1
    //  - keep word1 0's for simics
    l_buf64.flush<0>().insertFromRight<0, 32>(INTERRUPT_SRC_MASK_REG);
    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OIMR0_OR,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Source Mask Register0 (OIMR0)");

    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OIMR1_OR,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Source Mask Register1 (OIMR1)");

    // Clear OCC Interrupt Type Registers 0 & 1
    l_buf64.flush<1>();
    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OITR0_CLEAR,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Type Register0 (OITR0)");

    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OITR1_CLEAR,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Type Register1 (OITR1)");

    // Clear OCC Interupt Edge/Polarity Registers 0 & 1
    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OIEPR0_CLEAR,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Edge Polarity Register0 (OIEPR0)");

    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OIEPR1_CLEAR,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Edge Polarity Register1 (OIEPR1)");

    // Clear OCC Interrupt Source Registers 0 & 1
    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OISR0_CLEAR,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Source Register0 (OISR0)");

    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OISR1_CLEAR,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Source Register1 (OISR1)");

    // Clear Interrupt Route (A, B, C) Registers 0 & 1
    FAPI_TRY(fapi2::putScom(i_target, PU_OCB_OCI_OIRR0A_SCOM, 0),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt 0 Route A Register (OIRR0A)");

    FAPI_TRY(fapi2::putScom(i_target, PU_OCB_OCI_OIRR0B_SCOM, 0),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt 0 Route B Register (OIRR0A)");

    FAPI_TRY(fapi2::putScom(i_target, PU_OCB_OCI_OIRR0C_SCOM, 0),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt 0 Route C Register (OIRR0A)");

    FAPI_TRY(fapi2::putScom(i_target, PU_OCB_OCI_OIRR1A_SCOM, 0),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt 1 Route A Register (OIRR1A)");

    FAPI_TRY(fapi2::putScom(i_target, PU_OCB_OCI_OIRR1B_SCOM, 0),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt 1 Route B Register (OIRR1B)");

    FAPI_TRY(fapi2::putScom(i_target, PU_OCB_OCI_OIRR1C_SCOM, 0),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt 1 Route C Register (OIRR1C)");

    // Clear OCC Interrupt Timer Registers 0 & 1
    //  - need bits 0&1 set to clear register
    l_buf64.flush<0>().setBit<0, 2>();

    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OTR0_SCOM,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Timer0 Register (OTR0)");

    FAPI_TRY(fapi2::putScom(i_target,
                            PU_OCB_OCI_OTR1_SCOM,
                            l_buf64),
             "**** ERROR : Unexpected error encountered in write to OCC "
             "Interrupt Timer1 Register (OTR1)");

    // Clear PBA Enable Marker Acknowledgement mode to remove collisions
    // with any accesses to the OCB DCR registers (eg OSTOESR).
    // This function is only enabled by OCC firmware and is not via
    // hardware procedures.
    FAPI_TRY(fapi2::getScom(i_target, PU_PBAMODE_SCOM, l_buf64),
             "**** ERROR : Failed to fetch PBA mode control status");
    l_buf64.clearBit<PU_PBAMODE_EN_MARKER_ACK>();
    FAPI_TRY(fapi2::putScom(i_target, PU_PBAMODE_SCOM, l_buf64),
             "**** ERROR : Failed to write PBA mode control");

    // Clear OCC special timeout error status register
    FAPI_TRY(fapi2::putScom(i_target, PU_OCB_PIB_OSTOESR, 0),
             "**** ERROR : Failed to write OSTESR");

    // Explicitly disable the OCC Heartbeat (RTC: 172638)
    // Only clearing the OCB_OCI_OCCHBR_OCC_HEARTBEAT_EN and leaving the
    // Heartbeat count intact as this may prove useful for debug later.
    FAPI_TRY(fapi2::getScom(i_target, PU_OCB_OCI_OCCHBR_SCOM, l_buf64),
             "**** ERROR : Failed to read OCBHBR");
    l_buf64.clearBit<PU_OCB_OCI_OCCHBR_OCC_HEARTBEAT_EN>();
    FAPI_TRY(fapi2::putScom(i_target, PU_OCB_OCI_OCCHBR_SCOM, l_buf64),
             "**** ERROR : Failed to write OCBHBR");

fapi_try_exit:
    return fapi2::current_err;
}
