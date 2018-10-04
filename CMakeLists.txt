###############################################################################
# curlpipe - https://github.com/xquery/curlpipe
###############################################################################
# Copyright (c) 2017-2018 James Fuller <jim.fuller@webcomposite.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
###############################################################################

cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules;${CMAKE_MODULE_PATH}")
include(cmake-modules/DownloadProject.cmake)

project(curlpipe CXX)
set(CMAKE_CXX_STANDARD 14)

set(CURLPIPE_VERSION_MAJOR 0)
set(CURLPIPE_VERSION_MINOR 1)
set(CURLPIPE_VERSION_PATCH 0)

# build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic -O3 -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++ -lpthread -ldl")

#
set(CURLPIPE_DISABLE_TESTS false)
set(CURLPIPE_BUILD_LIB true)
set(CURLPIPE_BUILD_TOOL true)
set(USE_RAPIDJSON true)
set(USE_PUGIXML true)

# retrieve deps
#add_subdirectory(deps)

download_project(PROJ                curl
        GIT_REPOSITORY      https://github.com/curl/curl.git
        GIT_TAG             07f09093ac32c94b759bff402fecc8f08eed76ae
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )
download_project(PROJ                googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )
download_project(PROJ                rapidjson
        GIT_REPOSITORY      https://github.com/miloyip/rapidjson.git
        GIT_TAG             master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )
download_project(PROJ                pugixml
        GIT_REPOSITORY      https://github.com/zeux/pugixml.git
        GIT_TAG             master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )
download_project(PROJ                loguru
        GIT_REPOSITORY      https://github.com/emilk/loguru.git
        GIT_TAG              v1.8.0
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )
download_project(PROJ                cxxopts
        GIT_REPOSITORY      https://github.com/jarro2783/cxxopts.git
        GIT_TAG             master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )

# include header deps here
include_directories(${cxxopts_SOURCE_DIR}/include)
include_directories(${rapidjson_SOURCE_DIR}/include)
include_directories(${curl_SOURCE_DIR}/include)
include_directories(${loguru_SOURCE_DIR})
include_directories(${pugixml_SOURCE_DIR}/src)

# add deps
add_subdirectory(${pugixml_SOURCE_DIR})
add_subdirectory(${curl_SOURCE_DIR})

if(NOT CURLPIPE_DISABLE_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    add_subdirectory(test)
endif()

# build targets
if(CURLPIPE_BUILD_LIB)
    add_subdirectory(lib)
endif()

if(CURLPIPE_BUILD_TOOL)
    add_subdirectory(src)
endif()

if (WIN32)
    set(CPACK_SOURCE_GENERATOR "ZIP")
elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set(CPACK_GENERATOR "productbuild;STGZ;TGZ")
else ()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_SOURCE_GENERATOR "TGZ")
endif ()

# define package
# must set DISABLE_TESTS=true to generate package
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "curlpipe")
set(CPACK_PACKAGE_VENDOR "James Fuller")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_PACKAGE_VERSION "${CURLPIPE_VERSION_MAJOR}.${CURLPIPE_VERSION_MINOR}.${CURLPIPE_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${CURLPIPE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CURLPIPE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CURLPIPE_VERSION_PATCH}")

set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/build/;${CMAKE_SOURCE_DIR}/.git/")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
include(CPack)
