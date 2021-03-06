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

#ifndef GLIBC_BACKTRACE_INCLUDED
#define GLIBC_BACKTRACE_INCLUDED

#include "TraceUtils.h"

#include <vector>
#include <string>

namespace asl {

/** Policy to create backtraces on systems using GNU libc
 * It is used in conjunction with the StackTracer
 * template.
 * The code is adapted form the harmonia package (http://harmonia.cs.berkeley.edu)
 * in file <pre>common/backtrace.cc</pre>
 */
class GlibcBacktrace {
    public:
        struct GlibcStackFrame : public StackFrameBase {
            std::string file;
            unsigned    line;
        };
        typedef GlibcStackFrame StackFrame;

        static void trace(std::vector<StackFrame> & theStack, int theMaxDepth);

    private:
        GlibcBacktrace();

};

}

#endif // GLIBC_BACKTRACE_INCLUDED
