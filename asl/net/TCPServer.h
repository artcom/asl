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
//    recommendations: change namespace, fix doxygen doctags,
//                     explain use of this class, move example to tests
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef INCL_INET_TCPSERVER
#define INCL_INET_TCPSERVER

#include "asl_net_settings.h"

#include "TCPSocket.h"
#include "INetEndpoint.h"

namespace inet {

    /**
     * Implements a TCP Server (Listener).
     *
     * @code
     * try {
     *      myTCPServer = new TCPServer(serverPort, INADDR_ANY);
     *      break;
     * }
     * catch (SocketException & se)
     * {
     *      AC_ERROR << "failed to listen on port " << serverPort << ":" << se.where() << endl;
     *      myTCPServer = 0;
     * }
     *
     * TCPServer * myTCPServer = (TCPServer*)arg; // wait for a client to connect
     *
     * inet::TCPSocket *mySocket = myTCPServer->createNewConnection(); // get new connection
     *
     * mySocket->write("READY",5)
     * mySocket->read(myInputBuffer, 5);
     *
     * delete mySocket; // disconnect from the client
     * @endcode
     */
    class ASL_NET_DECL TCPServer {
        public:
            /// creates a new TCPServer listening on port theport on address fromhost.
            TCPServer(asl::Unsigned32 myHost, asl::Unsigned16 myPort, bool theReusePortFlag=false);
            virtual ~TCPServer();

            void close();
            /// waits for a client to connect.
            // @returns a new TCPSocket for each new connection
            TCPSocket* waitForConnection() const;

            virtual void setBlockingMode(bool isBlocking);

            asl::Unsigned32 getHost() {
                return _myHost;
            }

            asl::Unsigned16 getPort() {
                return _myPort;
            }

        private:
            asl::Unsigned32 _myHost;
            asl::Unsigned16 _myPort;
            asl::INetEndpoint _myFromAddr;
            int fd;
    };

}
#endif
