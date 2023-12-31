# Tetris

C implementation of a simple tetris game. It was written as an exercise (and for fun). My goals were to explore modern C and to learn the basics of clean C code in a "large" project. 

## Building

Building the game requires `cmake` and SDL.

```bash
$ mkdir build; cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
$ ./tetris
```

You may use `-DCMAKE_BUILD_TYPE=Debug` instead if you want to print debug information.
