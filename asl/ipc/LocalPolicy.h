/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2012, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description:
//     Classes for networked or local communication between processes
//
// Recommendations: add high-level documentation, improve doxygen documentation
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef __asl_LocalPolicy_included
#define __asl_LocalPolicy_included

#include "asl_ipc_settings.h"

#ifdef _WIN32

#include "NamedPipePolicy.h"
namespace asl {
/*! \addtogroup ipcPolicies */
/* @{ */
    typedef NamedPipePolicy LocalPolicy;
/* @} */
}

#else

#include "UnixSocketPolicy.h"
namespace asl {
/*! \addtogroup ipcPolicies */
/* @{ */
    /// Platform abstraction: This typedefed to UnixSocketPolicy (POSIX) or NamedPipePolicy (Win32).
    typedef UnixSocketPolicy LocalPolicy;
/* @} */
}

#endif

#endif
