
add_subdirectory(text2src)
add_subdirectory(CheckSchema)
add_subdirectory(namesearch)
add_subdirectory(greplace)

cpack_add_component_group( Utilities
                           DESCRIPTION "various utilities"
                           EXPANDED)