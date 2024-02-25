/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/

#pragma once

#include <alternate_shared_mutex.hpp>
#include <naive_spin_mutex.hpp>
#include <ttas_spin_mutex.hpp>
#include <yamc_rwlock_sched.hpp>

namespace wmoge {

    /**
     * @class SpinMutexTas
     * @brief TAS spinlock, non-recursive
     */
    class SpinMutexTas : public yamc::spin::mutex {};

    /**
     * @class SpinMutexTtas
     * @brief TTAS spinlock, non-recursive
     */
    class SpinMutexTtas : public yamc::spin_ttas::mutex {};

    /**
     * @class RwMutexReadPrefer
     * @brief Read-write lock with policy to prefer readers
     * 
     * Policy yamc::rwlock::ReaderPrefer: Reader prefer locking. While any reader thread owns shared lock, 
     * subsequent other reader threads can immediately acquire shared lock, but subsequent writer threads 
     * will be blocked until all reader threads release shared lock. This policy might 
     * introduce "Writer Starvation" if reader threads continuously hold shared lock.
     */
    class RwMutexReadPrefer : public yamc::alternate::basic_shared_mutex<yamc::rwlock::ReaderPrefer> {};

    /**
     * @class RwMutexWritePrefer
     * @brief Read-write lock with policy to prefer writers
     * 
     * Policy yamc::rwlock::WriterPrefer: Writer prefer locking. While any reader thread owns 
     * shared lock and there are a waiting writer thread, subsequent other reader threads 
     * which try to acquire shared lock are blocked until writer thread's work is done. 
     * This policy might introduce "Reader Starvation" if writer threads continuously 
     * try to acquire exclusive lock.
     */
    class RwMutexWritePrefer : public yamc::alternate::basic_shared_mutex<yamc::rwlock::WriterPrefer> {};

    /**
     * @brief Default engine spin mutex
     */
    using SpinMutex = SpinMutexTtas;

}// namespace wmoge