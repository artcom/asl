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

#include "SampleSource.h"

namespace asl {

using namespace std;

    SampleSource::SampleSource(const std::string & theName, SampleFormat theSampleFormat,
                               unsigned theSampleRate, unsigned theNumChannels) :
        _mySampleRate(theSampleRate),
        _mySampleFormat(theSampleFormat),
        _numChannels(theNumChannels),
        _myEnableState(false)
    {
        AC_DEBUG << "SampleSource::SampleSource (" << _myName << ")";
    }


    SampleSource::~SampleSource() {
        AC_DEBUG << "~SampleSource end (" << _myName << ")";
    }


    unsigned SampleSource::getNumChannels() const {
        return _numChannels;
    }


    unsigned SampleSource::getSampleRate() const {
        return _mySampleRate;
    }


    inline unsigned SampleSource::getBytesPerSecond() const {
        return _mySampleRate*getBytesPerFrame();
    }


    inline SampleFormat SampleSource::getSampleFormat() const {
        return _mySampleFormat;
    }


    inline unsigned SampleSource::getBytesPerFrame() const {
        return _numChannels*getBytesPerSample(getSampleFormat());
    }


    std::string SampleSource::getName() const {
        return _myName;
    }


    void SampleSource::lock() {
        _mySampleSourceLock.lock();
    }


    void SampleSource::unlock() {
        _mySampleSourceLock.unlock();
    }


    void SampleSource::enable() {
        _myEnableState = true;
    }


    void SampleSource::disable() {
        _myEnableState = false;
    }


    bool SampleSource::isEnabled() {
        return _myEnableState;
    }

}

