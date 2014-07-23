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

Writting simple examples like the above is easy, but as the complexity of an expression (Or an entire metaprogram) increases, there are more nested template expressions, more
dependencies, and the most important IMHO, the syntax become unreadable.

MetaPipeline provides a fluent interface for such functionality in the form of a *pipeline* of commands, where the input of each command is the output of the previous, and so on.
Each command represents some functionality of the Turbo library, and different argumments for that commands could be specified. Here is an example equivalent to the previous one:

    using doubled = mp::pipeline<tml::integer_list<1,2,3,4,5> , 
                                 mp::stage< mp::commands::filter , filter                           >,
                                 mp::stage< mp::commands::map    , tml::lambda<_1 , tml::add<_1,_1> >
                                >;
