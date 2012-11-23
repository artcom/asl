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
#ifndef __asl_ConduitPolicy_included
#define __asl_ConduitPolicy_included

#include "asl_ipc_settings.h"

#include <asl/base/Exception.h>
#include <asl/base/Ptr.h>
#include <asl/base/Block.h>
#include <vector>
#include <deque>
#include <sstream>

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

DEFINE_EXCEPTION(ConduitException, asl::Exception);
DEFINE_EXCEPTION(ConduitInUseException, ConduitException);
DEFINE_EXCEPTION(ConduitRefusedException, ConduitException);

typedef std::vector<char> CharBuffer;
typedef asl::Ptr<CharBuffer> CharBufferPtr;
typedef std::deque<CharBufferPtr> BufferQueue;

//! Abstract base for Conduit Policy classes.
/**
  The use of policy classes for the low-level protocols allow the use of mix-and-match conduits. Example policy
  classes are TCPPolicy and NamedPipePolicy. They can be combined with application-level protocols (e.g. framed
  message-based communication with asl::Block based payloads) to provide Messages-over-tcp (MessageAcceptor<TCPPolicy>)
  or Messages-over-Named Pipes (MessageAcceptor<NamedPipePolicy>).
**/
class ASL_IPC_DECL ConduitPolicy {
    public:
//        enum State { COMPLETED, PENDING, NO_CONNECTION };

};

/* @} */
}

#endif

