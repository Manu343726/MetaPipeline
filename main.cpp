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

using namespace tml::placeholders;


using result = mp::pipeline<tml::list<int,float,double,long long int,short unsigned int>,
                            mp::commands::map<tml::util::func::size_of<_1>>,
                            mp::commands::filter<tml::bind<tml::less_or_equal,tml::size_t<8>,_1>>,
                            mp::commands::map_direct<tml::bind<tml::repeat,_1,tml::size_t<200>>>
                           >;

int main( ) 
{
    std::cout << tml::to_string<result>() << std::endl;
}

