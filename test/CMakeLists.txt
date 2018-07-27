# copy data directory into build directory to make available to test binary
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# force gtest cache
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# build the binary
add_executable(runAllTests unit_test_main.cpp)

# link gtest and curlscript lib
target_link_libraries(runAllTests gtest gmock_main libcurlscript)

# ensure this target gets run with generic runTests target
add_test(NAME runAllTests COMMAND runAllTests)
