//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "TCPPolicy.h"

#include <asl/net.h>
#include <asl/string_functions.h>
#include <errno.h>

using namespace std;
namespace asl {

#ifdef WIN32
#define EADDRINUSE WSAEADDRINUSE 
#else
#define INVALID_SOCKET -1
#endif

TCPPolicy::Handle 
TCPPolicy::connectTo(Endpoint theRemoteEndpoint) {
    inet::initSockets();
    
    Handle myHandle = socket(AF_INET,SOCK_STREAM,0);   
    if (myHandle == INVALID_SOCKET) {
        throw ConduitException(string("TCPPolicy::ctor: create - ") + 
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
    }
    if( ::connect(myHandle,(struct sockaddr*)&theRemoteEndpoint,sizeof(theRemoteEndpoint)) != 0 )
    {
        throw ConduitException(string("TCPPolicy::ctor: connect - ") + 
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
    }
    return myHandle;
}

void
TCPPolicy::disconnect(Handle & theHandle) {
    SocketPolicy::disconnect(theHandle);
    inet::terminateSockets();
}

void 
TCPPolicy::stopListening(Handle theHandle) {
    SocketPolicy::stopListening(theHandle);
    inet::terminateSockets();
}


TCPPolicy::Handle 
TCPPolicy::createOnConnect(Handle & theListenHandle, unsigned theMaxConnectionCount, 
        int theTimeout) {
    Handle myHandle = SocketPolicy::createOnConnect(theListenHandle, theMaxConnectionCount, theTimeout);
    if (myHandle > 0) {
        inet::initSockets();
    }
    
    return myHandle;
}

TCPPolicy::Handle
TCPPolicy::startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount) {
    inet::initSockets();

    Handle myHandle=socket(AF_INET,SOCK_STREAM,0);    
    
    int myReuseSocketFlag = 1;
    if (setsockopt(myHandle, SOL_SOCKET, SO_REUSEADDR, (char*)&myReuseSocketFlag, sizeof(myReuseSocketFlag)) != 0) {
        throw ConduitException("can`t set already bound rcv socket to reuse.", PLUS_FILE_LINE);
    }

    if (bind(myHandle,(struct sockaddr*)&theEndpoint,sizeof(theEndpoint))<0) {
        int myLastError = inet::getLastSocketError();
        if (myLastError == EADDRINUSE) {
            throw ConduitInUseException(string("TCPPolicy::ctor create"), PLUS_FILE_LINE);
        } else {
            throw ConduitException(string("TCPPolicy::TCPPolicy bind - ")+
                    inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
        }
    }
    if (listen(myHandle, theMaxConnectionCount)<0) {
        throw ConduitException("TCPPolicy::TCPPolicy: listen - "+
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
    }
    return myHandle;
}
}

