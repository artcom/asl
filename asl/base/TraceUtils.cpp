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

// own header
#include "TraceUtils.h"

#include <string>
#include <cstdlib>

#ifndef _WIN32
#include <cxxabi.h>
#endif

using namespace std;

#ifndef _WIN32

// demangling for gcc
std::string
beautify(const std::string & theSymbol) {
    string::size_type myEndPos = theSymbol.find(':');
    if (myEndPos == string::npos) {
        myEndPos = theSymbol.size();
    }
    string::size_type myStartPos = 0;
    // XXX
    if (theSymbol[0] == '_' && theSymbol[1] == '_') {
        myStartPos = 1;
    }
    std::string myMangledSymbol = theSymbol.substr(myStartPos, myEndPos);
    int myStatus;
    char * myDemangledCString = abi::__cxa_demangle(myMangledSymbol.c_str(), 0, 0, &myStatus);
    string myResult;
    if (myStatus != 0) {
        myResult = myMangledSymbol;
    } else {
        myResult = myDemangledCString;
    }
    if (myDemangledCString) {
        free(myDemangledCString);
    }
    return myResult;
}

#else

std::string
beautify(const std::string & theSymbol) {
    return theSymbol;
}
#endif

