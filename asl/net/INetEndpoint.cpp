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
//    recommendations: change namespace, explain use of this class
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "INetEndpoint.h"

#include "net_functions.h"

#include <string.h>

namespace asl {
    INetEndpoint::INetEndpoint(const char * theHost, Unsigned16 myPort) {
        memset(this, 0, sizeof(*this));
        sin_family=AF_INET;
        sin_addr.s_addr=htonl(asl::hostaddress(theHost));
        sin_port=htons(myPort);
    }

    INetEndpoint::INetEndpoint(Unsigned32 theHost, Unsigned16 myPort) {
        memset(this, 0, sizeof(*this));
        sin_family=AF_INET;
        sin_addr.s_addr=htonl(theHost);
        sin_port=htons(myPort);
    }

    INetEndpoint::INetEndpoint() {
        memset(this, 0, sizeof(*this));
        sin_family=AF_INET;
    }
    std::ostream &
    INetEndpoint::print(std::ostream & os) const {
        return os << as_dotted_address(ntohl(sin_addr.s_addr)) << ":" << ntohs(sin_port);
    }

    std::ostream & operator << (std::ostream & os, const INetEndpoint & theEndpoint) {
        return theEndpoint.print(os);
    }
}
