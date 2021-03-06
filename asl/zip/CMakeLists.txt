
set(SOURCES
# aslzip source
DirectoryPackage.cpp
PackageManager.cpp
ZipPackage.cpp
ZipReader.cpp
ZipWriter.cpp

# Info-ZIP source
ioapi.c
iowin32.c
mztools.c
unzip.c
zip.c
)

set(HEADERS
asl_zip_settings.h
# aslzip source
DirectoryPackage.h
IPackage.h
PackageManager.h
ZipPackage.h
ZipReader.h
ZipWriter.h

# Info-ZIP headers not exported
)

set(TESTS
#PackageManager
ZipFile
)

list(APPEND EXTERNALS ZLIB)

ac_add_library(
    aslzip asl/zip
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    TESTS   ${TESTS}
    DEPENDS aslbase
    EXTERNS ${EXTERNALS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)
