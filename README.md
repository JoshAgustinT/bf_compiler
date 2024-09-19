This is a bf langauge compiler written by Joshua Tlatelpa-Agustin on 9/18/24.

To output x86 assembly:
* Compile the bf.cpp file with a c++ compiler, output is our bf compiler
> g++ bf.cpp
* First argument of this output will be treated as source bf program and output x86 assembly
> ./a.out file.b
* Link our new 'bf.s' assembly file
> gcc bf.s
* This is our executable file, simply run!
> ./a.out 

bf programs
* Various bf programs found in benches folder

Makefile commands
* make test - runs all programs in benches folder except input.b
* also a few other make targets which just run individual tests in benches folder

caveats
* tape limit is 100_000 bytes and we start at byte 50_000
* relatively naive implementation, room for improvement 

Refrence to spec
* https://github.com/sunjay/brainfuck/blob/master/brainfuck.md
