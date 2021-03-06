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

#include "DirectSoundPump.h"

#include "DirectSoundMessages.h"

#include <asl/base/Logger.h>
#include <asl/base/os_functions.h>
#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/Assure.h>
#include <asl/base/Auto.h>

#ifdef USE_DASHBOARD
#include <asl/base/Dashboard.h>
#endif

#include <exception>
#include <sstream>
#include <string.h>

#pragma warning(push)
#pragma warning(disable: 4201)
//#include <atlconv.h>
#include <mmsystem.h>
#include <Mmreg.h>
#include <Ks.h>
#include <Ksmedia.h>
#pragma warning(pop)

using namespace std;

namespace asl {
    typedef map<string,LPGUID> SoundDeviceMap;

    BOOL CALLBACK enumerateDSSoundCards(LPGUID theId,
                LPSTR theDescription,
                LPSTR theDriverName,
                LPVOID theStorage )
    {
        LPGUID myIdPtr = NULL;

        SoundDeviceMap * myMap = static_cast<SoundDeviceMap*>(theStorage);
        if (theId != NULL)  {
            if ((myIdPtr = (LPGUID)malloc(sizeof(GUID))) == NULL) {
                return true;
            }
            memcpy(myIdPtr, theId, sizeof(GUID));
        }

        (*myMap)[std::string((LPCTSTR)theDescription)] = myIdPtr;
        return true;
    }

void dumpDSCaps(const DSCAPS& theDSCaps);

DirectSoundPump::~DirectSoundPump () {
    AC_INFO << "DirectSoundPump::~DirectSoundPump";

    Pump::stop();
    closeOutput();


    if (_myNumUnderruns > 0) {
        AC_WARNING << "DirectSoundPump had " << _myNumUnderruns << " underruns.";
    }
}

void DirectSoundPump::dumpState () const {
    Pump::dumpState();
    if (isOutputOpen()) {
        dumpDSCaps(_myDSCaps);
    }
}

Time DirectSoundPump::getCurrentTime() {
    updateFramesPlayed();
    while (_myFramesPlayed < getNativeSampleRate()) {
        msleep(10);
        updateFramesPlayed();
    }
    return double(_myFramesPlayed)/getNativeSampleRate();
}

DirectSoundPump::DirectSoundPump ()
    : Pump(SF_F32, 1),
      _myIsOutOpen(false),
      _myDS(NULL),
      _myPrimaryBuffer(NULL),
      _myWriteCursor(0),
      _myNumUnderruns(0),
      _myFramesPlayed(0),
      _mySoundCardId(NULL)
{
    AC_INFO << "DirectSoundPump::DirectSoundPump";

    _myFramesPerBuffer = unsigned(getLatency() * getNativeSampleRate())*2;

    string myDeviceName = "DirectSound default device";//default";
    get_environment_var_as("Y60_SOUND_DEVICE_NAME", myDeviceName);
    AC_DEBUG << "DirectSound Device name: \"" << myDeviceName << "\"";
    setDeviceName(myDeviceName);


    setCardName("");

    handleDeviceSelection();

//    dumpState();
    start();
}

void
DirectSoundPump::handleDeviceSelection() {
    unsigned myDeviceNum = 0;
    get_environment_var_as("Y60_SOUND_DEVICE_NUM", myDeviceNum);
    AC_DEBUG << "DirectSound Device num: \"" << myDeviceNum << "\"";
    SoundDeviceMap mySoundCardMap;
    if (FAILED(DirectSoundEnumerate((LPDSENUMCALLBACK)enumerateDSSoundCards, (LPVOID)&mySoundCardMap))) {
        AC_WARNING << "Sorry, an error occured while enumerating sounddevices";
    }
    if (myDeviceNum > mySoundCardMap.size()-1) {
        AC_WARNING << "Sorry, Y60_SOUND_DEVICE_NUM: " << myDeviceNum << " invalid, there are only "
                   << mySoundCardMap.size()-1 << " sounddevices";
    }
    SoundDeviceMap::iterator myBegin = mySoundCardMap.begin();
    SoundDeviceMap::iterator myEnd = mySoundCardMap.end();
    unsigned myCounter = 0;
    for (;myBegin !=  myEnd; myBegin++) {
        AC_DEBUG <<  "Audio device: " << myBegin->first << " -> " << &myBegin->second;
        if (myCounter == myDeviceNum) {
            _mySoundCardId = myBegin->second;
            AC_DEBUG <<  "    use Audio device: " << myBegin->first;
        }
        myCounter++;
    }
    openOutput();

    myBegin = mySoundCardMap.begin();
    myEnd = mySoundCardMap.end();
    for (;myBegin !=  myEnd; myBegin++) {
        free(myBegin->second);
    }

}

void
DirectSoundPump::openOutput() {
    AC_INFO << "DirectSoundPump::openOutput";
    if (isOutputOpen()) {
        AC_WARNING << "DirectSoundPump::openOutput: Device already open";
        return;
    }
    HRESULT theRetVal;

    theRetVal = DirectSoundCreate(_mySoundCardId, &_myDS, NULL);
    checkDSRetVal(theRetVal, PLUS_FILE_LINE);

    // Determine window. This stuff is mostly bullshit because SetCooperativeLevel
    // really doesn't do anything for WDM sound cards.
    HWND myWindow = NULL;
    myWindow = ::GetForegroundWindow();
    if (!myWindow) {
        myWindow = ::GetDesktopWindow();
    }
    if (myWindow) {
        theRetVal = _myDS->SetCooperativeLevel(myWindow, DSSCL_PRIORITY);
        checkDSRetVal(theRetVal, PLUS_FILE_LINE);
    }
    ASSURE_MSG(myWindow, "Could not retrieve a window and hence could not set Cooperative level");
    ZeroMemory(&_myDSCaps, sizeof(DSCAPS));
    _myDSCaps.dwSize = sizeof(DSCAPS);
    theRetVal = _myDS->GetCaps(&_myDSCaps);
    checkDSRetVal(theRetVal, PLUS_FILE_LINE);

//    initPrimaryBuffer();
    initSecondaryBuffer();
    initNotification();

    _myOutputBuffer.init(_myFramesPerBuffer, getNumOutputChannels(), getNativeSampleRate());

    HRESULT hr;
    DWORD myPlayCursor;
    hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, &_myWriteCursor);
    checkDSRetVal (hr, PLUS_FILE_LINE);
    hr = _myDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
    checkDSRetVal (hr, PLUS_FILE_LINE);

    _myIsOutOpen = true;
}

void
DirectSoundPump::closeOutput() {
    AC_INFO << "DirectSoundPump::closeOutput";
    if (isOutputOpen()) {
        if (_myDSBuffer) {
            _myDSBuffer->Stop();
        }
        if (_myPrimaryBuffer) {
            _myPrimaryBuffer->Release();
            _myPrimaryBuffer = NULL;
        }
        if (_myDS) {
            _myDS->Release();
            _myDS = NULL;
        }
    }
    _myIsOutOpen = false;
}


bool
DirectSoundPump::isOutputOpen() const {
    return _myIsOutOpen;
}

void DirectSoundPump::initPrimaryBuffer() {
    // This whole function is probably completely unnessesary for WDM driver model cards.
    // We're not calling it anymore anyway...
    HRESULT theRetVal;
    WAVEFORMATEX wfx;
    DSBUFFERDESC dsbdesc;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = static_cast<WORD>(getNumOutputChannels());
    wfx.nSamplesPerSec = getNativeSampleRate();
    wfx.wBitsPerSample = static_cast<WORD>(getBytesPerSample(getNativeSampleFormat())*8);
    wfx.nBlockAlign = 4;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags       = DSBCAPS_PRIMARYBUFFER | DSBCAPS_GLOBALFOCUS; // all panning, mixing, etc done by synth
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat   = NULL;
    theRetVal = _myDS->CreateSoundBuffer( &dsbdesc, &_myPrimaryBuffer, NULL);
    checkDSRetVal(theRetVal, PLUS_FILE_LINE);
    theRetVal = _myPrimaryBuffer->SetFormat(&wfx);
    checkDSRetVal(theRetVal, PLUS_FILE_LINE);
}

void DirectSoundPump::initSecondaryBuffer() {
    WAVEFORMATEX* myWF = 0;
    DSBUFFERDESC dsbdesc;
    HRESULT hr;

    switch (getNativeSampleFormat()) {
        case SF_S16:
        case SF_S8:
            {
                myWF = new WAVEFORMATEX;
                // Set up wave format structure.
                memset(myWF, 0, sizeof(WAVEFORMATEX));
                myWF->wFormatTag = WAVE_FORMAT_PCM;
                myWF->nChannels = static_cast<WORD>(getNumOutputChannels());
                // This is really frames (samples per channel) per second...
                myWF->nSamplesPerSec = getNativeSampleRate();
                myWF->wBitsPerSample = static_cast<WORD>(getBytesPerSample(getNativeSampleFormat())*8);
                myWF->nBlockAlign = static_cast<WORD>(getOutputBytesPerFrame());
                myWF->nAvgBytesPerSec = myWF->nSamplesPerSec * myWF->nBlockAlign;
                myWF->cbSize = sizeof(WAVEFORMATEX);
            }
            break;
        case SF_F32:
            {
                WAVEFORMATEXTENSIBLE * myFloatWF = new WAVEFORMATEXTENSIBLE;
//                WAVEFORMATIEEEFLOATEX * myFloatWF = new WAVEFORMATIEEEFLOATEX;
                // Set up wave format structure.
                memset(myFloatWF, 0, sizeof(WAVEFORMATEXTENSIBLE));
                myFloatWF->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
                myFloatWF->Format.nChannels = static_cast<WORD>(getNumOutputChannels());
                // This is really frames (samples per channel) per second...
                myFloatWF->Format.nSamplesPerSec = getNativeSampleRate();
                myFloatWF->Format.wBitsPerSample = static_cast<WORD>(getBytesPerSample(getNativeSampleFormat())*8);
                myFloatWF->Format.nBlockAlign = static_cast<WORD>(getOutputBytesPerFrame());
                myFloatWF->Format.nAvgBytesPerSec = myFloatWF->Format.nSamplesPerSec *
                        myFloatWF->Format.nBlockAlign;
                myFloatWF->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
                myFloatWF->Samples.wValidBitsPerSample = static_cast<WORD>(getBytesPerSample(getNativeSampleFormat())*8);
                myFloatWF->dwChannelMask = 0; // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT; ?!
                myFloatWF->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
                myWF = (WAVEFORMATEX *)myFloatWF;
            }
            break;
        default:
            ASSURE_MSG(false, "Illegal sample format in DSSampleSink::createDSBuffer");
    }

    // Set up DSBUFFERDESC structure.
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);

    dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLPOSITIONNOTIFY |
            DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STICKYFOCUS | DSBCAPS_GLOBALFOCUS;
    dsbdesc.dwBufferBytes = _myFramesPerBuffer*getOutputBytesPerFrame();
    dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)myWF;

    // Create buffer.
    IDirectSoundBuffer * myDSBuffer;
    hr = _myDS->CreateSoundBuffer(&dsbdesc, &myDSBuffer, NULL);
    checkDSRetVal(hr, PLUS_FILE_LINE);

    hr = myDSBuffer->QueryInterface(IID_IDirectSoundBuffer, (void**)&_myDSBuffer);
    checkDSRetVal(hr, PLUS_FILE_LINE);
    myDSBuffer->Release();
    delete myWF;

    // Clear the buffer
    zeroDSBuffer();
}

void
DirectSoundPump::initNotification()
{
    LPDIRECTSOUNDNOTIFY myDsNotify;
    HRESULT hr = _myDSBuffer->QueryInterface(IID_IDirectSoundNotify,
            (LPVOID *)&myDsNotify);
    checkDSRetVal (hr, PLUS_FILE_LINE);

    _myWakeup0Handle = CreateEvent(NULL, FALSE, TRUE, NULL);
    _myWakeup1Handle = CreateEvent(NULL, FALSE, TRUE, NULL);
    _myWakeupOnStopHandle = CreateEvent(NULL, FALSE, TRUE, NULL);
    DSBPOSITIONNOTIFY PositionNotify[3];

    PositionNotify[0].dwOffset = 0;
    PositionNotify[0].hEventNotify = _myWakeup0Handle;
    PositionNotify[1].dwOffset = _myFramesPerBuffer*getOutputBytesPerFrame()/2;
    PositionNotify[1].hEventNotify = _myWakeup1Handle;
    PositionNotify[2].dwOffset = DSBPN_OFFSETSTOP;
    PositionNotify[2].hEventNotify = _myWakeupOnStopHandle;
    hr = myDsNotify->SetNotificationPositions(3, PositionNotify);
    checkDSRetVal (hr, PLUS_FILE_LINE);

    myDsNotify->Release();
}

void DirectSoundPump::zeroDSBuffer()
{
    HRESULT hr;
    BYTE * myBufferData = NULL;
    DWORD myDataLen;
    unsigned myBytesToWrite = _myFramesPerBuffer*getOutputBytesPerFrame();

    hr = _myDSBuffer->Lock(0, myBytesToWrite, (LPVOID*) &myBufferData, &myDataLen, NULL, 0, 0);
    checkDSRetVal (hr, PLUS_FILE_LINE);
    ZeroMemory(myBufferData, myDataLen);
    hr = _myDSBuffer->Unlock(myBufferData, myDataLen, NULL, 0);
    checkDSRetVal (hr, PLUS_FILE_LINE);

}

void DirectSoundPump::pump()
{
    HANDLE myHandles[3];
    myHandles[0] = _myWakeup0Handle;
    myHandles[1] = _myWakeup1Handle;
    myHandles[2] = _myWakeupOnStopHandle;
    DWORD mySignaledIndex = WaitForMultipleObjects(3, myHandles, false, 1000);
        if (mySignaledIndex != 2) {
            // Beginning of buffer - increase time offset.
            addFramesToTime(_myFramesPerBuffer/2);
        }
    AutoLocker<Pump> myLocker(*this);
    if (mySignaledIndex == WAIT_TIMEOUT) {
        AC_WARNING << "Timeout while waiting for event from buffer";
    } else if (mySignaledIndex == WAIT_FAILED) {
        AC_WARNING << "Error in DirectSoundPump::pump():"
                << DSoundMessages::WindowsError(GetLastError());
   } else if (mySignaledIndex == 2) {
       AC_DEBUG << "DirectSoundPump::pump: Stop signaled." << endl;
    } else {
        writeToDS();
    }
}

void DirectSoundPump::writeToDS() {
    HRESULT hr;

    DWORD myPlayCursor;
    DWORD myWriteCursor;
    hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, &myWriteCursor);
    checkDSRetVal(hr, PLUS_FILE_LINE);

    unsigned numBytesToDeliver ;
    if (myPlayCursor > _myWriteCursor) {
        numBytesToDeliver = myPlayCursor-_myWriteCursor;
    } else {
        numBytesToDeliver = myPlayCursor+(_myFramesPerBuffer*getOutputBytesPerFrame())
                -_myWriteCursor;
    }
    AC_TRACE << "_myWriteCursor: " << _myWriteCursor << ", numBytesToDeliver: "
            << numBytesToDeliver << endl;
    AC_TRACE << "Before mix: DS PlayCursor: " << myPlayCursor
            << ", DS Write Cursor: " << myWriteCursor << endl;
    unsigned oldWriteCursor = _myWriteCursor;

    unsigned numFramesToDeliver = numBytesToDeliver/getOutputBytesPerFrame();
    // Obtain memory address of write block. This will be in two parts
    // if the block wraps around.
    void * myWritePtr1;
    unsigned long myWriteBytes1;
    void * myWritePtr2;
    unsigned long myWriteBytes2;
    mix(_myOutputBuffer, numFramesToDeliver);
    hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, &myWriteCursor);
    checkDSRetVal(hr, PLUS_FILE_LINE);
    AC_TRACE << "After mix: DS PlayCursor: " << myPlayCursor
            << ", DS Write Cursor: " << myWriteCursor << endl;

#ifdef USE_DASHBOARD
    MAKE_SCOPE_TIMER(DSBufferLock);
#endif
    hr = _myDSBuffer->Lock(_myWriteCursor, numBytesToDeliver, &myWritePtr1,
            &myWriteBytes1, &myWritePtr2, &myWriteBytes2, 0);
    // If the buffer was lost, restore and retry lock.
    // (I don't think this ever happens with current drivers/os'es)
    if (DSERR_BUFFERLOST == hr)
    {
        AC_DEBUG << "Lost buffer!";
        _myDSBuffer->Restore();
        hr = _myDSBuffer->Lock(_myWriteCursor, numBytesToDeliver, &myWritePtr1,
                &myWriteBytes1, &myWritePtr2, &myWriteBytes2, 0);
    }
    checkDSRetVal(hr, PLUS_FILE_LINE);

    // Copy data to DirectSound memory.
    _myOutputBuffer.copyToRawMem(myWritePtr1, 0, myWriteBytes1);
    _myWriteCursor += myWriteBytes1;
    if (myWritePtr2 != 0) {
        _myOutputBuffer.copyToRawMem(myWritePtr2, myWriteBytes1, myWriteBytes2);
        _myWriteCursor += myWriteBytes2;
    }
    _myWriteCursor %= _myFramesPerBuffer*getOutputBytesPerFrame();

    // Release the data back to DirectSound.
    hr = _myDSBuffer->Unlock(myWritePtr1, myWriteBytes1, myWritePtr2, myWriteBytes2);
    checkDSRetVal (hr, PLUS_FILE_LINE);

    // Test for buffer underrun
    hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, &myWriteCursor);
    checkDSRetVal (hr, PLUS_FILE_LINE);
    if (myPlayCursor < myWriteCursor) {
        // Nonusable part of buffer isn't split.
        if (oldWriteCursor < myWriteCursor && oldWriteCursor > myPlayCursor) {
            handleUnderrun();
        }
    } else {
        // Nonusable part of buffer is split.
        if (oldWriteCursor > myPlayCursor || oldWriteCursor < myWriteCursor) {
            handleUnderrun();
        }
    }
    updateFramesPlayed();
}

void DirectSoundPump::handleUnderrun() {
    _myNumUnderruns++;
    if (_myNumUnderruns == 1) {
        AC_WARNING << "DirectSound buffer underrun.";
    } else {
        AC_TRACE << "DirectSound buffer underrun.";
    }
}

void DirectSoundPump::updateFramesPlayed() {
    DWORD myPlayCursor;
    AutoLocker<ThreadLock> myLocker(_myTimeLock);
    HRESULT hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, 0);
    checkDSRetVal (hr, PLUS_FILE_LINE);
    int myDelta = (int)myPlayCursor/getOutputBytesPerFrame()
            - int(_myFramesPlayed % _myFramesPerBuffer);
    if (myDelta < 0) {
        myDelta += _myFramesPerBuffer;
        AC_TRACE << "### Wrap around.";
    }
    _myFramesPlayed+=myDelta;
}

void dumpDSCaps(const DSCAPS& theDSCaps) {
    AC_DEBUG << "DirectSound driver caps:";
    AC_DEBUG << "  Certified or WDM driver: " <<
        ((theDSCaps.dwFlags & DSCAPS_CERTIFIED) ? "true" : "false");
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_CONTINUOUSRATE ) ? "Supports" : "Does not support")
         << " variable sample rate playback.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_EMULDRIVER) ? "Is" : "Is not")
        << " being emulated with waveform audio functions.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_PRIMARY16BIT) ? "Supports" : "Does not support")
        << " 16-bit primary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_PRIMARY8BIT) ? "Supports" : "Does not support")
        << " 8-bit primary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_PRIMARYMONO) ? "Supports" : "Does not support")
        << " mono primary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_PRIMARYSTEREO) ? "Supports" : "Does not support")
        << " stereo primary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_SECONDARY16BIT ) ? "Supports" : "Does not support")
        << " 16-bit secondary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_SECONDARY8BIT ) ? "Supports" : "Does not support")
        << " 8-bit secondary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_SECONDARYMONO) ? "Supports" : "Does not support")
        << " mono secondary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_SECONDARYSTEREO) ? "Supports" : "Does not support")
        << " stereo secondary buffers.";
    AC_DEBUG << "  Minimum secondary buffer sample rate = " << theDSCaps.dwMinSecondarySampleRate;
    AC_DEBUG << "  Maximum secondary buffer sample rate = " << theDSCaps.dwMaxSecondarySampleRate;
    AC_DEBUG << "  Number of primary buffers = " << theDSCaps.dwPrimaryBuffers;
    AC_DEBUG << "  Maximum hardware buffers = " << theDSCaps.dwMaxHwMixingAllBuffers;
    AC_DEBUG << "  Maximum hardware static buffers = " << theDSCaps.dwMaxHwMixingStaticBuffers ;
    AC_DEBUG << "  Maximum hardware streaming buffers = " << theDSCaps.dwMaxHwMixingStreamingBuffers ;
    AC_DEBUG << "  Free hardware buffers = " << theDSCaps.dwFreeHwMixingAllBuffers ;
    AC_DEBUG << "  Free hardware static buffers = " << theDSCaps.dwFreeHwMixingStaticBuffers ;
    AC_DEBUG << "  Free hardware streaming buffers = " << theDSCaps.dwFreeHwMixingStreamingBuffers ;
    AC_DEBUG << "  Maximum hardware 3D buffers = " << theDSCaps.dwMaxHw3DAllBuffers ;
    AC_DEBUG << "  Maximum hardware 3D static buffers = " << theDSCaps.dwMaxHw3DStaticBuffers ;
    AC_DEBUG << "  Maximum hardware 3D streaming buffers = " << theDSCaps.dwMaxHw3DStreamingBuffers ;
    AC_DEBUG << "  Free hardware 3D buffers = " << theDSCaps.dwFreeHw3DAllBuffers ;
    AC_DEBUG << "  Free hardware 3D static buffers = " << theDSCaps.dwFreeHw3DStaticBuffers ;
    AC_DEBUG << "  Free hardware 3D streaming buffers = " << theDSCaps.dwFreeHw3DStreamingBuffers ;
    AC_DEBUG << "  Total hardware memory = " << theDSCaps.dwTotalHwMemBytes ;
    AC_DEBUG << "  Free hardware memory = " << theDSCaps.dwFreeHwMemBytes ;
    AC_DEBUG << "  Max contiguous free memory = " << theDSCaps.dwMaxContigFreeHwMemBytes ;
    AC_DEBUG << "  Hardware buffer data transfer rate = " << theDSCaps.dwUnlockTransferRateHwBuffers ;
    AC_DEBUG << "  CPU overhead for software buffers = " << theDSCaps.dwPlayCpuOverheadSwBuffers ;
}

}

