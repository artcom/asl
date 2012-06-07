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

#ifndef INCL_ISAMPLESINK
#define INCL_ISAMPLESINK

#include "asl_audio_settings.h"

#include "AudioBufferBase.h"

#include <asl/base/Ptr.h>

namespace asl {

class ASL_AUDIO_DECL ISampleSink
{
    public:
        virtual ~ISampleSink() {};
        virtual bool queueSamples(AudioBufferPtr& theBuffer) = 0; // Return false on error.
};

typedef Ptr<ISampleSink, MultiProcessor, PtrHeapAllocator<MultiProcessor> > ISampleSinkPtr;

}

#endif
