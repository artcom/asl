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
//    recommendations: change namespace, add doxygen doctags,
//                     shouldn't the example be in the test?
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef INCL_INET_TCPCLIENTSOCKET
#define INCL_INET_TCPCLIENTSOCKET

#include "asl_net_settings.h"

#include "TCPSocket.h"

namespace inet {
    /**
     * Implements a TCP Client.
     *
     */
    class ASL_NET_DECL TCPClientSocket : public TCPSocket {
        public:
            TCPClientSocket();
            void connect();
            int retry(int n);
    };
    typedef asl::Ptr<TCPClientSocket> TCPClientSocketPtr;

}
#endif
