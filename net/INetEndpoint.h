/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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
//    documentation           :      poor
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
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
//    recommendations: change namespace
//                     explain use of this class
*/

#ifndef INCL_INET_ENDPOINT
#define INCL_INET_ENDPOINT

#include <asl/settings.h>

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <netdb.h>
    #include <sys/socket.h>
#endif  

#ifdef OSX
    #include <netinet/in.h>
#endif

#include <ostream>

namespace asl {

    // This is a POD type. Don't add virtual functions.
    class INetEndpoint: public sockaddr_in {
        public:
            INetEndpoint(const char * myHost, asl::Unsigned16 myPort);
            INetEndpoint(asl::Unsigned32 myHost, asl::Unsigned16 myPort);
            INetEndpoint();
            std::ostream & print(std::ostream & os) const;
    };

    std::ostream & operator << (std::ostream & os, const INetEndpoint & theEndpoint);
}
#endif
