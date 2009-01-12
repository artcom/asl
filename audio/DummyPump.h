/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
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
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef INCL_AUDIO_DUMMYPUMP
#define INCL_AUDIO_DUMMYPUMP

#include "asl_audio_settings.h"

#include "Pump.h"
#include "SampleFormat.h"
#include "HWSampleSink.h"
#include "AudioBuffer.h"

#include <asl/base/Singleton.h>

#include <string>
#include <vector>

namespace asl {

class DummyPump : public Singleton<DummyPump>, public Pump
{
        friend class SingletonManager;
    public:
        virtual ~DummyPump();
        
        virtual Time getCurrentTime();

    private:
        DummyPump();
        virtual void pump();
        
        unsigned _curFrame;
        AudioBuffer<float> _myOutputBuffer;
};

}

#endif 

