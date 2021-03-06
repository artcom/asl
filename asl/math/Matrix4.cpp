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

//own header
#include "Matrix4.h"


static const char * MatrixTypeStrings[] =
{
    "identity",
    "x_rotating",
    "y_rotating",
    "z_rotating",
    "rotating",
    "scaling",
    "orthogonal",
    "linear",
    "translating",
    "affine",
    "unknown",
    ""
};

#ifdef verify
    #ifndef _SETTING_NO_UNDEF_WARNING_
        #warning Symbol 'verify' defined as macro, undefining. (Outrageous namespace pollution by Apples AssertMacros.h, revealing arrogance and incompetence)
    #endif
#undef verify
#endif

namespace asl
{
    IMPLEMENT_ENUM( asl::MatrixType, MatrixTypeStrings );
}