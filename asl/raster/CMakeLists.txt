set(HEADERS
asl_raster_settings.h
contain.h
fraction.h
palgo.h
piterator.h
pixel_functions.h
pixels.h
raster.h
standard_pixel_types.h
subraster.h
)

set(TESTS
fraction
PixelRaster
Pixels
)

ac_add_library(
    aslraster asl/raster
    HEADERS ${HEADERS}
    DEPENDS aslbase
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)
