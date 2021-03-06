cmake_minimum_required(VERSION 3.19)
project("hnsw-mch")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(EXTERN "${PROJECT_SOURCE_DIR}/extern")
set(MODULE "${PROJECT_SOURCE_DIR}/module")
set(NATIVE_APP "${PROJECT_SOURCE_DIR}/native_app")

set(BINDINGS "${MODULE}/bindings.cpp")

file(GLOB NativeSources "${MODULE}/*.cpp")
list(REMOVE_ITEM NativeSources ${BINDINGS})

add_subdirectory(${EXTERN}/pybind11)

pybind11_add_module(
	hnsw_mch
	${BINDINGS}
)

target_include_directories(
	hnsw_mch
	PRIVATE ${MODULE}
)

add_executable(
	native_app
	${NATIVE_APP}/main.cpp
)

target_include_directories(
	native_app
	PRIVATE ${NATIVE_APP}
	PRIVATE ${MODULE}
)

add_library(NativeLibrary ${NativeSources})

target_link_libraries(
	native_app
	PRIVATE NativeLibrary
)
