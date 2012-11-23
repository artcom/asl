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
#ifndef __asl_TCPPolicy_included
#define __asl_TCPPolicy_included

#include "asl_ipc_settings.h"

#include "SocketPolicy.h"

#include <asl/net/INetEndpoint.h>
#include <asl/base/Exception.h>

namespace asl {

/*! \addtogroup ipcPolicies */
/* @{ */

//! Conduit policy for TCP-based communication.

class ASL_IPC_DECL TCPPolicy : public SocketPolicy {
    public:
        typedef INetEndpoint Endpoint;
        /// Creates a (client) conduit connected to the remote endpoint
        // @throws ConduitException
        static Handle connectTo(Endpoint theRemoteEndpoint);

        // Acceptor methods
        static Handle startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount);
        static void disconnect(Handle & theHandle);
        static void stopListening(Handle theHandle);
        static Handle createOnConnect(Handle & theListenHandle, unsigned theMaxConnectionCount,
                int theTimeout);
};
/* @} */
}
#endif
