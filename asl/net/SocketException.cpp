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
//    recommendations: change namespace, fix names
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "SocketException.h"

#include "net.h"

#include <asl/base/string_functions.h>

#include <iostream>
#include <algorithm>
#include <string>

#ifndef _WIN32
#include <errno.h>
#endif

using namespace std;

namespace inet {

}
