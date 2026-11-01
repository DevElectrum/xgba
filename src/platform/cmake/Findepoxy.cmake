# Locate libepoxy in an ordinary system prefix or a vcpkg installation.
find_path(epoxy_INCLUDE_DIRS NAMES epoxy/gl.h)
find_library(epoxy_LIBRARIES NAMES epoxy libepoxy)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(epoxy DEFAULT_MSG epoxy_INCLUDE_DIRS epoxy_LIBRARIES)
