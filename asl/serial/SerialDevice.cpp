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
//
//    $RCSfile: SerialDevice.cpp,v $
//
//     $Author: ulrich $
//
//   $Revision: 1.5 $
//
//=============================================================================

//own header
#include "SerialDevice.h"


namespace asl {

    static const char * ParityModeStrings[] = {
        "no_parity",
        "even_parity",
        "odd_parity",
        ""
    };

    IMPLEMENT_ENUM( SerialDevice::ParityMode, ParityModeStrings );


    SerialDevice::SerialDevice(const std::string & theDeviceName) :
        _isOpen(false),
        _myDeviceName(theDeviceName)
    {}

    SerialDevice::~SerialDevice() {
    }

    bool
    SerialDevice::isOpen() const {
        return _isOpen;
    }

    void
    SerialDevice::isOpen(bool theFlag) {
        _isOpen = theFlag;
    }

    const std::string &
    SerialDevice::getDeviceName() const {
        return _myDeviceName;
    }
}
