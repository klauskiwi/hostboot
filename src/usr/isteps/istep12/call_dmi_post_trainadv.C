/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/isteps/istep12/call_dmi_post_trainadv.C $             */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2019                        */
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
#include    <stdint.h>

#include    <trace/interface.H>
#include    <initservice/taskargs.H>
#include    <initservice/isteps_trace.H>
#include    <errl/errlentry.H>
#include    <errl/errludtarget.H>
#include    <isteps/hwpisteperror.H>
#include    <util/utilmbox_scratch.H>
#include    <util/misc.H>


//  targeting support.
#include    <targeting/common/commontargeting.H>
#include    <targeting/common/utilFilter.H>

//Fapi Support
#include    <config.h>
#include    <fapi2.H>
#include    <fapi2/plat_hwp_invoker.H>

//HWP
#include    <p9_io_dmi_post_trainadv.H>

#ifdef CONFIG_AXONE
#include    <p9a_omi_train_check.H>
#include    <exp_omi_train_check.H>
#include    <chipids.H> // for EXPLORER ID
#endif

using   namespace   ISTEP;
using   namespace   ISTEP_ERROR;
using   namespace   ERRORLOG;
using   namespace   TARGETING;


namespace ISTEP_12
{
void* call_dmi_post_trainadv (void *io_pArgs)
{
    IStepError l_StepError;
    errlHndl_t l_err = NULL;

    TRACDCOMP( ISTEPS_TRACE::g_trac_isteps_trace, "call_dmi_post_trainadv entry" );

    TARGETING::TargetHandleList l_dmiTargetList;
    getAllChiplets(l_dmiTargetList, TYPE_DMI);

    TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace, "call_dmi_post_trainadv: %d DMIs found",
            l_dmiTargetList.size());

    for (const auto & l_dmi_target : l_dmiTargetList)
    {
        TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
            "p9_io_dmi_post_trainadv HWP target HUID %.8x",
            TARGETING::get_huid(l_dmi_target));

        //get the membuf associated with this DMI.
        TARGETING::TargetHandleList l_pChildMembufList;
        getChildAffinityTargetsByState(l_pChildMembufList,
                                       l_dmi_target,
                                       CLASS_CHIP,
                                       TYPE_MEMBUF,
                                       UTIL_FILTER_PRESENT);
        // call the HWP p9_io_dmi_post_trainadv only if membuf connected.
        //we can't expect more than one membufs connected to a DMI
        if (l_pChildMembufList.size() == 1)
        {
            //  call the HWP with each DMI target
            fapi2::Target<fapi2::TARGET_TYPE_DMI> l_fapi_dmi_target
                (l_dmi_target);

            fapi2::Target<fapi2::TARGET_TYPE_MEMBUF_CHIP> l_fapi_membuf_target
                (l_pChildMembufList[0]);

            FAPI_INVOKE_HWP(l_err, p9_io_dmi_post_trainadv, l_fapi_dmi_target, l_fapi_membuf_target );

            //  process return code.
            if ( l_err )
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                    "ERROR 0x%.8X:  p9_io_dmi_post_trainadv HWP on target HUID %.8x",
                    l_err->reasonCode(), TARGETING::get_huid(l_dmi_target) );

                // capture the target data in the elog
                ErrlUserDetailsTarget(l_dmi_target).addToLog( l_err );

                // Create IStep error log and cross reference to error that occurred
                l_StepError.addErrorDetails( l_err );

                // Commit Error
                errlCommit( l_err, ISTEP_COMP_ID );
            }
            else
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                         "SUCCESS :  p9_io_dmi_post_trainadv HWP");
            }
        }
        else    //No associated membuf
        {
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                      "p9_io_dmi_post_trainadv HWP skipped, no associated membufs %d"
                      ,l_pChildMembufList.size());
        }

    }

#ifdef CONFIG_AXONE
    if( ! Util::isSimicsRunning() )
    {
        // Find ocmb targets
        TARGETING::TargetHandleList l_chipList;
        TARGETING::getAllChips(l_chipList, TARGETING::TYPE_OCMB_CHIP, true);

        for (auto & l_ocmb: l_chipList)
        {
            // Only run exp_omi_train on EXPLORER OCMB targets. This step
            // cannot run on GEMINI targets.
            uint32_t chipId = l_ocmb->getAttr< TARGETING::ATTR_CHIP_ID>();
            if (chipId == POWER_CHIPID::EXPLORER_16)
            {
                fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> l_ocmb_target( l_ocmb );
                FAPI_INVOKE_HWP(l_err, exp_omi_train_check, l_ocmb_target );

                //  process return code.
                if ( l_err )
                {
                    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                        "ERROR 0x%.8X:  exp_omi_train_check HWP on target HUID %.8x",
                        l_err->reasonCode(), TARGETING::get_huid(l_ocmb) );

                    // capture the target data in the elog
                    ErrlUserDetailsTarget(l_ocmb).addToLog( l_err );

                    // Create IStep error log and cross reference to error that occurred
                    l_StepError.addErrorDetails( l_err );

                    // Commit Error
                    errlCommit( l_err, ISTEP_COMP_ID );
                }
                else
                {
                    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                              "SUCCESS :  exp_omi_train_check HWP on target HUID %.08x",
                              TARGETING::get_huid(l_ocmb));
                }
            }
            else
            {
                // Gemini, just skip exp_omi_train_check call
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                    "Skipping exp_omi_train_check HWP on because target HUID 0x%.8X, chipId 0x%.4X is a Gemini OCMB",
                    TARGETING::get_huid(l_ocmb), chipId );
            }
        }

        // Find omi targets
        TARGETING::TargetHandleList l_omiTargetList;
        getAllChiplets(l_omiTargetList, TYPE_OMI);

        for (const auto & l_omi_target : l_omiTargetList)
        {
            //  call the HWP with each OMI target
            fapi2::Target<fapi2::TARGET_TYPE_OMI> l_fapi_omi_target(l_omi_target);

            FAPI_INVOKE_HWP(l_err, p9a_omi_train_check, l_fapi_omi_target );

            //  process return code.
            if ( l_err )
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                    "ERROR 0x%.8X:  p9a_omi_train_check HWP on target HUID %.8x",
                    l_err->reasonCode(), TARGETING::get_huid(l_omi_target) );

                // capture the target data in the elog
                ErrlUserDetailsTarget(l_omi_target).addToLog( l_err );

                // Create IStep error log and cross reference to error that occurred
                l_StepError.addErrorDetails( l_err );

                // Commit Error
                errlCommit( l_err, ISTEP_COMP_ID );
            }
            else
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                          "SUCCESS :  p9a_omi_train_check HWP on target HUID %.08x",
                          TARGETING::get_huid(l_omi_target));
            }
        }
    }
#endif
    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace, "call_dmi_post_trainadv exit" );

    // end task, returning any errorlogs to IStepDisp
    return l_StepError.getErrorHandle();
}

};
