# ScriptCompiler

Architecture for compiling and interpreting/running assembler-like scripts.

## Script language features
The language is based on Intel asm but with a number of (presumably) user-friendly features.
The primary target was to create an interpretable assembler-like script language, but this limited the number of 
system specific features available to use in the script. To solve the issue, a way of interaction with the standart library,
consisting of importing standard functions (such as `printf`) and of an interface to call them via Intel x86-64 asm were developed.

A variety of types and basic functions to work with them was also implemented. 
The user has access to a variety of registers, a stack and memory to create variables.
As an attempt to widen the features of the script language, the ability to write and use fully-fledged classes was also added.
Consequently a static type checking system was developed.

## Compilation overview
In order to follow the path C-like code is compiled, the script files are compiled into an object file, contatining no symbols, 
but still with type information to ease class methods calling (not passing pointer through rcx, as the interpreter has a lot more error chasing features compared to bare asm).

## Script execution
To execute the script you can chose to interpret it or to run through JIT compilation directly on the CPU.
Interpreting keeps a lot of information while running and helps with debugging, while JIT compilation greatly improves speed.

## JIT compiler
The development of the JIT compiler was heavily based on [Intel's IA32-64 manual](https://software.intel.com/en-us/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4).
The JIT compiler consists of 3 parts: the external API built on templates to free the user of any type management headache, the translator, 
which builds ready to run code for the CPU, and a running module to keep track of exceptions and signals.
For not yet implemented functions the interpreter is called from CPU code.

## Error management 
Error management can be quite tricky, as numerous layers of classes are running at almost any moment, from parsing to running the script. 
To make debugging easier for the user, a C++-exceptions based cascade error management system was developed. Each layer of code presumes
that any called function can crash, and if so happens, an exception of predefined type is created, contatining information only 
about what happened in the current function and a pointer to a deeper level exception that was thrown, if the error happened deeper in the call hierarchy. 
This way, after printing out the chain of exceptions, a complete stack trace is shown, with comments at each level.

[Presentation](https://www.dropbox.com/s/k72qhwevmqwre52/JIT.pdf?dl=0)
