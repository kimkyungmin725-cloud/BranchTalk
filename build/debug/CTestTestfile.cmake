# CMake generated Testfile for 
# Source directory: /Users/kyungmin/BrankTalk
# Build directory: /Users/kyungmin/BrankTalk/build/debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test("branchtalk_client.smoke" "/Users/kyungmin/BrankTalk/build/debug/branchtalk_client")
set_tests_properties("branchtalk_client.smoke" PROPERTIES  PASS_REGULAR_EXPRESSION [[\[client\] application started]] _BACKTRACE_TRIPLES "/Users/kyungmin/BrankTalk/CMakeLists.txt;79;add_test;/Users/kyungmin/BrankTalk/CMakeLists.txt;0;")
add_test("branchtalk_server.smoke" "/Users/kyungmin/BrankTalk/build/debug/branchtalk_server")
set_tests_properties("branchtalk_server.smoke" PROPERTIES  PASS_REGULAR_EXPRESSION [[\[server\] application started]] _BACKTRACE_TRIPLES "/Users/kyungmin/BrankTalk/CMakeLists.txt;80;add_test;/Users/kyungmin/BrankTalk/CMakeLists.txt;0;")
add_test("branchtalk_cmake_presets.contract" "/opt/homebrew/bin/cmake" "-DBRANCHTALK_SOURCE_DIR=/Users/kyungmin/BrankTalk" "-P" "/Users/kyungmin/BrankTalk/tests/cmake/verify_presets.cmake")
set_tests_properties("branchtalk_cmake_presets.contract" PROPERTIES  _BACKTRACE_TRIPLES "/Users/kyungmin/BrankTalk/CMakeLists.txt;90;add_test;/Users/kyungmin/BrankTalk/CMakeLists.txt;0;")
add_test("branchtalk_core.dependencies" "/Users/kyungmin/BrankTalk/build/debug/branchtalk_core_dependencies_smoke")
set_tests_properties("branchtalk_core.dependencies" PROPERTIES  _BACKTRACE_TRIPLES "/Users/kyungmin/BrankTalk/CMakeLists.txt;113;add_test;/Users/kyungmin/BrankTalk/CMakeLists.txt;0;")
add_test("branchtalk_core.result" "/Users/kyungmin/BrankTalk/build/debug/branchtalk_core_result_smoke")
set_tests_properties("branchtalk_core.result" PROPERTIES  _BACKTRACE_TRIPLES "/Users/kyungmin/BrankTalk/CMakeLists.txt;133;add_test;/Users/kyungmin/BrankTalk/CMakeLists.txt;0;")
add_test("branchtalk_core.application_config" "/Users/kyungmin/BrankTalk/build/debug/branchtalk_core_application_config_smoke" "/Users/kyungmin/BrankTalk/tests/fixtures/config")
set_tests_properties("branchtalk_core.application_config" PROPERTIES  _BACKTRACE_TRIPLES "/Users/kyungmin/BrankTalk/CMakeLists.txt;153;add_test;/Users/kyungmin/BrankTalk/CMakeLists.txt;0;")
add_test("branchtalk_core.logging" "/opt/homebrew/bin/cmake" "-DBRANCHTALK_LOGGING_SMOKE=/Users/kyungmin/BrankTalk/build/debug/branchtalk_core_logging_smoke" "-P" "/Users/kyungmin/BrankTalk/tests/cmake/verify_logging.cmake")
set_tests_properties("branchtalk_core.logging" PROPERTIES  _BACKTRACE_TRIPLES "/Users/kyungmin/BrankTalk/CMakeLists.txt;175;add_test;/Users/kyungmin/BrankTalk/CMakeLists.txt;0;")
add_test("branchtalk_desktop.disabled" "/opt/homebrew/bin/cmake" "-DBRANCHTALK_CORE_TARGET=1" "-DBRANCHTALK_SERVER_TARGET=1" "-DBRANCHTALK_DESKTOP_TARGET=0" "-P" "/Users/kyungmin/BrankTalk/tests/cmake/verify_desktop_disabled.cmake")
set_tests_properties("branchtalk_desktop.disabled" PROPERTIES  _BACKTRACE_TRIPLES "/Users/kyungmin/BrankTalk/CMakeLists.txt;189;add_test;/Users/kyungmin/BrankTalk/CMakeLists.txt;0;")
