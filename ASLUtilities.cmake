
macro(asl_add_schema XSD)
    parse_arguments(
        THIS_XSD
        "CXX_FILE;CXX_NAMESPACE;CXX_VARNAME;CXX_INCLUDE"
        ""
        ${ARGN}
    )

    set(THIS_XSD_FILE ${XSD})

    if(THIS_XSD_CXX_FILE)
        get_target_property(
            TEXT2SRC asl-text2src LOCATION
        )
        add_custom_command(
            OUTPUT  ${THIS_XSD_CXX_FILE}
            DEPENDS ${THIS_XSD_FILE} asl-text2src
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND 
                ${TEXT2SRC}
                --inputfile  "${CMAKE_CURRENT_SOURCE_DIR}/${THIS_XSD_FILE}"
                --outputfile "${CMAKE_CURRENT_BINARY_DIR}/${THIS_XSD_CXX_FILE}"
                --include    "${CMAKE_CURRENT_SOURCE_DIR}/${THIS_XSD_CXX_INCLUDE}"
                --namespace  ${THIS_XSD_CXX_NAMESPACE}
                --varname    ${THIS_XSD_CXX_VARNAME}
        )
    endif(THIS_XSD_CXX_FILE)

endmacro(asl_add_schema)
