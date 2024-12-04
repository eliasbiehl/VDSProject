project(VDSProject_Betreuer_test CXX C)
cmake_minimum_required(VERSION 3.10)


add_executable(VDSProject_Betreuer_test main_test_PL.cpp)
target_link_libraries(VDSProject_Betreuer_test Manager)
target_link_libraries(VDSProject_Betreuer_test gtest gtest_main pthread)


