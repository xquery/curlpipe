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

set(LIB_NAME libcurlpipe)

file(GLOB SRC_FILES curlpipe/*.cpp curlpipe/*.h* curlpipe/curlpipe.h)
configure_file (
        "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/curlpipe/version.h")
add_library(${LIB_NAME} STATIC ${SRC_FILES})
target_link_libraries(${LIB_NAME} -lpthread -ldl libcurl pugixml)
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories (${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${LIB_NAME} PROPERTIES PREFIX "")

install(TARGETS ${LIB_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT library)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/curlpipe/curlpipe.h DESTINATION include/curlpipe)
