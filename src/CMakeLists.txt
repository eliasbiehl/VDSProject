add_subdirectory(test)

add_library(Manager Manager.cpp)
target_include_directories(Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
