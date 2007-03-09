//============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================


#ifndef ASL_SERIAL_DEVICE_FACTORY_INCLUDED
#define ASL_SERIAL_DEVICE_FACTORY_INCLUDED

#include <string>

namespace asl {

    class SerialDevice;

    // This function creates a new serial device. Don't forget to delete
    // it when you're done with it.
    SerialDevice * getSerialDevice(unsigned int theIndex);
    SerialDevice * getSerialDeviceByName(const std::string & theDevice);
}

#endif // ASL_SERIAL_DEVICE_FACTORY_INCLUDED
