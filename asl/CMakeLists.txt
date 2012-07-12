
# Traverse our subdirectories
add_subdirectory(base)
add_subdirectory(thread)
add_subdirectory(math)
add_subdirectory(audio)
add_subdirectory(raster)
add_subdirectory(dom)
add_subdirectory(xpath)
add_subdirectory(serial)
add_subdirectory(net)
add_subdirectory(ipc)
add_subdirectory(zip)

if(Boost_VERSION GREATER 104099)
    # we need (boost-sprit >= 2.1), which is in Boost 1.41
    add_subdirectory(csv)
endif(Boost_VERSION GREATER 104099)

add_subdirectory(utilities)


