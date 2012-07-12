

ac_add_executable(
    checkschema
    SOURCES
       CheckSchema.cpp
    DEPENDS aslbase asldom
    INSTALL_COMPONENT checkschema
)
cpack_add_component( checkschema
            DESCRIPTION "checkschema"
            GROUP Utilities)