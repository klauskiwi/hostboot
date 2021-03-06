/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/sbeio/sbe_memRegionMgr.H $                            */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2018                        */
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
#ifndef __SBE_MEMREGIONMGR_H
#define __SBE_MEMREGIONMGR_H

#include <stdint.h>
#include <errl/errlentry.H>
#include <list>
#include <sbeio/sbeioif.H>
#include <targeting/common/target.H>
#include <map>

namespace SBEIO
{
/*
 * @brief Define Specifying The Maximum Number Of Unsecure Memory Regions That
 *        the SBE Supports
 */
#define SBEIO_MAX_UNSECURE_MEMORY_REGIONS 8

/*
 * @brief Struct containing specifics of a Unsecure Memory Region
 */
struct regionData
{
    uint64_t start_addr; // Start address of region
    uint32_t size;       // Size of region in bytes

    // Control Flags for PSU command -
    // - see SbePsu::psuSetUnsecureMemoryRegionControlFlags
    uint8_t  flags;

    // Target associated with the SBE
    // - If tgt == nullptr it will eventually be updated to the Master Proc
    TARGETING::TargetHandle_t tgt;

    regionData() : start_addr(0), size(0), flags(0), tgt(nullptr) {}
};

typedef std::list<regionData> RegionDataList;
typedef std::pair<TARGETING::TargetHandle_t, RegionDataList>  MemRegionMapPair;
typedef std::map<TARGETING::TargetHandle_t, RegionDataList> MemRegionMap;

/** @class MemRegionMgr
 *  @brief   Responsible for managing the SBE Unsecure Memory Regions
 *
 */
class MemRegionMgr
{

  private:

    /** Cache of Unsecure Memory Regions that are currently open
     *  Key = Target Pointer, Value = List of Memory Regions
     */
    MemRegionMap iv_memRegionMap;

    /* For Debug purposes: Print out memory region map */
    void printIvMemRegions() const;

    /* Cache master proc */
    TARGETING::TargetHandle_t iv_masterProc;

    /**
     * @brief Local Function To Open/Close Unsecure Memory Regions
     *
     * @param[in] i_region  Structure containing information about the region
     *
     * @return errlHndl_t Error log handle on failure.
     *
     */
    errlHndl_t doUnsecureMemRegionOp(regionData & i_region);

    /**
     * @brief Checks That The Maximum Number of Unsecure Memory Regions That The
     *        SBE Supports Is Not Exceeded If A New Region Is Opened
     *
     * @param[in] i_count Current count of opened unsecure memory regions
     *
     * @return errlHndl_t Error log handle if current size of iv_memRegions is
     *                    equal to or exceeds the maximum number of regions that
     *                    the SBE Supports; otherwise, returns nullptr
     */
    errlHndl_t checkNumberOfMemRegions(uint8_t i_count) const;

    /**
     * @brief Append region data to region list of specified target
     *
     * @param[in] i_target Proc target to apply region data to
     *            Note: nullptr assumed to be acting master proc
     * @param[in] i_region Region data
     *
     * @return N/A
     */
    void addToTargetRegionList(TARGETING::TargetHandle_t i_target,
                               const regionData& i_region);

    /**
     * @brief Get region data list of specified target
     *
     * @param[in] i_target Proc target to obtain region data list from
     *            Note: nullptr assumed to be acting master proc
     *
     * @return N/A
     */
    RegionDataList* getTargetRegionList(TARGETING::TargetHandle_t i_target);

    /**
     * @brief Get number of all memory regions amongst all targets
     *
     * @return N/A
     */
    uint8_t getTotalNumOfRegions() const;

  public:
    /**
     * @brief Constructor.  Initializes instance variables.
     */
    MemRegionMgr();

    /**
     * Destructor.
     */
    ~MemRegionMgr();

    /**
     * Delete Copy Constructor
     */
    MemRegionMgr(const MemRegionMgr&) = delete;

    /**
     * Delete Copy Assignment
     */
    MemRegionMgr& operator= (const MemRegionMgr&) = delete;

    /**
     * Delete Move Constructor
     */
    MemRegionMgr (MemRegionMgr&&) = delete;

    /**
     * Delete Move Assignment
     */
    MemRegionMgr& operator = (MemRegionMgr&&) = delete;

    /**
     * @brief Open Unsecure Memory Region via the SBE
     *
     * @note: See openUnsecureMemRegion() function in sbeioif.H for details
     *
     */
    errlHndl_t openUnsecureMemRegion(
        const uint64_t i_start_addr,
        const uint32_t i_size,
        const bool     i_isWritable,
        TARGETING::Target* i_target);

    /**
     * @brief Close Unsecure Memory Region
     *
     * @note: See closeUnsecureMemRegion() function in sbeioif.H for details
     *
     */
    errlHndl_t closeUnsecureMemRegion(
        const uint64_t i_start_addr,
        TARGETING::Target* i_target);


    /**
     * @brief Close All Unsecure Memory Regions
     *
     * @note: See closeAllUnsecureMemRegions() function in sbeioif.H for details
     *
     */
    errlHndl_t closeAllUnsecureMemRegions();

}; // class MemRegionMgr


}; // end namespace SBEIO

#endif

