find_path(OPUS_INCLUDE_DIR NAME opus/opus.h PATH_SUFFIXES include)
find_library(OPUS_LIBRARY NAMES opus)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Opus DEFAULT_MSG OPUS_LIBRARY OPUS_INCLUDE_DIR)
