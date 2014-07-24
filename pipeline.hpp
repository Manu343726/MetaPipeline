/* 
 * File:   pipeline.hpp
 * Author: manu343726
 *
 * Created on 23 de julio de 2014, 20:16
 */

#ifndef PIPELINE_HPP
#define	PIPELINE_HPP

#include "Turbo/list.hpp"
#include "Turbo/high_order_functions.hpp"
#include "command.hpp"

namespace mp
{    
    
    template<typename... COMMANDS>
    using pipe = tml::list<COMMANDS...>;
    
    namespace impl
    {
        template<typename INPUT , typename... COMMANDS>
        struct pipeline
        {
            using pipe = mp::pipe<COMMANDS...>;

            using result = tml::foldl<tml::lazy<mp::command_executor>,INPUT,pipe>;
        };
    }
    
    template<typename INPUT , typename... COMMANDS>
    using pipeline = typename mp::impl::pipeline<INPUT,COMMANDS...>::result;
}

#endif	/* PIPELINE_HPP */

