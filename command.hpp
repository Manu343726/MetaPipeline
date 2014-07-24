/* 
 * File:   command.hpp
 * Author: manu343726
 *
 * Created on 23 de julio de 2014, 20:19
 */

#ifndef COMMAND_HPP
#define	COMMAND_HPP

#include "Turbo/algorithm.hpp"

namespace mp
{
    template<template<typename...> class C , typename... ARGS>
    using command = tml::lazy<C,ARGS...>;
    
    namespace commands
    {
        template<typename... ARGS>
        using map    = mp::command<tml::map,ARGS...>;
        
        template<typename... ARGS>
        using filter = mp::command<tml::filter,ARGS...>;
        
        template<typename F>
        using map_direct = F;
    }
    
    
    template<typename STATE , typename COMMAND>
    struct command_executor :
        public tml::function<tml::eval<COMMAND,STATE>>
    {};
}

namespace tml
{
    template<typename STATE , typename COMMAND>
    struct overrides_eval<mp::command_executor<STATE,COMMAND>> : public tml::true_type
    {};
    
    namespace impl
    {
        template<typename STATE , typename COMMAND>
        struct eval<mp::command_executor<STATE,COMMAND>,tml::empty_list> : 
            public mp::command_executor<STATE,COMMAND>
        {};
    }
}

#endif	/* COMMAND_HPP */

