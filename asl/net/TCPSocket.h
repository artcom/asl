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
//                     explain use of this class, if abstract, make dtor pure virtual
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef INCL_INET_TCPSOCKET
#define INCL_INET_TCPSOCKET

#include "asl_net_settings.h"

#include "Socket.h"
#include <asl/base/Ptr.h>

#include "INetEndpoint.h"

namespace inet {

    class ASL_NET_DECL TCPSocket : public Socket {
        public:
            TCPSocket(asl::Unsigned32 thehost, asl::Unsigned16 theport);

            // Used by TCPServer.
            TCPSocket(int myFD, const asl::INetEndpoint & localEndpoint,
                      const asl::INetEndpoint & remoteEndpoint);

            virtual void open();

        protected:
            virtual int getSendFlags();
    };
    typedef asl::Ptr<TCPSocket> TCPSocketPtr;
}
#endif
