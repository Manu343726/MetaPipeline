/* 
 * File:   main.cpp
 * Author: manu343726
 *
 * Created on 23 de julio de 2014, 20:16
 */

#include <iostream>

#include "Turbo/utility.hpp"
#include "Turbo/placeholders.hpp"
#include "Turbo/to_runtime.hpp"
#include "Turbo/bind.hpp"

#include "pipeline.hpp"
#include "commands.hpp"

using namespace tml::placeholders;


struct X : public tml::value_chameleon {};
struct Y : public tml::value_chameleon {};
struct Z : public tml::value_chameleon {};



using breakpoint = mp::pipeline<mp::start<tml::Int<4>>,
                                mp::save<X>,
                                mp::repeat<tml::size_t<10>>,
                                mp::Break
                               >;

using result = mp::pipeline<mp::Continue<breakpoint>,
                            mp::map<tml::lambda<_1 , tml::mul<_1,_1>>>
                           >;

int main( ) 
{
    std::cout << std::boolalpha << tml::to_string<result>() << std::endl; //[16,16,16...,16] please...
}

