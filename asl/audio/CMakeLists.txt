
set(SOURCES
AudioBufferBase.cpp
AudioBuffer.cpp
AudioTimeSource.cpp
DummyPump.cpp
Effect.cpp
ExternalTimeSource.cpp
HWSampleSink.cpp
Pump.cpp
SampleFormat.cpp
SampleSource.cpp
TestPump.cpp
VolumeFader.cpp
)

set(HEADERS
asl_audio_settings.h
AudioBufferBase.h
AudioBuffer.h
AudioException.h
AudioTimeSource.h
DummyPump.h
Effect.h
ExternalTimeSource.h
HWSampleSink.h
ISampleSink.h
Pump.h
SampleFormat.h
SampleSource.h
TestPump.h
TimeSource.h
VolumeFader.h
)

set(TESTS Effect)

#audio tests fail randomly on linux64, disabled for now, needs investigation 11/12
if(NOT LINUX)
    list(APPEND TESTS DummyPump RealPump)
endif(NOT LINUX)

if(OSX)
    set(SOURCES ${SOURCES} CoreAudioPump.cpp)
    set(HEADERS ${HEADERS} CoreAudioPump.h)
    list(APPEND EXTERNS ${AUDIO_UNIT}) # XXX: framework reference
endif(OSX)

if(LINUX)
    set(SOURCES ${SOURCES} ALSAPump.cpp)
    set(HEADERS ${HEADERS} ALSAPump.h)
    list(APPEND EXTERNS ALSA)
endif(LINUX)

if(WIN32)
    set(SOURCES ${SOURCES} DirectSoundPump.cpp)
    set(HEADERS ${HEADERS} DirectSoundPump.h DirectSoundMessages.h)
    list(APPEND EXTERNS WINMM DIRECTX)
endif(WIN32)

ac_add_library(
    aslaudio asl/audio
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS}
    DEPENDS aslbase
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)
