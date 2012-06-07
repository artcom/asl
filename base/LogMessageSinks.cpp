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
//
//
// Description: TODO
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "LogMessageSinks.h"

namespace asl {

ThreadLock StreamPrinter::myLock;
ThreadLock FilePrinter::myLock;

#ifdef _WIN32
ThreadLock OutputWindowPrinter::myLock;
#endif
#ifndef _WIN32
ThreadLock SyslogPrinter::myLock;
#endif

}
