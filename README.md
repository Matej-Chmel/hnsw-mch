# Custom HNSW implementation

This is my implementation of the Hiearchical Navigable Small World (HNSW) data structure proposed by Malkov, Yu A., and D. A. Yashunin.

Their implementation can be found at [github](https://github.com/nmslib/hnswlib).

## Supported platforms
- Linux
- Windows

The source uses preprocessor to build cross-platform code.

## Workspace

- Supports Visual Studio Code with [C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) and [tasks](.vscode/tasks.json).
- Supports CMake. Tested in Visual Studio 2019.

## References

Malkov, Yu A., and D. A. Yashunin. "Efficient and robust approximate nearest neighbor search using Hierarchical Navigable Small World graphs." Available at [arxiv](https://arxiv.org/abs/1603.09320).
