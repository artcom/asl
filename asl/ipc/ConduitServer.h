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
#ifndef __asl_Conduit_server_included_
#define __asl_Conduit_server_included_

#include "asl_ipc_settings.h"

#include "ThreadedConduit.h"

#include <string>

namespace asl {

#define DB(x) // x;

/*! \addtogroup aslipc */
/* @{ */

//! Abstract Server.
/**
    Each server runs in its own thread. Implement ThreadedConduit<POLICY>::processData() in your derived class.
**/
template <class POLICY> class ConduitAcceptor;

template <class POLICY>
class ConduitServer : public ThreadedConduit<POLICY> {
        friend class ConduitAcceptor<POLICY>;
    public:
        typedef asl::Ptr<ConduitServer<POLICY> > Ptr;
        typedef asl::WeakPtr<ConduitServer<POLICY> > WPtr;

        WPtr getSelf() const {
            return _mySelf;
        }

    protected:
        ConduitServer(typename POLICY::Handle theHandle) :
            ThreadedConduit<POLICY>(theHandle), _myAcceptor(0) { }

        ConduitAcceptor<POLICY> * getAcceptor() const {
            return _myAcceptor;
        }

        void virtual setAcceptor(ConduitAcceptor<POLICY> * theAcceptor) {
            _myAcceptor = theAcceptor;
        }

    private:
        // hide ctors - can only be constructed by factory method
        ConduitServer();
        ConduitServer(const ConduitServer &);
        ConduitServer & operator=(const ConduitServer &);


        void setSelf (Ptr theSelfPtr) {
            _mySelf = theSelfPtr;
        }
        WPtr _mySelf;
        ConduitAcceptor<POLICY> * _myAcceptor;
};

/* @} */
}

#undef DB
#endif
