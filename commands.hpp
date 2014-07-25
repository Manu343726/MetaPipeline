/* 
 * File:   commands.hpp
 * Author: manu343726
 *
 * Created on 25 de julio de 2014, 13:26
 */

#ifndef COMMANDS_HPP
#define	COMMANDS_HPP

#include "command.hpp"
#include "variable.hpp"

#include "Turbo/bind.hpp"
#include "Turbo/algorithm.hpp"
#include "Turbo/lazy.hpp"
#include "Turbo/placeholders.hpp"

namespace mp
{
    using namespace tml::placeholders;
    
    namespace impl
    {
        template<typename STATE>
        struct break_command : public tml::function<mp::computation_state<typename STATE::variables,STATE>>
        {};
        
        template<typename STATE , typename OLD_STATE>
        struct continue_command : public tml::function<OLD_STATE>
        {};
        
        template<typename STATE , typename NAME , typename VALUE>
        struct set_command 
        {
            using result = typename mp::write_variable<STATE,mp::variable<NAME,VALUE>>::result;
        };
        
        template<typename STATE , typename NAME>
        struct save_command : public set_command<STATE,NAME,typename STATE::comp_value>
        {};
    }
    
    template<typename NAME , typename VALUE>
    using set = mp::mutable_command<tml::bind<mp::impl::set_command,tml::placeholders::_1,NAME,VALUE>, VALUE>;
    
    template<typename VALUE>
    using load = mp::inmutable_command<tml::lambda<_1 , VALUE>    ,VALUE>;
    
    template<typename VALUE>
    using start = mp::load<VALUE>;
    
    template<typename NAME>
    using save = mp::mutable_command<tml::bind<mp::impl::save_command,tml::placeholders::_1,NAME>>;
    
    using Break = mp::mutable_command<tml::lazy<mp::impl::break_command>>;
    
    template<typename BREAKPOINT>
    using Continue = mp::mutable_command<tml::bind<mp::impl::continue_command,tml::placeholders::_1,BREAKPOINT> , BREAKPOINT>;
    
    
    
    template<typename TRANSFORM>
    using map = mp::inmutable_command<tml::bind<tml::map,TRANSFORM,tml::placeholders::_1> , TRANSFORM>;
    
    
    template<typename N>
    using repeat = mp::inmutable_command<tml::bind<tml::repeat,tml::placeholders::_1,N> , N>;

    
    
}

#endif	/* COMMANDS_HPP */

