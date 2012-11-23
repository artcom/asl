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
//    recommendations: change namespace, fix unreferenced local variable work-around
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "TCPClientSocket.h"

#include "net.h"

#include <asl/base/Logger.h>
#include <asl/base/Time.h>

#ifndef _WIN32
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <errno.h>

using namespace asl;
using namespace std;

namespace inet {

    TCPClientSocket::TCPClientSocket()
        : TCPSocket(0, 0)
    {
    }


    void TCPClientSocket::connect()
    {
        open();

        if( ::connect(fd,(struct sockaddr*)&_myRemoteEndpoint,sizeof(_myRemoteEndpoint)) != 0 ) {
            int err = getLastSocketError();
            throw SocketError(err, "TCPClientSocket::ConnectSocket()");
        }

        _myIsConnected = true;
    }

    int TCPClientSocket::retry(int n)
    {
        for (int retry=1 ;retry<=n ; retry++ )
        {
            try {
                connect();
                return true;
            } catch (SocketException & se)
            {
                (void)se;  // Avoid unreferenced variable warning.
                AC_DEBUG << retry << ". retry failed" << endl;
                msleep(2000);
            }
        }
        return false;
    }

}
