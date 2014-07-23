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
    template<template<typename...> class C>
    using command = tml::lazy<C>;
    
    namespace commands
    {
        using map    = mp::command<tml::map>;
        using filter = mp::command<tml::filter>;
    }
    
    template<typename COMMAND , typename... ARGS>
    struct stage
    {
        using command = COMMAND;
        using args    = tml::list<ARGS...>;
    };
    
    template<typename S>
    using stage_command = typename S::command;
    
    template<typename S>
    using stage_args    = typename S::args;
    
    
    
    template<typename STATE , typename STAGE>
    struct stage_executor;
    
    template<typename STATE , typename COMMAND , typename... ARGS>
    struct stage_executor<STATE,mp::stage<COMMAND,ARGS...>> :
        public tml::function<tml::eval<COMMAND,ARGS...,STATE>>
    {};
}

namespace tml
{
    template<typename STATE , typename STAGE>
    struct overrides_eval<mp::stage_executor<STATE,STAGE>> : public tml::true_type
    {};
    
    namespace impl
    {
        template<typename STATE , typename STAGE>
        struct eval<mp::stage_executor<STATE,STAGE>,tml::empty_list> : 
            public mp::stage_executor<STATE,STAGE>
        {};
    }
}

#endif	/* COMMAND_HPP */

