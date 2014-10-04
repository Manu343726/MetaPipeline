MetaPipeline
============

Human friendly template metaprogramming through fluent interfaces


## The Turbo C++11 Metaprogramming Library

The [Turbo C++11 Metaprogramming Library](https://github.com/Manu343726/Turbo) provides, in addition to other features, high-level metaprogramming constructs, to make metaprogramming in C++ easy and not 
that cumbersome process. Since template metaprogramming has a functional spirit (Its stateless, since we cannot change a type after its instanced), Turbo provides constructs
similar to what the most common functional languages provides: High-order metafunctions for type mapping, folding, filtering..., lambda expressions, tools to evaluate such functional expressions,
and some utilities for parameter binding, curryfying, etc.

    using numbers = tml::integer_list<1,2,3,4,5>;

    template<typename T> struct filter : public tml::function<tml::Bool<T::value % 2 == 0>> {};
    
    using filtered = tml::filter<filter,numbers>;
    using doubled  = tml::map<tml::lambda<_1 , tml::add<_1,_1>>,filtered>;

## Flux

Writting simple examples like the above is easy, but as the complexity of an expression (Or an entire metaprogram) grows, there are more nested template expressions, more
dependencies, and the most important IMHO, the syntax become unreadable.

C++ template metaprogramming is in essence a pure functional language inside C++, so feels natural to use functional constructions and program in a functional way when one is dealing with template metaprogramming.  
And one of the key concepts of functional programming are monads. 

So, what is Flux? A library to take C++ template metaprogramming to the top level, thanks to a rich and expressive continuation monad. 

Flux provides a fluent interface for such functionality in the form of a *pipeline* of commands, where the input of each command is the output of the previous, and so on.
Each command represents some functionality of the Turbo library, and different argumments for that commands could be specified. Here is an example equivalent to the previous one:

    using doubled = flux::pipe<flux::start<tml::integer_list<1,2,3,4,5>> , 
                               flux::filter<filter> ,
                               flux::map<tml::lambda<_1 , tml::add<_1,_1>>>
                              >;

## Features

The continuation pipelines implemented by Flux are not simple chains of sequential commands. Each pipeline has such metadata to carry, customize, or inspect the current state of the computation.

    using result = mp::pipeline<mp::start<tml::Int<1>>       , //Load the integer 1
                                mp::repeat<tml::size_t<4>>   , //Generate a list with the current value (the integer 1) repeated four times 
                                mp::save<X>                  , //Save the current value ( [1,1,1,1] ) on the variable X
                                mp::map<tml::lambda<_1 , X>>   //For each element of the current value ( [1,1,1,1] ), put the value of X
                               >;

In the example above, `result` is `[ [1,1,1,1] , [1,1,1,1] , [1,1,1,1] , [1,1,1,1] ]`.

Also the execution of a pipeline could be explicitly interrumped and continued later, using commands like `mp::Break` and `mp::Continue`:


    using breakpoint = mp::pipeline<mp::start<tml::integer_list<1,2,3,4>>      ,
                                    mp::filter<tml::lambda<_1 , tml::odd<_1>>> ,
                                    mp::Break
                                   >;

    using result = tml::pipeline<mp::Continue<breakpoint> , 
                                 mp::map<tml::bind<tml::div,_1,tml::Integer<2>>>
                                >;

