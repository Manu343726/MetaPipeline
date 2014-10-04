Flux
====

Human friendly template metaprogramming. Take the continuation monad to the extreme.


## The Turbo C++11 Metaprogramming Library

The [Turbo C++11 Metaprogramming Library](https://github.com/Manu343726/Turbo) provides, in addition to other features, high-level metaprogramming constructs, to make metaprogramming in C++ easy and not 
that cumbersome process. Since template metaprogramming has a functional spirit (Its stateless, since we cannot change a type after its instanced), Turbo provides constructs
similar to what the most common functional languages provides: High-order metafunctions for type mapping, folding, filtering..., lambda expressions, tools to evaluate such functional expressions,
and some utilities for parameter binding, curryfying, etc.

``` cpp
using numbers = tml::integer_list<1,2,3,4,5>;

template<typename T> struct filter : public tml::function<tml::Bool<T::value % 2 == 0>> {};
    
using filtered = tml::filter<filter,numbers>;
using doubled  = tml::map<tml::lambda<_1 , tml::add<_1,_1>>,filtered>;
```

## What's Flux?

Writting simple examples like the above is easy, but as the complexity of an expression (Or an entire metaprogram) grows, there are more nested template expressions, more
dependencies, and the most important IMHO, the syntax become unreadable.

C++ template metaprogramming is in essence a pure functional language inside C++, so feels natural to use functional constructions and program in a functional way when one is dealing with template metaprogramming.  
And one of the key concepts of functional programming are monads. 

So, what is Flux? A library to take C++ template metaprogramming to the top level, thanks to a rich and expressive continuation monad. 

Flux provides a fluent interface for such functionality in the form of a *pipeline* of commands, where the input of each command is the output of the previous, and so on.
Each command represents some functionality of the Turbo library, and different argumments for that commands could be specified. Here is an example equivalent to the previous one:

``` cpp
using doubled = flux::pipe<flux::start<tml::integer_list<1,2,3,4,5>> , 
                           flux::filter<filter> ,
                           flux::map<tml::lambda<_1 , tml::add<_1,_1>>>
                          >;
```

The continuation pipelines implemented by Flux are not simple chains of sequential commands. Each pipeline has such metadata to carry, customize, or inspect the current state of the computation.

``` cpp
using result = flux::pipe<flux::start<tml::Int<1>>       , //Load the integer 1
                          flux::repeat<tml::size_t<4>>   , //Generate a list with the current value (the integer 1) repeated four times 
                          flux::save<X>                  , //Save the current value ( [1,1,1,1] ) on the variable X
                          flux::map<tml::lambda<_1 , X>>   //For each element of the current value ( [1,1,1,1] ), put the value of X
                         >;
```

In the example above, `result` is `[ [1,1,1,1] , [1,1,1,1] , [1,1,1,1] , [1,1,1,1] ]`.

Also the execution of a pipeline could be explicitly interrumped and continued later, using commands like `flux::Break` and `flux::Continue`:

``` cpp
using breakpoint = flux::pipe<flux::start<tml::integer_list<1,2,3,4>>      ,
                              flux::filter<tml::lambda<_1 , tml::odd<_1>>> ,
                              flux::Break
                             >;

using result = flux::pipe<flux::Continue<breakpoint> , 
                          flux::map<tml::bind<tml::div,_1,tml::Integer<2>>>
                         >;
```

## The powert of flux: Not a simple Continuation only, but a runtime

These are only simple examples of what Flux can do. A pipe (Which really acts as a program) has those properties and data a common running program has: A memory with the declared variables and its values, the current state of the pipe, a command buffer, etc; all accessible and manipulable by the user with such APIs. 

The idea is to build up a real "runtime" and "language" to deal with template metaprogramming. You write a *program*, that program has a *runtime* containing program *variables* and *commands* (Instructions). Stop thinking on ugly templates, start thinking on algorithms.

``` cpp
using result = flux::pipe<flux::start<tml::Int<0> , //Start the "program" with 0 as state
                          flux::label<L1>         , //Save a label pointing to the current command
                          flux::apply<tml::bind<tml::add,_1,tml::Int<1>>> , //Increment state
                          flux::jump_if<tml::lambda<_1,tml::greater_than<_1,tml::Int<5>>> , L1> //Repeat if state <= 5
                         >;
```

This could be seen as compile-time assembly, but imagine what you could even do. Do metaprogramming in a functional/fluent way, but use variables in a procedural way if you like, do jups, ifs, etc. 
