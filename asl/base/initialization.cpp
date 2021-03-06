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
// Description: initializes some globaklly needed stuff
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "initialization.h"


#if defined(_WIN32)
#   include <crtdbg.h>
#endif

namespace {

#   if defined(_WIN32)
#       ifdef DEBUG_VARIANT
        inline void setCrtReportModeToFile(int reportType)
        {
            _CrtSetReportMode( reportType, _CRTDBG_MODE_DEBUG
                                         | _CRTDBG_MODE_FILE );
            _CrtSetReportFile( reportType, _CRTDBG_FILE_STDERR );
        }
        inline void init_platform() {
                _CrtSetDbgFlag(0); // disable heap checking for debug builds by default
                setCrtReportModeToFile(_CRT_WARN  );
                setCrtReportModeToFile(_CRT_ERROR );
                setCrtReportModeToFile(_CRT_ASSERT);
        }
#       else
        inline void init_platform() {
        }
#       endif
#   elif defined(LINUX)
        inline void init_platform()
        {
        }
#   elif defined(OSX)
        inline void init_platform()
        {
        }
#   else
#       error Unknown Platform!
#   endif


}


struct AslInitializer {
    AslInitializer()
    {
        init_platform();
    }
} ourAslInitializer;


namespace asl {
    void initialize() {
        (void)&ourAslInitializer;
    }
} // namespace asl
