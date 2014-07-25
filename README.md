MetaPipeline
============

Fluent interfaces for C++11 high-level functional metaprogramming facilities


## The Turbo C++11 Metaprogramming Library

The [Turbo C++11 Metaprogramming Library]() provides, in addition to other features, high-level metaprogramming constructs, to make metaprogramming in C++ easy and not 
that cumbersome process. Since template metaprogramming has a functional spirit (Its stateless, since we cannot change a type after its instanced), Turbo provides constructs
similar to what the most common functional languages provides: High-order metafunctions for type mapping, folding, filtering..., lambda expressions, tools to evaluate such functional expressions,
and some utilities for parameter binding, curryfying, etc.

    using numbers = tml::integer_list<1,2,3,4,5>;

    template<typename T> struct filter : public tml::function<tml::Bool<T::value % 2 == 0>> {};
    
    using filtered = tml::filter<filter,numbers>;
    using doubled  = tml::map<tml::lambda<_1 , tml::add<_1,_1>>,filtered>;

## MetaPipeline

Writting simple examples like the above is easy, but as the complexity of an expression (Or an entire metaprogram) grows, there are more nested template expressions, more
dependencies, and the most important IMHO, the syntax become unreadable.

MetaPipeline provides a fluent interface for such functionality in the form of a *pipeline* of commands, where the input of each command is the output of the previous, and so on.
Each command represents some functionality of the Turbo library, and different argumments for that commands could be specified. Here is an example equivalent to the previous one:

    using doubled = mp::pipeline<tml::integer_list<1,2,3,4,5> , 
                                 mp::filter<filter> ,
                                 mp::map<tml::lambda<_1 , tml::add<_1,_1>>
                                >;

## Features

The functional pipelines implemented by MetaPipeline are not simple chains of secuential commands. Each pipeline carries an internal state of the execution, which includes the
current value of the computation and the set of variables defined at that time. Yes, MetaPipeline allows you to define variables inside a pipeline, store values on them, and use
that variables in the expressions passed to the pipeline commands:

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

### Writting custom commands

MetaPipeline differentiates from two types of commands:

 - **Mutable commands**: Are commands designed to modify in some way the internal state of the pipeline, not to do computations. An example could be a command for
   writting a value into a variable. Must inherit (aliase) from the template `mp::mutable_command`, and the signature of the command should be an unary function expecting
   a pipeline state (An instance of the `mp::computation_sate<VARIABLES,VALUE>`, where the first argumment `VARIABLES` is a list of variables and the second the current value 
   of the computation. A variable is just a pair `(name,value)` represented by the template `mp::variable<NAME,VALUE>`.  

   `mp::mutable_command` takes the command to be defined and an unlimited number of extra parameters representing the parameters of the command subject to apply variable
   substitution:
   
        template<typename COMMAND , typename... ARGS>
        struct mutable_command
        {};

   Consider this example:

        template<typename VALUE>
        using write_on_x = mp::mutable_command<tml::bind<mp::write_variable,_1,mp::variable<X,VALUE>> , VALUE>;

   The metafunction `mp::write_variable<STATE,VARIABLE>` adds or overwrites the specified variable. Our command, designed to give a value to the variable `X`, binds that
   function making it an unary function expecting the computation state, which is what the pipeline execution engine passes to mutable commands.  
   Also the command parameter, the value to be asigned to `X`, is passed as a variable substitution target. Thats done to allow the user to pass pipeline variables as values,
   making possible commands like this:

        ...
        mp::write_on_x<Y>,
        ... 
    

 - **Inmutable commands**: These commands perform computations. Their signature is a unary metafunnction expecting the current value of the computation, and their result is
   the new one. They inherit (aliase) from `mp::inmutable_command`.  
   
   Exactly as mutable commands, the base template wraps the command and have extra argumments for command parameters expecting variable substitution. Here is an example:

        template<typename VALUE , typename COMPARATOR = tml::lazy<tml::equal>>
        using erase_if_equal = mp::inmutable_command<tml::bind<tml::filter,tml::lambda<_1 , tml::deval<COMPARATOR,_1,VALUE>>,_1> , VALUE>;
                              
   The example above is a command to filter all the elements of the current computation value equal to a given value. Also allows you to pass a custom binary comparison function.
