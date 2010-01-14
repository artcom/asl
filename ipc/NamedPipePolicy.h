/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
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
// Last Review:  ms 2007-08-15
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      fair
//    documentation           :      ok
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      fair
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
//    recommendations: add high-level documentation, improve doxygen documentation
*/
#ifndef __asl_NamedPipePolicy_included
#define __asl_NamedPipePolicy_included

#include "asl_ipc_settings.h"

#include "ConduitPolicy.h"
#include <asl/base/Ptr.h>

#include <string>
#include <vector>
#include <deque>

#include <windows.h>

namespace asl {

/*! \addtogroup ipcPolicies */
/* @{ */

//! Conduit policy for Win32 Named Pipe-based communication (Windows only).

class ASL_IPC_DECL NamedPipePolicy : public ConduitPolicy {
    public:
        typedef std::string Endpoint;

        struct PipeInfo {
            PipeInfo() :
                pipeInstance(0), pipeName(""),
                isServerHandle(false), isSending(false),
                outQueue(0), isReceiving(false), inQueue(0), isValid(false)
                {
                    memset(&accept_overlap,0,sizeof(OVERLAPPED));
                };
            PipeInfo(HANDLE theInstance, Endpoint theName) :
                pipeInstance(theInstance), pipeName(theName),
                isServerHandle(false), isSending(false),
                outQueue(0), isReceiving(false), inQueue(0), isValid(true)
                {
                    memset(&accept_overlap,0,sizeof(OVERLAPPED));
                };
                inline operator const void*() const { return pipeInstance ? this : NULL; };
            HANDLE pipeInstance;
            Endpoint pipeName;
            OVERLAPPED accept_overlap;
            bool isValid;
            bool isServerHandle;
            bool isSending;
            CharBufferPtr outBuffer;
            BufferQueue * outQueue;
            bool isReceiving;
            CharBuffer inBuffer;
            BufferQueue * inQueue;
        };
        typedef PipeInfo Handle;

        /// Creates a (client) conduit connected to the remote endpoint
        // @throws ConduitException
        static Handle connectTo(Endpoint theRemoteEndpoint);

        static bool isValid(const Handle & theHandle) {
            return theHandle.isValid;
        }

        /// disconnects a conduit
        // @throws ConduitException
        static void disconnect(Handle & theHandle);
        /// Handles pending IO operations - should be called periodically (e.g. per frame)
        // @returns true if the conduit is still valid
        // @throws ConduitException
        static bool
        handleIO(Handle & theHandle, BufferQueue & theInQueue, BufferQueue & theOutQueue, int theTimeout = 0);

        // Acceptor methods
        static Handle startListening(Endpoint theEndpoint, unsigned theMaxConnectionCount);
        static void stopListening(Handle theHandle);
        static Handle createOnConnect(Handle & theListenHandle, unsigned theMaxConnectionCount,
                int theTimeout);

    private:
        static void WINAPI onWriteCompleted(DWORD theError, DWORD theBytesTransferred, LPOVERLAPPED theOverlap);
        static void WINAPI onReadCompleted(DWORD theError, DWORD theBytesTransferred, LPOVERLAPPED theOverlap);
        static void sendNextBuffer(Handle & theHandle);
        static void receiveNextBuffer(Handle & theHandle);
        static Handle createListenHandle(Endpoint theEndpoint, unsigned theMaxConnectionCount, bool theMasterListener);
        static int waitForOverlapped(Handle & theHandle, LPDWORD theBytesTransferred,
                bool isSending, int theTimeout=-1);
} ;

inline std::ostream& operator<<(std::ostream & s, const NamedPipePolicy::Handle & h) {
    s << (h.isServerHandle ? "SERVER(" : "CLIENT(") << h.pipeInstance << ")";
    return s;
}


/* @} */
} // namespace

#endif
