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
//    recommendations: merge with net_functions.h
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef INCL_INET_NET
#define INCL_INET_NET

#include "asl_net_settings.h"

#include <string>

namespace inet {
    ASL_NET_DECL void initSockets();
    ASL_NET_DECL void terminateSockets();
    ASL_NET_DECL int getLastSocketError();
    ASL_NET_DECL std::string getSocketErrorMessage(int ErrorID);
}
#endif
