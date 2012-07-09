if(WIN32)

    find_path(OPENEXR_INCLUDE_DIR ImfRgbaFile.h PATH_SUFFIXES OpenEXR)

    set(OPENEXR_DEFINITIONS OPENEXR_DLL)

    set(OPENEXR_INCLUDE_DIRS ${OPENEXR_INCLUDE_DIR})

    mark_as_advanced(OPENEXR_DEFINITIONS OPENEXR_INCLUDE_DIR OPENEXR_INCLUDE_DIRS)

    set(OPENEXR_SUBLIBS Half Iex IlmImf IlmThread Imath )

    set(OPENEXR_LIBRARIES)
    set(OPENEXR_LIBRARIES_D)
    foreach(SUBLIB ${OPENEXR_SUBLIBS})

       find_library(OPENEXR_SUBLIB_${SUBLIB} NAMES ${SUBLIB})
       if(OPENEXR_SUBLIB_${SUBLIB}-NOTFOUND)
           set(OPENEXR-NOTFOUND TRUE)
           break()
       else(OPENEXR_SUBLIB_${SUBLIB}-NOTFOUND)
           list(APPEND OPENEXR_LIBRARIES ${OPENEXR_SUBLIB_${SUBLIB}})
       endif(OPENEXR_SUBLIB_${SUBLIB}-NOTFOUND)
       
       find_library(OPENEXR_SUBLIB_${SUBLIB}_D NAMES ${SUBLIB}d) 
       if(OPENEXR_SUBLIB_${SUBLIB}_D-NOTFOUND)
           set(OPENEXR-NOTFOUND TRUE)
           break()
       else(OPENEXR_SUBLIB_${SUBLIB}_D-NOTFOUND )
           list(APPEND OPENEXR_LIBRARIES_D ${OPENEXR_SUBLIB_${SUBLIB}_D})
       endif(OPENEXR_SUBLIB_${SUBLIB}_D-NOTFOUND)

       mark_as_advanced(OPENEXR_SUBLIB_${SUBLIB} OPENEXR_SUBLIB_${SUBLIB}_D)
    endforeach(SUBLIB ${OPENEXR_ALL_LIBRARIES})

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(
        OPENEXR DEFAULT_MSG
        OPENEXR_LIBRARIES OPENEXR_INCLUDE_DIR
    )

else(WIN32)
    pkg_search_module(OPENEXR REQUIRED OpenEXR)
endif(WIN32)
