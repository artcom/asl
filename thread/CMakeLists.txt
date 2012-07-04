
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
)

install(
    DIRECTORY threadpool
    DESTINATION include/asl/thread/
    FILES_MATCHING PATTERN "*.hpp"
)
