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
//     Classes for networked or local communication between processes
//
// Recommendations: add high-level documentation, improve doxygen documentation
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "UnixSocketPolicy.h"

#include <asl/net/net.h>
#include <asl/base/string_functions.h>

#ifdef LINUX
    #include <unistd.h>
#endif
#include <errno.h>

using namespace std;

#define DB(x) // x

namespace asl {

#ifndef WIN32
#define INVALID_SOCKET -1
#endif

const int UNIX_PATH_MAX = 108;
const string UnixAddress::PIPE_PREFIX("/tmp/ArtCom_");

UnixAddress::UnixAddress(const std::string & thePath) {
    sun_family = AF_UNIX;
    string myPath = UnixAddress::PIPE_PREFIX+thePath;
    strncpy(sun_path, myPath.c_str(), UNIX_PATH_MAX);
}

UnixAddress::UnixAddress(const char * thePath) {
    sun_family = AF_UNIX;
    string myPath = UnixAddress::PIPE_PREFIX+thePath;
    strncpy(sun_path, myPath.c_str(), UNIX_PATH_MAX);
}

UnixAddress::UnixAddress() {
    sun_family = AF_UNIX;
    *sun_path = 0;
}

std::ostream &
UnixAddress::print(std::ostream & os) const {
    return os << sun_path;
}

std::ostream & operator << (std::ostream & os, const UnixAddress & theAddress) {
    return theAddress.print(os);
}

UnixSocketPolicy::Handle
UnixSocketPolicy::connectTo(Endpoint theRemoteEndpoint) {
    Handle myHandle = socket(AF_UNIX,SOCK_STREAM,0);
    if (myHandle == INVALID_SOCKET) {
        throw ConduitException(string("UnixSocketPolicy::ctor: create - ") +
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
    }

    if( ::connect(myHandle,(struct sockaddr*)&theRemoteEndpoint,sizeof(theRemoteEndpoint)) != 0 )
    {
        int myLastError = inet::getLastSocketError();
        if (myLastError == ECONNREFUSED) {
            throw ConduitRefusedException(string("UnixSocketPolicy::ctor: connect - ") +
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
        } else {
            throw ConduitException(string("UnixSocketPolicy::ctor: connect - ") +
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
        }
    }
    DB(cerr << "Client connected " << endl);
    return myHandle;
}


UnixSocketPolicy::Handle
UnixSocketPolicy::startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount) {
    Handle myHandle=socket(AF_UNIX,SOCK_STREAM,0);
    if (myHandle == INVALID_SOCKET) {
        throw ConduitException(string("UnixSocketPolicy::startListening") +
                inet::getSocketErrorMessage(inet::getLastSocketError()) +
                "'" + as_string(theEndpoint)+"'", PLUS_FILE_LINE);
    }

    if (bind(myHandle,(struct sockaddr*)&theEndpoint,sizeof(theEndpoint))<0) {
        int myLastError = inet::getLastSocketError();
        if (myLastError == EADDRINUSE) {
            throw ConduitInUseException(string("UnixSocketPolicy::bind") + "'" +
                    as_string(theEndpoint)+"'", PLUS_FILE_LINE);
        } else {
            throw ConduitException(string("UnixSocketPolicy::UnixSocketPolicy bind - ")+
                    inet::getSocketErrorMessage(inet::getLastSocketError()) +
                    "'" + as_string(theEndpoint)+"'", PLUS_FILE_LINE);
        }
    }

    if (listen(myHandle, theMaxConnectionCount)<0) {
        throw ConduitException("UnixSocketPolicy::UnixSocketPolicy: listen - "+
                inet::getSocketErrorMessage(inet::getLastSocketError()), PLUS_FILE_LINE);
    }
    DB(cerr << "Listening " << endl);
    return myHandle;
}

void
UnixSocketPolicy::stopListening(Handle theHandle) {
    Endpoint localEndpoint;
    socklen_t myNameLength = sizeof(localEndpoint);
    getsockname(theHandle, (struct sockaddr*)(&localEndpoint), &myNameLength);
    SocketPolicy::stopListening(theHandle);
    unlink(localEndpoint.sun_path);
}

}

