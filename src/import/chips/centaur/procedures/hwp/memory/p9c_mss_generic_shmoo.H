/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/centaur/procedures/hwp/memory/p9c_mss_generic_shmoo.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2018                        */
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
/// @file mss_generic_shmoo.H
/// @brief Memory Subsystem Generic Shmoo -- abstraction for HB
///
/// *HWP HWP Owner: Andre Marin <aamarin@us.ibm.com>
/// *HWP HWP Backup: Stephen Glancy <sglancy@us.ibm.com>
/// *HWP Team: Memory
/// *HWP Level: 2
/// *HWP Consumed by: HB:CI
///

#include <fapi2.H>

#ifndef generic_shmoo_H
#define generic_shmoo_H

//! Globals
#define SHMOO_DEBUG 0
#define SHMOO_DEBUG2 0
#include <p9c_mss_shmoo_common.H>
#include <p9c_mss_mcbist.H>
#include <map>

///
/// @brief MSS Generic Shmoo Class.. Inherits from PHY access class and the knob abstraction
///
class generic_shmoo
{
    private:

        //! MBS Config : Port + Socket + Knobs
        struct SHMOO_SCENARIO
        {
            struct MBS_CONFIG
            {
                struct PORT
                {
                    struct RANK
                    {
                        shmoo_knob_data_t K; // Set of knobs used by this shmoo
                    } S[MAX_RANK];      //Max Rank are 8
                } P[MAX_PORT];          // Max Port 2
            } MBA;
            shmoo_knob_config_t static_knob; // Static info regarding the knob
        } SHMOO;

        //! Result Data
        shmoo_algorithm_t iv_algorithm;
        shmoo_mode iv_mcbist_mode;
        uint8_t iv_mcbist_error_map[MAX_PORT][MAX_RANK][MAX_BYTE][MAX_NIBBLES];    //MAX byte is 10; Max Nibble are 2;
        uint8_t iv_count_bad_dq[MAX_PORT];
        uint8_t iv_shmoo_error_map[MAX_PORT][MAX_RANK][MAX_BYTE * MAX_NIBBLES];  // 10 * 2 = 20 Nibbles
        uint8_t iv_binary_done_map[MAX_PORT][MAX_RANK][MAX_BYTE * MAX_NIBBLES];   // 10 * 2 = 20 Nibbles
        shmoo_type_t iv_shmoo_mask;
        uint8_t iv_addr;
        uint8_t iv_MAX_RANKS[MAX_PORT];
        uint8_t iv_MAX_BYTES;
        uint32_t iv_pattern;
        uint32_t iv_test_type;
        uint8_t iv_dmm_type;
        uint8_t iv_SHMOO_ON;
        uint8_t iv_DQS_ON;
        uint8_t iv_shmoo_type;
        uint16_t iv_shmoo_param;
        uint16_t iv_binary_diff;
        uint32_t iv_vref_mul;
        uint8_t iv_valid_rank[MAX_PORT][MAX_RANK];
        uint16_t iv_dqs_rb_regval[MAX_PORT][MAX_RANK][MAX_DQ];
        uint16_t iv_dqs_lb_regval[MAX_PORT][MAX_RANK][MAX_DQ];
        uint16_t iv_dqs_nom_val[MAX_PORT][MAX_RANK][MAX_DQ];
        uint8_t iv_isdm_c4_dq[MAX_PORTS_PER_CEN][DIMM_TO_C4_DQ_ENTRIES];
        uint8_t iv_isdm_c4_dqs[MAX_PORTS_PER_CEN][DIMM_TO_C4_DQS_ENTRIES];

    public:

        enum bound_t {LEFT, RIGHT};
        ///
        /// @brief generic shmoo constructor
        ///
        generic_shmoo(uint8_t i_addr, shmoo_type_t i_shmoo_mask, shmoo_algorithm_t shmoo_algorithm);

        ///
        /// @brief generic shmoo constructor
        ///
        generic_shmoo() = default;

        ///
        /// @brief generic shmoo destructor
        ///
        ~generic_shmoo() = default;

        ///
        /// @brief Sets up shmoo instance variables
        /// @param[in] i_target Centaur input mba
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode setup(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief This function does the initialization of various schmoo parameters
        /// @param[in] i_init_val initial value to initialize to
        /// @param[out] io_array address of array
        ///
        void init_multi_array(const uint16_t i_init_val,
                              uint16_t (&io_array)[MAX_DQ]);

        ///
        /// @brief This function gets the nominal values for each DQ of relevant knobs
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode get_all_noms(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief  set all bits
        /// @param[in] i_target Centaur input MBA
        /// @param[in] bound RIGHT/LEFT
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode set_all_binary(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                         const bound_t bound);

        ///
        /// @brief Read in all nominal values of relevant knobs
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode get_all_noms_dqs(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief This function calls the knob_update for each DQ which is used to find bound  that is left/right according to schmoo type
        /// @param[in] i_target Centaur input MBA
        /// @param[in] bound RIGHT/LEFT
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode find_bound(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                     const bound_t);

        ///
        /// @brief Does the box shmoo
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode do_box_shmoo(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief This is a key function is used to find right and left bound using new algorithm -- there is an option u can chose not to use it by setting a flag
        /// @param[in] i_target Centaur input MBA Parameters: Target:MBA,bound:RIGHT/LEFT,scenario:type of schmoo,iv_port:0/1,rank:0-7,byte:0-7,nibble:0/1,bit:0-3,pass,
        /// @param[in] bound RIGHT/LEFT
        /// @param[in] scenario type of shmoo
        /// @param[in] bit 0-3
        /// @param[in] pass
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode knob_update(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                      const bound_t bound,
                                      const uint8_t scenario,
                                      const uint8_t bit,
                                      uint8_t pass);

        ///
        /// @brief used to find right and left bound
        /// @param[in] i_target Centaur input MBA
        /// @param[in] bound RIGHT/LEFT
        /// @param[in] scenario type of shmoo
        /// @param[in] bit 0-3
        /// @param[in] pass
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode knob_update_bin(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                          const bound_t bound,
                                          const uint8_t scenario,
                                          const uint8_t bit,
                                          uint8_t pass);

        ///
        /// @brief used to find right and left bound
        /// @param[in] i_target Centaur input MBA
        /// @param[in] bound RIGHT/LEFT
        /// @param[in] scenario type of shmoo
        /// @param[in] bit 0-3
        /// @param[in] pass
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode knob_update_dqs_by8(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                              const bound_t bound,
                                              const uint8_t scenario,
                                              const uint8_t bit,
                                              uint8_t pass);

        ///
        /// @brief used to find right and left bound
        /// @param[in] i_target Centaur input MBA
        /// @param[in] bound RIGHT/LEFT
        /// @param[in] scenario type of shmoo
        /// @param[in] bit 0-3
        /// @param[in] pass
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode knob_update_dqs_by4(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                              const bound_t bound,
                                              const uint8_t scenario,
                                              const uint8_t bit,
                                              uint8_t pass);

        ///
        /// @brief This function is used to print the information needed such as freq,voltage etc, and also the right,left and total margin
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode print_report(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief This function is used to print the information needed such as freq,voltage etc, and also the right,left and total margin
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode print_report_dqs(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief This function is used to print the information needed such as freq,voltage etc, and also the right,left and total margin
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode print_report_dqs2(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief This function is used to get margins for setup and hold in Ps by using frequency
        /// @param[in] i_target Centaur input MBA
        /// @param[in,out] io_right_margin_val array reference of hold margins
        /// @param[in,out] io_left_margin_val array reference of setup margins
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode get_margin2(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                      uint16_t (&io_right_margin_val)[MAX_PORTS_PER_MBA][MAX_RANK][MAX_DQ],
                                      uint16_t (&io_left_margin_val)[MAX_PORTS_PER_MBA][MAX_RANK][MAX_DQ]);

        ///
        /// @brief This function is used to get DQS margins for setup and hold in Ps by using frequency for x4 DIMM
        /// @param[in] i_target Centaur input MBA
        /// @param[in,out] io_right_margin_val array reference of right-side margins
        /// @param[in,out] io_left_margin_val array reference of left-side margins
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode get_margin_dqs_by4(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                             uint16_t (&right_margin_val)[MAX_PORTS_PER_MBA][MAX_RANK][MAX_DQ],
                                             uint16_t (&left_margin_val)[MAX_PORTS_PER_MBA][MAX_RANK][MAX_DQ]);

        ///
        /// @brief This function is used to get DQS margins for setup and hold in Ps by using frequency for x8 DIMM
        /// @param[in] i_target Centaur input MBA
        /// @param[in,out] io_right_margin_val array reference of right-side margins
        /// @param[in,out] io_left_margin_val array reference of left-side margins
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode get_margin_dqs_by8(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                             uint16_t (&right_margin_val)[MAX_PORTS_PER_MBA][MAX_RANK][MAX_DQ],
                                             uint16_t (&left_margin_val)[MAX_PORTS_PER_MBA][MAX_RANK][MAX_DQ]);

        ///
        /// @brief This function is used to get margin for setup,hold and total eye width in Ps by using frequency
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode get_min_margin(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                         uint32_t* o_right_min_margin,
                                         uint32_t* o_left_min_margin);

        ///
        /// @brief This function is used to get margin for setup,hold and total eye width in Ps by using frequency
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode get_min_margin2(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                          uint32_t* o_right_min_margin,
                                          uint32_t* o_left_min_margin);

        ///
        /// @brief This function is used to get margin for setup,hold and total eye width in Ps by using frequency
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode get_min_margin_dqs(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                             uint32_t* o_right_min_margin,
                                             uint32_t* o_left_min_margin);

        ///
        /// @brief do mcbist check for error on particular nibble
        /// @param[in] i_target: centaur input mba
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode do_mcbist_test(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief do mcbist reset
        /// @param[in] i_target Centaur input mba
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode do_mcbist_reset(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief used by do_mcbist_test  to check the error map for particular nibble
        /// @param[in] i_target Centaur input MBA
        /// @param[in] l_p Centaur input port
        /// @param[out] pass 1 = error found in mcb_error_map
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode check_error_map(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                          uint8_t port,
                                          uint8_t& pass);

        ///
        /// @brief used by do_mcbist_test  to check the error map for particular nibble
        /// @param[in] i_target Centaur input MBA
        /// @param[in] l_p Centaur input port
        /// @param[out] pass 1 = error found in mcb_error_map
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode check_error_map2(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                           uint8_t port,
                                           uint8_t& pass);

        ///
        /// @brief do intial mcbist check in nominal and report spd if any bad bit found
        /// @param[in] i_target Centaur input mba
        /// @return FAPI2_RC_SUCCESS iff succesful
        ///
        fapi2::ReturnCode sanity_check(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief parse the MCBIST error map and report spd if any bad bit found
        /// @param[in] i_target Centaur input mba
        /// @return FAPI2_RC_SUCCESS iff succesful
        ///
        fapi2::ReturnCode parse_error_map(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief call setup_mcbist
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode schmoo_setup_mcb(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief used to find right and left bound
        /// @param[in] i_target Centaur input MBA
        /// @param[in] bound RIGHT/LEFT
        /// @param[in] scenario type of shmoo
        /// @param[in] bit 0-3
        /// @param[in] pass
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode knob_update_dqs_by8_isdimm(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                const bound_t bound,
                const uint8_t scenario,
                const uint8_t bit,
                uint8_t pass);

        ///
        /// @brief used to find right and left bound
        /// @param[in] i_target Centaur input MBA
        /// @param[in] bound RIGHT/LEFT
        /// @param[in] scenario type of shmoo
        /// @param[in] bit 0-3
        /// @param[in] pass
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode knob_update_dqs_by4_isdimm(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                const bound_t bound,
                const uint8_t scenario,
                const uint8_t bit,
                uint8_t pass);

        ///
        /// @brief Delegator function that runs shmoo using other  functions
        /// @param[in] i_target Centaur input mba
        /// @param[out] o_right_min_margin Minimum hold time
        /// @param[out] o_left_min_margin Minimum setup time
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode run(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                              uint32_t* right_min_margin,
                              uint32_t* left_min_margin,
                              uint32_t i_vref_mul);
        ///
        /// @brief save and restore registers before and after shmoo
        /// @param[in] i_target Centaur input MBA
        /// @param[in] i_content_array register contents to save/restore
        /// @param[in] i_mode 0 = save; 1 = restore
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode shmoo_save_rest(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                          uint64_t i_content_array[],
                                          const uint8_t i_mode);

        ///
        /// @brief Sets the PDA nibble table  with shmoo values
        /// @param[in] i_target Centaur input MBA
        /// @param[in] pda_nibble_table Per nibble vref + margins
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode get_nibble_pda(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                                         uint32_t (&pda_nibble_table)[MAX_PORTS_PER_MBA][MAX_DIMM_PER_PORT][MAX_RANKS_PER_DIMM][MAX_DRAMS_PER_RANK_X4][2]);

        ///
        /// @brief used to find right and left bound
        /// @param[in] i_target Centaur input MBA
        /// @param[in] bound RIGHT/LEFT
        /// @param[in] scenario type of shmoo
        /// @param[in] bit 0-3
        /// @param[in] pass
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode knob_update_bin_composite(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target,
                const bound_t bound,
                const uint8_t scenario,
                const uint8_t bit,
                uint8_t pass);

        ///
        /// @brief This function is used to print the information needed such as freq,voltage etc, and also the right,left and total margin
        /// @param[in] i_target Centaur input MBA
        /// @return FAPI2_RC_SUCCESS iff successful
        ///
        fapi2::ReturnCode print_report2(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target);

        ///
        /// @brief Calculate the absolute value of the difference between two values
        /// @param[in] i_val1 first value
        /// @param[in] i_val2 second value
        /// @return absolute value of the difference between i_val1 and i_val2
        ///
        uint16_t absolute_difference(const uint16_t i_val1,
                                     const uint16_t i_val2);

};

// Below are various training_advanced helper functions, placed here so they can be unit tested in p9c_mss_generic_shmoo_ut.C

///
/// @brief helper function for testing ternary search
/// @param[in] i_results map of test results that have been found so far
/// @param[in,out] io_low_bound lower search boundary's vref value
/// @param[in,out] io_high_bound higher search boundary's vref value
/// @param[out] o_test_vref next Vref value to test
/// @param[out] o_complete true if the search is complete, false otherwise
/// @return FAPI2_RC_SUCCESS if no errors encountered
///
fapi2::ReturnCode ternary_search_helper(const std::map<uint8_t, uint32_t>& i_results,
                                        uint8_t& io_low_bound,
                                        uint8_t& io_high_bound,
                                        uint8_t& o_test_vref,
                                        bool& o_complete);

#endif
