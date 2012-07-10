
set(SOURCES
)

set(HEADERS
asl_thread_settings.h
concurrent_queue.h
completion.h
threadpool.hpp
)

set(TESTS
_concurrent_queue
_completion
)

find_package(Boost COMPONENTS thread)

set(EXTERNS Boost)

ac_add_library(
    aslthread asl/thread
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS}
    TESTS   ${TESTS}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

install(
    DIRECTORY threadpool
    DESTINATION include/asl/thread/
    COMPONENT ${DEVELOPMENT_COMPONENT}
    FILES_MATCHING PATTERN "*.hpp"
)
