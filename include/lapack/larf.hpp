/// @file larf.hpp
/// @author Weslley S Pereira, University of Colorado Denver, USA
/// Adapted from @see https://github.com/langou/latl/blob/master/include/larf.h
//
// Copyright (c) 2013-2022, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef __LARF_HH__
#define __LARF_HH__

#include "lapack/types.hpp"
#include "lapack/utils.hpp"

#include "tblas.hpp"

namespace lapack {



template<typename vector_t>
class vector_that_starts_with_one {
    using T = type_t<vector_t>;
    using idx_t = size_type<vector_t>;

    public:
    vector_that_starts_with_one( vector_t const& v ): v_(v){}

    T operator[](idx_t i) const {
        if( i == 0){
            return T(1);
        }
        return v_[i];
    }

    vector_t const& v_;
};


template< typename vector_t >
inline constexpr auto
size( const vector_that_starts_with_one<vector_t>& v){ return size(v.v_); }

template< typename vector_t, class SliceSpec >
inline constexpr auto
subvector( const vector_that_starts_with_one<vector_t>& v, SliceSpec&& rows ) noexcept {
    if(rows.first == 0)
        return vector_that_starts_with_one<vector_t>(subvector(v.v_,rows));
    else
        return subvector(v.v_,rows);
}

template< typename vector_t >
struct type_trait< vector_that_starts_with_one<vector_t> > { using type = type_t<vector_t>; };

/** Applies an elementary reflector H to a m-by-n matrix C.
 *
 * The elementary reflector H can be applied on either the left or right, with
 * \[
 *        H = I - \tau v v^H.
 * \]
 * If tau = 0, then H is taken to be the unit matrix.
 * 
 * @param[in] layout
 *     Matrix storage, Layout::ColMajor or Layout::RowMajor.
 * 
 * @param[in] side Specifies whether the elementary reflector H is applied on the left or right.
 *
 *              side='L': form  H * C
 *              side='R': form  C * H
 * 
 * @param[in] m Number of rows of the matrix C.
 * @param[in] n Number of columns of the matrix C.
 * @param[in] v Vector of containing the elementary reflector.
 *
 *              If side='R', v is of length n.
 *              If side='L', v is of length m.
 * 
 * @param[in] incv Increment of the vector v.
 * @param[in] tau Value of tau in the representation of H.
 * @param[in,out] C m-by-n matrix.  On exit, C is overwritten with
 *
 *                H * C if side='L',
 *             or C * H if side='R'.
 * 
 * @param[in] ldC Column length of matrix C.  ldC >= m.
 * @param work Workspace vector of the following length:
 *
 *          n if side='L'
 *          m if side='R'.
 * 
 * @ingroup auxiliary
 */
template< class side_t, class vector_t, class tau_t, class matrix_t, class work_t >
inline void larf(
    side_t side,
    vector_t const& v, tau_t& tau,
    matrix_t& C, work_t& work )
{
    using blas::gemv;
    using blas::ger;

    // data traits
    using T = type_t<matrix_t>;

    // constants
    const T one(1.0);
    const T zero(0.0);

    // check arguments
    blas_error_if( side != Side::Left &&
                   side != Side::Right );
    blas_error_if(  access_denied( dense, write_policy(C) ) );

    vector_that_starts_with_one<vector_t> v2(v);

    if( side == Side::Left ) {
        gemv(Op::ConjTrans, one, C, v2, zero, work);
        ger(-tau, v2, work, C);
    }
    else {
        gemv(Op::NoTrans, one, C, v2, zero, work);
        ger(-tau, work, v2, C);
    }
}

} // lapack

#endif // __LARF_HH__
