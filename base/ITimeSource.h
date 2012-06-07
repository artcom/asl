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
// Description:  Simple time source interface. getCurrentTime() should return
//               the time in seconds since the TimeSource was constructed.
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef ASL_I_TIME_SOURCE_INCLUDED
#define ASL_I_TIME_SOURCE_INCLUDED

#include "asl_base_settings.h"

namespace asl {

/*! \addtogroup aslbase */
/* @{ */

class ITimeSource {
    public:
        virtual double getCurrentTime() = 0;
};

/* @} */

}
#endif // ASL_I_TIME_SOURCE_INCLUDED
