/******************************************************************************
* Turbo C++11 metaprogramming Library                                         *
*                                                                             *
* Copyright (C) 2013 - 2014, Manuel Sánchez Pérez                             *
*                                                                             *
* This file is part of The Turbo Library.                                     *
*                                                                             *
* The Turbo Library is free software: you can redistribute it and/or modify   *
* it under the terms of the GNU Lesser General Public License as published by *
* the Free Software Foundation, version 2 of the License.                     *
*                                                                             *
* The Turbo Library is distributed in the hope that it will be useful,        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              * 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU Lesser General Public License for more details.                         *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with The Turbo Library. If not, see <http://www.gnu.org/licenses/>.   *
******************************************************************************/

#ifndef LET_HPP
#define LET_HPP

#include "manu343726/turbo_core/eval.hpp"
#include "manu343726/turbo_core/list.hpp"
#include "manu343726/turbo_core/list_algorithms.hpp"


/*
 * Haskell-like let expressions.
 * 
 * Based on http://www.sciencedirect.com/science/article/pii/S157106611100185X
 */

namespace tml
{
    namespace impl
    {
        /*
         * Internal namespace holds the implementation of the tml::let metafunction.
         * 
         * tml::let has three parameters:
         *    NAME: The variable (name) the value will be binded with in the expression.
         *    VALUE: The value to be binded.
         *    EXPRESSION: The functional expression which used the NAME variable.
         *                This expression could have any number of parameters.
         * 
         * The implementation is divided in two phases: High-level and low-level:
         *  - The high-level phase is in charge of taking the let parameters
         *    and passing them to the low-level phase. 
         * 
         *  - The low-level phase selects the different substitution actions to be 
         *    performed depending on the category of the let parameters. 
         *    For example, it covers the cases when the expression is not a functional
         *    expression (Thus there is no NAME to bind into the expression), and general
         *    case when the expression passed is a functional expression.
         * 
         *    The only case which the low-level phase doesn't cover is the case when the
         *    parameters of the functional expression passed matches the binding name. That
         *    case is covered in the high-level phase. In the other cases, the high-level phase
         *    just forwards the parameters to the low-level phase. 
         */
        
        
        /*
         * Forward declaration of the high-level phase.
         */
        template<typename NAME , typename VALUE , typename EXPRESSION>
        struct let_impl_high;
        
        /*
         * Forward declaration of the low-level phase.
         */
        template<typename NAME , typename VALUE , typename EXPRESSION>
        struct let_impl_low;
        
        
        /* Low level phase: */
        
        /*
         * Low-level phase. The expression passed is a functional expression. 
         * We apply let reursively on the parameters of the expression (Could be functional
         * expressions too). 
         */
        template<typename NAME , typename VALUE , template<typename...> class EXPRESSION , typename... PARAMETERS>
        struct let_impl_low<NAME,VALUE,EXPRESSION<PARAMETERS...>> :
        public tml::function<EXPRESSION<typename let_impl_high<NAME,VALUE,PARAMETERS>::result...>> 
        {
            //static_assert( sizeof(NAME) != sizeof(NAME) , "Instanced" );
        };
        
        /*
         * Special case for lambda bodies expressions.
         * 
         * Lambda bodies with evaluating expressions such as 'tml::eval<F,_1>' doesn't work because the placeholder is substituted after the 
         * expression evaluation.
         * 
         * The library provides the template tml::delayed_eval<F,ARGS...> for that purpose: It holds a functional expression reevaluation
         * with parameters that may be placeholders. When doing let on such template, tml::let substitutes the letted placeholders with its
         * values, and the tml::delayed_eval template with tml::eval.
         * 
         * For that purpose, the low_level phase of tml::let is overrided.
         */
        template<typename NAME , typename VALUE , 
                 typename F , typename... ARGS
                >
        struct let_impl_low<NAME,VALUE,tml::delayed_eval<F,ARGS...>> 
            : tml::function<tml::eval<typename let_impl_high<NAME,VALUE,F>::result , typename let_impl_high<NAME,VALUE,ARGS>::result...>>
        {
        };
        
        /*
         * General case of the low-level phase: The expression is not a functional expression,
         * so there are no variables to bind a value with.
         * The result is the expression itself.
         */
        template<typename NAME , typename VALUE , typename EXPRESSION>
        struct let_impl_low : public tml::function<EXPRESSION> 
        {
            /* Debug trace (DISABLED) */
            //static_assert( sizeof(VALUE) != sizeof(VALUE) , "LOW_PHASE: Simple expression case instanced" );
        };
        
        
        /* High level phase: */
        
        /*
         * General case of the high-level phase: Just parameter forwarding to the 
         * second phase.
         */
        template<typename NAME , typename VALUE , typename EXPRESSION>
        struct let_impl_high : let_impl_low<NAME,VALUE,EXPRESSION> 
        {
            /* Debug trace (DISABLED) */
            //static_assert( sizeof(NAME) != sizeof(NAME) , "HIGH_PHASE: LOW_PHASE call" );
        };
        
        
        /*
         * The variable to be binded and the expression are the same: Apply the
         * substitution of the value:
         */
        template<typename NAME , typename VALUE>
        struct let_impl_high<NAME,VALUE,NAME> : public tml::function<VALUE> 
        {
            /* Debug trace (DISABLED) */
            //static_assert( sizeof(NAME) != sizeof(NAME) , "HIGH_PHASE: Variable --> Name binding specialization instanced" );    
        };
    }
    
    /*
     * Haskell-like let expression
     * 
     * Binds a value to a named variable in the specified expression.
     * This metafunction has the following three parameters:
     *  - NAME: The name (variable) to bind the value with.
     *  - VALUE: The value to be binded.
     *  - EXPRESSION: The expression where the substitution will be applied.
     * 
     * The result of this metafunction is a copy of the passed expression where 
     * the ocurences of the named variable have been substituted with the value.
     * 
     * NOTE: Explicit "typename ::result" construct is used instead of tml::eval
     *       here because the evaluation process chacks for function signature
     *       (See 'tml::impl::is_function' trait in "functional.hpp"), which instanttiates
     *       the expression. The problem is because the use of variables and/or placeholders
     *       could lead to instantation errors in metafunctions which expect some properties
     *       (Being a function, for example) on its parameters. Of course variables and 
     *       and placeholders doesn't has that properties (Are just empty types), so when such
     *       expressions are instantiated with placeholders the instantation fails.
     *       Thats the reason to avoid tml::let here, a context (tml::let) where the usage of
     *       placeholders and variables is common.
     */
    template<typename NAME , typename VALUE , typename EXPRESSION>
    using let = typename impl::let_impl_high<NAME,VALUE,EXPRESSION>::result;
    
    
    
    
    namespace impl
    {            
        template<typename ARGS>
        struct multi_let_data_parser;

        template<typename... ARGS>
        struct multi_let_data_parser<tml::list<ARGS...>>
        {
            enum state { reading_vars , reading_args };


            template<typename VARIABLES , typename ARGS_ , typename LIST , std::size_t i , bool args_parsed>
            struct parse;

            template<typename... VARIABLES , typename ARGS_ , typename HEAD , typename... TAIL , std::size_t i>
            struct parse<tml::list<VARIABLES...>,ARGS_,tml::list<HEAD,TAIL...>,i,false>
            {
                using variables = typename parse<tml::list<VARIABLES...,HEAD>,ARGS_,tml::list<TAIL...>, i-1, (i-1) == 0>::variables;
                using args      = typename parse<tml::list<VARIABLES...,HEAD>,ARGS_,tml::list<TAIL...>, i-1, (i-1) == 0>::args;
                using body      = typename parse<tml::list<VARIABLES...,HEAD>,ARGS_,tml::list<TAIL...>, i-1, (i-1) == 0>::body;
            };

            template<typename VARIABLES , typename... ARGS_ , typename HEAD , typename T , typename... TAIL>
            struct parse<VARIABLES,tml::list<ARGS_...>,tml::list<HEAD,T,TAIL...>,0,true>
            {
                using variables = VARIABLES;
                using args      = typename parse<VARIABLES,tml::list<ARGS_...,HEAD>,tml::list<T,TAIL...>, 0,true>::args;
                using body      = typename parse<VARIABLES,tml::list<ARGS_...,HEAD>,tml::list<T,TAIL...>, 0,true>::body;
            };

            template<typename VARIABLES , typename ARGS_ , typename HEAD>
            struct parse<VARIABLES,ARGS_,tml::list<HEAD>,0,true>
            {
                using variables = VARIABLES;
                using args      = ARGS_;
                using body      = HEAD;
            };

            using variables = typename parse<tml::empty_list,tml::empty_list,tml::list<ARGS...>,sizeof...(ARGS)/2,false>::variables;
            using args      = typename parse<tml::empty_list,tml::empty_list,tml::list<ARGS...>,sizeof...(ARGS)/2,false>::args;
            using body      = typename parse<tml::empty_list,tml::empty_list,tml::list<ARGS...>,sizeof...(ARGS)/2,false>::body;
        };

        /*
         * Evaluate a multiple-variable let expression and returns the currified unary let equivalent expression.
         * 
         * A multiple-variable let expression is of the form:
         * 
         *     multi_let<vars...,values...,expression>
         * 
         * where vars is a set of expression variables, values a set of values to bind, and expression the expression where
         * to bind the values. 
         * Each variable of the set of variables passed has to have a corresponding one-by-one value in the values set specified,
         * in other words, the number of values passed should be the same as the number of variables passed.
         * The binding correspondence is done in order. For example:
         * 
         *     multi_let<x,y,z,a,b,c,function<x,y,z>>
         * 
         * Binds the value a to the variable x on expression, the value b to the variable y, and the value c to the variable z. 
         * So the result of that multi-variable let expression is:
         * 
         *     function<a,b,c>
         * 
         * The currifier parses the argumment pack in the way explained above, so the currifier expects an odd number of let parameters
         * (The last for the expression and one odd set of argumments with the (variable,value) correspondences).
         * 
         * The parsing is executed as follows:
         *  - The currifier extracts the last argumment as the target expression.
         *  - The rest odd set of argumments is paritiones in two sets, the first corresponding to the set of variables,
         *    and the later corresponding to the set of values.
         *  - Then the currifier generates the expected currified let expression recursively. 
         */
        template<typename ARGS>
        struct multi_let_currifier
        {
            static_assert( ( ARGS::length - 1 ) > 0 , "A let expression should have at least one variable-value pair to bind." );
            //static_assert( ( ARGS::length - 1 ) % 2 == 0 , "The set of variables and values should be odd (One value per variable)" );
            
            using target = tml::lists::back<ARGS>; //Target expression (See parsing description above)
            
            /* Just a type with no meaning used as sentinel in some contexts */
            struct nil {};
           
            
            using variables = typename multi_let_data_parser<ARGS>::variables;
            using args      = typename multi_let_data_parser<ARGS>::args;
            using body      = typename multi_let_data_parser<ARGS>::body;
            
            
            
            /*
             * This metafunction gets the set of variables and values and generates a
             * currified version of the left expression.
             * 
             * The last optional parameter is only to lead with the explicit two-empty-lists specialization
             * on the base case (See the comments on the base case).
             */
            template<typename VARIABLES , typename VALUES , typename = nil>
            struct currify;
            
            /*
             * Recursive case: There are more variables to process, generate an enclosing unary let expression
             * to bind the current variable-value correspondence.
             */
            template<typename CURRENT_VAR , typename... VARS_TAIL , typename CURRENT_VAL , typename... VALS_TAIL>
            struct currify<list<CURRENT_VAR,VARS_TAIL...>,list<CURRENT_VAL,VALS_TAIL...>,nil>
            {
                using next_call = currify<list<VARS_TAIL...>,list<VALS_TAIL...>>;
                using result = tml::let<CURRENT_VAR,CURRENT_VAL,typename next_call::result>;
            };
            
            /*
             * Base case: There are no more variables, just return the original expression to apply
             * let of the last variable on it.
             * 
             * NOTE: The template-partial-specialization parameter is not neccesary. An explicit
             * specialization for two empty lists should do the work, but explicit template specializations
             * are not valid on non-global contexts.
             * 
             * To ride over that the metafunction has an optional unsused parameter which is specified 
             * as a partial-specialization parameter here to solve the problem.
             */
            template<typename SENTINEL>
            struct currify<list<>,list<>,SENTINEL>
            {
                using result = body;
            };
            
            /*
             * The result of the currification process
             */
            using result = typename currify<variables,args,nil>::result;
        };
    }
    
    /*
     * Haskell-like multiple-variable let expression
     * 
     * Binds a  set of values to a set of named variables in the specified expression.
     * This metafunction 
     *  - NAME: The name (variable) to bind the value with.
     *  - VALUE: The value to be binded.
     *  - EXPRESSION: The expression where the substitution will be applied.
     * 
     * The result of this metafunction is a copy of the passed expression where 
     * the ocurences of the named variable have been substituted with the value.
     */
    template<typename... ARGS>
    using multi_let = typename impl::multi_let_currifier<tml::list<ARGS...>>::result;
}

#endif  /* LET_HPP */