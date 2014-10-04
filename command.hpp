/* 
 * File:   command.hpp
 * Author: manu343726
 *
 * Created on 23 de julio de 2014, 20:19
 */

#ifndef COMMAND_HPP
#define	COMMAND_HPP

#include "manu343726/turbo_core/algorithm.hpp"
#include "manu343726/turbo_core/bind.hpp"
#include "computation_state.hpp"
#include "variable.hpp"

namespace mp
{
    template<typename COMMAND , typename... ARGS>
    struct mutable_command
    {};
    
    template<typename COMMAND , typename... ARGS>
    struct inmutable_command
    {};
    
    template<typename COMMAND , typename STATE>
    struct apply_variables_to_command;
    
    template<typename STATE , typename COMMAND , typename... ARGS>
    struct apply_variables_to_command<mp::mutable_command<COMMAND,ARGS...>,STATE>
    {
        using args = tml::list<mp::apply_variables<ARGS,STATE>...>;
        
        template<typename... APPLIED_ARGS>
        struct compute_result
        {
            using result = tml::multi_let<ARGS...,APPLIED_ARGS...,COMMAND>;
        };
        
        using result = tml::eval<tml::uncurry<compute_result>,args>;
    };
    
    template<typename STATE , typename COMMAND , typename... ARGS>
    struct apply_variables_to_command<mp::inmutable_command<COMMAND,ARGS...>,STATE>
    {
        using args = tml::list<mp::apply_variables<ARGS,STATE>...>;
        
        template<typename... APPLIED_ARGS>
        struct compute_result
        {
            using result = tml::multi_let<ARGS...,APPLIED_ARGS...,COMMAND>;
        };
        
        using result = tml::eval<tml::uncurry<compute_result>,args>;
    };
    
    template<typename STATE , typename COMMAND>
    struct apply_variables_to_command<mp::mutable_command<COMMAND>,STATE> : public tml::function<COMMAND>
    {};
    
    template<typename STATE , typename COMMAND>
    struct apply_variables_to_command<mp::inmutable_command<COMMAND>,STATE> : public tml::function<COMMAND>
    {};
    
    
    
    template<typename STATE , typename COMMAND>
    struct command_executor;
    
    template<typename STATE , typename COMMAND , typename... ARGS>
    struct command_executor<STATE,mutable_command<COMMAND,ARGS...>>
    {
        using original_command = mp::inmutable_command<COMMAND,ARGS...>;
        
        //using command          = COMMAND;
        using command          = typename mp::apply_variables_to_command<original_command,STATE>::result;
        
        using result           = tml::eval<command,STATE>;
    };
    
    template<typename STATE , typename COMMAND , typename... ARGS>
    struct command_executor<STATE,inmutable_command<COMMAND,ARGS...>>
    {
        using original_command = mp::inmutable_command<COMMAND,ARGS...>;
        
        //using command          = COMMAND;
        using command          = typename mp::apply_variables_to_command<original_command,STATE>::result;
        
        using result           = mp::computation_state<typename STATE::variables,tml::eval<command,typename STATE::comp_value>>;
    };
}

namespace tml
{
    template<typename STATE , typename COMMAND>
    struct overrides_eval<mp::command_executor<STATE,COMMAND>> : public tml::true_type
    {};
    
    namespace impl
    {
        template<typename STATE , typename COMMAND>
        struct eval<mp::command_executor<STATE,COMMAND>,tml::empty_list> : public tml::function<typename mp::command_executor<STATE,COMMAND>::result>
        {};
    }
}

#endif	/* COMMAND_HPP */

