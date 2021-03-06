/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/nest/p9_tod_utils.H $      */
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
//------------------------------------------------------------------------------
//
/// @file p9_tod_utils.H
/// @brief Utilities and classes for the tod_init and tod_setup procedures
///
// *HWP HWP Owner: Nick Klazynski jklazyns@us.ibm.com
// *HWP HWP Owner: Joachim Fenkes fenkes@de.ibm.com
// *HWP FW Owner: Manish Chowdhary manichow@in.ibm.com
// *HWP Team: Nest
// *HWP Level: 3
// *HWP Consumed by: PRD,HB
//
//------------------------------------------------------------------------------

#ifndef _P9_TOD_UTILS_H_
#define _P9_TOD_UTILS_H_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <fapi2.H>
#include <p9_perv_scom_addresses.H>
#include <p9_perv_scom_addresses_fld.H>
#include <p9_misc_scom_addresses.H>
#include <p9_misc_scom_addresses_fld.H>
#include <p9_quad_scom_addresses.H>
#include <p9_quad_scom_addresses_fld.H>


//------------------------------------------------------------------------------
// Constant definitions
//------------------------------------------------------------------------------
// TOD-grid-cycles added to MDMT delay to account for slave staging
const uint32_t MDMT_TOD_GRID_CYCLE_STAGING_DELAY = 6;

// Duration of TOD GRID cycle in picoseconds
const uint32_t TOD_GRID_PS = 400;

// Minimum/Maximum allowable delay for any node
const uint32_t MIN_TOD_DELAY = 0;
const uint32_t MAX_TOD_DELAY = 0xFF;

// TOD operation delay times for HW/sim
const uint32_t P9_TOD_UTILS_HW_NS_DELAY     = 50000;
const uint32_t P9_TOD_UTILS_SIM_CYCLE_DELAY = 50000;

// TOD retry count for hardware-cleared bits
const uint32_t P9_TOD_UTIL_TIMEOUT_COUNT = 20;

// SCOMC register field definitions
const uint32_t SPRC_REG_SEL_TFMR_T0 = 8;
const uint32_t SPRC_REG_SEL_TFMR_T1 = 9;
const uint32_t SPRC_REG_SEL_TFMR_T2 = 10;
const uint32_t SPRC_REG_SEL_TFMR_T3 = 11;
const uint32_t SPRC_REG_SEL_TFMR_T4 = 12;
const uint32_t SPRC_REG_SEL_TFMR_T5 = 13;
const uint32_t SPRC_REG_SEL_TFMR_T6 = 14;
const uint32_t SPRC_REG_SEL_TFMR_T7 = 15;
const uint32_t SPRC_REG_SEL = 54;
const uint32_t SPRC_REG_SEL_LEN = 7;

// C_SPR_MODE register field definitions
const uint32_t SPR_MODE_REG_MODE_SPRC0_SEL = 16;
const uint32_t SPR_MODE_REG_MODE_SPRC_WR_EN = 3;

// TFMR register field definitions
const uint32_t TFMR_N_CLKS_PER_STEP_4CLK  = 3;

const uint32_t TFMR_SYNC_BIT_SEL_16US = 4;

const uint32_t TFMR_STATE_TB_RESET = 0;
const uint32_t TFMR_STATE_TB_NOT_SET = 2;
const uint32_t TFMR_STATE_TB_SYNC_WAIT = 6;
const uint32_t TFMR_STATE_GET_TOD = 7;
const uint32_t TFMR_STATE_TB_RUNNING = 8;

// PB_[EO]LINK_DLY register field definitions
const uint32_t PB_EOLINK_DLY_FMR_LINK_DELAY_LEN = 12;

// PB_ELINK_RT_DELAY register field definitions
const uint32_t PB_ELINK_RT_DELAY_CTL_SET_LINK_0 = 0;
const uint32_t PB_ELINK_RT_DELAY_CTL_SET_LINK_1 = 1;
const uint32_t PB_ELINK_RT_DELAY_CTL_SET_LINK_2 = 2;
const uint32_t PB_ELINK_RT_DELAY_CTL_SET_LINK_3 = 3;
const uint32_t PB_ELINK_RT_DELAY_CTL_SET_LINK_4 = 4;
const uint32_t PB_ELINK_RT_DELAY_CTL_SET_LINK_5 = 5;

// PB_OLINK_RT_DELAY register field definitions
const uint32_t PB_OLINK_RT_DELAY_CTL_SET_LINK_0 = 0;
const uint32_t PB_OLINK_RT_DELAY_CTL_SET_LINK_1 = 1;
const uint32_t PB_OLINK_RT_DELAY_CTL_SET_LINK_2 = 2;
const uint32_t PB_OLINK_RT_DELAY_CTL_SET_LINK_3 = 3;
const uint32_t PB_OLINK_RT_DELAY_CTL_SET_LINK_4 = 4;
const uint32_t PB_OLINK_RT_DELAY_CTL_SET_LINK_5 = 5;
const uint32_t PB_OLINK_RT_DELAY_CTL_SET_LINK_6 = 6;
const uint32_t PB_OLINK_RT_DELAY_CTL_SET_LINK_7 = 7;

// TOD_M_PATH_CTRL_REG register field definitions
const uint32_t TOD_M_PATH_CTRL_REG_M_PATH_SYNC_CREATE_SPS_512  = 0;
const uint32_t TOD_M_PATH_CTRL_REG_M_PATH_SYNC_CREATE_SPS_128  = 1;
const uint32_t TOD_M_PATH_CTRL_REG_M_PATH_SYNC_CREATE_SPS_64   = 2;
const uint32_t TOD_M_PATH_CTRL_REG_M_PATH_SYNC_CREATE_SPS_32   = 3;
const uint32_t TOD_M_PATH_CTRL_REG_M_PATH_SYNC_CREATE_SPS_4096 = 4;
const uint32_t TOD_M_PATH_CTRL_REG_M_PATH_SYNC_CREATE_SPS_2048 = 5;
const uint32_t TOD_M_PATH_CTRL_REG_M_PATH_SYNC_CREATE_SPS_1024 = 6;
const uint32_t TOD_M_PATH_CTRL_REG_M_PATH_SYNC_CREATE_SPS_256  = 7;

// TOD_S_PATH_CTRL_REG register field definitions
// The number of syncs that we are ok with the slave path missing (this must
// be set to at least 1 or there will be a fail)
const uint32_t TOD_S_PATH_CTRL_REG_REMOTE_SYNC_MISS_COUNT_2 = 5;

// TOD_[MSI]_PATH_CTRL_REG register field definitions
const uint32_t STEP_CHECK_CPS_DEVIATION_FACTOR_1 = 0;
const uint32_t STEP_CHECK_CPS_DEVIATION_FACTOR_2 = 1;
const uint32_t STEP_CHECK_CPS_DEVIATION_FACTOR_4 = 2;
const uint32_t STEP_CHECK_CPS_DEVIATION_FACTOR_8 = 3;

const uint32_t STEP_CHECK_CPS_DEVIATION_00_00_PCENT = 0x0;
const uint32_t STEP_CHECK_CPS_DEVIATION_06_25_PCENT = 0x1;
const uint32_t STEP_CHECK_CPS_DEVIATION_12_50_PCENT = 0x2;
const uint32_t STEP_CHECK_CPS_DEVIATION_18_75_PCENT = 0x3;
const uint32_t STEP_CHECK_CPS_DEVIATION_25_00_PCENT = 0x4;
const uint32_t STEP_CHECK_CPS_DEVIATION_31_25_PCENT = 0x5;
const uint32_t STEP_CHECK_CPS_DEVIATION_37_50_PCENT = 0x6;
const uint32_t STEP_CHECK_CPS_DEVIATION_43_75_PCENT = 0x7;
const uint32_t STEP_CHECK_CPS_DEVIATION_50_00_PCENT = 0x8;
const uint32_t STEP_CHECK_CPS_DEVIATION_56_25_PCENT = 0x9;
const uint32_t STEP_CHECK_CPS_DEVIATION_62_50_PCENT = 0xA;
const uint32_t STEP_CHECK_CPS_DEVIATION_68_75_PCENT = 0xB;
const uint32_t STEP_CHECK_CPS_DEVIATION_75_00_PCENT = 0xC;
const uint32_t STEP_CHECK_CPS_DEVIATION_81_25_PCENT = 0xD;
const uint32_t STEP_CHECK_CPS_DEVIATION_87_50_PCENT = 0xE;
const uint32_t STEP_CHECK_CPS_DEVIATION_93_75_PCENT = 0xF;

const uint32_t STEP_CHECK_VALIDITY_COUNT_1   = 0;
const uint32_t STEP_CHECK_VALIDITY_COUNT_2   = 1;
const uint32_t STEP_CHECK_VALIDITY_COUNT_4   = 2;
const uint32_t STEP_CHECK_VALIDITY_COUNT_8   = 3;
const uint32_t STEP_CHECK_VALIDITY_COUNT_16  = 4;
const uint32_t STEP_CHECK_VALIDITY_COUNT_32  = 5;
const uint32_t STEP_CHECK_VALIDITY_COUNT_64  = 6;
const uint32_t STEP_CHECK_VALIDITY_COUNT_128 = 7;

// TOD_CHIP_CTRL register field definitions
const uint32_t TOD_CHIP_CTRL_REG_I_PATH_CORE_SYNC_PERIOD_SEL_8   = 0;
const uint32_t TOD_CHIP_CTRL_REG_I_PATH_CORE_SYNC_PERIOD_SEL_4   = 1;
const uint32_t TOD_CHIP_CTRL_REG_I_PATH_CORE_SYNC_PERIOD_SEL_2   = 2;
const uint32_t TOD_CHIP_CTRL_REG_I_PATH_CORE_SYNC_PERIOD_SEL_1   = 3;
const uint32_t TOD_CHIP_CTRL_REG_I_PATH_CORE_SYNC_PERIOD_SEL_128 = 4;
const uint32_t TOD_CHIP_CTRL_REG_I_PATH_CORE_SYNC_PERIOD_SEL_64  = 5;
const uint32_t TOD_CHIP_CTRL_REG_I_PATH_CORE_SYNC_PERIOD_SEL_32  = 6;
const uint32_t TOD_CHIP_CTRL_REG_I_PATH_CORE_SYNC_PERIOD_SEL_16  = 7;

const uint32_t TOD_CHIP_CTRL_REG_LOW_ORDER_STEP_VAL_1  = 0x1;
const uint32_t TOD_CHIP_CTRL_REG_LOW_ORDER_STEP_VAL_2  = 0x2;
const uint32_t TOD_CHIP_CTRL_REG_LOW_ORDER_STEP_VAL_3  = 0x3;
const uint32_t TOD_CHIP_CTRL_REG_LOW_ORDER_STEP_VAL_4  = 0x4;
const uint32_t TOD_CHIP_CTRL_REG_LOW_ORDER_STEP_VAL_3F = 0x3F;

// TOD_LOAD_REG register field definitions
const uint64_t PERV_TOD_LOAD_REG_LOAD_VALUE = 0x0000000000003FFCULL;

// TOD_TX_TTYPE_CTRL_REG register field definitions
const uint64_t PERV_TOD_TX_TTYPE_CTRL_REG_MASK_VALUE = 0x00010AA310000000ULL;

// TOD_[PRI, SEC]_PORT_[0,1]_CTRL_REG register field definitions
const uint32_t TOD_PORT_CTRL_REG_TX_X0_SEL = 4;
const uint32_t TOD_PORT_CTRL_REG_TX_X1_SEL = 6;
const uint32_t TOD_PORT_CTRL_REG_TX_X2_SEL = 8;
const uint32_t TOD_PORT_CTRL_REG_TX_X3_SEL = 10;
const uint32_t TOD_PORT_CTRL_REG_TX_X4_SEL = 12;
const uint32_t TOD_PORT_CTRL_REG_TX_X5_SEL = 14;
const uint32_t TOD_PORT_CTRL_REG_TX_X6_SEL = 16;
const uint32_t TOD_PORT_CTRL_REG_TX_X7_SEL = 18;
const uint32_t TOD_PORT_CTRL_REG_TX_X0_EN = 20;
const uint32_t TOD_PORT_CTRL_REG_TX_X1_EN = 21;
const uint32_t TOD_PORT_CTRL_REG_TX_X2_EN = 22;
const uint32_t TOD_PORT_CTRL_REG_TX_X3_EN = 23;
const uint32_t TOD_PORT_CTRL_REG_TX_X4_EN = 24;
const uint32_t TOD_PORT_CTRL_REG_TX_X5_EN = 25;
const uint32_t TOD_PORT_CTRL_REG_TX_X6_EN = 26;
const uint32_t TOD_PORT_CTRL_REG_TX_X7_EN = 27;
const uint32_t TOD_PORT_CTRL_REG_I_PATH_DELAY = 32;

const uint32_t TOD_PORT_CTRL_REG_RX_LEN = 3;
const uint32_t TOD_PORT_CTRL_REG_TX_LEN = 2;
const uint32_t TOD_PORT_CTRL_REG_I_PATH_DELAY_LEN = 8;

const uint32_t TOD_PORT_CTRL_REG_RX_X0_SEL = 0x0;
const uint32_t TOD_PORT_CTRL_REG_RX_X1_SEL = 0x1;
const uint32_t TOD_PORT_CTRL_REG_RX_X2_SEL = 0x2;
const uint32_t TOD_PORT_CTRL_REG_RX_X3_SEL = 0x3;
const uint32_t TOD_PORT_CTRL_REG_RX_X4_SEL = 0x4;
const uint32_t TOD_PORT_CTRL_REG_RX_X5_SEL = 0x5;
const uint32_t TOD_PORT_CTRL_REG_RX_X6_SEL = 0x6;
const uint32_t TOD_PORT_CTRL_REG_RX_X7_SEL = 0x7;

const uint32_t TOD_PORT_CTRL_REG_S_PATH_0 = 0x0;
const uint32_t TOD_PORT_CTRL_REG_S_PATH_1 = 0x1;
const uint32_t TOD_PORT_CTRL_REG_M_PATH_0 = 0x2;
const uint32_t TOD_PORT_CTRL_REG_M_PATH_1 = 0x3;


//------------------------------------------------------------------------------
// Structure definitions
//------------------------------------------------------------------------------
extern "C" {

    enum p9_tod_setup_tod_sel
    {
        TOD_PRIMARY,  // configure primary TOD
        TOD_SECONDARY // configure secondary TOD
    };

    enum p9_tod_setup_bus
    {
        // for MDMT
        NONE,
        // FBC x0
        XBUS0,
        // FBC x1
        XBUS1,
        // FBC x2
        XBUS2,
        // FBC x3/a0
        OBUS0,
        // FBC x4/a1
        OBUS1,
        // FBC x5/a2
        OBUS2,
        // FBC x6/a3
        OBUS3,
        // should not be used
        XBUS7,
        // added to satisfy HWSV code dependencies
        BUS_MAX = OBUS3
    };

    enum tod_action
    {
        TOD_SETUP,
        TOD_INIT,
        TOD_STATUS,
        TOD_CLEANUP,
        TOD_CHECK_OSC,
        TOD_MOVE_TOD_TO_TB,
        TOD_SAVE_CONFIG
    };

    // Input which determines the master oscillator to use
    enum p9_tod_setup_osc_sel
    {
        TOD_OSC_0,             // oscillator connected to OSC0 and not OSC1
        TOD_OSC_1,             // oscillator connected to OSC1 and not OSC0
        TOD_OSC_0_AND_1,       // oscillators connected to both OSC0 and OSC1
        TOD_OSC_0_AND_1_SEL_0, // oscillators connected to both OSC0 and OSC1,
        //   but OSC0 will always be selected
        TOD_OSC_0_AND_1_SEL_1, // oscillators connected to both OSC0 and OSC1,
        //   but OSC1 will always be selected
        TOD_OSC_NONE           // no oscillators connected
    };

    // enum that gives which oscilators we need to disable,
    // used in p9_tod_cleanup
    enum p9_tod_osc_disable
    {
        TOD_OSC_0_DISABLE,
        TOD_OSC_1_DISABLE,
        TOD_OSC_0_AND_1_DISABLE,
        TOD_OSC_DISABLE_NONE
    };

    struct p9_tod_setup_conf_regs
    {
        fapi2::buffer<uint64_t> tod_m_path_ctrl_reg;
        fapi2::buffer<uint64_t> tod_pri_port_0_ctrl_reg;
        fapi2::buffer<uint64_t> tod_pri_port_1_ctrl_reg;
        fapi2::buffer<uint64_t> tod_sec_port_0_ctrl_reg;
        fapi2::buffer<uint64_t> tod_sec_port_1_ctrl_reg;
        fapi2::buffer<uint64_t> tod_s_path_ctrl_reg;
        fapi2::buffer<uint64_t> tod_i_path_ctrl_reg;
        fapi2::buffer<uint64_t> tod_pss_mss_ctrl_reg;
        fapi2::buffer<uint64_t> tod_chip_ctrl_reg;
    };

    struct tod_topology_node
    {
        fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>* i_target;
        bool i_tod_master;
        bool i_drawer_master;
        p9_tod_setup_bus i_bus_rx; // Current node's bus from which step/sync is
        //   received ("Receive Port" in eiinfo)
        p9_tod_setup_bus i_bus_tx; // Upstream node's bus from which step/sync
        //   is transmitted ("Drive Port" in eiinfo)
        std::list<tod_topology_node*> i_children;
        p9_tod_setup_conf_regs o_todRegs;
        uint32_t o_int_path_delay;
    };

} //extern "C"

#endif //_P9_TOD_UTILS_H_

