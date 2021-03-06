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
//
// Description:  Memory Block handling functions; should be used whenever
//               using memcpy is considered; features
//               - typesafe conversion,
//               - compile-time memory read-write control,
//               - convenient windows api interfaces and
//               - also a copy-on-write (COW) implementation
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_console_functions_h_
#define _included_asl_console_functions_h_

#include "asl_base_settings.h"

#include "settings.h"

#include <iostream>

namespace asl {;

/*! \addtogroup aslbase */
/* @{ */
    bool isTTY(std::ostream & os);

    ASL_BASE_DECL std::ostream & TTYGREEN (std::ostream & os );

    ASL_BASE_DECL std::ostream & TTYRED (std::ostream & os );
    ASL_BASE_DECL std::ostream & TTYYELLOW (std::ostream & os );
    ASL_BASE_DECL std::ostream & TTYBLUE (std::ostream & os );

    ASL_BASE_DECL std::ostream & ENDCOLOR (std::ostream & os );
/* @} */
}

#endif
