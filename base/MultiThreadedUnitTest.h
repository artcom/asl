/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description:  Wrapper to automatically run tests in parallel as thread
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : ok
//    documentation          : poor
//    test coverage          : ok
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : fair
//
//    recommendations:
//       - write some documentation
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _asl_MultiThreadedUnitTest_included_
#define _asl_MultiThreadedUnitTest_included_

#include "UnitTest.h"

#include "PosixThread.h"
#include "string_functions.h"

#include <string>
#include <iostream>
#include <vector>

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */
    
    struct ThreadInterface {
        virtual void fork() = 0;
        virtual void join() = 0;
    };

    template <class TEST>
        class ThreadedTemplateUnitTest : public TEST, public ThreadInterface {
            public:
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4355)     // "this" used in base member initializer list is unsafe.
                // use of this here is safe because we just store it on construction time
                // and use it later; derived "this" *must* not be used in the base class
                // to access the derived class during construction time because the derived
                // class might not be fully contructed at this point
#endif
                ThreadedTemplateUnitTest() : TEST(), _myThread(this) {
                    std::cerr << "Created ThreadedTemplateUnitTest this = " << this << ", _myThread = " << &_myThread << std::endl;
                }
#ifdef WIN32
#pragma warning(pop)
#endif
                PosixThread & getThread() {
                    return _myThread;
                }
                void fork() {
                    _myThread.fork();
                    std::cerr << "@@ Forked thread for test named '" << TEST::getMyName()
                        << "' with thread id " << _myThread.getThreadID() << std::endl;
                }
                void run() {
                    std::cerr << "run ThreadedTemplateUnitTest" << std::endl;

                    TEST::run();
                }
                void join() {
                    _myThread.join();
                }
            protected:
                struct MyThread : public PosixThread {
                    MyThread(TEST * theTest) : myTest(theTest) {}
                    TEST * myTest;
                    void run() {
                        std::cerr << "@@ Running test named '" << myTest->getMyName()
                            << "' with thread id " << getThreadID()
                            << " this= " << this << std::endl;
                        try {
                            myTest->run();
                        }
                        catch (asl::Exception & ex) {
                            std::cerr << "\n##### Exception in test '" << myTest->getMyName()
                                << "' with thread id " << getThreadID()
                                << " this= " << this <<  std::endl;
                            std::cerr << ex << std::endl;
                            myTest->FAILURE("Exception");
                        }
                        catch (...) {
                            std::cerr << "\n##### Unknown Exception in test '" << myTest->getMyName()
                                << "' with thread id " << getThreadID()
                                << " this= " << this << std::endl;
                            myTest->FAILURE("Exception");
                        }
                        std::cerr << "@@ Finished test named '" << myTest->getMyName()
                            << " with thread id '" << getThreadID() << std::endl;
                    };
                };
                MyThread _myThread;
        };

    class MultiThreadedTestSuite : public UnitTestSuite {
        public:
            MultiThreadedTestSuite(const char * theName, int argc, char *argv[]) : UnitTestSuite(theName, argc, argv) {  }
            virtual void run();
    };

    class CatchingUnitTestSuite : public UnitTestSuite {
        public:
            CatchingUnitTestSuite(const char * theName, int argc, char *argv[]) : UnitTestSuite(theName, argc, argv) {  }
            virtual void run();
    };

    /* @} */
}
#endif
