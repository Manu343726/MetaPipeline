/* 
 * File:   main.cpp
 * Author: manu343726
 *
 * Created on 23 de julio de 2014, 20:16
 */

#include <iostream>

#include "Turbo/utility.hpp"
#include "Turbo/placeholders.hpp"
#include "Turbo/runtime_placeholders.hpp"
#include "Turbo/to_runtime.hpp"
#include "Turbo/bind.hpp"
#include "Turbo/basic_types.hpp"

#include "pipeline.hpp"
#include "commands.hpp"

using namespace tml::placeholders;
using namespace tml::runtime::placeholders;


struct X : public tml::value_chameleon {};
struct Y : public tml::value_chameleon {};
struct Z : public tml::value_chameleon {};

using r = mp::pipeline<mp::start<tml::Int<0>>,
                       mp::repeat<tml::size_t<100>>
                      >;

int main()
{
    std::cout << tml::to_string<r>() << std::endl;
}

/*
using test_state = mp::computation_state<tml::list<mp::variable<X,tml::Int<0>> , mp::variable<Y,tml::Int<1>> , mp::variable<Z,tml::Int<2>>> , mp::uninitialized_pipeline>;



using breakpoint = mp::pipeline<mp::start<tml::Int<4>>,
                                mp::save<X>,
                                mp::repeat<tml::size_t<10>>,
                                mp::Break
                               >;

using result = mp::pipeline<mp::Continue<breakpoint>,
                            mp::map<tml::lambda<_1 , tml::mul<_1,Y>>>
                           >;

using command_parameter = tml::lambda<_1 , tml::mul<_1,X>>;
using command = tml::bind<tml::map,command_parameter,_1>;
using ex = tml::multi_let<X,Y,Z,tml::Int<0>,tml::Int<1>,tml::Int<2>,command>;
using ex2 = mp::apply_variables<command , test_state>;

int main( ) 
{
    std::cout << std::boolalpha << tml::to_string<ex>() << std::endl;
    std::cout << std::boolalpha << tml::to_string<ex2>() << std::endl;
    std::cout << std::boolalpha << tml::to_string<result>() << std::endl;
}
*/

