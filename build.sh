!/usr/bin/bash
 . /opt/intel/oneapi/setvars.sh --include-intel-llvm
cmake -S . -B ./build
cmake --build ./build

