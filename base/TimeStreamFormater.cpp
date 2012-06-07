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
//    $RCSfile: TimeStreamFormater.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.1 $
//
// Description: Class for simple measurement of time, resolution depends on
//              hardware, maximal 1 micro second
//
//
//=============================================================================

// own header
#include "TimeStreamFormater.h"

#include <ios>

namespace asl {

const int TimeStreamFormater::ourIsFormatedFlagIndex( std::ios_base::xalloc() );
const int TimeStreamFormater::ourFormatStringIndex( std::ios_base::xalloc() );

TimeStreamFormater::TimeStreamFormater(const char * theFormatString) :
    _myFormatString(theFormatString)
{}

TimeStreamFormater::TimeStreamFormater(const TimeStreamFormater & otherFormater) :
    _myFormatString(otherFormater._myFormatString)
{}

void
TimeStreamFormater::setStreamInfo(std::ios_base & theStream) const {
    theStream.iword(ourIsFormatedFlagIndex) = true;
    theStream.pword(ourFormatStringIndex) = (void *) _myFormatString ;
}

TimeStreamFormater
formatTime(const char * theFormatString) {
    return TimeStreamFormater(theFormatString);
}

std::ostream &
operator << (std::ostream & theStream, const TimeStreamFormater & theFormater) {
    theFormater.setStreamInfo( theStream );
    return theStream;
}


} // end of namespace asl
