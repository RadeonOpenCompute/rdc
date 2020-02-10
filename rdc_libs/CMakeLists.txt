# Copyright (c) 2020 - present Advanced Micro Devices, Inc. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

#
# Minimum version of cmake required
#
cmake_minimum_required(VERSION 3.5.0)

message("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&")
message("                       Cmake RDC Lib                            ")
message("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&")

## Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -std=c++11 ")
# Use this instead of above for 32 bit
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

if ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -O0 -DDEBUG")
endif ()


# Required Defines first:

message("")
message("Build Configuration:")
message("-----------BuildType: " ${CMAKE_BUILD_TYPE})
message("------------Compiler: " ${CMAKE_CXX_COMPILER})
message("-------------Version: " ${CMAKE_CXX_COMPILER_VERSION})
message("--------Proj Src Dir: " ${PROJECT_SOURCE_DIR})
message("--------Proj Bld Dir: " ${PROJECT_BINARY_DIR})
message("--------Proj Lib Dir: " ${PROJECT_BINARY_DIR}/lib)
message("--------Proj Exe Dir: " ${PROJECT_BINARY_DIR}/bin)
message("--------RSMI Lib Dir: " ${RSMI_LIB_DIR})
message("--------RSMI Inc Dir: " ${RSMI_INC_DIR})
message("")

## Set default module path if not already set
if(NOT DEFINED CMAKE_MODULE_PATH)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
endif()


set(SRC_DIR "${PROJECT_SOURCE_DIR}/rdc_libs")
set(RDC_LIB_INC_DIR "${PROJECT_SOURCE_DIR}/include")


################# Determine the library version #########################
## Setup the SO version based on git tags.
set(SO_VERSION_GIT_TAG_PREFIX "rdc_so_ver")

# provide git to utilities
find_program (GIT NAMES git)

# Debian package specific variables
# Set a default value for the package version
get_version_from_tag("1.0.0.0" ${SO_VERSION_GIT_TAG_PREFIX} GIT)

# VERSION_* variables should be set by get_version_from_tag
set(SO_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
message("SOVERSION: ${SO_VERSION_STRING}")

## Define default variable and variables for the optional build target
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
                   CACHE STRING "Location of RDC library source code.")
set(CMAKE_INSTALL_PREFIX ${RDC_LIB_ROOT_PATH}
                                CACHE STRING "Default installation directory.")
set(CPACK_PACKAGING_INSTALL_PREFIX ${RDC_LIB_ROOT_PATH}
                                      CACHE STRING "Default packaging prefix.")
set(CPACK_GENERATOR "DEB;RPM"  CACHE STRING "Default packaging generators.")

if (NOT DEFINED CPACK_PACKAGE_VENDOR)
    set(CPACK_PACKAGE_VENDOR "AMD")
endif()

if (NOT DEFINED CPACK_PACKAGE_CONTACT)
    set(CPACK_PACKAGE_CONTACT "Advanced Micro Devices Inc.")
endif()

if (NOT DEFINED CPACK_PACKAGE_DESCRIPTION_SUMMARY)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
                 "Radeon Data Center Tools")
endif()

if (NOT RDC_PACKAGE)
   set(RDC_PACKAGE RDC_lib64)
endif()

set(CPACK_PACKAGE_FILE_NAME "${RDC_PACKAGE}-${PKG_VERSION_STR}")
## Verbose output.
set(CMAKE_VERBOSE_MAKEFILE on)

# librdc_bootstrap.so set up
set(BOOTSTRAP_LIB "rdc_bootstrap")
set(BOOTSTRAP_LIB_COMPONENT "lib${BOOTSTRAP_LIB}")
set(BOOTSTRAP_LIB_SRC_LIST "${SRC_DIR}/bootstrap/src/RdcBootStrap.cc")
set(BOOTSTRAP_LIB_INC_LIST "${RDC_LIB_INC_DIR}/rdc/rdc.h")
set(BOOTSTRAP_LIB_INC_LIST ${BOOTSTRAP_LIB_INC_LIST} "${RDC_LIB_INC_DIR}/rdc_lib/rdc_common.h")
set(BOOTSTRAP_LIB_INC_LIST ${BOOTSTRAP_LIB_INC_LIST} "${RDC_LIB_INC_DIR}/rdc_lib/RdcHandler.h")

message("BOOTSTRAP_LIB_INC_LIST=${BOOTSTRAP_LIB_INC_LIST}")

add_library(${BOOTSTRAP_LIB} SHARED ${BOOTSTRAP_LIB_SRC_LIST} ${BOOTSTRAP_LIB_INC_LIST})
target_link_libraries(${BOOTSTRAP_LIB} pthread dl)
target_include_directories(${BOOTSTRAP_LIB} PRIVATE
                                         "${PROJECT_SOURCE_DIR}"
                                         "${PROJECT_SOURCE_DIR}/include"
                                         "${CMAKE_CURRENT_SOURCE_DIR}/include")

# TODO: set the properties for the library once we have one
## Set the VERSION and SOVERSION values
set_property(TARGET ${BOOTSTRAP_LIB} PROPERTY
             SOVERSION "${VERSION_MAJOR}")
set_property(TARGET ${BOOTSTRAP_LIB} PROPERTY
             VERSION "${SO_VERSION_STRING}")

# librdc.so set up
set(RDC_LIB "rdc")
set(RDC_LIB_COMPONENT "lib${RDC_LIB}")
set(RDC_LIB_SRC_LIST "${SRC_DIR}/rdc/src/RdcEmbeddedHandler.cc")
set(RDC_LIB_INC_LIST "${RDC_LIB_INC_DIR}/rdc_lib/impl/RdcEmbeddedHandler.h")
message("RDC_LIB_INC_LIST=${RDC_LIB_INC_LIST}")

link_directories(${RSMI_LIB_DIR})
add_library(${RDC_LIB} SHARED ${RDC_LIB_SRC_LIST} ${RDC_LIB_INC_LIST})
target_link_libraries(${RDC_LIB} pthread rocm_smi64)
target_include_directories(${RDC_LIB} PRIVATE
                                         "${PROJECT_SOURCE_DIR}"
                                         "${PROJECT_SOURCE_DIR}/include"
                                         "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                         "${RSMI_INC_DIR}")

# TODO: set the properties for the library once we have one
## Set the VERSION and SOVERSION values
set_property(TARGET ${RDC_LIB} PROPERTY
             SOVERSION "${VERSION_MAJOR}")
set_property(TARGET ${RDC_LIB} PROPERTY
             VERSION "${SO_VERSION_STRING}")


## If the library is a release, strip the target library
if ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
    add_custom_command(
          TARGET ${BOOTSTRAP_LIB}
          POST_BUILD COMMAND ${CMAKE_STRIP} lib${BOOTSTRAP_LIB}.so)
    add_custom_command(
          TARGET ${RDC_LIB}
          POST_BUILD COMMAND ${CMAKE_STRIP} lib${RDC_LIB}.so)
endif ()

## Add the install directives for the runtime library.
install(TARGETS ${BOOTSTRAP_LIB} ${RDC_LIB}
             LIBRARY DESTINATION ${RDC_LIB_ROOT_PATH}${RDC}/lib
                                            COMPONENT ${RDC_LIB_COMPONENT})
install(FILES ${SOURCE_DIR}/include/rdc/rdc.h
                      DESTINATION ${RDC_LIB_ROOT_PATH}${RDC}/include/rdc
                                         COMPONENT ${LIB_HEADER_COMPONENT})

message("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&")
message("                    Finished Cmake RDC Lib                      ")
message("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&")

