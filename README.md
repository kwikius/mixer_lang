A flexible actuator output mixer for ArduPlane
================================================

Intended specifically for [ArduPlane](http://plane.ardupilot.com), an actuator output mixer is 
created by reading a script file. The script uses a simple intuitive language.

As an example script, see [A very basic example mixer for an Easystar](EasyStar.mix)

The script consists of statements and a mixer function.

Statements
----------
Statements consist of expressions using Inputs, Outputs and symbols.


Inputs
------
Inputs are used to get external values. Inputs are read only.

syntax:
 ```   input{``` ***InputIdentifier*** ```}```

Inputs can have any of the types Boolean, Float or Integer. The actual type is dependent on the InputIdentifier

example:
   
``` roll = input{Roll};```

For ArduPlane

```
input{Pitch}    # float between 1.0 and -1.0 representing required pitch rate
input{Roll}     # float between 1.0 and -1.0 representing required roll rate
input{Yaw}      # float between 1.0 and -1.0 representing required yaw rate
input{Throttle} # float between 1.0 and -1.0 representing required throttle
```

Thoughts
As well as the basic inputs, some [ArduPlane parameter](http://plane.ardupilot.com/wiki/arduplane-parameters/)
can be available as a constant input. Other flight parameters, such as Airspeed, Altitude, Attitude could also be available, 
plucked straight from there latest runtime values in code.
Could also have constant inputs


Outputs
-------
Outputs represent actuators and escs using an array syntax.

syntax:

 ```   output[``` ***OutputIndex*** ```]```


OutputIndex must be of type constant integer. 
Expressions are assigned as if to an index of the array.
The output expressions are then evaluated and updated periodically.

example:

```
#  send Roll input to actuator 1
output[1] = input{Roll};

Symbols
-------
Symbols are used to hold intermediate calculation results.

syntax:
``` ***SymbolName*** = ***InitialiserExpression*** ;```

Symbols can have type integer, float or bool and are constant if they can be evaluated during the building of the mixer
or in other words if their InitialiserExpression doesnt need to evaluate non-constant inputs.
The type of the symbol is deduced from the InitialiserExpression.

```

Expressions
-----------
Expressions consists of operations on Symbols, Inputs and outputs.
An operation can only be done on objects of the same type. For example an integer multiplication by a double is disallowed.
The reason is to to keep operations as small as possible, so casting must be explicit.

Implementation
--------------

In this implementation, the expression is built *once* using an expression-tree built on the heap. 
Types are verified during building so there is no runtime casting or unboxing and no runtime
bytecode interpreter or lookup, so the resulting tree is fast to execute. 
During building of the tree, constants are folded, producing a small, fast footprint for the resulting expression.
  
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
~>$ make QUAN_ROOT=/d/user_me/lib/
```

If successful there should be a mixer_lang.exe file in the directory. Invoke:

```
~>$ ./mixer_lang.exe
```

to run it.

Note that though there are bison source and output, these are only used to check the grammar and not used in the build itself.
