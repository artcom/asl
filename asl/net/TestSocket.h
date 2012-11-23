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
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
//    recommendations: move to inner class of *.tst.cpp file
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef INCL_TESTSOCKETS
#define INCL_TESTSOCKETS

#include "asl_net_settings.h"

#include <string>
#include <asl/base/UnitTest.h>

#include "TCPServer.h"

class ASL_NET_DECL TestSocket: public UnitTest{
    public:
        TestSocket ()
            : UnitTest("TestSocket") {}

        virtual void run();
    private:

        /// creates a new tcp server. you 'll need to delete it
        inet::TCPServer * createTCPServer();

        void testHostAddressMethods ();
        void BaseTest();
        void TCPTest();
        void UDPTest();
        void testConnectionTimeout ();
        static void * TCPServerThread(void *arg);
        static void * UDPServerThread(void *arg);

};

#endif
