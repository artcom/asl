
set(SOURCES
CRC8.cpp
SerialDevice.cpp
SerialDeviceFactory.cpp
uCCP.cpp
)

set(HEADERS
asl_serial_settings.h
CRC8.h
DebugPort.h
SerialDeviceFactory.h
SerialDevice.h
uCCP.h
)

set(TESTS
SerialDevice
)

if(WIN32)
    set(SOURCES ${SOURCES} ComPort.cpp)
    set(HEADERS ${HEADERS} ComPort.h)
else(WIN32)
    set(SOURCES ${SOURCES} TTYPort.cpp)
    set(HEADERS ${HEADERS} TTYPort.h)  
endif(WIN32)

ac_add_library(
    aslserial asl/serial
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    TESTS   ${TESTS}
    DEPENDS aslbase
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)
