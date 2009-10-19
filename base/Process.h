/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2009, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_asl_Process_h_included_
#define _ac_asl_Process_h_included_

#include "asl_base_settings.h"

#ifdef UNIX
#include <sys/types.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <asl/base/Exception.h>

namespace asl {

class ProcessException : public asl::Exception {
public:
    ProcessException(const std::string & what = "",
                     const std::string & where = "")
        : asl::Exception(what, where)
    { }

    ~ProcessException() { }

};

class Process {
public:

    enum State {
        PROCESS_INITIALIZED,
        PROCESS_LAUNCHED,
        PROCESS_TERMINATED
    };

public:

    Process(const std::string & theCommand)
        : _myCommand(theCommand),
          _myState(PROCESS_INITIALIZED),
#ifdef UNIX
          _myPid(-1),
#endif
#ifdef WIN32
          _myHandle(INVALID_HANDLE_VALUE),
#endif
          _myStatusCode(-1)
    { }

    ~Process() { }

    bool hasTerminated() {
        return (_myState == PROCESS_TERMINATED);
    }

    const std::string & getCommand() {
        return _myCommand;
    }

    State getState() {
        return _myState;
    }

    int getStatusCode() {
        if(!hasTerminated()) {
            throw ProcessException("Can't get the status code of a process that has not yet terminated", PLUS_FILE_LINE);
        }
        return _myStatusCode;
    }

#ifdef WIN32
    void handleSystemError(const std::string & theSystemCall, const DWORD theError, const std::string & theLocationString = "");
#endif

    void launch();
    bool pollForTermination();
    void waitForTermination();

private:

    std::string _myCommand;

    State _myState;
#ifdef UNIX
    pid_t _myPid;
#endif
#ifdef WIN32
    HANDLE _myHandle;
#endif
    int   _myStatusCode;
};

}

#endif
