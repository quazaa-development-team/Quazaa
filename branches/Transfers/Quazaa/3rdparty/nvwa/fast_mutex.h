// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
// vim:tabstop=4:shiftwidth=4:expandtab:

/*
 * Copyright (C) 2004-2010 Wu Yongwei <adah at users dot sourceforge dot net>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software.  If you use this
 *    software in a product, an acknowledgement in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must
 *    not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 * This file is part of Stones of Nvwa:
 *      http://sourceforge.net/projects/nvwa
 *
 */

/**
 * @file    fast_mutex.h
 *
 * A fast mutex implementation for POSIX and Win32.
 *
 * @version 1.20, 2010/05/16
 * @author  Wu Yongwei
 *
 */

#ifndef _FAST_MUTEX_H
#define _FAST_MUTEX_H

#include <QMutex>

# ifndef _FAST_MUTEX_CHECK_INITIALIZATION
/**
 * Macro to control whether to check for initialization status for each
 * lock/unlock operation.  Defining it to a non-zero value will enable
 * the check, so that the construction/destruction of a static object
 * using a static fast_mutex not yet constructed or already destroyed
 * will work (with lock/unlock operations ignored).  Defining it to zero
 * will disable to check.
 */
#   define _FAST_MUTEX_CHECK_INITIALIZATION 1
# endif

# ifdef _DEBUG
#   include <stdio.h>
#   include <stdlib.h>
/** Macro for fast_mutex assertions.  Real version (for debug mode). */
#   define _FAST_MUTEX_ASSERT(_Expr, _Msg) \
        if (!(_Expr)) { \
            fprintf(stderr, "fast_mutex::%s\n", _Msg); \
            abort(); \
        }
# else
/** Macro for fast_mutex assertions.  Fake version (for release mode). */
#   define _FAST_MUTEX_ASSERT(_Expr, _Msg) \
        ((void)0)
# endif

    class fast_mutex
    {
		QMutex _M_mtx_impl;
#       if _FAST_MUTEX_CHECK_INITIALIZATION
        bool _M_initialized;
#       endif
#       ifdef _DEBUG
        bool _M_locked;
#       endif
    public:
        fast_mutex()
#       ifdef _DEBUG
            : _M_locked(false)
#       endif
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            _M_initialized = true;
#       endif
        }
        ~fast_mutex()
        {
            _FAST_MUTEX_ASSERT(!_M_locked, "~fast_mutex(): still locked");
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            _M_initialized = false;
#       endif
        }
        void lock()
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            if (!_M_initialized)
                return;
#       endif
			_M_mtx_impl.lock();
#       ifdef _DEBUG
            _FAST_MUTEX_ASSERT(!_M_locked, "lock(): already locked");
            _M_locked = true;
#       endif
        }
        void unlock()
        {
#       if _FAST_MUTEX_CHECK_INITIALIZATION
            if (!_M_initialized)
                return;
#       endif
#       ifdef _DEBUG
            _FAST_MUTEX_ASSERT(_M_locked, "unlock(): not locked");
            _M_locked = false;
#       endif
			_M_mtx_impl.unlock();
        }
    private:
        fast_mutex(const fast_mutex&);
        fast_mutex& operator=(const fast_mutex&);
    };

/** An acquistion-on-initialization lock class based on fast_mutex. */
class fast_mutex_autolock
{
    fast_mutex& _M_mtx;
public:
    explicit fast_mutex_autolock(fast_mutex& mtx) : _M_mtx(mtx)
    {
        _M_mtx.lock();
    }
    ~fast_mutex_autolock()
    {
        _M_mtx.unlock();
    }
private:
    fast_mutex_autolock(const fast_mutex_autolock&);
    fast_mutex_autolock& operator=(const fast_mutex_autolock&);
};

#endif // _FAST_MUTEX_H
