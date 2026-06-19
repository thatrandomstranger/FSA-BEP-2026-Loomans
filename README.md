# mCRL22PLC
A tool for generating out a controller from an mCRL2 model. Usage:
```
mcrl22plc model.mcrl2 config.yaml > controller.scl
```

## Build instructions
Building this tool requires:
- A working `c++20` compiler and `cmake` setup.
- An installed mCRL2 toolchain, with the headers available.
- The `Boost` and `YAML-cpp` libraries.
It can then be build like any other cmake project.
