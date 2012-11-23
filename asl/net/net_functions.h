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
//    recommendations: merge with net_functions.h, translate german comments to english
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_net_functions_
#define _included_asl_net_functions_

#include "asl_net_settings.h"

#include <asl/base/Exception.h>
#include <asl/base/Block.h>

#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef LINUX
    #include <unistd.h>
#endif

#ifdef _WIN32
    #include <winsock2.h>
#endif


namespace asl {

    ASL_NET_DECL std::string localhostname();
    ASL_NET_DECL std::string hostname(unsigned long hostAddress);
    ASL_NET_DECL unsigned long hostaddress(const std::string & theHost);

    ASL_NET_DECL unsigned long from_dotted_address(const std::string & dottedAddress);
    ASL_NET_DECL std::string as_dotted_address(unsigned long theAdress);

    ASL_NET_DECL asl::Block getHardwareAddress(const std::string & theInterfaceName = "");
} //Namespace asl

#endif

