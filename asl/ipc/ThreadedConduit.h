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
#ifndef _asl_ipc_threadedConduit_included_
#define _asl_ipc_threadedConduit_included_

#include "asl_ipc_settings.h"

#include "Conduit.h"
#include <asl/base/Ptr.h>
#include <asl/base/Logger.h>

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

//! A Conduit which uses threads for I/O
/**
    Each ThreadedConduit uses its own thread for I/O. Implement processData() in your derived class.
    @note: the I/O routines offered by the base class Conduit should only be called from the I/O thread, i.e. inside
    processData(). That's why we use protected inheritance to hide these routines from the outside.
**/

template <class POLICY>
class ThreadedConduit : protected Conduit<POLICY> {
    public:
        /// create a new client connected to theRemoteEndpoint
        ThreadedConduit(typename POLICY::Endpoint theRemoteEndpoint)
            : Conduit<POLICY>(theRemoteEndpoint), _myThread(0), _cancelFlag(false)
        {
        };
        virtual ~ThreadedConduit() {
            if (_myThread) {
                stop();
            }
        }
        /// starts the I/O thread
        bool start() {
            return pthread_create(&_myThread, NULL, threadMainLoop, (void *)this) == 0;
        }

        /// stops the I/O thread
        bool stop() {
            bool mySuccessFlag = false;
            if (_myThread) {
                //_cancelFlag = true;
                pthread_cancel(_myThread);
                mySuccessFlag = !pthread_join(_myThread, NULL);
                _myThread = 0;
            }
            return mySuccessFlag;
        }
    protected:
        /// attachs a new ThreadedConduit around an already connected i/o handle.
        ThreadedConduit(typename POLICY::Handle theHandle)
            : Conduit<POLICY>(theHandle), _myThread(0)
        {
        }

        /// the conduit-thread's main loop (called in a tight loop).
        /** This is called from the thread context of the conduit. Be sure to
             use only thread-safe communication with the main application thread.
             Remember: processData is called in a tight loop in its own thread. On the one hand,
             this permits the use of blocking i/o calls without slowing the main application. On the
             other hand, if you return immediately, you will waste CPU time (busy waiting).
          @returns true if the connection should be terminated.
          @code Example: a simple conduit which sends everything back.

virtual bool processData() {
    CharBuffer myInputBuffer;
    if (this->receiveData(myInputBuffer)) { // blocking call
        this->sendData(myInputBuffer);
    }
    return true;
}
          @endcode
          **/
        virtual bool processData()  = 0;
    private:
        bool _cancelFlag;

        // hide default ctors
        ThreadedConduit();
        ThreadedConduit(const ThreadedConduit<POLICY> &);
        ThreadedConduit<POLICY> & operator=(const ThreadedConduit &);

        pthread_t _myThread;

        static void * threadMainLoop(void * theThisPointer) {
            int myResult = 0;
            int myOldCancelState = 0;
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &myOldCancelState);
            pthread_cleanup_push(onThreadDone, theThisPointer);
            ThreadedConduit<POLICY> * mySelf =
                reinterpret_cast<ThreadedConduit<POLICY>*>(theThisPointer);
            try {
                for(;;) {
                    pthread_testcancel();
                    if (!mySelf->isValid()) {
                        break;
                    }
                    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,0);
                    if (!mySelf->processData()) {
                        break;
                    }
                    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &myOldCancelState);
                }
            } catch (ConduitException & ex) {
                AC_ERROR << ex;
            }
            pthread_cleanup_pop(0);
            pthread_setcancelstate(myOldCancelState,0);
            return (void*)static_cast<ptrdiff_t>(myResult);
        }
        static void onThreadDone(void * theThisPointer) {
            //AC_WARNING << "onThreadDone";
        }
};

/* @} */
}

#endif
