A flexible actuator output mixer for ArduPlane
================================================

Intended specifically for [ArduPlane](http://plane.ardupilot.com), an actuator output mixer is 
created by reading a script file. The script uses a simple intuitive language. Currently the mixer
works as a sim on a Linux PC, using an FrSky Taranis as Joystick input.

As an example script, see [A very basic example mixer for an Easystar](EasyStar.mix)

For a more complex example see [A mixer for a glider with differential aileron, variable camber and crow flaps](glider.mix)

The script consists of statements and a mixer function. example (to keep it simple, for a rudder only glider)

(N.B defining valid input and output ranges is TODO)


``` 
x = 0.5;

mixer()
{
   output[0] = input{Roll} * x;
   
}
```

Comments
--------
comments start with a # and continue to the end of the line

syntax:

``` # this is a comment ```

Statements
----------
Statements consist of expressions using Inputs, Outputs Volatiles and symbols.


Inputs
------
Inputs are used to get external values. Inputs are read only.

syntax:

```my_value  = input{``` ***InputIdentifier*** ```};```

Inputs can have any of the types Boolean, Float or Integer. The actual type is dependent on the InputIdentifier

example:
   
``` roll = input{Roll};```

For ArduPlane, the available inputs are as follows ( See [main.cpp](main.cpp#L31) for how this is implemented and can be changed)
(Very preliminary)

```
input{Pitch}    # float between 1.0 and -1.0 representing required pitch rate
input{Roll}     # float between 1.0 and -1.0 representing required roll rate
input{Yaw}      # float between 1.0 and -1.0 representing required yaw rate
input{Throttle} # float between 1.0 and -1.0 representing required throttle
```

Volatiles
---------

(Status: TODO)
Volatiles are variables that can be externally modified via hidden means or 'via the back door' while the mixer is running. 
The ***VolatileIdentifier*** is entirely up to the script author.
They can be used to tune a gain for example. Using a volatile, one could send a message from the GCS to increment a gain 
a little while  the mixer is running.

The type of the volatile is deduced as with normal symbols from the type of its ***ConstInitialiserExpression***.
The volatiles value is initialised with the initialiser expression **which expression must be a constant**.
The current value can be read just like other symbols.

syntax:

```volatile{``` ***VolatileIdentifier*** ```} = ``` ***ConstInitialiserExpression***  ``` ; ```

```roll_gain = volatile{RollGain};```

Functions
---------

(Other functions: TODO)

   min
   ===
   T min( T a, T b)  # requires a is same type as a and type is not bool.
   If a is less than b return a else returns b

   syntax:

   ```x = min(1,2); ``` # x = 1

   max
   ===
   T max( T A, T B)  # requires a is same type as b and type is not bool.
   If a is greater than b return a else returns b 

   syntax:

   ``` x = max(1.0,2.0); ```  # x = 2.0

   if
   ==
   T if(bool cond, T v_true, T v_false) # requires cond is type bool, v_true is same type as v_false.
   If the condition is true return v_true else return v_false

   syntax:

   ``` x = if(v1 < v1,v3,v4);```

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
Expressions consists of operations on Symbols, Inputs, Volatiles and Outputs.
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


