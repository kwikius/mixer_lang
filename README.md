A flexible actuator output mixer for ArduPlane
================================================

Intended specifically for [ArduPlane](http://plane.ardupilot.com), an actuator output mixer is 
created by reading a script file. The script uses a simple intuitive language. Currently the mixer
works as a sim on a Linux PC, using an FrSky Taranis as Joystick input.

As an example script, see [A very basic example mixer for an Easystar](EasyStar.mix)

For a more complex example see [A mixer for a glider with differential aileron, variable camber and crow flaps](glider.mix)

The script consists of statements and a mixer function.

Statements
----------
Statements consist of expressions using Inputs, Outputs and symbols.


Inputs
------
Inputs are used to get external values. Inputs are read only.

syntax:

 ***Result*** ``` = input{``` ***InputIdentifier*** ```}```

Inputs can have any of the types Boolean, Float or Integer. The actual type is dependent on the InputIdentifier

example:
   
``` roll = input{Roll};```

For ArduPlane, the available inputs are as follows ( See [main.cpp](main.cpp#L31) for how this is implemented and can be changed)

```
input{Pitch}    # float between 1.0 and -1.0 representing required pitch rate
input{Roll}     # float between 1.0 and -1.0 representing required roll rate
input{Yaw}      # float between 1.0 and -1.0 representing required yaw rate
input{Throttle} # float between 1.0 and -1.0 representing required throttle
```

Outputs
-------
Outputs represent actuators and escs using an array syntax.

syntax:

 ```   output[``` ***OutputIndex*** ```] = ``` ***Expression*** ;```


OutputIndex must be of type constant integer. 
Expressions are assigned as if to an index of the array.
The output expressions are then evaluated and updated periodically.

example:

```
#  send Roll input to actuator 1
output[1] = input{Roll};
```

Symbols
-------
Symbols are used to hold intermediate calculation results.

syntax:

***SymbolName*** ``` =  ``` ***InitialiserExpression***  ``` ; ```

Symbols can have type integer, float or bool and are constant if they can be evaluated during the building of the mixer
or in other words if their InitialiserExpression doesnt need to evaluate non-constant inputs.
The type of the symbol is deduced from the InitialiserExpression.

Expressions
-----------
Expressions consists of operations on Symbols, Inputs and outputs.
An operation can only be done on objects of the same type. For example an integer multiplication by a double is disallowed.
The reason is to to keep operations as small as possible, so casting must be explicit. (Casting TODO)

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

Running
-------

If build is successful there should be a mixer_lang.exe file in the directory. To run, Switch on your Taranis , 
then connect to your Linux PC via USB and Invoke:

```
~>$ ./mixer_lang.exe EasyStar.mix
```

to run The App. (Alternatively call the app on your own mixer script). To start and stop press any key.
When running, Twiddle the sticks and you should see some changing outputs.

Notes
-----

The Joystick is currently hardwired as "/dev/input/js0"
If your outputs are  messy, try running through the Taranis calibration routine.


