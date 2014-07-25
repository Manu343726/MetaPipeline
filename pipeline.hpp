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
    
    struct uninitialized_pipeline{};
    
    namespace impl
    {
        template<typename... COMMANDS>
        struct pipeline
        {
            using pipe = mp::pipe<COMMANDS...>;
            using init_state = mp::computation_state<tml::empty_list,mp::uninitialized_pipeline>;

            using result = typename tml::foldl<tml::lazy<mp::command_executor>,init_state,pipe>::comp_value;
        };
    }
    
    template<typename INPUT , typename... COMMANDS>
    using pipeline = typename mp::impl::pipeline<INPUT,COMMANDS...>::result;
}

#endif	/* PIPELINE_HPP */

