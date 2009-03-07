# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
# Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
#
# This file is part of the ART+COM CMake Library (acmake).
#
# It is distributed under the Boost Software License, Version 1.0. 
# (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)             
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
#
# Macro to declare an executable within the current acmake project.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

macro(ac_add_executable EXECUTABLE_NAME)
    # put arguments into the THIS_EXECUTABLE namespace
    parse_arguments(THIS_EXECUTABLE
        "SOURCES;HEADERS;DEPENDS;EXTERNS;OSX_BUNDLE_RESOURCES"
        "DONT_INSTALL;NO_REVISION_INFO;OSX_BUNDLE"
        ${ARGN})

    # bring name into our namespace
    set(THIS_EXECUTABLE_NAME "${EXECUTABLE_NAME}")
    
    # find out how the build info file will be called
    if(NOT THIS_EXECUTABLE_NO_REVISION_INFO)
        _ac_buildinfo_filename(${THIS_EXECUTABLE_NAME} THIS_EXECUTABLE_BUILDINFO_FILE)
    endif(NOT THIS_EXECUTABLE_NO_REVISION_INFO)

    # gather bundle resources
    if(THIS_EXECUTABLE_OSX_BUNDLE)
        ac_get_all_resources(THIS_EXECUTABLE_RESOURCES
                ${THIS_EXECUTABLE_OSX_BUNDLE_RESOURCES})
    endif(THIS_EXECUTABLE_OSX_BUNDLE)

    # create src symlink in binary dir
    _ac_create_source_symlinks()

    # define the target
    add_executable(
        ${THIS_EXECUTABLE_NAME}
            ${THIS_EXECUTABLE_SOURCES}
            ${THIS_EXECUTABLE_HEADERS}
            ${THIS_EXECUTABLE_RESOURCES}
            ${THIS_EXECUTABLE_BUILDINFO_FILE}
    )

    # add global include and library paths
    _ac_add_global_paths(${THIS_EXECUTABLE_NAME})

    # add paths for external headers and libs
    _ac_add_dependency_paths(
        ${THIS_EXECUTABLE_NAME}
        "${THIS_EXECUTABLE_DEPENDS}" "${THIS_EXECUTABLE_EXTERNS}"
    )

    # set up bundle resources
    if(THIS_EXECUTABLE_OSX_BUNDLE)
        _ac_attach_resources(${THIS_EXECUTABLE_OSX_BUNDLE_RESOURCES})
    endif(THIS_EXECUTABLE_OSX_BUNDLE)

    # update build information
    if(NOT THIS_EXECUTABLE_NO_REVISION_INFO)
        _ac_add_repository_info(
            ${THIS_EXECUTABLE_NAME} "${THIS_EXECUTABLE_BUILDINFO_FILE}"
            EXECUTABLE ${THIS_EXECUTABLE_SOURCES} ${THIS_EXECUTABLE_HEADERS}
        )
    endif(NOT THIS_EXECUTABLE_NO_REVISION_INFO)

    # attach rpath configuration
    _ac_attach_rpath(${THIS_EXECUTABLE_NAME})

    # attach depends and externs
    _ac_attach_depends(${THIS_EXECUTABLE_NAME} "${THIS_EXECUTABLE_DEPENDS}" "${THIS_EXECUTABLE_EXTERNS}")

    # configure target as bundle
    if(THIS_EXECUTABLE_OSX_BUNDLE)
        set_target_properties(
            ${THIS_EXECUTABLE_NAME}
                PROPERTIES
                    MACOSX_BUNDLE ON
        )
    endif(THIS_EXECUTABLE_OSX_BUNDLE)

    # define installation
    if(NOT THIS_EXECUTABLE_DONT_INSTALL)
        install(
            TARGETS ${THIS_EXECUTABLE_NAME}
            EXPORT ${CMAKE_PROJECT_NAME}
            RUNTIME DESTINATION bin
            BUNDLE  DESTINATION Applications
        )
    endif(NOT THIS_EXECUTABLE_DONT_INSTALL)
    
    # add our target to the current project
    if(NOT THIS_EXECUTABLE_DONT_INSTALL)
        ac_project_add_target(
            EXECUTABLES ${THIS_EXECUTABLE_NAME}
            EXTERNS     ${THIS_EXECUTABLE_EXTERNS}
            DEPENDS     ${THIS_EXECUTABLE_DEPENDS}
        )
    endif(NOT THIS_EXECUTABLE_DONT_INSTALL)
    
endmacro(ac_add_executable)
