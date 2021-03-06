# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: src/usr/isteps/istep10/makefile $
#
# OpenPOWER HostBoot Project
#
# Contributors Listed Below - COPYRIGHT 2015,2020
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
ROOTPATH = ../../../..
MODULE = istep10

NEST_HWP_PATH += ${ROOTPATH}/src/import/chips/p9/procedures/hwp/nest
INITFILES_HWP_PATH += ${ROOTPATH}/src/import/chips/p9/procedures/hwp/initfiles
PERV_HWP_PATH += ${ROOTPATH}/src/import/chips/p9/procedures/hwp/perv
PROCEDURES_PATH += ${ROOTPATH}/src/import/chips/p9/procedures
XIP_CUSTOMIZE_PATH = ${PROCEDURES_PATH}/hwp/customize
HWP_ACC_PATH += ${PROCEDURES_PATH}/hwp/accessors
OCMB_HWP_PATH += ${ROOTPATH}/src/import/chips/ocmb/explorer/procedures/hwp/memory/

EXTRAINCDIR += ${HWP_ACC_PATH}
EXTRAINCDIR += ${XIP_CUSTOMIZE_PATH}
EXTRAINCDIR += ${PROCEDURES_PATH}/hwp/pm/
EXTRAINCDIR += ${PROCEDURES_PATH}/hwp/io/
EXTRAINCDIR += ${PROCEDURES_PATH}/hwp/lib/
EXTRAINCDIR += ${PROCEDURES_PATH}/hwp/pm/include/registers
EXTRAINCDIR += ${ROOTPATH}/src/usr/isteps/
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/fapi2/
EXTRAINCDIR += ${ROOTPATH}/src/import/hwpf/fapi2/include/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/utils/imageProcs/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/common/utils/imageProcs/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/common/include/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/xip
EXTRAINCDIR += ${NEST_HWP_PATH}
EXTRAINCDIR += ${INITFILES_HWP_PATH}
EXTRAINCDIR += ${PERV_HWP_PATH}
EXTRAINCDIR += ${OCMB_HWP_PATH}
EXTRAINCDIR += ${ROOTPATH}/src/import/
EXTRAINCDIR += ${ROOTPATH}/obj/genfiles/chips/ocmb/explorer/procedures/hwp/memory/lib/


OBJS += call_proc_build_smp.o
OBJS += call_host_slave_sbe_update.o
OBJS += call_proc_cen_ref_clk_enable.o
OBJS += call_proc_enable_osclite.o
OBJS += call_proc_chiplet_scominit.o
OBJS += call_proc_abus_scominit.o
OBJS += call_proc_obus_scominit.o
OBJS += call_proc_npu_scominit.o

#Sensor Based Bifurcation implementation
OBJS += $(if $(CONFIG_SENSOR_BASED_BIFURCATION),sensor_based_bifurcation.o)

OBJS += call_proc_pcie_scominit.o
OBJS += call_proc_scomoverride_chiplets.o
OBJS += call_proc_chiplet_enable_ridi.o
OBJS += call_host_rng_bist.o
OBJS += call_host_update_redundant_tpm.o
OBJS += host_proc_pcie_scominit.o

# HWP objects needed to build smp in istep 10
# instead of using HWP p9_build_smp.mk.
# This is done to avoid inter-isteps dependencies
# p9_build_smp.mk
OBJS+=p9_build_smp.o
OBJS+=p9_fbc_smp_utils.o
OBJS+=p9_build_smp_fbc_ab.o
OBJS+=p9_build_smp_fbc_cd.o
OBJS+=p9_build_smp_adu.o

VPATH += ${NEST_HWP_PATH}
VPATH += ${INITFILES_HWP_PATH}
VPATH += ${PERV_HWP_PATH}
VPATH += ${PROCEDURES_PATH}/hwp/io
VPATH += ${PROCEDURES_PATH}/hwp/pm
VPATH += ${PROCEDURES_PATH}/hwp/lib
VPATH +=  ${HWP_ACC_PATH}


#Required include before all the procedure.mk are included
include ${ROOTPATH}/procedure.rules.mk

#Include all the procedure makefiles
include ${INITFILES_HWP_PATH}/p9_fbc_ab_hp_scom.mk
include ${INITFILES_HWP_PATH}/p9_fbc_cd_hp1_scom.mk
include ${INITFILES_HWP_PATH}/p9_fbc_cd_hp2_scom.mk
include ${INITFILES_HWP_PATH}/p9_fbc_cd_hp3_scom.mk
include ${PERV_HWP_PATH}/p9_update_security_ctrl.mk
include ${PERV_HWP_PATH}/p9_sbe_lpc_init.mk

# Note that p9_int_scom.mk is included in fapi2.mk for
# workaround reasons so we are not including it here

# proc_pcie_scominit : Apply scom inits to PCIe chiplets
include ${PROCEDURES_PATH}/hwp/nest/p9_pcie_scominit.mk

# p9n_mcs_scom
include ${PROCEDURES_PATH}/hwp/initfiles/p9n_mcs_scom.mk

# proc_scomoverride_chiplets : Apply sequenced scom inits
include ${PROCEDURES_PATH}/hwp/nest/p9_scomoverride_chiplets.mk

# host_rng_bist : Trigger Built In Self Test
# HWP not ready - p9_trigger_rng_bist.mk
include ${PROCEDURES_PATH}/hwp/nest/p9_rng_init_phase1.mk

# p9_cen_ref_clk_enable : Centaur Ref Clock Enable
include $(PERV_HWP_PATH)/p9_cen_ref_clk_enable.mk

VPATH += $(if $(CONFIG_AXONE),${OCMB_HWP_PATH},)
OBJS += $(if $(CONFIG_AXONE),p9a_ocmb_enable.o,)
OBJS += $(if $(CONFIG_AXONE),exp_check_for_ready.o,)

# p9_avsbus_lib.mk sets MODULE, reset here to istep10
MODULE = istep10

include ${ROOTPATH}/config.mk
