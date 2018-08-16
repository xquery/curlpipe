------------
Build & Test
------------
> cmake -DCMAKE_BUILD_TYPE=Debug
> make
> make test
> make install

Note that running cmake will pull down most code dependencies, requiring access to internet.

Some code dependencies (eg. openssl) should already be existing on the platform before attempting
to build.

---------------
Release Package
---------------
1) edit CMakeLists.txt setting DISABLE_TESTS=true

2)  >cd cmake-build-release

3) > cpack --config CPackConfig.cmake