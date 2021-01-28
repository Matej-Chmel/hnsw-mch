# Custom HNSW implementation
This is my implementation of the Hiearchical Navigable Small World (HNSW) data structure proposed by Malkov, Yu A., and D. A. Yashunin.

Their implementation can be found at [github](https://github.com/nmslib/hnswlib).

## Requirements
- C++17 compliant compiler
- CMake 3.19
- Python 3.9.1

## Benchmarks
Graphs can be found in [benchmarks](benchmarks/) folder.

## Setup
Setting up the project is currently very cumbersome. Follow these steps to run it:
- execute [solution/generate.bat](solution/generate.bat)
	- this will download submodules and create a Visual Studio project
- open [solution/hnsw-mch.sln](solution/hnsw-mch.sln)
- in solution explorer open project **hnsw_mch**
- add a filter for header files
- add all *.h files from [module/](module/) to the filter for header files
- add all *.cpp files from [module/](module/) to the filter for source files (generated automatically)
- in solution explorer open project **native_app**
- add a filter for header files
- add all *.h files from [native_app/](native_app/) to filter for header files
- add all *.cpp files from [native_app/](native_app/) to filter for source files (generated automatically)
- build project **ALL_BUILD**
- change configuration to *Release*
- build project **ALL_BUILD**
- execute [app/create_symlinks.ps1](app/create_symlinks.ps1) with administrative privileges

## References
Malkov, Yu A., and D. A. Yashunin. "Efficient and robust approximate nearest neighbor search using Hierarchical Navigable Small World graphs." Available at [arxiv](https://arxiv.org/abs/1603.09320).
