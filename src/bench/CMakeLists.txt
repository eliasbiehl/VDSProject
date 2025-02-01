add_library(Benchmark
        BenchParser.cpp
        BenchmarkLib.cpp
        CircuitToBDD.cpp
        bench_grammar.hpp
        BenchParser.hpp
        skip_parser.hpp)

#Boost
find_package(Boost)

target_include_directories(Benchmark PRIVATE ${CMAKE_SOURCE_DIR}/lib/tqdm)

#Executable
add_executable(VDSProject_bench main_bench.cpp)
target_link_libraries(VDSProject_bench Manager)
target_link_libraries(VDSProject_bench Benchmark)
target_link_libraries(VDSProject_bench ${Boost_LIBRARIES})


