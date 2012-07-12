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
//    $RCSfile: SingletonBase.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.3 $
//
// Description:
//
//=============================================================================

#ifndef _SINGLETON_BASE_INCLUDED_
#define _SINGLETON_BASE_INCLUDED_

#include "asl_base_settings.h"

namespace asl {


/*! \addtogroup aslbase */
/* @{ */

class ASL_BASE_DECL SingletonBase {
public:
    // The Singleton Manager will call this method on all singletons before it starts deleting them.
    // This way singletons can correctly shut down, independent of the order of destruction.
    virtual void stop() {};

	virtual ~SingletonBase() {}
};

/* @} */


}

#endif
