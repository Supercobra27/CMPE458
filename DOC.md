# Lexer
## How to Compile and run

To build using CMAKE: build the project using the `CMakeLists.txt` file. From the `my-mini-compiler` directory, run the following commands in the terminal.
```
mkdir build
cd build
cmake ..
cmake --build .
```

Run the in the terminal by running the executable with the your input file of choice. For example:  

```
build/Debug/my-mini-compiler.exe phase1-w25/test/comments.cisc
```

You can run different tests by changing the input file from `comments.cisc` to perhaps `keywords&identifiers.cisc` or by creating your own file with file extension `.cisc`.