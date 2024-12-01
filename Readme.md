# Advent of Code 2023
My solutions to the puzzles of [Advent of Code 2023](https://adventofcode.com/2023) (so far). I also created a small header-only library ([aoclib](aoclib/)) of utility functions and data-structures which were useful for solving the puzzles. 

The puzzles' answers (to my puzzle inputs) can be found as comments in the source-files of each day, but keep in mind each user gets different sets of puzzle inputs (and therefore different correct puzzle answers). 

My answers are all correct for my puzzle inputs and for the example inputs, but in my experience it is definitely possible to come up with *incorrect* code which may produce the *correct* answers for your own puzzle input, but does not work for all other possible/legal puzzle inputs.

## Project Structure

### [bin/](bin/)
Will contain the executables after building: 
- **bin/day-nn** will be the Release build for *day-nn*
- **bin/day-nn_dbg** will be the Debug build for *day-nn*

### [input/](input/)
The puzzle input file for *day-nn* must be saved as **input/day-nn.txt** (e.g. your puzzle input for *day-01* must be put into **input/day-01.txt**) and will be automatically read when you run `bin/day-nn` or `bin/day-nn_dbg`)

### day-nn/
Contains the source code for the puzzles of *day-nn*, e.g. [day-11](day-11)

### [aoclib/](aoclib/) 
My reusable header-only utility library for advent of code: 

- In namespace `aocio`: [aocio.hpp](aoclib/aocio.hpp) for input-output and parsing functions (the most useful probably being ``aocio::line_tokenise``)
    
- In namespace `aocutil`: [grid.hpp](aoclib/grid.hpp) for handling generic 2D grids (I even implemented custom iterators!); [vec.hpp](aoclib/vec.hpp) for 2D vector and direction operations; [hash.hpp](aoclib/hash.hpp) for a copy-pasted hash-combine function (not mine); [prio-queue.hpp](aoclib/prio-queue.hpp) and [lru-cache.hpp](aoclib/lru-cache.hpp) should be self-explanatory (and not that useful/good).

### [build/](build/)
Will contain the cmake build files:
- in [build/Release](build/Release) for the Release variant
- in [build/Debug](build/Debug) for the Debug variant


## How to build and run

### 1. Create and switch to the build directories
*Make sure you are at the [top-level directory](./) of this repository*

`mkdir -p bin`

`mkdir -p build/Release; cd build/Release` (or `mkdir -p build/Debug; cd build/Debug` for debug builds)

### 2. Configure cmake 
*You are in directory [build/Release](build/Release) (or [build/Debug](build/Debug) for debug builds) now*

`cmake -DCMAKE_BUILD_TYPE=Release ../..` (or `cmake -DCMAKE_BUILD_TYPE=Debug ../..` for debug builds)

### 3. Build 
*You are still in directory [build/Release](build/Release) (or [build/Debug](build/Debug) for debug builds)*

`cmake --build .` to build all (or `cmake --build . --target day-nn` to build only *day-nn*).

   You can also build and run in one step instead (but make sure to save your input of puzzle *day-nn* into **input/day-nn.txt** before):
   In directory [build/Release](build/Release) (or [build/Debug](build/Debug)) run `cmake --build . --target run-day-nn` to build and run *day-nn* (or `cmake --build . --target run-all` to build and run all). 

### 4. Run
   The resulting executable(s) can be found in the repository's [bin/](bin/) directory (**bin/day-nn** for Release-builds and **bin/day-nn_dbg** for Debug-builds).
   
   Make sure to save the input of puzzle *day-nn* as **input/day-nn.txt** before running the executable **bin/day-nn** (or **bin/day-nn_dbg**).