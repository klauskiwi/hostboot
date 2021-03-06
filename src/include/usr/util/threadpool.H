/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/include/usr/util/threadpool.H $                           */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2019                        */
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
#ifndef __UTIL_THREADPOOL_H
#define __UTIL_THREADPOOL_H

/** @file threadpool.H
 *  @brief Defines the interfaces for a thread-pool.
 *
 *  Provides two classes, one of which is templatized: ThreadPool and
 *  ThreadPoolManager.
 *
 *  ThreadPool is an implementation of a thread-pool.  It accepts a template
 *  parameter of the class describing the work that can be done by this
 *  thread-pool.  The only requirement is that this class provides an
 *  operator().
 *
 *  The default behavior of the ThreadPool is to be a FIFO.  If the desire is
 *  for the ThreadPool to operate in a non-FIFO manner then the work item
 *  class must support less-than comparison.  The pool will then execute the
 *  oldest work item (a) such that all other work items (b) have:
 *      (false == (b < a))
 *
 *  Work Item Prototypes:
 *      void operator()() { ... execute work ... }
 *      bool operator<(const WorkItem& rhs);
 *
 *  ThreadPoolManager can be used to globally control the number of threads
 *  started when a new thread pool is started.  The intent of this class is
 *  that the maximum number of threads will be adjusted by an the IPL step
 *  management based on the phase of the IPL we are in.  When we are cache
 *  contained the number should be kept lower and when we exit cache contained
 *  we can allow much more parallelism.
 *
 */

#include <stdint.h>
#include <util/traits/has_lessthan.H>
#include <util/impl/threadpool.H>
#include <errl/errlentry.H>

namespace Util
{

/**
 * @brief Definition of the thread pool.
 *
 * @param[in] WorkItem - The class to accept as the work.
 *
 * The thread-pool will execute the operator() on WorkItems.  Work will be done
 * in a FIFO manner unless the < comparison works on the WorkItems, in which
 * case the oldest WorkItem for which all other work items fail (b<a) will be
 * processed.
 *
 * The thread-pool will only create worker threads once the 'start' operation
 * has been called.  When the 'shutdown' operation is called, the thread-pool
 * will complete its outstanding work and destroy all children worker threads.
 *
 * @note The thread-safety of this object ensures that it is self-consistent
 *       between insert and worker-thread operations but does not protect
 *       against multiple threads calling the start / shutdown operations.
 *       Multiple threads may insert safely but only one thread should be
 *       responsible for the state management.
 */
template <typename WorkItem>
class ThreadPool : public Util::__Util_ThreadPool_Impl::ThreadPoolImpl
{
    public:
            /** Basic Constructor.  Initialize ThreadPool. */
        ThreadPool(bool i_checkChildRc = true) :
                Util::__Util_ThreadPool_Impl::ThreadPoolImpl(i_checkChildRc)
        {
        };
            /** Basic Destructor.  Ensures pool is properly shut down. */
        ~ThreadPool() { shutdown(); };

            /** @brief Creates worker threads and begins processing work.
             */
        void start()
            {
                __start(reinterpret_cast<start_fn_t>(&run), this);
            };
            /** @brief Completes outstanding work and destroys worker threads.
             *         Returns an error log when any child task crashes when
             *         iv_checkChildRc is set.
             *
             *  @note This function will block until all work is completed and
             *        worker threads are destroyed.
             *
             * @return nullptr if all child tasks finished (child status
             *         checking disabled);
             *         nullptr if all child tasks finished successfully, or a
             *         pointer to error log otherwise (child status checking
             *         enabled).
             */
        errlHndl_t shutdown()
            { return __shutdown(); };

            /** @brief Insert a work item onto the thread-pool's queue.
             *
             *  Ownership of the object is transferred to the thread-pool.
             *  After completing the work, the thread-pool will delete the
             *  work item.
             *
             *  @param[in] i_workItem - A work item to process.
             */
        void insert(WorkItem* i_workitem)
            { __insert(i_workitem); };

    private:
            /** Entry point for worker thread. */
        static void* run(ThreadPool*);
            /** Useful constant to determine FIFO vs non-FIFO behavior. */
        static const bool has_comparison =
                Util::Traits::has_lessthan<WorkItem>::value;
};

/**
 * @brief Manager of the thread pools.
 *
 * When new thread-pools are created, they query the manager to determine
 * how many worker threads to create.  This should be manipulated by some
 * higher level service that understands the resources available to
 * thread-pools and sets the value to balance memory usage and efficiency.
 */
class ThreadPoolManager
{
    public:
        /** Query the desired worker-thread count. */
        static size_t getThreadCount()
            { return cv_size; };
        /** Set the desired worker-thread count. */
        static void setThreadCount(size_t i_size)
            { cv_size = i_size; };
    private:
        static size_t cv_size;
};

// Implementation of the worker-thread run function.
template <typename WorkItem>
void* ThreadPool<WorkItem>::run(
        ThreadPool<WorkItem>* self)
{
    while(1)
    {
        // Obtain next work item from queue.
        WorkItem* wi =
            static_cast<WorkItem*>(
                self->__remove(
                    reinterpret_cast<order_fn_t>(
                        &Util::__Util_ThreadPool_Impl::ThreadPoolWorklistSearch
                            <WorkItem, has_comparison>::search
                    )
                )
            );

        if (wi) // Work was given, do it.
        {
            (*wi)();
            delete wi;
        }
        else // No work item was given, we must be done.
        {
            return NULL; // task_end() called automatically by returning.
        }
    }
    return NULL;
}

};

#endif
