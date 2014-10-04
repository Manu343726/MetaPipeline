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

#ifndef LAMBDA_HPP
#define	LAMBDA_HPP

#include "manu343726/turbo_core/eval.hpp"
#include "manu343726/turbo_core/let_expressions.hpp"
#include "manu343726/turbo_core/list_algorithms.hpp"

namespace tml
{
    namespace impl
    {
        /*
         * Represents an unary (One argumment) lambda expression.
         * 
         * A lambda expression is just a template that holds a variable and a functional
         * expression (The lambda body) where the variable is used. 
         * The lambda expression is dessigned to act as a functional expression to be evaluated lazily,
         * when the value of the parameter is specified. Thats why the 'result'  member of function entities
         * is parametrized.
         * 
         * Because the 'result' member doesn't work like in other function entities (In other function entities returns
         * the result, in a lambda computes the result), a lambda cannot be considered a classic function entity.
         * The implementation trait 'tml::impl::is_function' and the 'tml::eval' implementation are overrided to
         * cover this special behaviour of lambda expressions. 
         */
        template<typename X , typename BODY>
        struct lambda
        {
            /*
             * Lambda body evaluation: Substitute the variable of the lambda in the body with
             * the argumment passed to the lambda and evaluate the resulting expression.
             */
            template<typename ARG>
            using result = tml::eval<tml::let<X,ARG,BODY>>;
        };
    }
    
    /*
     * An unary lambda overrides tml::eval:
     */
    template<typename X , typename BODY>
    struct overrides_eval<tml::impl::lambda<X,BODY>> : public tml::true_type
    {};
    
    namespace impl
    {
        /*
         * A unary lambda expression is not considered a function entity (See documentation above)
         */
        template<typename X , typename BODY>
        struct is_function<tml::impl::lambda<X,BODY>>
        {
            static constexpr bool result = false;
        };
        
        /*
         * tml::eval is overrided to compute the result of the lambda when the lambda itselft is evaluated.
         * Of course a lambda have to be evaluated with its calling parameter, like:
         * 
         *     using result = tml::eval<lambda,param>;
         * 
         * so tml::eval acts as a calling high-level metafunction (See the tml::eval documentation) always when a 
         * lambda is passed.
         */
        template<typename X , typename BODY , typename ARG>
        struct eval<tml::impl::lambda<X,BODY>,tml::list<ARG>> : 
        public tml::function<typename tml::impl::lambda<X,BODY>::template result<tml::eval<ARG>>> 
        {};
        

        /*
         * Represents a multiple-variable lambda expression.
         * 
         * A lambda expression is just a template that holds a set of variables and a functional
         * expression (The lambda body) where the variable is used. 
         * The lambda expression is dessigned to act as a functional expression to be evaluated lazily,
         * when the values of the parameters are specified. Thats why the 'result'  member of function entities
         * is parametrized.
         * 
         * Because the 'result' member doesn't work like in other function entities (In other function entities returns
         * the result, in a lambda computes the result), a lambda cannot be considered a classic function entity.
         * The implementation trait 'tml::impl::is_function' and the 'tml::eval' implementation are overrided to
         * cover this special behaviour of lambda expressions. 
         */
        template<typename BODY , typename... VARIABLES>
        struct multi_lambda
        {};
        
   
        
        /*
         * Variadic packs must be placed at the end of the set of template argumments,
         * but tml::multi_lambda takes variables first and the lambda body later.
         * 
         * This templates places the body parameter to the begin of the tml::impl::multi_lambda 
         * template to allow that template read the pack of variables.
         * 
         * An extra step (Performed by 'lambda_builder_2' is needed to fill a list with the parameters
         * of the builder. Using a list is the only way to match the parameters through partial specialization.
         * 
         * tml::impl::lambda_builder is specialized to return an unary lambda if ony one variable is passed to the lambda builder.
         * Unary lambdas are preferred because they not currify their let expression, which is a complex process.
         */
        template<typename ARGS>
        struct lambda_builder;
        
        template<typename ARGS>
        struct lambda_builder_2;
        
        template<typename... ARGS>
        struct lambda_builder_2<tml::list<ARGS...>> : public tml::function<multi_lambda<ARGS...>>
        {};
        
        template<typename... ARGS>
        struct lambda_builder<tml::list<ARGS...>>
        {
            //static_assert( sizeof...(ARGS) != sizeof...(ARGS) , "Builder instanced" );
        
            using body = tml::lists::back<tml::list<ARGS...>>;
            using vars = tml::lists::pop_back<tml::list<ARGS...>>;
            
            using result = typename lambda_builder_2<tml::lists::push_front<vars,body>>::result;
        };
        
        template<typename X , typename BODY>
        struct lambda_builder<tml::list<X,BODY>> : public tml::function<tml::impl::lambda<X,BODY>>
        {};
        
        /*
         * A lambda expression is not considered a function entity (See documentation above)
         */
        template<typename... VARIABLES , typename BODY>
        struct is_function<multi_lambda<BODY,VARIABLES...>>
        {
            static constexpr bool result = false;
            
            //static_assert( sizeof(BODY) != sizeof(BODY) , "Instanced" );
        };
        
        /*
         * tml::let is overrided to compute the result of the lambda when the lambda itselft is evaluated.
         * Of course a lambda have to be evaluated with its calling parameters, like:
         * 
         *     using result = tml::eval<lambda,params...>;
         * 
         * so tml::eval acts as a calling high-level metafunction (See the tml::eval documentation) always when a 
         * lambda is passed.
         */
        template<typename... VARIABLES , typename BODY , typename ARG , typename... ARGS>
        struct eval<multi_lambda<BODY,VARIABLES...>,tml::list<ARG,ARGS...>>
        {
            using let_args = tml::list<VARIABLES...,ARG,ARGS...,BODY>;
            using result = tml::eval<typename tml::impl::multi_let_currifier<let_args>::result>;
        };
    }
    
    /*
     * A multiple-variable lambda overrides tml::eval:
     */
    template<typename... VARIABLES , typename BODY>
    struct overrides_eval<tml::impl::multi_lambda<BODY,VARIABLES...>> : public tml::true_type
    {};
    
    template<typename... ARGS>
    using lambda = typename tml::impl::lambda_builder<tml::list<ARGS...>>::result;
}

#endif	/* LAMBDA_HPP */









