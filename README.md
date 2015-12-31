A library to build a control mixer from a script
================================================

Intended specifically for [ArduPlane](http://plane.ardupilot.com), the mixer builds an expression for each 
actuator output using the input script.

See the example1.mix file in this directory for example syntax. (Still in early stages)
See bison.y for grammar ( Again still being worked on)

Each output expression can consist of inputs, intermediate constants and variables and other outputs.

Inputs
------

For ArduPlane, for example, the usual basic inputs are Pitch, Yaw, Roll, and Throttle, though others can be defined as you wish.
As well as the basic inputs, every [ArduPlane parameter](http://plane.ardupilot.com/wiki/arduplane-parameters/)
is available as a constant input. Other flight parameters are also available if you wish to add them as inputs.

Outputs
-------

Outputs are represented as an array. Expressions are assigned to the elements of the array.
The output expressions are then evaluated and updated peridically.

Implementation
--------------

In this implementation, the expression is built *once* using an expression-tree built on the heap. 
Types are verified during building so there is no runtime casting or unboxing and no runtime
bytecode interpreter or lookup, so the resulting tree is fast to execute. 
During building of the tree, constants are folded so producing a fast,small footprint for the resulting expression.
  
Image Size
----------

During building some extra heap is used to hold symbolic names, but once the tree is built the symbol table memory is freed.
With constant folding the resulting tree is expected to be compact.
The code has only been tested on a 32 bit Linux PC so far and the complete app image is coming in at under 30K.
I hope it will be smaller when built as a library for STM32 arm. The aim is under 10K of rom space but we shall see!

Dependencies
------------

Requires my [quan](https://github.com/kwikius/quan-trunk.git) library  

Compilation/ Building
---------------------

There is a Make file included . See [Gnu Make](https://www.gnu.org/software/make).

If on Windows, I recommend installing [MinGW](http://www.mingw.org) or [Cygwin](https://www.cygwin.com).

Assuming your quan-trunk is installed in /d/user_me/lib , enter the mixer_lang source directory and invoke:

```
<$ make QUAN_ROOT=/d/user_me/lib/
```

If successful there should be a mixer_lang.exe file in the directory. Invoke:

```
<$ ./mixer_lang.exe
```

to run it.

Note that though there are bison source and output, these are only used to check the grammar and not used in the build itself.
