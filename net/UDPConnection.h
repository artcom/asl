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
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      fair
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      fair
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
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
//    recommendations: change namespace, fix doxygen doctags,
//                     explain what this class does (what are UDP connections?)
//                     shouldn't the example be in the test?
*/

#ifndef INCL_INET_UDPCONNECTION
#define INCL_INET_UDPCONNECTION

#include "asl_net_settings.h"

#include "UDPSocket.h"

#include <asl/base/Ptr.h>

namespace inet {
    /** implements connected UDP 
     *
     * @code
     *
     * try {
     *     myUDPConnection = new UDPConnection(myPort, INADDR_ANY);
     *     break;
     * }
     * catch (SocketException & se)
     * {
     *     AC_ERROR << "failed to listen on port " << myPort << ":" << se.where() << endl;
     *     myUDPConnection = 0;
     * }
     *
     * inet::asl::Unsigned32 otherAddress = inet::Socket::getHostAddress("www.anyhost.de");
     * 
     * myUDPConnection.connect(otherPort, otherAddress);
     * 
     * myUDPConnection.read(myInputBuffer,bufferSize);
     * myUDPConnection.write(myOutputBuffer, bufferSize);
     *
     * @endcode
     */
    class ASL_NET_DECL UDPConnection : public UDPSocket {
        public:
            UDPConnection(asl::Unsigned32 thehost, asl::Unsigned16 theport);
            virtual bool connect(asl::Unsigned32 thehost, asl::Unsigned16 theport);
    };

    typedef asl::Ptr<UDPConnection> UDPConnectionPtr;
    
}
#endif
