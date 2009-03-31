/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "SerialDeviceFactory.h"

#include <asl/base/string_functions.h>

#include <string>

#ifdef _WIN32
#   include "ComPort.h"
#else
#   include "TTYPort.h"
#endif

namespace asl {

SerialDevice * 
getSerialDevice(unsigned int theIndex) {
#ifdef _WIN32
    return new ComPort(std::string("\\\\.\\COM") + as_string(theIndex + 1));
#else
    return new TTYPort(std::string("/dev/ttyS") + as_string(theIndex));
#endif
}

SerialDevice * getSerialDeviceByName(const std::string & theDevice) {
#ifdef _WIN32
    return new ComPort( theDevice );
#else
    return new TTYPort( theDevice );
#endif
}
}
