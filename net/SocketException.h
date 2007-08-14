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
//    names                   :      fair
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
//    recommendations: change namespace, fix names
*/

#ifndef SocketException_class
#define SocketException_class

#include <string>
#include <asl/Exception.h>

#ifdef WIN32
#include <winsock2.h>

#define OS_SOCKET_ERROR(errcode) WSA##errcode
#else
#define OS_SOCKET_ERROR(errcode) errcode
#endif

namespace inet {

class SocketException : public asl::Exception
{
public:
    SocketException (const std::string & where);
 
    SocketException (int theErrorCode, const std::string & where);
 
    SocketException (int theErrorCode, const std::string & what, const std::string & where) : 
        asl::Exception(what, where, "inet::SocketException" ), 
        _myErrorCode ( theErrorCode ) 
        {};
    ~SocketException (){};

    int getErrorCode() const { return _myErrorCode; }
private:
    int _myErrorCode;
};

}
#endif

