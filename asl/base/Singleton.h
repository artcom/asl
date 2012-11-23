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
//    $RCSfile: Singleton.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.5 $
//
// Description:
//
//=============================================================================

#ifndef _SINGLETON_INCLUDED_
#define _SINGLETON_INCLUDED_

#include "SingletonManager.h"

namespace asl {


/*! \addtogroup aslbase */
/* @{ */

template<class T>
class Singleton : public SingletonBase {
public:
    static T& get() {
        // A Singleton caches the instance returned by SingletonManager
        // in a local static variable.
        static T& myInstance(SingletonManager::get().SingletonManager::getSingletonInstance<T>());
        return myInstance;
    }
};

/* @} */


}

#endif
