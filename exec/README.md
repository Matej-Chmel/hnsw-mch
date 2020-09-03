# VS Code endpoint

> Warning: When compiling with `gcc` from VS Code, this directory is expected to exist. You can change this in the [task definitions](../.vscode/tasks.json#L15). The value should be changed for each `Build` task to be consistent.

This directory should only contain executables produced without CMake. CMake executables use their own folder named `out` instead which is created automatically when the build process starts.
