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
//    recommendations: change namespace
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "TCPSocket.h"

#include "SocketException.h"

using namespace asl;

namespace inet {

    TCPSocket::TCPSocket(asl::Unsigned32 thehost, asl::Unsigned16 theport)
        : Socket(thehost, theport)
    {
        open();
        _myIsConnected = false;
    }

    TCPSocket::TCPSocket(int myFD, const INetEndpoint & localEndpoint,
                      const INetEndpoint & remoteEndpoint)
        : Socket (0,0)
    {
          _myLocalEndpoint = localEndpoint;
          _myRemoteEndpoint = remoteEndpoint;
          fd = myFD;
          _myIsConnected = true;
    }


    void TCPSocket::open()
    {
        fd=socket(AF_INET,SOCK_STREAM,0);
    }

    int TCPSocket::getSendFlags()
    {
#ifndef MSG_NOSIGNAL
        return 0;
#else
        return MSG_NOSIGNAL; // suppress SIGPIPE
#endif
    }
}
