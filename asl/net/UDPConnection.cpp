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
//    recommendations: change namespace, styleguide conformance
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "UDPConnection.h"

#include "SocketException.h"
#include "net.h"

#include <asl/base/Logger.h>

#include <errno.h>
#ifndef _WIN32
#include <unistd.h>
#endif

namespace inet {

    UDPConnection::UDPConnection(asl::Unsigned32 thehost, asl::Unsigned16 theport):UDPSocket(thehost, theport)
    {
    }

    bool UDPConnection::connect(asl::Unsigned32 thehost, asl::Unsigned16 theport)
    {
        setRemoteAddr(thehost, theport);

        if ( ::connect(fd,(struct sockaddr*)&_myRemoteEndpoint,sizeof(_myRemoteEndpoint)) != 0 ) {
            int err = getLastSocketError();
            throw SocketError(err, "UDPConnection::Connect()");
        }
        return true;
    }
    
    unsigned UDPConnection::send(const void *data, unsigned len)
    {
        unsigned bytesSent = 0;
        try {
            bytesSent = UDPSocket::send(data, len);
        } catch (const inet::SocketError & err) {
            if(err.getErrorCode() == OS_SOCKET_ERROR(ECONNREFUSED)) {
                //http://www.unixguide.net/network/socketfaq/5.4.shtml
                AC_WARNING << "Received ICMP Error from Receiver " << getRemoteAddress() << " at port "
                           << getRemotePort() << ": " << err;
            } else {
                throw err;
            }
        }
        return bytesSent;
    }

}
