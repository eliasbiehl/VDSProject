project(VDSProject_test CXX C)
cmake_minimum_required(VERSION 3.10)


add_executable(VDSProject_test main_test.cpp)
target_include_directories(VDSProject_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VDSProject_test Manager)
target_link_libraries(VDSProject_test gtest gtest_main pthread)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})