
ac_add_executable(
  text2src
  SOURCES text2src.cpp
  DEPENDS aslbase
  INSTALL_COMPONENT text2src
)
cpack_add_component( text2src
            DESCRIPTION "text2src"
            GROUP Utilities)
