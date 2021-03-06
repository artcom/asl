
set(SOURCES
Arguments.cpp
Block.cpp
console_functions.cpp
Dashboard.cpp
Exception.cpp
ExitHandler.cpp
file_functions.cpp
initialization.cpp
Logger.cpp
LogMessageFormatters.cpp
LogMessageSinks.cpp
MultiThreadedUnitTest.cpp
os_functions.cpp
Path.cpp
PlugInBase.cpp
PlugInManager.cpp
PosixThread.cpp
proc_functions.cpp
Process.cpp
Ptr.cpp
ReadWriteLock.cpp
SingletonManager.cpp
StackTrace.cpp
StdOutputRedirector.cpp
Stream.cpp
string_functions.cpp
ThreadHelper.cpp
ThreadLock.cpp
ThreadSemaphore.cpp
Time.cpp
TimeStreamFormater.cpp
TraceUtils.cpp
UnitTest.cpp
)

set(HEADERS
Arguments.h
asl_base_settings.h
Assure.h
AtomicCount.h
Auto.h
begin_end.h
buildinfo.h
Block.h
checked_delete.h
checksum.h
console_functions.h
Dashboard.h
Enum.h
error_functions.h
Exception.h
ExitHandler.h
file_functions.h
initialization.h
ITimeSource.h
Logger.h
LogMessageFormatters.h
LogMessageSinks.h
MappedBlock.h
MemoryPool.h
MultiThreadedUnitTest.h
os_functions.h
os_functions.impl
Path.h
PlugInBase.h
PlugInManager.h
PosixThread.h
Process.h
proc_functions.h
Ptr.h
RangeSet.h
ReadWriteLock.h
RunLengthEncoder.h
settings.h
SingletonBase.h
Singleton.h
SingletonManager.h
StackTrace.h
static_dict.h
static_vector.h
StdOutputRedirector.h
Stream.h
string_functions.h
ThreadFifo.h
ThreadHelper.h
ThreadLock.h
ThreadSemaphore.h
Time.h
TimeStreamFormater.h
TraceUtils.h
UnitTest.h
)

set(TESTS
Arguments
Assure
AtomicCount
Auto
Block
Dashboard
Enum
Exception
_file_functions
Logger
MappedBlock
MemoryPool
MultiThreadedUnitTest
MyClass
_os_functions
Path
PlugInManager
PosixThread
_proc_functions
Ptr
ReadWriteLock
RunLengthEncoder
StackTrace
Stream
_string_functions
#ThreadFifo
ThreadLock
ThreadSemaphore
Time
UnitTest
)

if(UNIX)
    list(APPEND SOURCES signal_functions.cpp)
    list(APPEND HEADERS signal_functions.h)
    list(APPEND TESTS Itimer PrioCeilingRWLock ShellScript)
endif(UNIX)

set(EXTERNS Boost)

if(OSX)
    list(APPEND SOURCES MachOBacktrace.cpp)
    list(APPEND HEADERS MachOBacktrace.h)
    list(APPEND EXTERNS ${CARBON}) # XXX: framework reference
endif(OSX)

if(LINUX)
    list(APPEND SOURCES GlibcBacktrace.cpp)
    list(APPEND HEADERS GlibcBacktrace.h)
    list(APPEND EXTERNS GLIB2)
endif(LINUX)

if(WIN32)
    list(APPEND SOURCES Win32Backtrace.cpp SystemInfo.cpp)
    list(APPEND HEADERS Win32Backtrace.h SystemInfo.h)
    list(APPEND EXTERNS PTHREADVC PSAPI)
endif(WIN32)

ac_add_library(
    aslbase asl/base
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS}
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

ac_add_plugin(
    asltestplugin asl/testplugin
    SOURCES testPlugIn.cpp
    HEADERS testPlugIn.h
    DEPENDS aslbase
    DONT_INSTALL
)
