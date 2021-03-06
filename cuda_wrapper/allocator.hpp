/* Allocator that wraps cudaMalloc -*- C++ -*-
 *
 * Copyright (C) 2008  Peter Colberg
 *
 * This file is derived from ext/malloc_allocator.h of the
 * GNU Standard C++ Library, which wraps "C" malloc.
 *
 * Copyright (C) 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.
 *
 * This file is part of HALMD.
 *
 * HALMD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CUDA_ALLOCATOR_HPP
#define CUDA_ALLOCATOR_HPP

#include <bits/functexcept.h>
#include <cstdlib>
#include <cuda_runtime.h>
#include <new>

#include <cuda_wrapper/error.hpp>

namespace cuda {

using std::size_t;
using std::ptrdiff_t;


template<typename _Tp>
class allocator
{
public:
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;
    typedef _Tp*       pointer;
    typedef const _Tp* const_pointer;
    typedef _Tp&       reference;
    typedef const _Tp& const_reference;
    typedef _Tp        value_type;

    template<typename _Tp1>
    struct rebind
    {
        typedef allocator<_Tp1> other;
    };

    allocator() throw() { }

    allocator(const allocator&) throw() { }

    template<typename _Tp1>
    allocator(const allocator<_Tp1>&) throw() { }

    ~allocator() throw() { }

    pointer address(reference __x) const
    {
        return &__x;
    }

    const_pointer address(const_reference __x) const
    {
        return &__x;
    }

    // NB: __n is permitted to be 0.  The C++ standard says nothing
    // about what the return value is when __n == 0.
    pointer allocate(size_type __n, const void* = 0)
    {
        void* __ret;

        if (__builtin_expect(__n > this->max_size(), false))
            std::__throw_bad_alloc();

        CUDA_CALL(cudaMalloc(&__ret, __n * sizeof(_Tp)));

        return reinterpret_cast<pointer>(__ret);
    }

    // __p is not permitted to be a null pointer.
    void deallocate(pointer __p, size_type) throw() // no-throw guarantee
    {
        cudaFree(reinterpret_cast<void *>(__p));
    }

    size_type max_size() const throw()
    {
        return size_t(-1) / sizeof(_Tp);
    }

    void construct(pointer __p, const _Tp& __val)
    {
        ::new(__p) value_type(__val);
    }

    void destroy(pointer __p)
    {
        __p->~_Tp();
    }
};

template<typename _Tp>
inline bool operator==(const allocator<_Tp>&, const allocator<_Tp>&)
{
    return true;
}

template<typename _Tp>
inline bool operator!=(const allocator<_Tp>&, const allocator<_Tp>&)
{
    return false;
}

} // namespace cuda

#endif
