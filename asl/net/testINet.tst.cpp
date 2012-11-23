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
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//    recommendations: add comments
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _WIN32
#include "TestINetMonitor.h"
#endif
#include "TestSocket.h"
#include "net.h"

#include <asl/base/Logger.h>
#include <asl/base/settings.h>

using namespace std;
using namespace inet;

int main( int argc, char *argv[] ) {
    initSockets();

    UnitTestSuite mySuite ("INet tests", argc, argv);

#ifndef _WIN32
    mySuite.addTest (new TestINetMonitor);
#endif
    mySuite.addTest (new TestSocket);

    mySuite.run();

    AC_INFO << ">> Finished test '" << argv[0] << "'"
           << ", return status = " << mySuite.returnStatus();

    terminateSockets();

    return mySuite.returnStatus();
}

