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

#ifndef _included_asl_checksum_
#define _included_asl_checksum_

#include "asl_base_settings.h"

#include "zlib.h"
#include <string.h>

namespace asl {
    template<class T>
    void
    appendCRC32( unsigned long & theCRC, const T & theValue) {
        const Bytef * myValuePtr = (const Bytef*)&theValue;
        theCRC = crc32(theCRC, myValuePtr, sizeof(T));
    }
    inline void
    appendCRC32( unsigned long & theCRC, const std::string & theValue) {
        const Bytef * myValuePtr = (const Bytef*)( theValue.c_str());
        theCRC = crc32(theCRC, myValuePtr, theValue.size());
    }
    inline void
    appendCRC32( unsigned long & theCRC, const char * theValue) {
        const Bytef * myValuePtr = (const Bytef*)( theValue);
        theCRC = crc32(theCRC, myValuePtr, strlen(theValue));
    }
} // end of namespace asl
#endif
