# CMake generated Testfile for 
# Source directory: /Users/kyungmin/BrankTalk
# Build directory: /Users/kyungmin/BrankTalk/build/debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test("branchtalk_desktop.disabled" "/opt/homebrew/bin/cmake" "-DBRANCHTALK_CORE_TARGET=1" "-DBRANCHTALK_SERVER_TARGET=1" "-DBRANCHTALK_DESKTOP_TARGET=0" "-P" "/Users/kyungmin/BrankTalk/tests/cmake/verify_desktop_disabled.cmake")
set_tests_properties("branchtalk_desktop.disabled" PROPERTIES  _BACKTRACE_TRIPLES "/Users/kyungmin/BrankTalk/CMakeLists.txt;87;add_test;/Users/kyungmin/BrankTalk/CMakeLists.txt;0;")
subdirs("tests")
