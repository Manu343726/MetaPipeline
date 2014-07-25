/* 
 * File:   variable.hpp
 * Author: manu343726
 *
 * Created on 24 de julio de 2014, 18:59
 */

#ifndef VARIABLE_HPP
#define	VARIABLE_HPP

#include "Turbo/let_expressions.hpp"
#include "Turbo/high_order_functions.hpp"
#include "Turbo/placeholders.hpp"
#include "Turbo/bind.hpp"

#include "computation_state.hpp"


/*
 * A pipeline can declare some "variables" during its execution.
 * That variables could be readed, written, and used in expressions of any stage
 * of the pipeline.
 */

namespace mp
{
    /* A variable is a pair (name,value) where name is a tag type used to refer to the variable */
    template<typename NAME , typename VALUE>
    struct variable
    {
        using name = NAME;
        using value = VALUE;
    };
    
    template<typename VARIABLE>
    struct variable_name;
    
    template<typename NAME , typename VALUE>
    struct variable_name<variable<NAME,VALUE>> : public tml::function<NAME>
    {};
    
    template<typename VARIABLE>
    struct variable_value;
    
    template<typename NAME , typename VALUE>
    struct variable_value<variable<NAME,VALUE>> : public tml::function<VALUE>
    {};
    
    
    
    namespace impl
    {
        /*
         * Given an expression and a set of variables, returns the same expression with the values of the 
         * variables substituted.
         */
        template<typename EXPRESSION , typename VARIABLES>
        struct apply_variables;

        template<typename EXPRESSION , typename... NAMES , typename... VALUES>
        struct apply_variables<EXPRESSION , tml::list<mp::variable<NAMES,VALUES>...>> :
            public tml::function<tml::multi_let<NAMES...,VALUES...,EXPRESSION>>
        {};
        
        template<typename EXPRESSION>
        struct apply_variables<EXPRESSION , tml::empty_list> :
            public tml::function<EXPRESSION>
        {};
    }
    
    template<typename EXPRESSION , typename STATE>
    using apply_variables = typename mp::impl::apply_variables<EXPRESSION,typename STATE::variables>::result;
    
    
    template<typename VARIABLES , typename VARIABLE>
    struct update_variable
    {
        using v_name = typename VARIABLE::name;
        using v_value = typename VARIABLE::value;

        template<typename T>
        struct transform
        {
            using name = typename T::name;
            using value = typename T::value;

            using result = tml::conditional<tml::Bool<std::is_same<v_name,name>::value>,
                                            mp::variable<name,v_name>,
                                            mp::variable<name,value>
                                           >;                                      
        };

        using result = tml::map<tml::lazy<transform>,VARIABLES>;
    };
    
    

    template<typename NAME , typename VARIABLE>
    struct variable_exists_filter : public tml::function<tml::false_type>
    {};
    
    template<typename NAME , typename VALUE>
    struct variable_exists_filter<NAME,mp::variable<NAME,VALUE>> : public tml::function<tml::true_type>
    {};
    
    template<typename NAME>
    using search_name = tml::bind<mp::variable_exists_filter,NAME,tml::placeholders::_1>;
    
    using search_result = tml::eval<search_name<tml::true_type>,tml::true_type>;

    template<typename VARIABLES , typename NAME>
    struct variable_exists : public tml::function<tml::any<mp::search_name<NAME>,VARIABLES>>
    {};
    
    template<typename NAME>
    struct variable_exists<tml::empty_list,NAME> : public tml::function<tml::false_type>
    {};

    template<typename STATE , typename VARIABLE>
    struct write_variable
    {
        using name = typename VARIABLE::name;
        using variables = typename STATE::variables;
        
        using result = mp::computation_state<tml::conditional<mp::variable_exists<variables,name>,
                                                              typename mp::update_variable<variables,VARIABLE>::result,
                                                              tml::lists::push_back<variables,VARIABLE>
                                                             >,
                                             typename STATE::comp_value
                                            >;
    };
        
        
        template<typename STATE , typename NAME>
        struct read_variable
        {            
            using result = typename tml::lists::head<tml::filter<mp::search_name<NAME>,typename STATE::variables>>::value;
        };
}

#endif	/* VARIABLE_HPP */

