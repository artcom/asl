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
# Add-macro for libraries.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

# Define a library in the current ACMake project.
#
# ac_add_library(
#    name path
#    HEADERS headerfile ...
#    [ SOURCES sourcefile ... ]
#    [ DEPENDS integrated_library ... ]
#    [ EXTERNS imported_package ... ]
#    [ TESTS   testname ... ]
#    [ HEADER_ONLY ]
#    [ DONT_INSTALL ]
# )
#
# The NAME is used in project exports and for the target name.
#
# The PATH defines where includes are installed within the installed include
# tree, which should be the path that will be used for actually including
# the files in a source file (i.e. "#include <foo/bar/bla.h>" => "foo/bar").
#
# HEADERS is a list of all public header files for the library.
#
# Unless the library is header-only, SOURCES must be a list of source files.
#
# DEPENDS can be used to reference libraries that are defined
# within this project or another cmake project.
#
# EXTERNS can be used to reference libraries that are defined
# outside the project (i.e. by a find package or pkg-config).
#
# TESTS is a list of tests using a certain old A+C naming scheme.
#
# The flag HEADER_ONLY must be given for guess what.
#
# DONT_INSTALL can be used to prevent installation of this library.
# Libraries used only for testing should get this flag.
#
macro(ac_add_library LIBRARY_NAME LIBRARY_PATH)
    # put arguments into the THIS_LIBRARY namespace
    parse_arguments(THIS_LIBRARY
        "SOURCES;HEADERS;DEPENDS;EXTERNS;TESTS"
        "HEADER_ONLY;DONT_INSTALL"
        ${ARGN})
    
    # do the same manually for name and path
    set(THIS_LIBRARY_NAME "${LIBRARY_NAME}")
    set(THIS_LIBRARY_PATH "${LIBRARY_PATH}")

    # generate a header describing our binary, source and install locations
    set(THIS_LIBRARY_PATHS_TEMPLATE ${ACMAKE_TEMPLATES_DIR}/AcPaths.h.in)
    ac_configure_file(
         ${THIS_LIBRARY_PATHS_TEMPLATE}
         ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/acmake/${THIS_LIBRARY_NAME}_paths.h
         "ac_add_library()"
    )
    
    # create src symlink in binary dir
    _ac_create_source_symlinks()

    if(THIS_LIBRARY_HEADER_ONLY)
        # for a header-only library
        
        if(NOT THIS_LIBRARY_DONT_INSTALL)
            # if we should be installed

            # define installation
            install(
                FILES ${THIS_LIBRARY_HEADERS}
                    DESTINATION include/${THIS_LIBRARY_PATH}
            )
        endif(NOT THIS_LIBRARY_DONT_INSTALL)
        
    else(THIS_LIBRARY_HEADER_ONLY)
        # for a full library
        
        # figure out file name for build info
        _ac_buildinfo_filename("${THIS_LIBRARY_NAME}" THIS_LIBRARY_BUILDINFO_FILE)

        # define the library target
        add_library(
            ${THIS_LIBRARY_NAME} SHARED
                ${THIS_LIBRARY_SOURCES}
                ${THIS_LIBRARY_HEADERS}
                ${THIS_LIBRARY_BUILDINFO_FILE}
                ${CMAKE_CURRENT_SOURCE_DIR}/.svn/entries # XXX: really needed? consistent for all target types?
        )

        # add global include and library paths
        _ac_add_global_paths(${THIS_LIBRARY_NAME})

        # declare include and library searchpath
        _ac_add_dependency_paths(
            ${THIS_LIBRARY_NAME}
            "${THIS_LIBRARY_DEPENDS}" "${THIS_LIBRARY_EXTERNS}"
        )

        # add path to generated headers (XXX: only done for libs right now)
        _ac_add_include_path(${THIS_LIBRARY_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR} NO)

        # update repository and revision information
        _ac_add_repository_info(
                ${THIS_LIBRARY_NAME} "${THIS_LIBRARY_BUILDINFO_FILE}"
                LIBRARY ${THIS_LIBRARY_SOURCES} ${THIS_LIBRARY_HEADERS}
        )

        # attach headers to target
        set_target_properties(
            ${THIS_LIBRARY_NAME} PROPERTIES
                PUBLIC_HEADER "${THIS_LIBRARY_HEADERS}"
        )

        # attach rpath configuration
        _ac_attach_rpath(${THIS_LIBRARY_NAME})
        
        # attach depends and externs
        _ac_attach_depends(${THIS_LIBRARY_NAME} "${THIS_LIBRARY_DEPENDS}" "${THIS_LIBRARY_EXTERNS}")
        
        # define installation
        if(NOT THIS_LIBRARY_DONT_INSTALL)
            install(
                TARGETS ${THIS_LIBRARY_NAME}
                    DESTINATION lib
                EXPORT  ${CMAKE_PROJECT_NAME}
                RUNTIME
                    DESTINATION bin
                LIBRARY
                    DESTINATION lib
                PUBLIC_HEADER
                    DESTINATION include/${THIS_LIBRARY_PATH}
            )
        endif(NOT THIS_LIBRARY_DONT_INSTALL)
        
    endif(THIS_LIBRARY_HEADER_ONLY)

    # create tests
    # TODO: clean up test naming
    # XXX: This is goddamn ugly.
    #      Maybe, we should replace it with a separate subsystem.
    if(ACMAKE_BUILD_TESTS)
        set(TEST_NAMESPACE "${THIS_LIBRARY_NAME}")
        foreach(TEST ${THIS_LIBRARY_TESTS})
            set(TEST_NAME "${TEST_NAMESPACE}_test${TEST}")
            
            # define the executable
            ac_add_executable(
                ${TEST_NAME}
                SOURCES test${TEST}.tst.cpp
                DEPENDS ${THIS_LIBRARY_DEPENDS} ${THIS_LIBRARY_NAME} # XXX: asl-specifics
                EXTERNS ${THIS_LIBRARY_EXTERNS}
                DONT_INSTALL
                NO_REVISION_INFO
            )
        
            # add path to generated headers from our parent library
            # XXX: maybe remove when executable has own conf header?
            _ac_add_include_path(${TEST_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR} NO)
        
            # XXX: multiple build types in VS
            get_target_property(
                TEST_LOCATION ${TEST_NAME} LOCATION_${CMAKE_BUILD_TYPE}
            )
	    
            # tell ctest about it
            add_test(${THIS_LIBRARY_NAME}_${TEST} ${TEST_LOCATION})
        endforeach(TEST)
    endif(ACMAKE_BUILD_TESTS)

    # add our target to the current project
    if(NOT THIS_LIBRARY_DONT_INSTALL)
        ac_project_add_target(
            LIBRARIES ${THIS_LIBRARY_NAME}
            EXTERNS ${THIS_LIBRARY_EXTERNS}
            DEPENDS ${THIS_LIBRARY_DEPENDS}
        )
    endif(NOT THIS_LIBRARY_DONT_INSTALL)

endmacro(ac_add_library)
