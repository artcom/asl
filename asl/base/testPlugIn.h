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

#ifndef ASL_TESTPLUGIN_H_
#define ASL_TESTPLUGIN_H_

#include <string>

#include <asl/base/PlugInBase.h>

namespace asl {

class ITestPlugIn : public PlugInBase {
public:
    ITestPlugIn(DLHandle theDLHandle)
        : PlugInBase(theDLHandle) {}

    virtual void touchThatString(std::string& theString) = 0;
};

}

#endif /* !ASL_TESTPLUGIN_H_ */
