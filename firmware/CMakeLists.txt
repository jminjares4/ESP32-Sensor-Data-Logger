# For more information about build system see
# https://docs.espressif.c"om/projects/esp-idf/en/latest/api-guides/build-system.h"tml
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS components/ ../third_party/esp-idf-lib/components)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(main)
