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

#include "Effect.h"

namespace asl {

Effect::Effect(asl::Ptr<EffectFunctorBase> myFunctor)
    : _myFunctor(myFunctor)
{
}

void Effect::apply(AudioBufferBase & theBuffer, Unsigned64 theAbsoluteFrame) {
    (*_myFunctor)(this, theBuffer, theAbsoluteFrame);
}

NullEffect::NullEffect(SampleFormat theSampleFormat)
    : Effect(createEffectFunctor<NullEffectFunctor>(theSampleFormat))
{
}

} // namespace

