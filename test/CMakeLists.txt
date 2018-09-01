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

# copy data directory into build directory to make available to test binary
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# force gtest cache
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# build the binary
add_executable(runAllTests unit_test_main.cpp)

# link gtest and curlpipe lib
target_link_libraries(runAllTests gtest gmock_main libcurlpipe)

# ensure this target gets run with generic runTests target
add_test(NAME runAllTests COMMAND runAllTests)

