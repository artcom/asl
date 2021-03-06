enable_testing()

cmake_minimum_required(VERSION 2.6.2)

project(ASL)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 9)
set(VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

list(INSERT CMAKE_PREFIX_PATH 0 ${CMAKE_BINARY_DIR})

find_package(AcMake)

set(REQ_PACKAGES ZLIB GLIB2 CURL Boost)
if(WIN32)
    list(APPEND REQ_PACKAGES PTHREADVC DirectX WINSOCK Winmm Psapi)
endif(WIN32)

set(REQ_PKGCONFIG)
if(LINUX)
    list(APPEND REQ_PKGCONFIG ALSA alsa)
endif(LINUX)

# XXX: framework handling
if(OSX)
    find_library(CARBON     Carbon)
    find_library(AUDIO_UNIT AudioUnit)
    mark_as_advanced(CARBON)
    mark_as_advanced(AUDIO_UNIT)
endif(OSX)

# declare our packaging solution
ac_begin_solution(
    ASL
    VERSION_MAJOR ${VERSION_MAJOR} VERSION_MINOR ${VERSION_MINOR} VERSION_PATCH ${VERSION_PATCH}
    DESCRIPTION  "ASL ART+COM Standard Library"
    VENDOR       "ART+COM AG"
    CONTACT      "info@artcom.de"
    DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/DESCRIPTION.txt"
    LICENSE_FILE     "${CMAKE_SOURCE_DIR}/LICENSE.txt"
    PACKAGE_ICON     "${CMAKE_SOURCE_DIR}/installer/windows/NSIS-Header.bmp"
    INSTALLER_ICON   "${CMAKE_SOURCE_DIR}/installer/windows/shopping_bag.ico"
    UNINSTALLER_ICON "${CMAKE_SOURCE_DIR}/installer/windows/shopping_bag_grey.ico"
)

ac_add_project(
    ASL
    DISPLAY_NAME "ASL"
    VENDOR "ART+COM AG"
    CONTACT "info@artcom.de"
    DESCRIPTION "ART+COM AG Standard Library"
    REQUIRED_PACKAGES
        ${REQ_PACKAGES}
    REQUIRED_PKGCONFIG
        ${REQ_PKGCONFIG}
    CUSTOM_SCRIPTS
        ASLUtilities.cmake
)

# declare cpack components
set(RUNTIME_COMPONENT ASL_runtime)
set(DEVELOPMENT_COMPONENT ASL_development)

cpack_add_component( ${RUNTIME_COMPONENT}
            DISPLAY_NAME "Runtime"
            DESCRIPTION "runtime libraries" REQUIRED)
cpack_add_component( ${DEVELOPMENT_COMPONENT}
            DISPLAY_NAME "Development"
            DESCRIPTION "header files and import libraries")


# Make sure we can access our includes without staging
include_directories(. ${CMAKE_CURRENT_BINARY_DIR})
ac_project_add_build_include_dir(${CMAKE_CURRENT_BINARY_DIR})
ac_project_add_build_include_dir(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(asl)

ac_add_doxygen(ASL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen Doxyfile.in)

ac_end_project(ASL)
ac_end_solution(ASL)

