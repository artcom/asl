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
#ifndef __asl_SocketPolicy_included
#define __asl_SocketPolicy_included

#include "asl_ipc_settings.h"

#include "ConduitPolicy.h"

#include <asl/net/INetEndpoint.h>
#include <asl/base/Exception.h>

namespace asl {

/*! \addtogroup ipcPolicies */
/* @{ */

//! Abstract base class for all socket-based conduit policies.

class ASL_IPC_DECL SocketPolicy : public ConduitPolicy {
    public:
        typedef int Handle;

        static bool isValid(const Handle & theHandle) {
            return theHandle != 0;
        }

        static void disconnect(Handle & theHandle);
        /// Handles pending IO operations - should be called periodically (e.g. per frame)
        // @returns true if the conduit is still valid
        // @throws ConduitException
        static bool
        handleIO(Handle & theHandle, BufferQueue & theInQueue, BufferQueue & theOutQueue, int theTimeout = 0);
        // Acceptor methods
        static void stopListening(Handle theHandle);
        static Handle createOnConnect(Handle & theListenHandle, unsigned theMaxConnectionCount,
                int theTimeout);
    protected:
        static bool receiveData(Handle & theHandle, BufferQueue & theInQueue);
        static bool sendData(Handle & theHandle, BufferQueue & theOutQueue);
};
/* @{ */
}
#endif
