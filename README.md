# bf-interpreter
A BF interpreter written in C++.

The interpreter requires a file to run. Run the following command in a shell to run your BF program:
```
./BFInterpreter.exe <source>
```

For example,

```
./BFInterpreter.exe bfsource.bf
```

This interpreter is based on the following specifications: https://github.com/brain-lang/brainfuck/blob/master/brainfuck.md

Some important things to keep in mind about my interpreter implementation:
- The program tape is made up of cells each consisting of unsigned 8 bit integer values (unsigned char)
- Attempting to move left on the program tape when the pointer is at cell 0 will quit the program and display an error message
- Attempting to move right on the program tape when the program tape has reached its maximum size will quit program and display an error message (max size depends on system, but is based on the max size of an STL C++ vector)
- Incrementing a cell when at its max value will cause it to overflow and wrap back down to 0
- Decrementing a cell when at its min value will cause it to underflow and wrap back up to its max value
- The input instruction (',') is designed to take a single byte of input from the console, and the input will be read off the keyboard buffer after the user presses the return key
- If mutliple bytes of input are entered, only the first character is taken and the keyboard buffer is cleared of remaining bytes
