/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/htmgt/htmgt_occ.H $                                   */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2014,2019                        */
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

#ifndef HTMGT_OCC_H
#define HTMGT_OCC_H

#include <stdint.h>
#include <vector>
#if defined(CONFIG_BMC_IPMI)
#include <ipmi/ipmisensor.H>
#endif


namespace HTMGT
{

    const uint8_t  MASTER_OCC = 0xFF;

    const uint32_t OCC_POLL_DATA_MIN_SIZE = 40;

    const uint16_t OCC_COMM_INIT_COMPLETE = 0x0EFF;

    const uint16_t OCC_INIT_FAILURE = 0xE000;

    const uint16_t OCC_TRACE_BUFFER_SIZE = 0x2000;

    enum occStateId
    {
        OCC_STATE_NO_CHANGE                = 0x00,
        OCC_STATE_STANDBY                  = 0x01,
        OCC_STATE_OBSERVATION              = 0x02,
        OCC_STATE_ACTIVE                   = 0x03,
        OCC_STATE_SAFE                     = 0x04,
        OCC_STATE_CHARACTERIZATION         = 0x05,
        // the following states are internal to TMGT
        OCC_STATE_RESET                    = 0x85,
        OCC_STATE_IN_TRANSITION            = 0x87,
        OCC_STATE_LOADING                  = 0x88,
        OCC_STATE_UNKNOWN                  = 0x89,
    };

    enum occRole
    {
        OCC_ROLE_SLAVE          = 0x00,
        OCC_ROLE_MASTER         = 0x01,
        OCC_ROLE_BACKUP_MASTER  = 0x02,
        OCC_ROLE_FIR_MASTER     = 0x80
    };

    enum
    {
        OCC_RESET_COUNT_THRESHOLD   = 3,
        WOF_RESET_COUNT_THRESHOLD   = 3,
    };

    enum occResetReason
    {
        OCC_RESET_REASON_NONE              = 0x00,
        OCC_RESET_REASON_CRIT_FAILURE      = 0x01,
        OCC_RESET_REASON_PWR_ON_FAIL       = 0x02,
        OCC_RESET_REASON_ERROR             = 0x03,
        OCC_RESET_REASON_POWER_FAULT       = 0x04,
        OCC_RESET_REASON_DIFF_OCC          = 0x05,
        OCC_RESET_REASON_OCC_REQUEST       = 0x06,
        OCC_RESET_REASON_EXTERNAL_REQUEST  = 0x0A,
        OCC_RESET_REASON_WOF_REQUEST       = 0x0C,
        OCC_RESET_REASON_CHECKPOINT_FAIL   = 0x0D,
    };

    // OCC Callout Structure
    struct occErrlCallout
    {
        uint8_t     type;
        uint64_t    calloutValue;
        uint8_t     priority;
        uint16_t    reserved1;
    } __attribute__ ((__packed__));
    typedef struct occErrlCallout occErrlCallout_t;

    // PGPE Callout Structure
    struct pgpeErrlCallout
    {
        uint64_t    calloutValue;
        uint8_t     type;
        uint8_t     priority;
        uint8_t     reserved[6];
    } __attribute__ ((__packed__));
    typedef struct pgpeErrlCallout pgpeErrlCallout_t;


    /**
     *  @class Occ
     *
     *  @brief OCC data class
     *
     *  @par Detailed Description:
     *      Provides configuration data for a specific OCC.
     *      Data will be saved during IPL so it can be used
     *      at runtime.
     */
    class Occ
    {
        friend class OccManager;
        friend class OccCmd;

        public:
            /**
             * @brief Constructor
             *
             * @param[in] i_instance       OCC instance number
             * @param[in] i_masterCapable  Is OCC capable of being master
             * @param[in] i_homer          Virtual address of HOMER
             * @param[in] i_target         OCC target pointer
             * @param[in] i_role           The role of this OCC
             */
            Occ(const uint8_t   i_instance,
                const bool      i_masterCapable,
                uint8_t       * i_homer,
                TARGETING::TargetHandle_t i_target,
                const occRole   i_role);


            /**
             * @brief Destructor
             */
            ~Occ();


            /**
             * @brief Return the instance nubmer
             *
             * @return instance number for this OCC
             */
            uint8_t getInstance() { return iv_instance; };


            /**
             * @brief Return pointer to the last poll response
             *
             * @param[out] o_pollRspPtr  pointer to last poll rsp data
             *
             * @return true if data is valid, else false
             */
            bool getLastPollRsp(const uint8_t * & o_pollRspPtr)
            {
                o_pollRspPtr = iv_lastPollResponse;
                return iv_lastPollValid;
            };


            /**
             * @brief Return pointer OCC target
             *
             * @return pointer to OCC target (not processor target)
             */
            TARGETING::TargetHandle_t getTarget() { return iv_target; };


            /**
             * @brief Poll for Errors
             *
             * @param[in]  i_flushAllErrors:
             *      If set to true, HTMGT will send poll cmds
             *      to the OCC as long as the OCC continues
             *      to report errors.  If false, only one
             *      poll will be sent.
             *
             * @return NULL on success, else error handle
             */
            errlHndl_t pollForErrors(const bool i_flushAllErrors = false);


            /**
             * @brief Return true if the specified status bit was set in
             *        the last poll response
             *
             * @param[in]  i_statusBit  Bit(s) to check in the poll response
             *
             * @return true if the bit(s) are set, else false
             */
            bool statusBitSet(const uint8_t i_statusBit);


            /**
             * @brief Set the state of the OCC.
             *        This is only allowed on the master OCC
             *
             * @param[in]  i_state  Desired OCC state
             *
             * @return NULL on success, or error handle on failure
             */
            errlHndl_t setState(const occStateId i_state);


            /**
             * @brief Return OCC role
             *
             * @return role of this OCC
             */
            occRole getRole() { return iv_role; };


            /**
             * @brief Return OCC state
             *
             * @return state of this OCC
             */
            occStateId getState() { return iv_state; };


            /**
             * @brief Prepare this OCC for reset
             * @return return true if at threshold otherwise false
             */
            bool resetPrep();


            /**
             * @brief Set IPMI OCC sensor state
             * @param i_activate: true - set active
             *                    false - set inactive
             *
             * @return error log on error
             */
            errlHndl_t ipmiSensor(bool i_activate)
            {
#if !defined(CONFIG_BMC_IPMI)
                return NULL;
#else
                return SENSOR::OCCActiveSensor(iv_target).setState
                    (i_activate ? SENSOR::OCCActiveSensor::OCC_ACTIVE :
                                SENSOR::OCCActiveSensor::OCC_NOT_ACTIVE);
#endif
            }


            /**
             * @brief Set failed state
             * @param[in] failed state
             */
            void failed(bool i_state) { iv_failed = i_state; }


            /**
             * @brief Determine if OCC needs to be reset
             *
             * @return true if this OCC needs to be reset
             */
            bool needsReset() { return iv_needsReset; }


            /**
             * @brief Determine if OCC needs to be reset due to WOF
             *
             * @return true if this OCC needs to be reset
             */
            bool needsWofReset() { return iv_needsWofReset; }


            /**
             * @brief Returns the number of times a WOF reset has occured
             *
             * @return Number of WOF resets for this OCC
             */
            uint8_t wofResetCount() { return iv_wofResetCount; }


            /**
             * @brief Returns a bit vector representing aggregate WOF
             *        resets reasons
             *
             * @return WOF Reset reason bit vector
             */
            uint32_t getWofResetReasons() { return iv_wofResetReasons; }

            /**
             * @brief Return OCCs present bits
             *
             * @return bitmask representing this OCC position
             */
            uint8_t getPresentBits() { return iv_occsPresent; };


            /**
             * @brief Update OCCs present bits in the master OCC
             *
             * @note Should only be called for Maseter OCC.  This is
             *       used to ensure the master can see all Slave OCCs
             *       and that no two slaves have same chip id.
             *
             * @param[in]  i_slavePresent  Bitmask for slave OCC to add
             */
            void updateOccPresentBits(uint8_t i_slavePresent);


            /**
             * @brief Add channel 1 (circular buffer) SCOM data to elog
             *
             * @param[in,out]  i_err  Error log to add data to
             */
            void collectCheckpointScomData(errlHndl_t i_err);


            /**
             * @brief Add OCC trace buffers to given error log (ERR, IMP, INF)
             *
             * @param[in,out] - the error log handle to add user data for
             */
            void addOccTrace( errlHndl_t & io_errl );


        private:   // functions

            /**
             * @brief Process an OCC poll response
             *
             * @param[in] i_pollResponse      pointer to the response
             * @param[in] i_pollResponseSize  length of the poll response
             */
            void pollRspHandler(const uint8_t * i_pollResponse,
                                const uint16_t i_pollResponseSize);


            /**
             * @brief Process elog entry from OCC poll response.
             *        Collect, Commit and Clear error log from the OCC.
             *
             * @param[in] i_id       OCC elog id to retrieve
             * @param[in] i_address  SRAM address for elog entry
             * @param[in] i_length   size of the elog entry
             * @param[in] i_source   OCC Error Log Source (405, PGPE, etc)
             */
            void occProcessElog(const uint8_t  i_id,
                                const uint32_t i_address,
                                const uint16_t i_length,
                                const uint8_t  i_source);


            /**
             * @brief Determine what actions are required for elog
             *
             * @param[in]     i_actions        Action flags requested by OCC
             * @param[in]     i_src            SRC being reported by OCC
             * @param[in]     i_data           Additional data used when
             *                                 processing actions
             * @param[in,out] io_errlSeverity  Severity to use for elog
             * @param[out]    o_call_home      True if info error should be
             *                                 reported to BMC
             */
            void elogProcessActions(const uint8_t  i_actions,
                                    const uint32_t i_src,
                                    const uint32_t i_data,
                                    ERRORLOG::errlSeverity_t & io_errlSeverity,
                                    bool         & o_call_home);


            /**
             * @brief Add specified callout to the error log
             *
             * @param[in,out] io_errlHndl     elog to add callout
             * @param[in]     i_priority      priority for callout
             * @param[in]     i_callout       callout from OCC
             * @param[in,out] io_numCallouts  number of callouts in elog,
             *                                incremented if new callout added
             * */
            bool elogAddCallout(errlHndl_t &               io_errlHndl,
                                HWAS::callOutPriority    & i_priority,
                                const occErrlCallout_t     i_callout,
                                uint8_t &                  io_callout_num);


            /**
             * @brief Add specified PGEP callout to the error log
             *
             * @param[in,out] io_errlHndl     elog to add callout
             * @param[in]     i_priority      priority for callout
             * @param[in]     i_callout       callout from PGPE
             * @param[in,out] io_numCallouts  number of callouts in elog,
             *                                incremented if new callout added
             * */
            bool elogAddPgpeCallout(errlHndl_t &               io_errlHndl,
                                    HWAS::callOutPriority    & i_priority,
                                    const pgpeErrlCallout_t    i_callout,
                                    uint8_t &                  io_callout_num);


            /**
             * @brief Update the GPU presence sensors in the system
             */
            void updateGpuPresence();


        protected:
            // Instance number of this OCC: 0 = first physical OCC
            uint8_t         iv_instance;
            // true if this OCC is capable of Master role (wired to APSS)
            bool            iv_masterCapable;
            // Role of this OCC
            occRole         iv_role;
            // State of this OCC
            occStateId      iv_state;
            // true if communication to this OCC has been established
            bool            iv_commEstablished;
            // true if OCC needs to be reset
            bool            iv_needsReset;
            // true if OCC needs to be reset due to WOF
            bool            iv_needsWofReset;
            // WOF reset count
            uint8_t         iv_wofResetCount;
            // WOF reset reason. Aggregate across all WOF resets until cleared.
            uint32_t        iv_wofResetReasons;
            // true if OCC failed
            bool            iv_failed;
            // Sequence number of last/current OCC command
            uint8_t         iv_seqNumber;
            // HOMER base address
            uint8_t *       iv_homer;
            // OCC target
            TARGETING::TargetHandle_t iv_target;
            // Last poll response (excluding sensor data)
            uint8_t         iv_lastPollResponse[OCC_POLL_DATA_MIN_SIZE];
            // true if lastPollResponse contains valid data
            bool            iv_lastPollValid;
            // expected occsPresent byte in POLL response
            uint8_t         iv_occsPresent;
            // GPU configuration from poll response data
            uint8_t         iv_gpuCfg;

            occResetReason  iv_resetReason;

            // Value of last exception committed (to prevent duplicates)
            uint8_t         iv_exceptionLogged;

            /**
             * @brief Clear flags after OCC has been reset
             */
            void postResetClear();


        private:
            // Reset count
            uint8_t         iv_resetCount;
            // Version of data stored (0 = not written)
            uint8_t         iv_version;
    };



    /**
     *  @class OccManager
     *
     *  @brief System / Node class
     *
     *  @par Detailed Description:
     *      Manages all Occ classes and contains data specific to
     *      this system / node.
     */
    class OccManager
    {
        friend class Occ;

        public:
            /**
             * @brief Constructor
             */
            OccManager();


            /**
             * @brief Destructor
             */
            ~OccManager();


            /**
             * @brief Query the functional OCCs and build OCC objects
             *
             * @param[in] i_occStart  true if call is being made after
             *                        OCCs have just been started.
             * @param[in] i_skipComm  true if sendOccPoll should not
             *                        be called
             * @return NULL on success, or error handle on failure
             */
            static errlHndl_t buildOccs(bool i_occStart = false,
                                        bool i_skipComm = false);


            /**
             * @brief Get number of functional OCCs
             *
             * @return number of OCCs
             */
            static uint8_t getNumOccs();


            /**
             * @brief Return array of current OCCs
             *
             * @return vector of OCC objects
             */
            static std::vector<Occ*> getOccArray();


            /**
             * @brief Return pointer to master OCC
             *
             * @return pointer to master OCC
             */
            static Occ * getMasterOcc();


            /**
             * @brief Return pointer to specified OCC
             *
             * @return pointer to specified OCC or NULL if not found
             */
            static Occ * getOcc(const uint8_t i_instance);


            /**
             * @brief Reset the OCCs
             *
             * @param[in] Failing occ target
             * @param[in] i_skipCountIncrement  true will prevent incrementing
             *            the system reset count
             * @param[in] i_skipComm            true will prevent attempts to
             *            communicate with OCC before reset (poll/resetPrep)
             *            Used when exiting safe mode.
             * @param[in] i_reason              reason for the reset
             *            Reason will not be updated for OCC_RESET_REASON_NONE
             * @return Error Log | NULL
             */
            static errlHndl_t resetOccs(TARGETING::Target * i_failedOccTarget,
                                        bool i_skipCountIncrement = false,
                                        bool i_skipComm = false,
                                        enum occResetReason i_reason =
                                        OCC_RESET_REASON_NONE);


            /**
             * @brief Set the state of the OCCs.  If i_state is
             *        not specified (NO_CHANGE), OCCs will be set
             *        to the target state (last requested state
             *        or if not requested the default is ACTIVE)
             *
             * @param[in]  i_state  Desired OCC state
             *
             * @return NULL on success, or error handle on failure
             */
            static errlHndl_t setOccState(const occStateId i_state =
                                          OCC_STATE_NO_CHANGE);


            /**
             * @brief Get the OCC state that should be used after the OCCs
             *        have been loaded or reset
             *
             * @note Target state will default to ACTIVE, but
             *       can be changed with HTMGT::enableOccActuation()
             *
             * @return target state
             */
            static occStateId getTargetState();



            /**
             * @brief Wait for all of the OCCs to reach their checkpoint
             *        state.  That indicates that the OCCs are ready to
             *        communicate and start handling commands.  This
             *        function will wait up to 10 seconds for all OCCs
             *        before returning to the caller.
             *
             * @return NULL on success, or error handle for the first OCC
             *         that did not reach its checkpoint.  If additional OCCs
             *         do not reach their checkpoint an elog will be committed
             *         for each.
             */
            static errlHndl_t waitForOccCheckpoint();


            /**
             * @brief Send a poll command to one or all OCCs
             *
             * @param[in]  i_flushAllErrors:
             *                 If set to true, HTMGT will send poll cmds
             *                 to each OCC that is selected as long as that OCC
             *                 continues to report errors.  If false, only one
             *                 poll will be send to each OCC.
             * @param[in] i_occTarget: The Selected OCC or NULL for all OCCs
             *
             * @return NULL on success, else error handle
             */
            static errlHndl_t
                sendOccPoll(const bool i_flushAllErrors = false,
                            TARGETING::Target * i_occTarget = NULL);


            /**
             * @brief Save the reason that the system is entering safe mode
             *
             * @param[in]  i_src       SRC which triggered safe mode
             * @param[in]  i_instance  OCC which triggered safe mode
             */
            static void updateSafeModeReason(uint32_t i_src,
                                             uint32_t i_instance);


            /**
             * @brief Return the reason the system entered safe mode
             *
             * @param[out]  o_instance   OCC instance
             *
             * @return     SRC which triggered safe mode
             */
            static uint32_t getSafeModeReason(uint32_t & o_instance);


            /**
             * @brief Check if any OCCs need to be reset
             *
             * @return true if any OCC needs to be reset
             */
            static bool occNeedsReset();

            /**
             * @brief Collect FFDC debug data for HTMGT and OCCs
             *
             * @param[out] o_length Length of data returned in o_data
             * @param[out] o_data   Buffer of 256 bytes where data will
             *                      be copied
             */
            static void getHtmgtData(uint16_t & o_length, uint8_t *o_data);

            /**
             * @brief Collect WOF reset reasons for ALL OCCs
             *
             * @param[out] o_length Length of data returned in o_data
             * @param[out] o_data   Buffer containing all OCCs 32-bit
             *                      WOF reset reason vector preceded by
             *                      their instanceId
             */
            static void getWOFResetReasons(uint16_t & o_length,
                                           uint8_t * o_data);

            /**
             * @brief Update error log with safe mode callouts and set
             *        attribute indicating system is in safe mode.
             *
             * @param[in,out] io_err  Error log to be updated
             * */
            void updateForSafeMode(errlHndl_t & io_err);


            /**
             * @brief Check if any OCC has failed
             *
             * @return true if any OCC has been marked as failed
             */
            static bool occFailed();


            /**
             * @brief Update OCC manager state with consolidated OCC state
             *
             */
            static void syncOccStates();


            /**
             * @brief Clear all OCC reset counts
             *
             */
            static void clearResetCounts();


        private:

            typedef std::vector<Occ*> occList_t;

            Occ *                   iv_occMaster;
            occList_t               iv_occArray;
            occStateId              iv_state;
            occStateId              iv_targetState;
            uint8_t                 iv_sysResetCount;


            /**
             * @brief SRC that caused system to enter safe mode
             */
            static uint32_t         cv_safeReturnCode;

            /**
             * @brief OCC instance that triggered safe mode
             */
            static uint32_t         cv_safeOccInstance;


            /* See buildOccs() above */
            errlHndl_t _buildOccs(bool i_occStart = false,
                                  bool i_skipComm = false);


            /* See getNumOccs() above */
            uint8_t _getNumOccs() { return iv_occArray.size(); };


            /* See getOccArray() above */
            std::vector<Occ*> _getOccArray() { return iv_occArray; };


            /* See getTargetState() above */
            occStateId _getTargetState() { return iv_targetState; };


            /**
             * @brief Remove all OCC objects
             */
            void _removeAllOccs();


            /**
             * @brief Add a functional OCC to be monitored
             *
             * @param[in] i_instance       OCC instance number
             * @param[in] i_masterCapable  Is OCC capable of being master
             * @param[in] i_homer          Virtual address of HOMER
             * @param[in] i_target         OCC target pointer
             * @param[in] i_role           OCC role
             */
            void  _addOcc(const uint8_t   i_instance,
                          const bool      i_masterCapable,
                          uint8_t       * i_homer,
                          TARGETING::TargetHandle_t i_target);


            /* See getMasterOcc() above */
            Occ * _getMasterOcc() { return iv_occMaster; };


            /* See getOcc() above */
            Occ * _getOcc(const uint8_t i_instance);


            /* See setOccState() above */
            errlHndl_t _setOccState(const occStateId i_state);

            /* See waitForOccCheckpoint() above */
            errlHndl_t _waitForOccCheckpoint();

            /* See resetOccs() above */
            errlHndl_t _resetOccs(TARGETING::Target * i_failedOccTarget,
                                  bool i_skipCountIncrement,
                                  bool i_skipComm,
                                  enum occResetReason i_reason);

            /** See sendOccPoll() above */
            /* @param[in] i_onlyIfEstablished: If true, only send poll if
             *                 communications has already been established
             *                 with that OCC */
            errlHndl_t
                _sendOccPoll(const bool i_flushAllErrors,
                             TARGETING::Target * i_occTarget,
                             const bool i_onlyIfEstablished = false);

            /** See updateSafeModeReason() above */
            void _updateSafeModeReason(uint32_t i_src,
                                       uint32_t i_instance);

            /** See getSafeModeReason() above */
            uint32_t _getSafeModeReason(uint32_t & o_instance);

            /** See occNeedsReset() above */
            bool _occNeedsReset();

            /** See occFailed() above */
            bool _occFailed();

            /** See getHtmgtData() above */
            void _getHtmgtData(uint16_t & o_length, uint8_t *o_data);

            /** See getWOFResetReasons above */
            void _getWOFResetReasons(uint16_t & o_length, uint8_t *o_data);

            /** See syncOccStates() above */
            void _syncOccStates();

            /** See clearResetCounts() above */
            void _clearResetCounts();
    };

    typedef Singleton<OccManager> occMgr;

} // end namespace
#endif
