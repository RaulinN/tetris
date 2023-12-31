# Tetris

C implementation of a simple tetris game. It was written as an exercise (and for fun). My goals were to explore modern C and to learn the basics of clean C code in a "large" project. 

## Building

Building the game requires C99, `cmake`, and [SDL](https://www.libsdl.org/).

```bash
$ mkdir build; cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
$ ./tetris
```

You may use `-DCMAKE_BUILD_TYPE=Debug` instead if you want to print debug information.

## Disclaimer

This project was heavily inspired from [InvertedMinds video](https://www.youtube.com/watch?v=kh3rkt6nZ2c) showing the thought process behind a C++ tetris implementation. His code can be found [here](https://github.com/odyssjii/tetris).
