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
*/

#ifndef ASL_TIME_SOURCE_INCLUDED
#define ASL_TIME_SOURCE_INCLUDED

#include "asl_audio_settings.h"

#include <asl/base/Time.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/settings.h>

namespace asl {

class ASL_AUDIO_DECL TimeSource {
    public:
        virtual Time getCurrentTime() = 0;
        virtual ~TimeSource() {}
};


}
#endif
