# ManuLab: a Manuscript Laboratory
ManuLab is universal application for loading, processing and analysis of historical manuscripts.

This README is being writen.

### General requirements

- c++11 compiler
- Cmake version 3 or later
- Qt5
- clang-format

### MacOs requirements

Install required packages first. On MacOs we recommend to use [Homebrew](https://brew.sh) for installing packages.

```
brew install llvm --with-toolchain # or use compiler provided by Xcode
brew install cmake
brew install qt
brew install clang-format
```

### TODO Linux/Windows requirements and steps to build

### Building

To build ManuLab use these steps.
```
git clone git@bitbucket.com:jugin/manulab
cd manulab
mkdir -p build
cd build
cmake ..
make -j4
```

You can now run ManuLab with `./src/manulab` command.
