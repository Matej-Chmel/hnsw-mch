cmake_minimum_required (VERSION 3.10)
project ("hnsw_mch")

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB lib_files ${CMAKE_SOURCE_DIR}/src/lib/*.cpp ${CMAKE_SOURCE_DIR}/src/lib/util/*.cpp)
add_library(Lib ${lib_files})

add_executable(GraphBuilder src/graph_builder.cpp)
add_executable(DataAnalyzer src/data_analyzer.cpp)
add_executable(DataReader src/data_reader.cpp)

target_link_libraries(GraphBuilder PUBLIC Lib)
target_link_libraries(DataAnalyzer PUBLIC Lib)
target_link_libraries(DataReader PUBLIC Lib)
