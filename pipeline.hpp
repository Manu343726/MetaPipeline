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
    
    template<typename... STAGES>
    using pipe = tml::list<STAGES...>;
    
    namespace impl
    {
        template<typename INPUT , typename... STAGES>
        struct pipeline
        {
            using pipe = mp::pipe<STAGES...>;

            using result = tml::foldl<tml::lazy<mp::stage_executor>,INPUT,pipe>;
        };
    }
    
    template<typename INPUT , typename... STAGES>
    using pipeline = typename mp::impl::pipeline<INPUT,STAGES...>::result;
}

#endif	/* PIPELINE_HPP */

