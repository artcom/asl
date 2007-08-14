/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: 
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      fair
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      fair
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
//    interfaces              :      ok
//    confidence              :      ok
//    integration             :      ok
//    dependencies            :      ok
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations: change namespace, fix doxygen doctags,
//                     explain use of this class, move example to tests
*/

#include <errno.h>

#include "TCPServer.h"
#include <asl/Logger.h>
#include <asl/Time.h>

#ifdef OSX
#include <netinet/in.h>
#endif

#ifndef WIN32
#include <arpa/inet.h>
#define SOCKET_ERROR -1
#endif

using namespace std;


namespace inet {

    TCPServer::TCPServer(asl::Unsigned32 myHost, asl::Unsigned16 myPort, bool theReusePortFlag)
        : _myPort(myPort),
          _myHost(myHost),
          _myFromAddr(myHost, myPort)
    {
        fd=socket(AF_INET,SOCK_STREAM,0);

        if (bind(fd,(struct sockaddr*)&_myFromAddr,sizeof(_myFromAddr))<0) {
            // Socket was in use, try again if theReusePortFlag=true
            if (theReusePortFlag) {
                int myReuseSocketLen = sizeof(bool);
                if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&theReusePortFlag, myReuseSocketLen) == SOCKET_ERROR) {
                    throw SocketException("TCPServer::TCPServer: can`t set already bound rcv socket to reuse.");
                }

                if (bind(fd,(struct sockaddr*)&_myFromAddr,sizeof(_myFromAddr))<0 ) {
                     throw SocketException("TCPServer::TCPServer: can`t bind rcv socket ");
                }
            } else {
                throw SocketException("TCPServer::TCPServer: can`t bind socket, already is use.");
            }
        }
        if (listen(fd,8)<0) {
            throw SocketException("TCPServer::TCPServer: can`t listen ");
        }
    }

    TCPServer::~TCPServer() {
        close();
    }

    void TCPServer::close() {
#ifdef WIN32
        closesocket(fd);
#else
        ::close(fd);
#endif
        fd = -1;
        AC_INFO << "TCPServer closed" << endl;
    }

    TCPSocket* TCPServer::waitForConnection() const
    {
#ifdef WIN32
        int remoteEndpointLen;
#else
        socklen_t remoteEndpointLen;
#endif
        char *myaddrstr;
        asl::INetEndpoint remoteEndpoint;

        remoteEndpointLen=sizeof(remoteEndpoint);
        int newFD;
        if ((newFD=accept(fd,(sockaddr *)&remoteEndpoint,&remoteEndpointLen))<0) {
            SocketException("TCPServer::waitForConnection: can't accept connection");
        }
        TCPSocket * newSocket = new TCPSocket(newFD, _myFromAddr, remoteEndpoint);

        AC_DEBUG << "new connection accepted on socket " << newFD << endl;

        myaddrstr=inet_ntoa(remoteEndpoint.sin_addr);
        if (myaddrstr) {
            AC_DEBUG << "from host " << myaddrstr << ',' << " port " << remoteEndpoint.sin_port << endl;
        }
        return newSocket;
    }
}
