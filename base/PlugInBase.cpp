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
//    $RCSfile: PlugInBase.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.3 $
//
// Description:
//
//=============================================================================

// own header
#include "PlugInBase.h"

#include <iostream>

#include "../base/SingletonManager.h"
#include "../base/Logger.h"

using namespace std;
using namespace asl;

#define DB(x) // x

/*
#ifdef LINUX
#else
__declspec (dllexport)
BOOL WINAPI DllMain (HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved) {
	static PlugInBase* thePlugInInstance = 0;
	if (ul_reason_for_call==DLL_PROCESS_ATTACH) {
	} else if (ul_reason_for_call==DLL_PROCESS_DETACH) {
		delete thePlugInInstance;
		thePlugInInstance = 0;
	}
	return TRUE;
}
#endif
*/

PlugInBase :: PlugInBase(DLHandle theDLHandle) :
    _myDLHandle(theDLHandle)
{}

void PlugInBase :: _initializeSingletonManager(SingletonManager* theSingletonManager) {
	// make sure our local SingletonManager delegates its calls
	// the the master SingletonManager of our process.
	AC_DEBUG << "SingletonManager in _initializeSingletonManager" << &SingletonManager::get() << std::endl;
	SingletonManager::get().setDelegate(theSingletonManager);
}

DLHandle PlugInBase :: getDLHandle() const {
	return _myDLHandle;
}


