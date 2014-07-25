/* 
 * File:   computation_state.hpp
 * Author: manu343726
 *
 * Created on 24 de julio de 2014, 18:57
 */

#ifndef COMPUTATION_STATE_HPP
#define	COMPUTATION_STATE_HPP

/*
 * A pipeline carries some state of the computation it represents, and that state is 
 * updated on each state. 
 * The computation state includes the internal pipeline variables (And its values), and the current value
 * of the computation.
 */

namespace mp
{
    template<typename VARIABLES , typename COMP_VALUE>
    struct computation_state
    {
        using variables = VARIABLES;
        using comp_value = COMP_VALUE;
    };
}

#endif	/* COMPUTATION_STATE_HPP */

