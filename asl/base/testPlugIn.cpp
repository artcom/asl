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

#include <string>
#include <iostream>

#include "testPlugIn.h"


namespace asl {

using namespace std;

class RealTestPlugIn : public ITestPlugIn {
public:

    RealTestPlugIn (DLHandle theDLHandle)
        : ITestPlugIn(theDLHandle) {};

    int
    initialize() {
        cout << "TestPlugIn initializing!" << endl;
        // cout << "SingletonManager in MyPlugIn :: initialize" << &SingletonManager::get() << endl;
        // cout << "PlugInManager search path:" << PlugInManager::get().getSearchPath() << endl;
        return 0;
    }

    void
    cleanUp() {
        cout << "TestPlugIn cleaning up!" << endl;
    }

    void
    touchThatString(std::string& theString) {
        theString = "touched";
    }
};

}

extern "C"
EXPORT asl::PlugInBase*
asltestplugin_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new asl::RealTestPlugIn(myDLHandle);
}

