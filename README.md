A flexible actuator output mixer for Flight controllers
=======================================================

Intended to replace hard coded mixers, an actuator output mixer is 
specified using a script file. The script uses a simple intuitive language.
The resulting mixer is fast to execute, compact and expressive.
Currently the mixer works as a sim on a Linux PC, using an FrSky Taranis as Joystick input.
A gui demo is also in progress.

[Compiling building Running](#compilation-building-running)

Language Features
-----------------
   
   *  High level Specification.

      The specification is based on a context free grammar and well documented semantics. 
      The goal is consistent semantics, so that you arent reliant on the vagaries of a particular implementation.

   *  Strongly typed, type deduction.

      Expressive economical language with type deduction, but strongly typed to give well defined and predictable qualities

   *  Access to system values.

      The language enables defining system values either constant or variable and of real, integer or bool type as inputs to the mixer.

   *  Inputs and Outputs as Integers, floats or booleans. 

      Inputs and Outputs can be comprised of combinations of integer real and boolean.
      You can select the underlying integer and floating point types to the best combination for
      space and performance on your system.

   *  Tunable at runtime.

      The language specifies how selected values in the mixer can be remotely modified in flight to tune best performance in real time.

   *  Multipoint mixers specified in the language

Implementation features
-----------------------

   *  Compiled to an expression tree with typed computations created during the build for maximum performance
   *  Optimising compiler. Constants are folded during the construction for small code size and higher performance.
   *  Can use integer or float calculations or both


Acknowledgments
---------------

  Thanks to @crashmatt for his input so far to the language, particularly with regard to the volatile varaiables.

   
Introduction to the language
----------------------------

The script specifying the mixer consists of statements and a mixer function. For example (to keep it simple), for a rudder only glider.

``` 
x = 0.5;

mixer()
{
   output[0] = input{Roll} * x;
}
```

For other example scripts, see [A very basic example mixer for an Easystar](https://github.com/kwikius/mixer_lang/blob/master/mixers/generic/EasyStar.mix)

For a more complex example see [A mixer for a glider with differential aileron, variable camber and crow flaps](https://github.com/kwikius/mixer_lang/blob/master/mixers/generic/glider.mix)

Below is a non technical description of the language. For a more technical description, consult the 
[grammar](https://github.com/kwikius/mixer_lang/blob/master/grammar/bison.grammar). 
The grammar can be parsed using [GNU Bison](https://www.gnu.org/software/bison/).


-----------------------------
-----------------------------


A basic description of the language
===================================


Comments
--------
comments start with a # and continue to the end of the line

syntax:

``` # this is a comment ```

Statements
----------
Statements consist of expressions using Inputs, Outputs and symbols.

syntax:

``` 
   myint = 1 * 2 * 3 * 4; # myint is deduced as type integer
   pi = 3.14159; # pi is a float
   myfloat = 2.0 * pi; # myfloat is deduced as float because its initialiser expression has type float
   mybool = true; #// deduced as a boolean type
```

Symbols
-------
Symbols are used to hold intermediate calculation results.
Symbols are folded into constants as they are defined if possible.
(Constant folding can be switched off for debugging if required)

syntax:

***SymbolName*** ``` =  ``` ***InitialiserExpression***  ``` ; ```

```
   myint = 1 * 2 * 3 * 4; # myint is deduced as type integer with value 24 
   # Note: (Symbols are folded to constants as they are defined if their 
   # InitialiserExpression resolves to a constant, so myint is a constant)
```

Symbols can have type integer, float or bool and are constant if they can be evaluated during the building of the mixer
or in other words if their InitialiserExpression doesnt need to evaluate non-constant inputs.
The type of the symbol is deduced from the InitialiserExpression.

Volatile Symbols
---------

(Status: TODO)
Volatile Symbols are Symbols that can be externally modified via hidden means or 'via the back door' while the mixer is running. 
Volatile symbols can be used to tune a gain for example. Using a volatile, one could send a message from the GCS to increment a gain 
a little while  the mixer is running.

To create a Volatile symbol just add the ```volatile``` modifier in front of the symbol at the point of definition.

The type of the volatile is deduced as with normal symbols from the type of its ***ConstInitialiserExpression***.
The volatiles value is initialised with the initialiser expression **which expression must be a constant**.
The volatile symbol will not be constant however so constant folding will not be done on a volatile symbol.
In other aspects the symbol is just like other symbols.

Note: There will also be an option to not allow volatile symbols in a script 

syntax:

```volatile ``` ***SymbolName*** ``` = ``` ***ConstInitialiserExpression***  ``` ; ```

Inputs
------
Inputs are used to get external values. Inputs are read only. They can be indexed by identifiers or numbers.
Inputs accesed by name are generic inputs

syntax:

```my_value  = input{``` ***InputIdentifier*** ```};```

```my_value  = input[``` ***IntegerExpression*** ```];```


Inputs can have any of the types Boolean, Float or Integer. The actual type is dependent on the InputIdentifier.
Note: For Inputs accessed as array the type is TBD.


example:
   
```
   roll = input{Roll};
   take_picture = input[12] > 1500; 
```

For ArduPlane, the available generic inputs are as follows ( See [./examples/TaranisUSBJoystick/main.cpp](https://github.com/kwikius/mixer_lang/blob/master/examples/TaranisUSBJoystick/main.cpp) for how this is implemented and can be changed)
(Very preliminary)

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

Expressions
-----------
Expressions consists of operations on Symbols, Inputs, Functions and Outputs.
An operation can only be done on objects of the same type. For example an integer multiplication by a double is disallowed.
The reason is to to keep operations as small as possible, so casting must be explicit. (Casting TODO)
Operators ( e.g +,- *, etc) are similar to those used in C, except & and or are logical operators rather than 
bit manipulation operators.

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

   T max( T a, T b)  # requires a is same type as b and type is not bool.
   If a is greater than b return a else returns b 

   syntax:

   ``` x = max(1.0,2.0); ```  # x = 2.0

if
==

   T if(bool cond, T v_true, T v_false) # requires cond is type bool, v_true is same type as v_false.
   If the condition is true return v_true else return v_false

   syntax:

   ``` x = if(v1 < v1,v3,v4);```

Curves
------

A curve is basically a function defined as line graph. It can be used to map an input to an output.

   syntax:

   ```
   my_curve = [a,b],[c,d],[e,f],[g,h]; # my curve type is deduced as a curve function

   # modify roll input with expo or whatever
   my_result = my_curve(input{Roll});
   ```

There are some rules re defining curves (TODO: rules)


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

None.  

Compilation Building Running
----------------------------

The library currently builds for linux systems

There is a Make file included . See [Gnu Make](https://www.gnu.org/software/make).

After downloading the mixer_lang library, get your Taranis Tx and its usb to pc connector handy
, enter the top level mixer_lang directory and invoke make with the following argument.

```
~>$ make run_taranis_easystar
```
This will build and start the mixer with the Easystar mixer. (Look in the Makefile to try other mixer scripts)
At the prompt, first switch on your Tarnis and then connect your Taranis USB to PC connector.
Once connected, to start and stop the app press any key.
When running, twiddle the sticks and you should see some changing outputs.
Note: To make sense of the output you will need to set up an "Ideal" model where the Taranis 
doesnt do any mixing of course, just sends stick inputs to outputs.

Try editing the scripts in the mixers/generic directory and run them to see the effects of your changes.

The executable is in the examples/TaranisUSBJoystick/bin/ subdirectory
To invoke the application with a mixer put the executable file in your path
, enter your mixer directory and supply a mixer_lang script file as an argument.

```
~>$ taranis_mixer.exe myfile.mix
```

Notes
-----

The Joystick is currently hardwired as "/dev/input/js0"
If your outputs are  messy, try running through the Taranis calibration routine.


