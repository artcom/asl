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
#ifndef __asl_UnixSocketPolicy_included
#define __asl_UnixSocketPolicy_included

#include "SocketPolicy.h"

#include <asl/net/INetEndpoint.h>
#include <asl/base/Exception.h>

#include <sys/socket.h>
#include <sys/un.h>

namespace asl {

/*! \addtogroup ipcPolicies */
/* @{ */

class UnixAddress : public sockaddr_un {
    public:
        UnixAddress(const std::string & thePath);
        UnixAddress(const char * thePath);
        UnixAddress();
        static const std::string PIPE_PREFIX;
        std::ostream & print(std::ostream & os) const;
};
std::ostream & operator << (std::ostream & os, const UnixAddress & theAddress);

/*! Conduit policy for Unix socket-based communication (POSIX only).
@note there is a typedef to asl::LocalPolicy (POSIX only)
*/

class UnixSocketPolicy : public SocketPolicy {
    public:
        typedef UnixAddress Endpoint;
        /// Creates a (client) conduit connected to the remote endpoint
        ///@throws ConduitException
        static Handle connectTo(Endpoint theRemoteEndpoint);

        // Acceptor methods
        static Handle startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount);
        static void stopListening(Handle theHandle);
};
/* @} */
}
#endif
