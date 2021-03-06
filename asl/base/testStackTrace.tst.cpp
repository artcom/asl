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
*/

#include "UnitTest.h"
#include "StackTrace.h"

#include <string>
#include <iostream>

using namespace std;
using namespace asl;


void throwAndTrace() {
    throw asl::Exception("Alles Ok!", PLUS_FILE_LINE );
}


class StackTraceUnitTest : public UnitTest {
public:
    StackTraceUnitTest() : UnitTest("StackTraceUnitTest") {  }
    void run() {
        StackTrace myStack;
        // if this fails because of optimization just change it to greater zero
#ifdef LINUX
        ENSURE( myStack.size() == 7 );
#ifndef DEBUG_VARIANT
#elif _WIN32
        ENSURE( myStack.size() == 8 );
#endif
#endif

        DPRINT( myStack );
        ENSURE_EXCEPTION( throwAndTrace(), asl::Exception );

        asl::Exception myException("Alles Ok!", PLUS_FILE_LINE);
#ifdef LINUX
        ENSURE( myException.stack().size() == 8 );
#ifndef DEBUG_VARIANT
#elif _WIN32
        ENSURE( myException.stack().size() == 9 );
#endif
#endif
        DPRINT( Exception::getDumpStackTraceFlag() );
        DPRINT( myException );
        Exception::dumpStackTrace( true );
        DPRINT( Exception::getDumpStackTraceFlag() );
        DPRINT( myException );
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new StackTraceUnitTest, 10000);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
