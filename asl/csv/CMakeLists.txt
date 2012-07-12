ac_add_library(
    aslcsv  asl/csv
    SOURCES CSVParser.cpp
    HEADERS CSVParser.h CSVParser.h asl_csv_settings.h
    EXTERNS Boost
    DEPENDS aslbase aslzip
    TESTS   CSVParser
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

