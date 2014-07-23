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


using lmap = tml::lazy<tml::map>;
using eval_lmap = tml::eval<lmap,tml::function<_1>,tml::integer_list<1,2,3,4,5,6,7,8,9>>;


using result = mp::pipeline<tml::list<int,float,double,long long int,short unsigned int>,
                            mp::stage<mp::commands::map    , tml::util::func::size_of<_1>                    >,
                            mp::stage<mp::commands::filter , tml::bind<tml::less_or_equal,tml::size_t<8>,_1> >
                           >;

int main( ) 
{
    std::cout << tml::to_string<eval_lmap>() << std::endl;
   
    for( const auto& v : tml::to_runtime<result>() )
        std::cout << v << " ";
}

