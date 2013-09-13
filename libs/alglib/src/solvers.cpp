/*************************************************************************
Copyright (c) Sergey Bochkanov (ALGLIB project).

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses
>>> END OF LICENSE >>>
*************************************************************************/
#include "stdafx.h"
#include "solvers.h"

// disable some irrelevant warnings
#if (AE_COMPILER==AE_MSVC)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#pragma warning(disable:4702)
#pragma warning(disable:4996)
#endif
using namespace std;

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS IMPLEMENTATION OF C++ INTERFACE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib
{


/*************************************************************************

*************************************************************************/
_densesolverreport_owner::_densesolverreport_owner()
{
    p_struct = (alglib_impl::densesolverreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::densesolverreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_densesolverreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_densesolverreport_owner::_densesolverreport_owner(const _densesolverreport_owner &rhs)
{
    p_struct = (alglib_impl::densesolverreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::densesolverreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_densesolverreport_init_copy(p_struct, const_cast<alglib_impl::densesolverreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_densesolverreport_owner& _densesolverreport_owner::operator=(const _densesolverreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_densesolverreport_clear(p_struct);
    if( !alglib_impl::_densesolverreport_init_copy(p_struct, const_cast<alglib_impl::densesolverreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_densesolverreport_owner::~_densesolverreport_owner()
{
    alglib_impl::_densesolverreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::densesolverreport* _densesolverreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::densesolverreport* _densesolverreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::densesolverreport*>(p_struct);
}
densesolverreport::densesolverreport() : _densesolverreport_owner() ,r1(p_struct->r1),rinf(p_struct->rinf)
{
}

densesolverreport::densesolverreport(const densesolverreport &rhs):_densesolverreport_owner(rhs) ,r1(p_struct->r1),rinf(p_struct->rinf)
{
}

densesolverreport& densesolverreport::operator=(const densesolverreport &rhs)
{
    if( this==&rhs )
        return *this;
    _densesolverreport_owner::operator=(rhs);
    return *this;
}

densesolverreport::~densesolverreport()
{
}


/*************************************************************************

*************************************************************************/
_densesolverlsreport_owner::_densesolverlsreport_owner()
{
    p_struct = (alglib_impl::densesolverlsreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::densesolverlsreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_densesolverlsreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_densesolverlsreport_owner::_densesolverlsreport_owner(const _densesolverlsreport_owner &rhs)
{
    p_struct = (alglib_impl::densesolverlsreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::densesolverlsreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_densesolverlsreport_init_copy(p_struct, const_cast<alglib_impl::densesolverlsreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_densesolverlsreport_owner& _densesolverlsreport_owner::operator=(const _densesolverlsreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_densesolverlsreport_clear(p_struct);
    if( !alglib_impl::_densesolverlsreport_init_copy(p_struct, const_cast<alglib_impl::densesolverlsreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_densesolverlsreport_owner::~_densesolverlsreport_owner()
{
    alglib_impl::_densesolverlsreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::densesolverlsreport* _densesolverlsreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::densesolverlsreport* _densesolverlsreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::densesolverlsreport*>(p_struct);
}
densesolverlsreport::densesolverlsreport() : _densesolverlsreport_owner() ,r2(p_struct->r2),cx(&p_struct->cx),n(p_struct->n),k(p_struct->k)
{
}

densesolverlsreport::densesolverlsreport(const densesolverlsreport &rhs):_densesolverlsreport_owner(rhs) ,r2(p_struct->r2),cx(&p_struct->cx),n(p_struct->n),k(p_struct->k)
{
}

densesolverlsreport& densesolverlsreport::operator=(const densesolverlsreport &rhs)
{
    if( this==&rhs )
        return *this;
    _densesolverlsreport_owner::operator=(rhs);
    return *this;
}

densesolverlsreport::~densesolverlsreport()
{
}

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix, x and b are vectors.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   return code:
                * -3    A is singular, or VERY close to singular.
                        X is filled by zeros in such cases.
                * -1    N<=0 was passed
                *  1    task is solved (but matrix A may be ill-conditioned,
                        check R1/RInf parameters for condition numbers).
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1], it contains:
                * solution of A*x=b if A is non-singular (well-conditioned
                  or ill-conditioned, but not very close to singular)
                * zeros,  if  A  is  singular  or  VERY  close to singular
                  (in this case Info=-3).

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* R1        reciprocal of condition number: 1/cond(A), 1-norm.
* RInf      reciprocal of condition number: 1/cond(A), inf-norm.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolve(const real_2d_array &a, const ae_int_t n, const real_1d_array &b, ae_int_t &info, densesolverreport &rep, real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::rmatrixsolve(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), n, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* optional iterative refinement
* O(N^3+M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvem(const real_2d_array &a, const ae_int_t n, const real_2d_array &b, const ae_int_t m, const bool rfs, ae_int_t &info, densesolverreport &rep, real_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::rmatrixsolvem(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), n, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, rfs, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*X=B,  where A is NxN non-denegerate
real matrix given by its LU decomposition, X and B are NxM real matrices.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolve(const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const real_1d_array &b, ae_int_t &info, densesolverreport &rep, real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::rmatrixlusolve(const_cast<alglib_impl::ae_matrix*>(lua.c_ptr()), const_cast<alglib_impl::ae_vector*>(p.c_ptr()), n, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves task with multiple right parts
(where b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolvem(const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const real_2d_array &b, const ae_int_t m, ae_int_t &info, densesolverreport &rep, real_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::rmatrixlusolvem(const_cast<alglib_impl::ae_matrix*>(lua.c_ptr()), const_cast<alglib_impl::ae_vector*>(p.c_ptr()), n, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where BOTH ORIGINAL A AND ITS
LU DECOMPOSITION ARE KNOWN. You can use it if for some  reasons  you  have
both A and its LU decomposition.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolveM
    Rep     -   same as in RMatrixSolveM
    X       -   same as in RMatrixSolveM

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolve(const real_2d_array &a, const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const real_1d_array &b, ae_int_t &info, densesolverreport &rep, real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::rmatrixmixedsolve(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), const_cast<alglib_impl::ae_matrix*>(lua.c_ptr()), const_cast<alglib_impl::ae_vector*>(p.c_ptr()), n, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver.

Similar to RMatrixMixedSolve() but  solves task with multiple right  parts
(where b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolveM
    Rep     -   same as in RMatrixSolveM
    X       -   same as in RMatrixSolveM

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolvem(const real_2d_array &a, const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const real_2d_array &b, const ae_int_t m, ae_int_t &info, densesolverreport &rep, real_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::rmatrixmixedsolvem(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), const_cast<alglib_impl::ae_matrix*>(lua.c_ptr()), const_cast<alglib_impl::ae_vector*>(p.c_ptr()), n, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3+M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolvem(const complex_2d_array &a, const ae_int_t n, const complex_2d_array &b, const ae_int_t m, const bool rfs, ae_int_t &info, densesolverreport &rep, complex_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::cmatrixsolvem(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), n, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, rfs, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolve(const complex_2d_array &a, const ae_int_t n, const complex_1d_array &b, ae_int_t &info, densesolverreport &rep, complex_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::cmatrixsolve(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), n, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixLUSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolvem(const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const complex_2d_array &b, const ae_int_t m, ae_int_t &info, densesolverreport &rep, complex_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::cmatrixlusolvem(const_cast<alglib_impl::ae_matrix*>(lua.c_ptr()), const_cast<alglib_impl::ae_vector*>(p.c_ptr()), n, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixLUSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolve(const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const complex_1d_array &b, ae_int_t &info, densesolverreport &rep, complex_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::cmatrixlusolve(const_cast<alglib_impl::ae_matrix*>(lua.c_ptr()), const_cast<alglib_impl::ae_vector*>(p.c_ptr()), n, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixMixedSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolveM
    Rep     -   same as in RMatrixSolveM
    X       -   same as in RMatrixSolveM

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolvem(const complex_2d_array &a, const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const complex_2d_array &b, const ae_int_t m, ae_int_t &info, densesolverreport &rep, complex_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::cmatrixmixedsolvem(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), const_cast<alglib_impl::ae_matrix*>(lua.c_ptr()), const_cast<alglib_impl::ae_vector*>(p.c_ptr()), n, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixMixedSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolveM
    Rep     -   same as in RMatrixSolveM
    X       -   same as in RMatrixSolveM

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolve(const complex_2d_array &a, const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const complex_1d_array &b, ae_int_t &info, densesolverreport &rep, complex_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::cmatrixmixedsolve(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), const_cast<alglib_impl::ae_matrix*>(lua.c_ptr()), const_cast<alglib_impl::ae_vector*>(p.c_ptr()), n, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixSolveM(), but for symmetric positive definite
matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve.
                Returns -3 for non-SPD matrices.
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolvem(const real_2d_array &a, const ae_int_t n, const bool isupper, const real_2d_array &b, const ae_int_t m, ae_int_t &info, densesolverreport &rep, real_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::spdmatrixsolvem(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), n, isupper, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixSolve(), but for SPD matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
                Returns -3 for non-SPD matrices.
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolve(const real_2d_array &a, const ae_int_t n, const bool isupper, const real_1d_array &b, ae_int_t &info, densesolverreport &rep, real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::spdmatrixsolve(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), n, isupper, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixLUSolveM(), but for SPD matrices  represented
by their Cholesky decomposition.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolvem(const real_2d_array &cha, const ae_int_t n, const bool isupper, const real_2d_array &b, const ae_int_t m, ae_int_t &info, densesolverreport &rep, real_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::spdmatrixcholeskysolvem(const_cast<alglib_impl::ae_matrix*>(cha.c_ptr()), n, isupper, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixLUSolve(), but for  SPD matrices  represented
by their Cholesky decomposition.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolve(const real_2d_array &cha, const ae_int_t n, const bool isupper, const real_1d_array &b, ae_int_t &info, densesolverreport &rep, real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::spdmatrixcholeskysolve(const_cast<alglib_impl::ae_matrix*>(cha.c_ptr()), n, isupper, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixSolveM(), but for Hermitian positive definite
matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve.
                Returns -3 for non-HPD matrices.
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolvem(const complex_2d_array &a, const ae_int_t n, const bool isupper, const complex_2d_array &b, const ae_int_t m, ae_int_t &info, densesolverreport &rep, complex_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::hpdmatrixsolvem(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), n, isupper, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixSolve(),  but for Hermitian positive definite
matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
                Returns -3 for non-HPD matrices.
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolve(const complex_2d_array &a, const ae_int_t n, const bool isupper, const complex_1d_array &b, ae_int_t &info, densesolverreport &rep, complex_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::hpdmatrixsolve(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), n, isupper, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixLUSolveM(), but for HPD matrices  represented
by their Cholesky decomposition.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolvem(const complex_2d_array &cha, const ae_int_t n, const bool isupper, const complex_2d_array &b, const ae_int_t m, ae_int_t &info, densesolverreport &rep, complex_2d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::hpdmatrixcholeskysolvem(const_cast<alglib_impl::ae_matrix*>(cha.c_ptr()), n, isupper, const_cast<alglib_impl::ae_matrix*>(b.c_ptr()), m, &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_matrix*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver. Same as RMatrixLUSolve(), but for  HPD matrices  represented
by their Cholesky decomposition.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolve(const complex_2d_array &cha, const ae_int_t n, const bool isupper, const complex_1d_array &b, ae_int_t &info, densesolverreport &rep, complex_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::hpdmatrixcholeskysolve(const_cast<alglib_impl::ae_matrix*>(cha.c_ptr()), n, isupper, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &info, const_cast<alglib_impl::densesolverreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Dense solver.

This subroutine finds solution of the linear system A*X=B with non-square,
possibly degenerate A.  System  is  solved in the least squares sense, and
general least squares solution  X = X0 + CX*y  which  minimizes |A*X-B| is
returned. If A is non-degenerate, solution in the usual sense is returned.

Algorithm features:
* automatic detection (and correct handling!) of degenerate cases
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..NRows-1,0..NCols-1], system matrix
    NRows   -   vertical size of A
    NCols   -   horizontal size of A
    B       -   array[0..NCols-1], right part
    Threshold-  a number in [0,1]. Singular values  beyond  Threshold  are
                considered  zero.  Set  it to 0.0, if you don't understand
                what it means, so the solver will choose good value on its
                own.

OUTPUT PARAMETERS
    Info    -   return code:
                * -4    SVD subroutine failed
                * -1    if NRows<=0 or NCols<=0 or Threshold<0 was passed
                *  1    if task is solved
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1,0..M-1], it contains:
                * solution of A*X=B (even for singular A)
                * zeros, if SVD subroutine failed

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* R2        reciprocal of condition number: 1/cond(A), 2-norm.
* N         = NCols
* K         dim(Null(A))
* CX        array[0..N-1,0..K-1], kernel of A.
            Columns of CX store such vectors that A*CX[i]=0.

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvels(const real_2d_array &a, const ae_int_t nrows, const ae_int_t ncols, const real_1d_array &b, const double threshold, ae_int_t &info, densesolverlsreport &rep, real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::rmatrixsolvels(const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), nrows, ncols, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), threshold, &info, const_cast<alglib_impl::densesolverlsreport*>(rep.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This object stores state of the LinLSQR method.

You should use ALGLIB functions to work with this object.
*************************************************************************/
_linlsqrstate_owner::_linlsqrstate_owner()
{
    p_struct = (alglib_impl::linlsqrstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::linlsqrstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_linlsqrstate_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_linlsqrstate_owner::_linlsqrstate_owner(const _linlsqrstate_owner &rhs)
{
    p_struct = (alglib_impl::linlsqrstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::linlsqrstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_linlsqrstate_init_copy(p_struct, const_cast<alglib_impl::linlsqrstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_linlsqrstate_owner& _linlsqrstate_owner::operator=(const _linlsqrstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_linlsqrstate_clear(p_struct);
    if( !alglib_impl::_linlsqrstate_init_copy(p_struct, const_cast<alglib_impl::linlsqrstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_linlsqrstate_owner::~_linlsqrstate_owner()
{
    alglib_impl::_linlsqrstate_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::linlsqrstate* _linlsqrstate_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::linlsqrstate* _linlsqrstate_owner::c_ptr() const
{
    return const_cast<alglib_impl::linlsqrstate*>(p_struct);
}
linlsqrstate::linlsqrstate() : _linlsqrstate_owner() 
{
}

linlsqrstate::linlsqrstate(const linlsqrstate &rhs):_linlsqrstate_owner(rhs) 
{
}

linlsqrstate& linlsqrstate::operator=(const linlsqrstate &rhs)
{
    if( this==&rhs )
        return *this;
    _linlsqrstate_owner::operator=(rhs);
    return *this;
}

linlsqrstate::~linlsqrstate()
{
}


/*************************************************************************

*************************************************************************/
_linlsqrreport_owner::_linlsqrreport_owner()
{
    p_struct = (alglib_impl::linlsqrreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::linlsqrreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_linlsqrreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_linlsqrreport_owner::_linlsqrreport_owner(const _linlsqrreport_owner &rhs)
{
    p_struct = (alglib_impl::linlsqrreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::linlsqrreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_linlsqrreport_init_copy(p_struct, const_cast<alglib_impl::linlsqrreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_linlsqrreport_owner& _linlsqrreport_owner::operator=(const _linlsqrreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_linlsqrreport_clear(p_struct);
    if( !alglib_impl::_linlsqrreport_init_copy(p_struct, const_cast<alglib_impl::linlsqrreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_linlsqrreport_owner::~_linlsqrreport_owner()
{
    alglib_impl::_linlsqrreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::linlsqrreport* _linlsqrreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::linlsqrreport* _linlsqrreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::linlsqrreport*>(p_struct);
}
linlsqrreport::linlsqrreport() : _linlsqrreport_owner() ,iterationscount(p_struct->iterationscount),nmv(p_struct->nmv),terminationtype(p_struct->terminationtype)
{
}

linlsqrreport::linlsqrreport(const linlsqrreport &rhs):_linlsqrreport_owner(rhs) ,iterationscount(p_struct->iterationscount),nmv(p_struct->nmv),terminationtype(p_struct->terminationtype)
{
}

linlsqrreport& linlsqrreport::operator=(const linlsqrreport &rhs)
{
    if( this==&rhs )
        return *this;
    _linlsqrreport_owner::operator=(rhs);
    return *this;
}

linlsqrreport::~linlsqrreport()
{
}

/*************************************************************************
This function initializes linear LSQR Solver. This solver is used to solve
non-symmetric (and, possibly, non-square) problems. Least squares solution
is returned for non-compatible systems.

USAGE:
1. User initializes algorithm state with LinLSQRCreate() call
2. User tunes solver parameters with  LinLSQRSetCond() and other functions
3. User  calls  LinLSQRSolveSparse()  function which takes algorithm state
   and SparseMatrix object.
4. User calls LinLSQRResults() to get solution
5. Optionally, user may call LinLSQRSolveSparse() again to  solve  another
   problem  with different matrix and/or right part without reinitializing
   LinLSQRState structure.

INPUT PARAMETERS:
    M       -   number of rows in A
    N       -   number of variables, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrcreate(const ae_int_t m, const ae_int_t n, linlsqrstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::linlsqrcreate(m, n, const_cast<alglib_impl::linlsqrstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  function  changes  preconditioning  settings of LinLSQQSolveSparse()
function. By default, SolveSparse() uses diagonal preconditioner,  but  if
you want to use solver without preconditioning, you can call this function
which forces solver to use unit matrix for preconditioning.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecunit(const linlsqrstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::linlsqrsetprecunit(const_cast<alglib_impl::linlsqrstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function.  LinCGSolveSparse() will use diagonal of the  system  matrix  as
preconditioner. This preconditioning mode is active by default.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecdiag(const linlsqrstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::linlsqrsetprecdiag(const_cast<alglib_impl::linlsqrstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets optional Tikhonov regularization coefficient.
It is zero by default.

INPUT PARAMETERS:
    LambdaI -   regularization factor, LambdaI>=0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetlambdai(const linlsqrstate &state, const double lambdai)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::linlsqrsetlambdai(const_cast<alglib_impl::linlsqrstate*>(state.c_ptr()), lambdai, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Procedure for solution of A*x=b with sparse A.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse M*N matrix in the CRS format (you MUST contvert  it
                to CRS format  by  calling  SparseConvertToCRS()  function
                BEFORE you pass it to this function).
    B       -   right part, array[M]

RESULT:
    This function returns no result.
    You can get solution by calling LinCGResults()

NOTE: this function uses lightweight preconditioning -  multiplication  by
      inverse of diag(A). If you want, you can turn preconditioning off by
      calling LinLSQRSetPrecUnit(). However, preconditioning cost is   low
      and preconditioner is very important for solution  of  badly  scaled
      problems.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsolvesparse(const linlsqrstate &state, const sparsematrix &a, const real_1d_array &b)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::linlsqrsolvesparse(const_cast<alglib_impl::linlsqrstate*>(state.c_ptr()), const_cast<alglib_impl::sparsematrix*>(a.c_ptr()), const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsA    -   algorithm will be stopped if ||A^T*Rk||/(||A||*||Rk||)<=EpsA.
    EpsB    -   algorithm will be stopped if ||Rk||<=EpsB*||B||
    MaxIts  -   algorithm will be stopped if number of iterations
                more than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE: if EpsA,EpsB,EpsC and MaxIts are zero then these variables will
be setted as default values.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetcond(const linlsqrstate &state, const double epsa, const double epsb, const ae_int_t maxits)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::linlsqrsetcond(const_cast<alglib_impl::linlsqrstate*>(state.c_ptr()), epsa, epsb, maxits, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
LSQR solver: results.

This function must be called after LinLSQRSolve

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    *  1    ||Rk||<=EpsB*||B||
                    *  4    ||A^T*Rk||/(||A||*||Rk||)<=EpsA
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            X contains best point found so far.
                            (sometimes returned on singular systems)
                * Rep.IterationsCount contains iterations count
                * NMV countains number of matrix-vector calculations

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrresults(const linlsqrstate &state, real_1d_array &x, linlsqrreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::linlsqrresults(const_cast<alglib_impl::linlsqrstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::linlsqrreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetxrep(const linlsqrstate &state, const bool needxrep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::linlsqrsetxrep(const_cast<alglib_impl::linlsqrstate*>(state.c_ptr()), needxrep, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This object stores state of the linear CG method.

You should use ALGLIB functions to work with this object.
Never try to access its fields directly!
*************************************************************************/
_lincgstate_owner::_lincgstate_owner()
{
    p_struct = (alglib_impl::lincgstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::lincgstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_lincgstate_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_lincgstate_owner::_lincgstate_owner(const _lincgstate_owner &rhs)
{
    p_struct = (alglib_impl::lincgstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::lincgstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_lincgstate_init_copy(p_struct, const_cast<alglib_impl::lincgstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_lincgstate_owner& _lincgstate_owner::operator=(const _lincgstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_lincgstate_clear(p_struct);
    if( !alglib_impl::_lincgstate_init_copy(p_struct, const_cast<alglib_impl::lincgstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_lincgstate_owner::~_lincgstate_owner()
{
    alglib_impl::_lincgstate_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::lincgstate* _lincgstate_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::lincgstate* _lincgstate_owner::c_ptr() const
{
    return const_cast<alglib_impl::lincgstate*>(p_struct);
}
lincgstate::lincgstate() : _lincgstate_owner() 
{
}

lincgstate::lincgstate(const lincgstate &rhs):_lincgstate_owner(rhs) 
{
}

lincgstate& lincgstate::operator=(const lincgstate &rhs)
{
    if( this==&rhs )
        return *this;
    _lincgstate_owner::operator=(rhs);
    return *this;
}

lincgstate::~lincgstate()
{
}


/*************************************************************************

*************************************************************************/
_lincgreport_owner::_lincgreport_owner()
{
    p_struct = (alglib_impl::lincgreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::lincgreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_lincgreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_lincgreport_owner::_lincgreport_owner(const _lincgreport_owner &rhs)
{
    p_struct = (alglib_impl::lincgreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::lincgreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_lincgreport_init_copy(p_struct, const_cast<alglib_impl::lincgreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_lincgreport_owner& _lincgreport_owner::operator=(const _lincgreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_lincgreport_clear(p_struct);
    if( !alglib_impl::_lincgreport_init_copy(p_struct, const_cast<alglib_impl::lincgreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_lincgreport_owner::~_lincgreport_owner()
{
    alglib_impl::_lincgreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::lincgreport* _lincgreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::lincgreport* _lincgreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::lincgreport*>(p_struct);
}
lincgreport::lincgreport() : _lincgreport_owner() ,iterationscount(p_struct->iterationscount),nmv(p_struct->nmv),terminationtype(p_struct->terminationtype),r2(p_struct->r2)
{
}

lincgreport::lincgreport(const lincgreport &rhs):_lincgreport_owner(rhs) ,iterationscount(p_struct->iterationscount),nmv(p_struct->nmv),terminationtype(p_struct->terminationtype),r2(p_struct->r2)
{
}

lincgreport& lincgreport::operator=(const lincgreport &rhs)
{
    if( this==&rhs )
        return *this;
    _lincgreport_owner::operator=(rhs);
    return *this;
}

lincgreport::~lincgreport()
{
}

/*************************************************************************
This function initializes linear CG Solver. This solver is used  to  solve
symmetric positive definite problems. If you want  to  solve  nonsymmetric
(or non-positive definite) problem you may use LinLSQR solver provided  by
ALGLIB.

USAGE:
1. User initializes algorithm state with LinCGCreate() call
2. User tunes solver parameters with  LinCGSetCond() and other functions
3. Optionally, user sets starting point with LinCGSetStartingPoint()
4. User  calls LinCGSolveSparse() function which takes algorithm state and
   SparseMatrix object.
5. User calls LinCGResults() to get solution
6. Optionally, user may call LinCGSolveSparse()  again  to  solve  another
   problem  with different matrix and/or right part without reinitializing
   LinCGState structure.

INPUT PARAMETERS:
    N       -   problem dimension, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgcreate(const ae_int_t n, lincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgcreate(n, const_cast<alglib_impl::lincgstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets starting point.
By default, zero starting point is used.

INPUT PARAMETERS:
    X       -   starting point, array[N]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetstartingpoint(const lincgstate &state, const real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgsetstartingpoint(const_cast<alglib_impl::lincgstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function. By default, SolveSparse() uses diagonal preconditioner,  but  if
you want to use solver without preconditioning, you can call this function
which forces solver to use unit matrix for preconditioning.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void lincgsetprecunit(const lincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgsetprecunit(const_cast<alglib_impl::lincgstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function.  LinCGSolveSparse() will use diagonal of the  system  matrix  as
preconditioner. This preconditioning mode is active by default.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void lincgsetprecdiag(const lincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgsetprecdiag(const_cast<alglib_impl::lincgstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsF    -   algorithm will be stopped if norm of residual is less than
                EpsF*||b||.
    MaxIts  -   algorithm will be stopped if number of iterations is  more
                than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
If  both  EpsF  and  MaxIts  are  zero then small EpsF will be set to small
value.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetcond(const lincgstate &state, const double epsf, const ae_int_t maxits)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgsetcond(const_cast<alglib_impl::lincgstate*>(state.c_ptr()), epsf, maxits, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Procedure for solution of A*x=b with sparse A.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse matrix in the CRS format (you MUST contvert  it  to
                CRS format by calling SparseConvertToCRS() function).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get solution by calling LinCGResults()

NOTE: this function uses lightweight preconditioning -  multiplication  by
      inverse of diag(A). If you want, you can turn preconditioning off by
      calling LinCGSetPrecUnit(). However, preconditioning cost is low and
      preconditioner  is  very  important  for  solution  of  badly scaled
      problems.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsolvesparse(const lincgstate &state, const sparsematrix &a, const bool isupper, const real_1d_array &b)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgsolvesparse(const_cast<alglib_impl::lincgstate*>(state.c_ptr()), const_cast<alglib_impl::sparsematrix*>(a.c_ptr()), isupper, const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
CG-solver: results.

This function must be called after LinCGSolve

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -5    input matrix is either not positive definite,
                            too large or too small
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                * Rep.IterationsCount contains iterations count
                * NMV countains number of matrix-vector calculations

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgresults(const lincgstate &state, real_1d_array &x, lincgreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgresults(const_cast<alglib_impl::lincgstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::lincgreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets restart frequency. By default, algorithm  is  restarted
after N subsequent iterations.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetrestartfreq(const lincgstate &state, const ae_int_t srf)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgsetrestartfreq(const_cast<alglib_impl::lincgstate*>(state.c_ptr()), srf, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets frequency of residual recalculations.

Algorithm updates residual r_k using iterative formula,  but  recalculates
it from scratch after each 10 iterations. It is done to avoid accumulation
of numerical errors and to stop algorithm when r_k starts to grow.

Such low update frequence (1/10) gives very  little  overhead,  but  makes
algorithm a bit more robust against numerical errors. However, you may
change it

INPUT PARAMETERS:
    Freq    -   desired update frequency, Freq>=0.
                Zero value means that no updates will be done.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetrupdatefreq(const lincgstate &state, const ae_int_t freq)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgsetrupdatefreq(const_cast<alglib_impl::lincgstate*>(state.c_ptr()), freq, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetxrep(const lincgstate &state, const bool needxrep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::lincgsetxrep(const_cast<alglib_impl::lincgstate*>(state.c_ptr()), needxrep, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************

*************************************************************************/
_nleqstate_owner::_nleqstate_owner()
{
    p_struct = (alglib_impl::nleqstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::nleqstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_nleqstate_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_nleqstate_owner::_nleqstate_owner(const _nleqstate_owner &rhs)
{
    p_struct = (alglib_impl::nleqstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::nleqstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_nleqstate_init_copy(p_struct, const_cast<alglib_impl::nleqstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_nleqstate_owner& _nleqstate_owner::operator=(const _nleqstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_nleqstate_clear(p_struct);
    if( !alglib_impl::_nleqstate_init_copy(p_struct, const_cast<alglib_impl::nleqstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_nleqstate_owner::~_nleqstate_owner()
{
    alglib_impl::_nleqstate_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::nleqstate* _nleqstate_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::nleqstate* _nleqstate_owner::c_ptr() const
{
    return const_cast<alglib_impl::nleqstate*>(p_struct);
}
nleqstate::nleqstate() : _nleqstate_owner() ,needf(p_struct->needf),needfij(p_struct->needfij),xupdated(p_struct->xupdated),f(p_struct->f),fi(&p_struct->fi),j(&p_struct->j),x(&p_struct->x)
{
}

nleqstate::nleqstate(const nleqstate &rhs):_nleqstate_owner(rhs) ,needf(p_struct->needf),needfij(p_struct->needfij),xupdated(p_struct->xupdated),f(p_struct->f),fi(&p_struct->fi),j(&p_struct->j),x(&p_struct->x)
{
}

nleqstate& nleqstate::operator=(const nleqstate &rhs)
{
    if( this==&rhs )
        return *this;
    _nleqstate_owner::operator=(rhs);
    return *this;
}

nleqstate::~nleqstate()
{
}


/*************************************************************************

*************************************************************************/
_nleqreport_owner::_nleqreport_owner()
{
    p_struct = (alglib_impl::nleqreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::nleqreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_nleqreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_nleqreport_owner::_nleqreport_owner(const _nleqreport_owner &rhs)
{
    p_struct = (alglib_impl::nleqreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::nleqreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_nleqreport_init_copy(p_struct, const_cast<alglib_impl::nleqreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_nleqreport_owner& _nleqreport_owner::operator=(const _nleqreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_nleqreport_clear(p_struct);
    if( !alglib_impl::_nleqreport_init_copy(p_struct, const_cast<alglib_impl::nleqreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_nleqreport_owner::~_nleqreport_owner()
{
    alglib_impl::_nleqreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::nleqreport* _nleqreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::nleqreport* _nleqreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::nleqreport*>(p_struct);
}
nleqreport::nleqreport() : _nleqreport_owner() ,iterationscount(p_struct->iterationscount),nfunc(p_struct->nfunc),njac(p_struct->njac),terminationtype(p_struct->terminationtype)
{
}

nleqreport::nleqreport(const nleqreport &rhs):_nleqreport_owner(rhs) ,iterationscount(p_struct->iterationscount),nfunc(p_struct->nfunc),njac(p_struct->njac),terminationtype(p_struct->terminationtype)
{
}

nleqreport& nleqreport::operator=(const nleqreport &rhs)
{
    if( this==&rhs )
        return *this;
    _nleqreport_owner::operator=(rhs);
    return *this;
}

nleqreport::~nleqreport()
{
}

/*************************************************************************
                LEVENBERG-MARQUARDT-LIKE NONLINEAR SOLVER

DESCRIPTION:
This algorithm solves system of nonlinear equations
    F[0](x[0], ..., x[n-1])   = 0
    F[1](x[0], ..., x[n-1])   = 0
    ...
    F[M-1](x[0], ..., x[n-1]) = 0
with M/N do not necessarily coincide.  Algorithm  converges  quadratically
under following conditions:
    * the solution set XS is nonempty
    * for some xs in XS there exist such neighbourhood N(xs) that:
      * vector function F(x) and its Jacobian J(x) are continuously
        differentiable on N
      * ||F(x)|| provides local error bound on N, i.e. there  exists  such
        c1, that ||F(x)||>c1*distance(x,XS)
Note that these conditions are much more weaker than usual non-singularity
conditions. For example, algorithm will converge for any  affine  function
F (whether its Jacobian singular or not).


REQUIREMENTS:
Algorithm will request following information during its operation:
* function vector F[] and Jacobian matrix at given point X
* value of merit function f(x)=F[0]^2(x)+...+F[M-1]^2(x) at given point X


USAGE:
1. User initializes algorithm state with NLEQCreateLM() call
2. User tunes solver parameters with  NLEQSetCond(),  NLEQSetStpMax()  and
   other functions
3. User  calls  NLEQSolve()  function  which  takes  algorithm  state  and
   pointers (delegates, etc.) to callback functions which calculate  merit
   function value and Jacobian.
4. User calls NLEQResults() to get solution
5. Optionally, user may call NLEQRestartFrom() to  solve  another  problem
   with same parameters (N/M) but another starting  point  and/or  another
   function vector. NLEQRestartFrom() allows to reuse already  initialized
   structure.


INPUT PARAMETERS:
    N       -   space dimension, N>1:
                * if provided, only leading N elements of X are used
                * if not provided, determined automatically from size of X
    M       -   system size
    X       -   starting point


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


NOTES:
1. you may tune stopping conditions with NLEQSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use NLEQSetStpMax() function to bound algorithm's steps.
3. this  algorithm  is  a  slightly  modified implementation of the method
   described  in  'Levenberg-Marquardt  method  for constrained  nonlinear
   equations with strong local convergence properties' by Christian Kanzow
   Nobuo Yamashita and Masao Fukushima and further  developed  in  'On the
   convergence of a New Levenberg-Marquardt Method'  by  Jin-yan  Fan  and
   Ya-Xiang Yuan.


  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqcreatelm(const ae_int_t n, const ae_int_t m, const real_1d_array &x, nleqstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::nleqcreatelm(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::nleqstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
                LEVENBERG-MARQUARDT-LIKE NONLINEAR SOLVER

DESCRIPTION:
This algorithm solves system of nonlinear equations
    F[0](x[0], ..., x[n-1])   = 0
    F[1](x[0], ..., x[n-1])   = 0
    ...
    F[M-1](x[0], ..., x[n-1]) = 0
with M/N do not necessarily coincide.  Algorithm  converges  quadratically
under following conditions:
    * the solution set XS is nonempty
    * for some xs in XS there exist such neighbourhood N(xs) that:
      * vector function F(x) and its Jacobian J(x) are continuously
        differentiable on N
      * ||F(x)|| provides local error bound on N, i.e. there  exists  such
        c1, that ||F(x)||>c1*distance(x,XS)
Note that these conditions are much more weaker than usual non-singularity
conditions. For example, algorithm will converge for any  affine  function
F (whether its Jacobian singular or not).


REQUIREMENTS:
Algorithm will request following information during its operation:
* function vector F[] and Jacobian matrix at given point X
* value of merit function f(x)=F[0]^2(x)+...+F[M-1]^2(x) at given point X


USAGE:
1. User initializes algorithm state with NLEQCreateLM() call
2. User tunes solver parameters with  NLEQSetCond(),  NLEQSetStpMax()  and
   other functions
3. User  calls  NLEQSolve()  function  which  takes  algorithm  state  and
   pointers (delegates, etc.) to callback functions which calculate  merit
   function value and Jacobian.
4. User calls NLEQResults() to get solution
5. Optionally, user may call NLEQRestartFrom() to  solve  another  problem
   with same parameters (N/M) but another starting  point  and/or  another
   function vector. NLEQRestartFrom() allows to reuse already  initialized
   structure.


INPUT PARAMETERS:
    N       -   space dimension, N>1:
                * if provided, only leading N elements of X are used
                * if not provided, determined automatically from size of X
    M       -   system size
    X       -   starting point


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


NOTES:
1. you may tune stopping conditions with NLEQSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use NLEQSetStpMax() function to bound algorithm's steps.
3. this  algorithm  is  a  slightly  modified implementation of the method
   described  in  'Levenberg-Marquardt  method  for constrained  nonlinear
   equations with strong local convergence properties' by Christian Kanzow
   Nobuo Yamashita and Masao Fukushima and further  developed  in  'On the
   convergence of a New Levenberg-Marquardt Method'  by  Jin-yan  Fan  and
   Ya-Xiang Yuan.


  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqcreatelm(const ae_int_t m, const real_1d_array &x, nleqstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::nleqcreatelm(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::nleqstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets stopping conditions for the nonlinear solver

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsF    -   >=0
                The subroutine finishes  its work if on k+1-th iteration
                the condition ||F||<=EpsF is satisfied
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsF=0 and MaxIts=0 simultaneously will lead to  automatic
stopping criterion selection (small EpsF).

NOTES:

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetcond(const nleqstate &state, const double epsf, const ae_int_t maxits)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::nleqsetcond(const_cast<alglib_impl::nleqstate*>(state.c_ptr()), epsf, maxits, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to NLEQSolve().

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetxrep(const nleqstate &state, const bool needxrep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::nleqsetxrep(const_cast<alglib_impl::nleqstate*>(state.c_ptr()), needxrep, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when target function  contains  exp()  or  other  fast
growing functions, and algorithm makes  too  large  steps  which  lead  to
overflow. This function allows us to reject steps that are too large  (and
therefore expose us to the possible overflow) without actually calculating
function value at the x+stp*d.

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetstpmax(const nleqstate &state, const double stpmax)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::nleqsetstpmax(const_cast<alglib_impl::nleqstate*>(state.c_ptr()), stpmax, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool nleqiteration(const nleqstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        ae_bool result = alglib_impl::nleqiteration(const_cast<alglib_impl::nleqstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return *(reinterpret_cast<bool*>(&result));
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void nleqsolve(nleqstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( func==NULL )
        throw ap_error("ALGLIB: error in 'nleqsolve()' (func is NULL)");
    if( jac==NULL )
        throw ap_error("ALGLIB: error in 'nleqsolve()' (jac is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::nleqiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needf )
            {
                func(state.x, state.f, ptr);
                continue;
            }
            if( state.needfij )
            {
                jac(state.x, state.fi, state.j, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'nleqsolve' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}



/*************************************************************************
NLEQ solver results

INPUT PARAMETERS:
    State   -   algorithm state.

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -4    ERROR:  algorithm   has   converged   to   the
                            stationary point Xf which is local minimum  of
                            f=F[0]^2+...+F[m-1]^2, but is not solution  of
                            nonlinear system.
                    *  1    sqrt(f)<=EpsF.
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations
                * ActiveConstraints contains number of active constraints

  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqresults(const nleqstate &state, real_1d_array &x, nleqreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::nleqresults(const_cast<alglib_impl::nleqstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::nleqreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
NLEQ solver results

Buffered implementation of NLEQResults(), which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqresultsbuf(const nleqstate &state, real_1d_array &x, nleqreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::nleqresultsbuf(const_cast<alglib_impl::nleqstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::nleqreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  subroutine  restarts  CG  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used for reverse communication previously
                allocated with MinCGCreate call.
    X       -   new starting point.
    BndL    -   new lower bounds
    BndU    -   new upper bounds

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void nleqrestartfrom(const nleqstate &state, const real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::nleqrestartfrom(const_cast<alglib_impl::nleqstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}
}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS IMPLEMENTATION OF COMPUTATIONAL CORE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
static void densesolver_rmatrixlusolveinternal(/* Real    */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     double scalea,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_bool havea,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_matrix* x,
     ae_state *_state);
static void densesolver_spdmatrixcholeskysolveinternal(/* Real    */ ae_matrix* cha,
     double sqrtscalea,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* a,
     ae_bool havea,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_matrix* x,
     ae_state *_state);
static void densesolver_cmatrixlusolveinternal(/* Complex */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     double scalea,
     ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_bool havea,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_matrix* x,
     ae_state *_state);
static void densesolver_hpdmatrixcholeskysolveinternal(/* Complex */ ae_matrix* cha,
     double sqrtscalea,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_matrix* a,
     ae_bool havea,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_matrix* x,
     ae_state *_state);
static ae_int_t densesolver_densesolverrfsmax(ae_int_t n,
     double r1,
     double rinf,
     ae_state *_state);
static ae_int_t densesolver_densesolverrfsmaxv2(ae_int_t n,
     double r2,
     ae_state *_state);
static void densesolver_rbasiclusolve(/* Real    */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     double scalea,
     ae_int_t n,
     /* Real    */ ae_vector* xb,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);
static void densesolver_spdbasiccholeskysolve(/* Real    */ ae_matrix* cha,
     double sqrtscalea,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* xb,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);
static void densesolver_cbasiclusolve(/* Complex */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     double scalea,
     ae_int_t n,
     /* Complex */ ae_vector* xb,
     /* Complex */ ae_vector* tmp,
     ae_state *_state);
static void densesolver_hpdbasiccholeskysolve(/* Complex */ ae_matrix* cha,
     double sqrtscalea,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* xb,
     /* Complex */ ae_vector* tmp,
     ae_state *_state);


static double linlsqr_atol = 1.0E-6;
static double linlsqr_btol = 1.0E-6;
static void linlsqr_clearrfields(linlsqrstate* state, ae_state *_state);


static double lincg_defaultprecision = 1.0E-6;
static void lincg_clearrfields(lincgstate* state, ae_state *_state);
static void lincg_updateitersdata(lincgstate* state, ae_state *_state);


static void nleq_clearrequestfields(nleqstate* state, ae_state *_state);
static ae_bool nleq_increaselambda(double* lambdav,
     double* nu,
     double lambdaup,
     ae_state *_state);
static void nleq_decreaselambda(double* lambdav,
     double* nu,
     double lambdadown,
     ae_state *_state);





/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix, x and b are vectors.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   return code:
                * -3    A is singular, or VERY close to singular.
                        X is filled by zeros in such cases.
                * -1    N<=0 was passed
                *  1    task is solved (but matrix A may be ill-conditioned,
                        check R1/RInf parameters for condition numbers).
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1], it contains:
                * solution of A*x=b if A is non-singular (well-conditioned
                  or ill-conditioned, but not very close to singular)
                * zeros,  if  A  is  singular  or  VERY  close to singular
                  (in this case Info=-3).

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* R1        reciprocal of condition number: 1/cond(A), 1-norm.
* RInf      reciprocal of condition number: 1/cond(A), inf-norm.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolve(/* Real    */ ae_matrix* a,
     ae_int_t n,
     /* Real    */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_move(&bm.ptr.pp_double[0][0], bm.stride, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    rmatrixsolvem(a, n, &bm, 1, ae_true, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_move(&x->ptr.p_double[0], 1, &xm.ptr.pp_double[0][0], xm.stride, ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* optional iterative refinement
* O(N^3+M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvem(/* Real    */ ae_matrix* a,
     ae_int_t n,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_bool rfs,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_matrix emptya;
    ae_vector p;
    double scalea;
    ae_int_t i;
    ae_int_t j;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_matrix_init(&da, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&emptya, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * 1. scale matrix, max(|A[i,j]|)
     * 2. factorize scaled matrix
     * 3. solve
     */
    scalea = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            scalea = ae_maxreal(scalea, ae_fabs(a->ptr.pp_double[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(scalea,0) )
    {
        scalea = 1;
    }
    scalea = 1/scalea;
    for(i=0; i<=n-1; i++)
    {
        ae_v_move(&da.ptr.pp_double[i][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
    }
    rmatrixlu(&da, n, n, &p, _state);
    if( rfs )
    {
        densesolver_rmatrixlusolveinternal(&da, &p, scalea, n, a, ae_true, b, m, info, rep, x, _state);
    }
    else
    {
        densesolver_rmatrixlusolveinternal(&da, &p, scalea, n, &emptya, ae_false, b, m, info, rep, x, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*X=B,  where A is NxN non-denegerate
real matrix given by its LU decomposition, X and B are NxM real matrices.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve
    
  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolve(/* Real    */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_move(&bm.ptr.pp_double[0][0], bm.stride, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    rmatrixlusolvem(lua, p, n, &bm, 1, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_move(&x->ptr.p_double[0], 1, &xm.ptr.pp_double[0][0], xm.stride, ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves task with multiple right parts
(where b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolvem(/* Real    */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;
    ae_int_t i;
    ae_int_t j;
    double scalea;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_matrix_init(&emptya, 0, 0, DT_REAL, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * 1. scale matrix, max(|U[i,j]|)
     *    we assume that LU is in its normal form, i.e. |L[i,j]|<=1
     * 2. solve
     */
    scalea = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=i; j<=n-1; j++)
        {
            scalea = ae_maxreal(scalea, ae_fabs(lua->ptr.pp_double[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(scalea,0) )
    {
        scalea = 1;
    }
    scalea = 1/scalea;
    densesolver_rmatrixlusolveinternal(lua, p, scalea, n, &emptya, ae_false, b, m, info, rep, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where BOTH ORIGINAL A AND ITS
LU DECOMPOSITION ARE KNOWN. You can use it if for some  reasons  you  have
both A and its LU decomposition.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolveM
    Rep     -   same as in RMatrixSolveM
    X       -   same as in RMatrixSolveM

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolve(/* Real    */ ae_matrix* a,
     /* Real    */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_move(&bm.ptr.pp_double[0][0], bm.stride, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    rmatrixmixedsolvem(a, lua, p, n, &bm, 1, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_move(&x->ptr.p_double[0], 1, &xm.ptr.pp_double[0][0], xm.stride, ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

Similar to RMatrixMixedSolve() but  solves task with multiple right  parts
(where b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolveM
    Rep     -   same as in RMatrixSolveM
    X       -   same as in RMatrixSolveM

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolvem(/* Real    */ ae_matrix* a,
     /* Real    */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_matrix* x,
     ae_state *_state)
{
    double scalea;
    ae_int_t i;
    ae_int_t j;

    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        return;
    }
    
    /*
     * 1. scale matrix, max(|A[i,j]|)
     * 2. factorize scaled matrix
     * 3. solve
     */
    scalea = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            scalea = ae_maxreal(scalea, ae_fabs(a->ptr.pp_double[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(scalea,0) )
    {
        scalea = 1;
    }
    scalea = 1/scalea;
    densesolver_rmatrixlusolveinternal(lua, p, scalea, n, a, ae_true, b, m, info, rep, x, _state);
}


/*************************************************************************
Dense solver. Same as RMatrixSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3+M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolvem(/* Complex */ ae_matrix* a,
     ae_int_t n,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_bool rfs,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_matrix emptya;
    ae_vector p;
    double scalea;
    ae_int_t i;
    ae_int_t j;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_matrix_init(&da, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&emptya, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * 1. scale matrix, max(|A[i,j]|)
     * 2. factorize scaled matrix
     * 3. solve
     */
    scalea = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            scalea = ae_maxreal(scalea, ae_c_abs(a->ptr.pp_complex[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(scalea,0) )
    {
        scalea = 1;
    }
    scalea = 1/scalea;
    for(i=0; i<=n-1; i++)
    {
        ae_v_cmove(&da.ptr.pp_complex[i][0], 1, &a->ptr.pp_complex[i][0], 1, "N", ae_v_len(0,n-1));
    }
    cmatrixlu(&da, n, n, &p, _state);
    if( rfs )
    {
        densesolver_cmatrixlusolveinternal(&da, &p, scalea, n, a, ae_true, b, m, info, rep, x, _state);
    }
    else
    {
        densesolver_cmatrixlusolveinternal(&da, &p, scalea, n, &emptya, ae_false, b, m, info, rep, x, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolve(/* Complex */ ae_matrix* a,
     ae_int_t n,
     /* Complex */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    cmatrixsolvem(a, n, &bm, 1, ae_true, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixLUSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolvem(/* Complex */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;
    ae_int_t i;
    ae_int_t j;
    double scalea;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_matrix_init(&emptya, 0, 0, DT_COMPLEX, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * 1. scale matrix, max(|U[i,j]|)
     *    we assume that LU is in its normal form, i.e. |L[i,j]|<=1
     * 2. solve
     */
    scalea = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=i; j<=n-1; j++)
        {
            scalea = ae_maxreal(scalea, ae_c_abs(lua->ptr.pp_complex[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(scalea,0) )
    {
        scalea = 1;
    }
    scalea = 1/scalea;
    densesolver_cmatrixlusolveinternal(lua, p, scalea, n, &emptya, ae_false, b, m, info, rep, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixLUSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolve(/* Complex */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    cmatrixlusolvem(lua, p, n, &bm, 1, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixMixedSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolveM
    Rep     -   same as in RMatrixSolveM
    X       -   same as in RMatrixSolveM

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolvem(/* Complex */ ae_matrix* a,
     /* Complex */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_matrix* x,
     ae_state *_state)
{
    double scalea;
    ae_int_t i;
    ae_int_t j;

    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        return;
    }
    
    /*
     * 1. scale matrix, max(|A[i,j]|)
     * 2. factorize scaled matrix
     * 3. solve
     */
    scalea = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            scalea = ae_maxreal(scalea, ae_c_abs(a->ptr.pp_complex[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(scalea,0) )
    {
        scalea = 1;
    }
    scalea = 1/scalea;
    densesolver_cmatrixlusolveinternal(lua, p, scalea, n, a, ae_true, b, m, info, rep, x, _state);
}


/*************************************************************************
Dense solver. Same as RMatrixMixedSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolveM
    Rep     -   same as in RMatrixSolveM
    X       -   same as in RMatrixSolveM

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolve(/* Complex */ ae_matrix* a,
     /* Complex */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    cmatrixmixedsolvem(a, lua, p, n, &bm, 1, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixSolveM(), but for symmetric positive definite
matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve.
                Returns -3 for non-SPD matrices.
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolvem(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    double sqrtscalea;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_matrix_init(&da, 0, 0, DT_REAL, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * 1. scale matrix, max(|A[i,j]|)
     * 2. factorize scaled matrix
     * 3. solve
     */
    sqrtscalea = 0;
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        for(j=j1; j<=j2; j++)
        {
            sqrtscalea = ae_maxreal(sqrtscalea, ae_fabs(a->ptr.pp_double[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(sqrtscalea,0) )
    {
        sqrtscalea = 1;
    }
    sqrtscalea = 1/sqrtscalea;
    sqrtscalea = ae_sqrt(sqrtscalea, _state);
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        ae_v_move(&da.ptr.pp_double[i][j1], 1, &a->ptr.pp_double[i][j1], 1, ae_v_len(j1,j2));
    }
    if( !spdmatrixcholesky(&da, n, isupper, _state) )
    {
        ae_matrix_set_length(x, n, m, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_double[i][j] = 0;
            }
        }
        rep->r1 = 0;
        rep->rinf = 0;
        *info = -3;
        ae_frame_leave(_state);
        return;
    }
    *info = 1;
    densesolver_spdmatrixcholeskysolveinternal(&da, sqrtscalea, n, isupper, a, ae_true, b, m, info, rep, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixSolve(), but for SPD matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
                Returns -3 for non-SPD matrices.
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolve(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_move(&bm.ptr.pp_double[0][0], bm.stride, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    spdmatrixsolvem(a, n, isupper, &bm, 1, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_move(&x->ptr.p_double[0], 1, &xm.ptr.pp_double[0][0], xm.stride, ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixLUSolveM(), but for SPD matrices  represented
by their Cholesky decomposition.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolvem(/* Real    */ ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;
    double sqrtscalea;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_matrix_init(&emptya, 0, 0, DT_REAL, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * 1. scale matrix, max(|U[i,j]|)
     * 2. factorize scaled matrix
     * 3. solve
     */
    sqrtscalea = 0;
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        for(j=j1; j<=j2; j++)
        {
            sqrtscalea = ae_maxreal(sqrtscalea, ae_fabs(cha->ptr.pp_double[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(sqrtscalea,0) )
    {
        sqrtscalea = 1;
    }
    sqrtscalea = 1/sqrtscalea;
    densesolver_spdmatrixcholeskysolveinternal(cha, sqrtscalea, n, isupper, &emptya, ae_false, b, m, info, rep, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixLUSolve(), but for  SPD matrices  represented
by their Cholesky decomposition.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolve(/* Real    */ ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_move(&bm.ptr.pp_double[0][0], bm.stride, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    spdmatrixcholeskysolvem(cha, n, isupper, &bm, 1, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_move(&x->ptr.p_double[0], 1, &xm.ptr.pp_double[0][0], xm.stride, ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixSolveM(), but for Hermitian positive definite
matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve.
                Returns -3 for non-HPD matrices.
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolvem(/* Complex */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    double sqrtscalea;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_matrix_init(&da, 0, 0, DT_COMPLEX, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * 1. scale matrix, max(|A[i,j]|)
     * 2. factorize scaled matrix
     * 3. solve
     */
    sqrtscalea = 0;
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        for(j=j1; j<=j2; j++)
        {
            sqrtscalea = ae_maxreal(sqrtscalea, ae_c_abs(a->ptr.pp_complex[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(sqrtscalea,0) )
    {
        sqrtscalea = 1;
    }
    sqrtscalea = 1/sqrtscalea;
    sqrtscalea = ae_sqrt(sqrtscalea, _state);
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        ae_v_cmove(&da.ptr.pp_complex[i][j1], 1, &a->ptr.pp_complex[i][j1], 1, "N", ae_v_len(j1,j2));
    }
    if( !hpdmatrixcholesky(&da, n, isupper, _state) )
    {
        ae_matrix_set_length(x, n, m, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_complex[i][j] = ae_complex_from_d(0);
            }
        }
        rep->r1 = 0;
        rep->rinf = 0;
        *info = -3;
        ae_frame_leave(_state);
        return;
    }
    *info = 1;
    densesolver_hpdmatrixcholeskysolveinternal(&da, sqrtscalea, n, isupper, a, ae_true, b, m, info, rep, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixSolve(),  but for Hermitian positive definite
matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
                Returns -3 for non-HPD matrices.
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolve(/* Complex */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    hpdmatrixsolvem(a, n, isupper, &bm, 1, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixLUSolveM(), but for HPD matrices  represented
by their Cholesky decomposition.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolvem(/* Complex */ ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;
    double sqrtscalea;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_matrix_init(&emptya, 0, 0, DT_COMPLEX, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * 1. scale matrix, max(|U[i,j]|)
     * 2. factorize scaled matrix
     * 3. solve
     */
    sqrtscalea = 0;
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        for(j=j1; j<=j2; j++)
        {
            sqrtscalea = ae_maxreal(sqrtscalea, ae_c_abs(cha->ptr.pp_complex[i][j], _state), _state);
        }
    }
    if( ae_fp_eq(sqrtscalea,0) )
    {
        sqrtscalea = 1;
    }
    sqrtscalea = 1/sqrtscalea;
    densesolver_hpdmatrixcholeskysolveinternal(cha, sqrtscalea, n, isupper, &emptya, ae_false, b, m, info, rep, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver. Same as RMatrixLUSolve(), but for  HPD matrices  represented
by their Cholesky decomposition.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Info    -   same as in RMatrixSolve
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolve(/* Complex */ ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* b,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_vector_clear(x);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    if( n<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    hpdmatrixcholeskysolvem(cha, n, isupper, &bm, 1, info, rep, &xm, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

This subroutine finds solution of the linear system A*X=B with non-square,
possibly degenerate A.  System  is  solved in the least squares sense, and
general least squares solution  X = X0 + CX*y  which  minimizes |A*X-B| is
returned. If A is non-degenerate, solution in the usual sense is returned.

Algorithm features:
* automatic detection (and correct handling!) of degenerate cases
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..NRows-1,0..NCols-1], system matrix
    NRows   -   vertical size of A
    NCols   -   horizontal size of A
    B       -   array[0..NCols-1], right part
    Threshold-  a number in [0,1]. Singular values  beyond  Threshold  are
                considered  zero.  Set  it to 0.0, if you don't understand
                what it means, so the solver will choose good value on its
                own.
                
OUTPUT PARAMETERS
    Info    -   return code:
                * -4    SVD subroutine failed
                * -1    if NRows<=0 or NCols<=0 or Threshold<0 was passed
                *  1    if task is solved
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1,0..M-1], it contains:
                * solution of A*X=B (even for singular A)
                * zeros, if SVD subroutine failed

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* R2        reciprocal of condition number: 1/cond(A), 2-norm.
* N         = NCols
* K         dim(Null(A))
* CX        array[0..N-1,0..K-1], kernel of A.
            Columns of CX store such vectors that A*CX[i]=0.

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvels(/* Real    */ ae_matrix* a,
     ae_int_t nrows,
     ae_int_t ncols,
     /* Real    */ ae_vector* b,
     double threshold,
     ae_int_t* info,
     densesolverlsreport* rep,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector sv;
    ae_matrix u;
    ae_matrix vt;
    ae_vector rp;
    ae_vector utb;
    ae_vector sutb;
    ae_vector tmp;
    ae_vector ta;
    ae_vector tx;
    ae_vector buf;
    ae_vector w;
    ae_int_t i;
    ae_int_t j;
    ae_int_t nsv;
    ae_int_t kernelidx;
    double v;
    double verr;
    ae_bool svdfailed;
    ae_bool zeroa;
    ae_int_t rfs;
    ae_int_t nrfs;
    ae_bool terminatenexttime;
    ae_bool smallerr;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverlsreport_clear(rep);
    ae_vector_clear(x);
    ae_vector_init(&sv, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&u, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&vt, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&rp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&utb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sutb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ta, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&buf, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);

    if( (nrows<=0||ncols<=0)||ae_fp_less(threshold,0) )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    if( ae_fp_eq(threshold,0) )
    {
        threshold = 1000*ae_machineepsilon;
    }
    
    /*
     * Factorize A first
     */
    svdfailed = !rmatrixsvd(a, nrows, ncols, 1, 2, 2, &sv, &u, &vt, _state);
    zeroa = ae_fp_eq(sv.ptr.p_double[0],0);
    if( svdfailed||zeroa )
    {
        if( svdfailed )
        {
            *info = -4;
        }
        else
        {
            *info = 1;
        }
        ae_vector_set_length(x, ncols, _state);
        for(i=0; i<=ncols-1; i++)
        {
            x->ptr.p_double[i] = 0;
        }
        rep->n = ncols;
        rep->k = ncols;
        ae_matrix_set_length(&rep->cx, ncols, ncols, _state);
        for(i=0; i<=ncols-1; i++)
        {
            for(j=0; j<=ncols-1; j++)
            {
                if( i==j )
                {
                    rep->cx.ptr.pp_double[i][j] = 1;
                }
                else
                {
                    rep->cx.ptr.pp_double[i][j] = 0;
                }
            }
        }
        rep->r2 = 0;
        ae_frame_leave(_state);
        return;
    }
    nsv = ae_minint(ncols, nrows, _state);
    if( nsv==ncols )
    {
        rep->r2 = sv.ptr.p_double[nsv-1]/sv.ptr.p_double[0];
    }
    else
    {
        rep->r2 = 0;
    }
    rep->n = ncols;
    *info = 1;
    
    /*
     * Iterative refinement of xc combined with solution:
     * 1. xc = 0
     * 2. calculate r = bc-A*xc using extra-precise dot product
     * 3. solve A*y = r
     * 4. update x:=x+r
     * 5. goto 2
     *
     * This cycle is executed until one of two things happens:
     * 1. maximum number of iterations reached
     * 2. last iteration decreased error to the lower limit
     */
    ae_vector_set_length(&utb, nsv, _state);
    ae_vector_set_length(&sutb, nsv, _state);
    ae_vector_set_length(x, ncols, _state);
    ae_vector_set_length(&tmp, ncols, _state);
    ae_vector_set_length(&ta, ncols+1, _state);
    ae_vector_set_length(&tx, ncols+1, _state);
    ae_vector_set_length(&buf, ncols+1, _state);
    for(i=0; i<=ncols-1; i++)
    {
        x->ptr.p_double[i] = 0;
    }
    kernelidx = nsv;
    for(i=0; i<=nsv-1; i++)
    {
        if( ae_fp_less_eq(sv.ptr.p_double[i],threshold*sv.ptr.p_double[0]) )
        {
            kernelidx = i;
            break;
        }
    }
    rep->k = ncols-kernelidx;
    nrfs = densesolver_densesolverrfsmaxv2(ncols, rep->r2, _state);
    terminatenexttime = ae_false;
    ae_vector_set_length(&rp, nrows, _state);
    for(rfs=0; rfs<=nrfs; rfs++)
    {
        if( terminatenexttime )
        {
            break;
        }
        
        /*
         * calculate right part
         */
        if( rfs==0 )
        {
            ae_v_move(&rp.ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,nrows-1));
        }
        else
        {
            smallerr = ae_true;
            for(i=0; i<=nrows-1; i++)
            {
                ae_v_move(&ta.ptr.p_double[0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,ncols-1));
                ta.ptr.p_double[ncols] = -1;
                ae_v_move(&tx.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,ncols-1));
                tx.ptr.p_double[ncols] = b->ptr.p_double[i];
                xdot(&ta, &tx, ncols+1, &buf, &v, &verr, _state);
                rp.ptr.p_double[i] = -v;
                smallerr = smallerr&&ae_fp_less(ae_fabs(v, _state),4*verr);
            }
            if( smallerr )
            {
                terminatenexttime = ae_true;
            }
        }
        
        /*
         * solve A*dx = rp
         */
        for(i=0; i<=ncols-1; i++)
        {
            tmp.ptr.p_double[i] = 0;
        }
        for(i=0; i<=nsv-1; i++)
        {
            utb.ptr.p_double[i] = 0;
        }
        for(i=0; i<=nrows-1; i++)
        {
            v = rp.ptr.p_double[i];
            ae_v_addd(&utb.ptr.p_double[0], 1, &u.ptr.pp_double[i][0], 1, ae_v_len(0,nsv-1), v);
        }
        for(i=0; i<=nsv-1; i++)
        {
            if( i<kernelidx )
            {
                sutb.ptr.p_double[i] = utb.ptr.p_double[i]/sv.ptr.p_double[i];
            }
            else
            {
                sutb.ptr.p_double[i] = 0;
            }
        }
        for(i=0; i<=nsv-1; i++)
        {
            v = sutb.ptr.p_double[i];
            ae_v_addd(&tmp.ptr.p_double[0], 1, &vt.ptr.pp_double[i][0], 1, ae_v_len(0,ncols-1), v);
        }
        
        /*
         * update x:  x:=x+dx
         */
        ae_v_add(&x->ptr.p_double[0], 1, &tmp.ptr.p_double[0], 1, ae_v_len(0,ncols-1));
    }
    
    /*
     * fill CX
     */
    if( rep->k>0 )
    {
        ae_matrix_set_length(&rep->cx, ncols, rep->k, _state);
        for(i=0; i<=rep->k-1; i++)
        {
            ae_v_move(&rep->cx.ptr.pp_double[0][i], rep->cx.stride, &vt.ptr.pp_double[kernelidx+i][0], 1, ae_v_len(0,ncols-1));
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal LU solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void densesolver_rmatrixlusolveinternal(/* Real    */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     double scalea,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_bool havea,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t rfs;
    ae_int_t nrfs;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;
    double v;
    double verr;
    double mxb;
    double scaleright;
    ae_bool smallerr;
    ae_bool terminatenexttime;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xa, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);

    ae_assert(ae_fp_greater(scalea,0), "Assertion failed", _state);
    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]>n-1||p->ptr.p_int[i]<i )
        {
            *info = -1;
            ae_frame_leave(_state);
            return;
        }
    }
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n+1, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = rmatrixlurcond1(lua, n, _state);
    rep->rinf = rmatrixlurcondinf(lua, n, _state);
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_double[i][j] = 0;
            }
        }
        rep->r1 = 0;
        rep->rinf = 0;
        *info = -3;
        ae_frame_leave(_state);
        return;
    }
    *info = 1;
    
    /*
     * solve
     */
    for(k=0; k<=m-1; k++)
    {
        
        /*
         * copy B to contiguous storage
         */
        ae_v_move(&bc.ptr.p_double[0], 1, &b->ptr.pp_double[0][k], b->stride, ae_v_len(0,n-1));
        
        /*
         * Scale right part:
         * * MX stores max(|Bi|)
         * * ScaleRight stores actual scaling applied to B when solving systems
         *   it is chosen to make |scaleRight*b| close to 1.
         */
        mxb = 0;
        for(i=0; i<=n-1; i++)
        {
            mxb = ae_maxreal(mxb, ae_fabs(bc.ptr.p_double[i], _state), _state);
        }
        if( ae_fp_eq(mxb,0) )
        {
            mxb = 1;
        }
        scaleright = 1/mxb;
        
        /*
         * First, non-iterative part of solution process.
         * We use separate code for this task because
         * XDot is quite slow and we want to save time.
         */
        ae_v_moved(&xc.ptr.p_double[0], 1, &bc.ptr.p_double[0], 1, ae_v_len(0,n-1), scaleright);
        densesolver_rbasiclusolve(lua, p, scalea, n, &xc, &tx, _state);
        
        /*
         * Iterative refinement of xc:
         * * calculate r = bc-A*xc using extra-precise dot product
         * * solve A*y = r
         * * update x:=x+r
         *
         * This cycle is executed until one of two things happens:
         * 1. maximum number of iterations reached
         * 2. last iteration decreased error to the lower limit
         */
        if( havea )
        {
            nrfs = densesolver_densesolverrfsmax(n, rep->r1, rep->rinf, _state);
            terminatenexttime = ae_false;
            for(rfs=0; rfs<=nrfs-1; rfs++)
            {
                if( terminatenexttime )
                {
                    break;
                }
                
                /*
                 * generate right part
                 */
                smallerr = ae_true;
                ae_v_move(&xb.ptr.p_double[0], 1, &xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
                for(i=0; i<=n-1; i++)
                {
                    ae_v_moved(&xa.ptr.p_double[0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,n-1), scalea);
                    xa.ptr.p_double[n] = -1;
                    xb.ptr.p_double[n] = scaleright*bc.ptr.p_double[i];
                    xdot(&xa, &xb, n+1, &tx, &v, &verr, _state);
                    y.ptr.p_double[i] = -v;
                    smallerr = smallerr&&ae_fp_less(ae_fabs(v, _state),4*verr);
                }
                if( smallerr )
                {
                    terminatenexttime = ae_true;
                }
                
                /*
                 * solve and update
                 */
                densesolver_rbasiclusolve(lua, p, scalea, n, &y, &tx, _state);
                ae_v_add(&xc.ptr.p_double[0], 1, &y.ptr.p_double[0], 1, ae_v_len(0,n-1));
            }
        }
        
        /*
         * Store xc.
         * Post-scale result.
         */
        v = scalea*mxb;
        ae_v_moved(&x->ptr.pp_double[0][k], x->stride, &xc.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal Cholesky solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void densesolver_spdmatrixcholeskysolveinternal(/* Real    */ ae_matrix* cha,
     double sqrtscalea,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* a,
     ae_bool havea,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Real    */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;
    double v;
    double mxb;
    double scaleright;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xa, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);

    ae_assert(ae_fp_greater(sqrtscalea,0), "Assertion failed", _state);
    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n+1, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = spdmatrixcholeskyrcond(cha, n, isupper, _state);
    rep->rinf = rep->r1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_double[i][j] = 0;
            }
        }
        rep->r1 = 0;
        rep->rinf = 0;
        *info = -3;
        ae_frame_leave(_state);
        return;
    }
    *info = 1;
    
    /*
     * solve
     */
    for(k=0; k<=m-1; k++)
    {
        
        /*
         * copy B to contiguous storage
         */
        ae_v_move(&bc.ptr.p_double[0], 1, &b->ptr.pp_double[0][k], b->stride, ae_v_len(0,n-1));
        
        /*
         * Scale right part:
         * * MX stores max(|Bi|)
         * * ScaleRight stores actual scaling applied to B when solving systems
         *   it is chosen to make |scaleRight*b| close to 1.
         */
        mxb = 0;
        for(i=0; i<=n-1; i++)
        {
            mxb = ae_maxreal(mxb, ae_fabs(bc.ptr.p_double[i], _state), _state);
        }
        if( ae_fp_eq(mxb,0) )
        {
            mxb = 1;
        }
        scaleright = 1/mxb;
        
        /*
         * First, non-iterative part of solution process.
         * We use separate code for this task because
         * XDot is quite slow and we want to save time.
         */
        ae_v_moved(&xc.ptr.p_double[0], 1, &bc.ptr.p_double[0], 1, ae_v_len(0,n-1), scaleright);
        densesolver_spdbasiccholeskysolve(cha, sqrtscalea, n, isupper, &xc, &tx, _state);
        
        /*
         * Store xc.
         * Post-scale result.
         */
        v = ae_sqr(sqrtscalea, _state)*mxb;
        ae_v_moved(&x->ptr.pp_double[0][k], x->stride, &xc.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal LU solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void densesolver_cmatrixlusolveinternal(/* Complex */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     double scalea,
     ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_bool havea,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t rfs;
    ae_int_t nrfs;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;
    ae_vector tmpbuf;
    ae_complex v;
    double verr;
    double mxb;
    double scaleright;
    ae_bool smallerr;
    ae_bool terminatenexttime;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_vector_init(&xc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&y, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&bc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xa, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xb, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&tx, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&tmpbuf, 0, DT_REAL, _state, ae_true);

    ae_assert(ae_fp_greater(scalea,0), "Assertion failed", _state);
    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]>n-1||p->ptr.p_int[i]<i )
        {
            *info = -1;
            ae_frame_leave(_state);
            return;
        }
    }
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    ae_vector_set_length(&tmpbuf, 2*n+2, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = cmatrixlurcond1(lua, n, _state);
    rep->rinf = cmatrixlurcondinf(lua, n, _state);
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_complex[i][j] = ae_complex_from_d(0);
            }
        }
        rep->r1 = 0;
        rep->rinf = 0;
        *info = -3;
        ae_frame_leave(_state);
        return;
    }
    *info = 1;
    
    /*
     * solve
     */
    for(k=0; k<=m-1; k++)
    {
        
        /*
         * copy B to contiguous storage
         */
        ae_v_cmove(&bc.ptr.p_complex[0], 1, &b->ptr.pp_complex[0][k], b->stride, "N", ae_v_len(0,n-1));
        
        /*
         * Scale right part:
         * * MX stores max(|Bi|)
         * * ScaleRight stores actual scaling applied to B when solving systems
         *   it is chosen to make |scaleRight*b| close to 1.
         */
        mxb = 0;
        for(i=0; i<=n-1; i++)
        {
            mxb = ae_maxreal(mxb, ae_c_abs(bc.ptr.p_complex[i], _state), _state);
        }
        if( ae_fp_eq(mxb,0) )
        {
            mxb = 1;
        }
        scaleright = 1/mxb;
        
        /*
         * First, non-iterative part of solution process.
         * We use separate code for this task because
         * XDot is quite slow and we want to save time.
         */
        ae_v_cmoved(&xc.ptr.p_complex[0], 1, &bc.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1), scaleright);
        densesolver_cbasiclusolve(lua, p, scalea, n, &xc, &tx, _state);
        
        /*
         * Iterative refinement of xc:
         * * calculate r = bc-A*xc using extra-precise dot product
         * * solve A*y = r
         * * update x:=x+r
         *
         * This cycle is executed until one of two things happens:
         * 1. maximum number of iterations reached
         * 2. last iteration decreased error to the lower limit
         */
        if( havea )
        {
            nrfs = densesolver_densesolverrfsmax(n, rep->r1, rep->rinf, _state);
            terminatenexttime = ae_false;
            for(rfs=0; rfs<=nrfs-1; rfs++)
            {
                if( terminatenexttime )
                {
                    break;
                }
                
                /*
                 * generate right part
                 */
                smallerr = ae_true;
                ae_v_cmove(&xb.ptr.p_complex[0], 1, &xc.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
                for(i=0; i<=n-1; i++)
                {
                    ae_v_cmoved(&xa.ptr.p_complex[0], 1, &a->ptr.pp_complex[i][0], 1, "N", ae_v_len(0,n-1), scalea);
                    xa.ptr.p_complex[n] = ae_complex_from_d(-1);
                    xb.ptr.p_complex[n] = ae_c_mul_d(bc.ptr.p_complex[i],scaleright);
                    xcdot(&xa, &xb, n+1, &tmpbuf, &v, &verr, _state);
                    y.ptr.p_complex[i] = ae_c_neg(v);
                    smallerr = smallerr&&ae_fp_less(ae_c_abs(v, _state),4*verr);
                }
                if( smallerr )
                {
                    terminatenexttime = ae_true;
                }
                
                /*
                 * solve and update
                 */
                densesolver_cbasiclusolve(lua, p, scalea, n, &y, &tx, _state);
                ae_v_cadd(&xc.ptr.p_complex[0], 1, &y.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
            }
        }
        
        /*
         * Store xc.
         * Post-scale result.
         */
        v = ae_complex_from_d(scalea*mxb);
        ae_v_cmovec(&x->ptr.pp_complex[0][k], x->stride, &xc.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1), v);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal Cholesky solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void densesolver_hpdmatrixcholeskysolveinternal(/* Complex */ ae_matrix* cha,
     double sqrtscalea,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_matrix* a,
     ae_bool havea,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_int_t* info,
     densesolverreport* rep,
     /* Complex */ ae_matrix* x,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;
    double v;
    double mxb;
    double scaleright;

    ae_frame_make(_state, &_frame_block);
    *info = 0;
    _densesolverreport_clear(rep);
    ae_matrix_clear(x);
    ae_vector_init(&xc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&y, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&bc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xa, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xb, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&tx, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(ae_fp_greater(sqrtscalea,0), "Assertion failed", _state);
    
    /*
     * prepare: check inputs, allocate space...
     */
    if( n<=0||m<=0 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n+1, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = hpdmatrixcholeskyrcond(cha, n, isupper, _state);
    rep->rinf = rep->r1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_complex[i][j] = ae_complex_from_d(0);
            }
        }
        rep->r1 = 0;
        rep->rinf = 0;
        *info = -3;
        ae_frame_leave(_state);
        return;
    }
    *info = 1;
    
    /*
     * solve
     */
    for(k=0; k<=m-1; k++)
    {
        
        /*
         * copy B to contiguous storage
         */
        ae_v_cmove(&bc.ptr.p_complex[0], 1, &b->ptr.pp_complex[0][k], b->stride, "N", ae_v_len(0,n-1));
        
        /*
         * Scale right part:
         * * MX stores max(|Bi|)
         * * ScaleRight stores actual scaling applied to B when solving systems
         *   it is chosen to make |scaleRight*b| close to 1.
         */
        mxb = 0;
        for(i=0; i<=n-1; i++)
        {
            mxb = ae_maxreal(mxb, ae_c_abs(bc.ptr.p_complex[i], _state), _state);
        }
        if( ae_fp_eq(mxb,0) )
        {
            mxb = 1;
        }
        scaleright = 1/mxb;
        
        /*
         * First, non-iterative part of solution process.
         * We use separate code for this task because
         * XDot is quite slow and we want to save time.
         */
        ae_v_cmoved(&xc.ptr.p_complex[0], 1, &bc.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1), scaleright);
        densesolver_hpdbasiccholeskysolve(cha, sqrtscalea, n, isupper, &xc, &tx, _state);
        
        /*
         * Store xc.
         * Post-scale result.
         */
        v = ae_sqr(sqrtscalea, _state)*mxb;
        ae_v_cmoved(&x->ptr.pp_complex[0][k], x->stride, &xc.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1), v);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal subroutine.
Returns maximum count of RFS iterations as function of:
1. machine epsilon
2. task size.
3. condition number

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static ae_int_t densesolver_densesolverrfsmax(ae_int_t n,
     double r1,
     double rinf,
     ae_state *_state)
{
    ae_int_t result;


    result = 5;
    return result;
}


/*************************************************************************
Internal subroutine.
Returns maximum count of RFS iterations as function of:
1. machine epsilon
2. task size.
3. norm-2 condition number

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static ae_int_t densesolver_densesolverrfsmaxv2(ae_int_t n,
     double r2,
     ae_state *_state)
{
    ae_int_t result;


    result = densesolver_densesolverrfsmax(n, 0, 0, _state);
    return result;
}


/*************************************************************************
Basic LU solver for ScaleA*PLU*x = y.

This subroutine assumes that:
* L is well-scaled, and it is U which needs scaling by ScaleA.
* A=PLU is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void densesolver_rbasiclusolve(/* Real    */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     double scalea,
     ae_int_t n,
     /* Real    */ ae_vector* xb,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    double v;


    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            v = xb->ptr.p_double[i];
            xb->ptr.p_double[i] = xb->ptr.p_double[p->ptr.p_int[i]];
            xb->ptr.p_double[p->ptr.p_int[i]] = v;
        }
    }
    for(i=1; i<=n-1; i++)
    {
        v = ae_v_dotproduct(&lua->ptr.pp_double[i][0], 1, &xb->ptr.p_double[0], 1, ae_v_len(0,i-1));
        xb->ptr.p_double[i] = xb->ptr.p_double[i]-v;
    }
    xb->ptr.p_double[n-1] = xb->ptr.p_double[n-1]/(scalea*lua->ptr.pp_double[n-1][n-1]);
    for(i=n-2; i>=0; i--)
    {
        ae_v_moved(&tmp->ptr.p_double[i+1], 1, &lua->ptr.pp_double[i][i+1], 1, ae_v_len(i+1,n-1), scalea);
        v = ae_v_dotproduct(&tmp->ptr.p_double[i+1], 1, &xb->ptr.p_double[i+1], 1, ae_v_len(i+1,n-1));
        xb->ptr.p_double[i] = (xb->ptr.p_double[i]-v)/(scalea*lua->ptr.pp_double[i][i]);
    }
}


/*************************************************************************
Basic Cholesky solver for ScaleA*Cholesky(A)'*x = y.

This subroutine assumes that:
* A*ScaleA is well scaled
* A is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void densesolver_spdbasiccholeskysolve(/* Real    */ ae_matrix* cha,
     double sqrtscalea,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* xb,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    double v;


    
    /*
     * A = L*L' or A=U'*U
     */
    if( isupper )
    {
        
        /*
         * Solve U'*y=b first.
         */
        for(i=0; i<=n-1; i++)
        {
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/(sqrtscalea*cha->ptr.pp_double[i][i]);
            if( i<n-1 )
            {
                v = xb->ptr.p_double[i];
                ae_v_moved(&tmp->ptr.p_double[i+1], 1, &cha->ptr.pp_double[i][i+1], 1, ae_v_len(i+1,n-1), sqrtscalea);
                ae_v_subd(&xb->ptr.p_double[i+1], 1, &tmp->ptr.p_double[i+1], 1, ae_v_len(i+1,n-1), v);
            }
        }
        
        /*
         * Solve U*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            if( i<n-1 )
            {
                ae_v_moved(&tmp->ptr.p_double[i+1], 1, &cha->ptr.pp_double[i][i+1], 1, ae_v_len(i+1,n-1), sqrtscalea);
                v = ae_v_dotproduct(&tmp->ptr.p_double[i+1], 1, &xb->ptr.p_double[i+1], 1, ae_v_len(i+1,n-1));
                xb->ptr.p_double[i] = xb->ptr.p_double[i]-v;
            }
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/(sqrtscalea*cha->ptr.pp_double[i][i]);
        }
    }
    else
    {
        
        /*
         * Solve L*y=b first
         */
        for(i=0; i<=n-1; i++)
        {
            if( i>0 )
            {
                ae_v_moved(&tmp->ptr.p_double[0], 1, &cha->ptr.pp_double[i][0], 1, ae_v_len(0,i-1), sqrtscalea);
                v = ae_v_dotproduct(&tmp->ptr.p_double[0], 1, &xb->ptr.p_double[0], 1, ae_v_len(0,i-1));
                xb->ptr.p_double[i] = xb->ptr.p_double[i]-v;
            }
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/(sqrtscalea*cha->ptr.pp_double[i][i]);
        }
        
        /*
         * Solve L'*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/(sqrtscalea*cha->ptr.pp_double[i][i]);
            if( i>0 )
            {
                v = xb->ptr.p_double[i];
                ae_v_moved(&tmp->ptr.p_double[0], 1, &cha->ptr.pp_double[i][0], 1, ae_v_len(0,i-1), sqrtscalea);
                ae_v_subd(&xb->ptr.p_double[0], 1, &tmp->ptr.p_double[0], 1, ae_v_len(0,i-1), v);
            }
        }
    }
}


/*************************************************************************
Basic LU solver for ScaleA*PLU*x = y.

This subroutine assumes that:
* L is well-scaled, and it is U which needs scaling by ScaleA.
* A=PLU is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void densesolver_cbasiclusolve(/* Complex */ ae_matrix* lua,
     /* Integer */ ae_vector* p,
     double scalea,
     ae_int_t n,
     /* Complex */ ae_vector* xb,
     /* Complex */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_complex v;


    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            v = xb->ptr.p_complex[i];
            xb->ptr.p_complex[i] = xb->ptr.p_complex[p->ptr.p_int[i]];
            xb->ptr.p_complex[p->ptr.p_int[i]] = v;
        }
    }
    for(i=1; i<=n-1; i++)
    {
        v = ae_v_cdotproduct(&lua->ptr.pp_complex[i][0], 1, "N", &xb->ptr.p_complex[0], 1, "N", ae_v_len(0,i-1));
        xb->ptr.p_complex[i] = ae_c_sub(xb->ptr.p_complex[i],v);
    }
    xb->ptr.p_complex[n-1] = ae_c_div(xb->ptr.p_complex[n-1],ae_c_mul_d(lua->ptr.pp_complex[n-1][n-1],scalea));
    for(i=n-2; i>=0; i--)
    {
        ae_v_cmoved(&tmp->ptr.p_complex[i+1], 1, &lua->ptr.pp_complex[i][i+1], 1, "N", ae_v_len(i+1,n-1), scalea);
        v = ae_v_cdotproduct(&tmp->ptr.p_complex[i+1], 1, "N", &xb->ptr.p_complex[i+1], 1, "N", ae_v_len(i+1,n-1));
        xb->ptr.p_complex[i] = ae_c_div(ae_c_sub(xb->ptr.p_complex[i],v),ae_c_mul_d(lua->ptr.pp_complex[i][i],scalea));
    }
}


/*************************************************************************
Basic Cholesky solver for ScaleA*Cholesky(A)'*x = y.

This subroutine assumes that:
* A*ScaleA is well scaled
* A is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void densesolver_hpdbasiccholeskysolve(/* Complex */ ae_matrix* cha,
     double sqrtscalea,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* xb,
     /* Complex */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_complex v;


    
    /*
     * A = L*L' or A=U'*U
     */
    if( isupper )
    {
        
        /*
         * Solve U'*y=b first.
         */
        for(i=0; i<=n-1; i++)
        {
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],ae_c_mul_d(ae_c_conj(cha->ptr.pp_complex[i][i], _state),sqrtscalea));
            if( i<n-1 )
            {
                v = xb->ptr.p_complex[i];
                ae_v_cmoved(&tmp->ptr.p_complex[i+1], 1, &cha->ptr.pp_complex[i][i+1], 1, "Conj", ae_v_len(i+1,n-1), sqrtscalea);
                ae_v_csubc(&xb->ptr.p_complex[i+1], 1, &tmp->ptr.p_complex[i+1], 1, "N", ae_v_len(i+1,n-1), v);
            }
        }
        
        /*
         * Solve U*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            if( i<n-1 )
            {
                ae_v_cmoved(&tmp->ptr.p_complex[i+1], 1, &cha->ptr.pp_complex[i][i+1], 1, "N", ae_v_len(i+1,n-1), sqrtscalea);
                v = ae_v_cdotproduct(&tmp->ptr.p_complex[i+1], 1, "N", &xb->ptr.p_complex[i+1], 1, "N", ae_v_len(i+1,n-1));
                xb->ptr.p_complex[i] = ae_c_sub(xb->ptr.p_complex[i],v);
            }
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],ae_c_mul_d(cha->ptr.pp_complex[i][i],sqrtscalea));
        }
    }
    else
    {
        
        /*
         * Solve L*y=b first
         */
        for(i=0; i<=n-1; i++)
        {
            if( i>0 )
            {
                ae_v_cmoved(&tmp->ptr.p_complex[0], 1, &cha->ptr.pp_complex[i][0], 1, "N", ae_v_len(0,i-1), sqrtscalea);
                v = ae_v_cdotproduct(&tmp->ptr.p_complex[0], 1, "N", &xb->ptr.p_complex[0], 1, "N", ae_v_len(0,i-1));
                xb->ptr.p_complex[i] = ae_c_sub(xb->ptr.p_complex[i],v);
            }
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],ae_c_mul_d(cha->ptr.pp_complex[i][i],sqrtscalea));
        }
        
        /*
         * Solve L'*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],ae_c_mul_d(ae_c_conj(cha->ptr.pp_complex[i][i], _state),sqrtscalea));
            if( i>0 )
            {
                v = xb->ptr.p_complex[i];
                ae_v_cmoved(&tmp->ptr.p_complex[0], 1, &cha->ptr.pp_complex[i][0], 1, "Conj", ae_v_len(0,i-1), sqrtscalea);
                ae_v_csubc(&xb->ptr.p_complex[0], 1, &tmp->ptr.p_complex[0], 1, "N", ae_v_len(0,i-1), v);
            }
        }
    }
}


ae_bool _densesolverreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    densesolverreport *p = (densesolverreport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _densesolverreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    densesolverreport *dst = (densesolverreport*)_dst;
    densesolverreport *src = (densesolverreport*)_src;
    dst->r1 = src->r1;
    dst->rinf = src->rinf;
    return ae_true;
}


void _densesolverreport_clear(void* _p)
{
    densesolverreport *p = (densesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _densesolverreport_destroy(void* _p)
{
    densesolverreport *p = (densesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


ae_bool _densesolverlsreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    densesolverlsreport *p = (densesolverlsreport*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_matrix_init(&p->cx, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _densesolverlsreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    densesolverlsreport *dst = (densesolverlsreport*)_dst;
    densesolverlsreport *src = (densesolverlsreport*)_src;
    dst->r2 = src->r2;
    if( !ae_matrix_init_copy(&dst->cx, &src->cx, _state, make_automatic) )
        return ae_false;
    dst->n = src->n;
    dst->k = src->k;
    return ae_true;
}


void _densesolverlsreport_clear(void* _p)
{
    densesolverlsreport *p = (densesolverlsreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->cx);
}


void _densesolverlsreport_destroy(void* _p)
{
    densesolverlsreport *p = (densesolverlsreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->cx);
}




/*************************************************************************
This function initializes linear LSQR Solver. This solver is used to solve
non-symmetric (and, possibly, non-square) problems. Least squares solution
is returned for non-compatible systems.

USAGE:
1. User initializes algorithm state with LinLSQRCreate() call
2. User tunes solver parameters with  LinLSQRSetCond() and other functions
3. User  calls  LinLSQRSolveSparse()  function which takes algorithm state 
   and SparseMatrix object.
4. User calls LinLSQRResults() to get solution
5. Optionally, user may call LinLSQRSolveSparse() again to  solve  another  
   problem  with different matrix and/or right part without reinitializing 
   LinLSQRState structure.
  
INPUT PARAMETERS:
    M       -   number of rows in A
    N       -   number of variables, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrcreate(ae_int_t m,
     ae_int_t n,
     linlsqrstate* state,
     ae_state *_state)
{
    ae_int_t i;

    _linlsqrstate_clear(state);

    ae_assert(m>0, "LinLSQRCreate: M<=0", _state);
    ae_assert(n>0, "LinLSQRCreate: N<=0", _state);
    state->m = m;
    state->n = n;
    state->prectype = 0;
    state->epsa = linlsqr_atol;
    state->epsb = linlsqr_btol;
    state->epsc = 1/ae_sqrt(ae_machineepsilon, _state);
    state->maxits = 0;
    state->lambdai = 0;
    state->xrep = ae_false;
    state->running = ae_false;
    
    /*
     * * allocate arrays
     * * set RX to NAN (just for the case user calls Results() without 
     *   calling SolveSparse()
     * * set B to zero
     */
    normestimatorcreate(m, n, 2, 2, &state->nes, _state);
    ae_vector_set_length(&state->rx, state->n, _state);
    ae_vector_set_length(&state->ui, state->m+state->n, _state);
    ae_vector_set_length(&state->uip1, state->m+state->n, _state);
    ae_vector_set_length(&state->vip1, state->n, _state);
    ae_vector_set_length(&state->vi, state->n, _state);
    ae_vector_set_length(&state->omegai, state->n, _state);
    ae_vector_set_length(&state->omegaip1, state->n, _state);
    ae_vector_set_length(&state->d, state->n, _state);
    ae_vector_set_length(&state->x, state->m+state->n, _state);
    ae_vector_set_length(&state->mv, state->m+state->n, _state);
    ae_vector_set_length(&state->mtv, state->n, _state);
    ae_vector_set_length(&state->b, state->m, _state);
    for(i=0; i<=n-1; i++)
    {
        state->rx.ptr.p_double[i] = _state->v_nan;
    }
    for(i=0; i<=m-1; i++)
    {
        state->b.ptr.p_double[i] = 0;
    }
    ae_vector_set_length(&state->rstate.ia, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 0+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************
This function sets right part. By default, right part is zero.

INPUT PARAMETERS:
    B       -   right part, array[N].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetb(linlsqrstate* state,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(!state->running, "LinLSQRSetB: you can not change B when LinLSQRIteration is running", _state);
    ae_assert(state->m<=b->cnt, "LinLSQRSetB: Length(B)<M", _state);
    ae_assert(isfinitevector(b, state->m, _state), "LinLSQRSetB: B contains infinite or NaN values", _state);
    state->bnorm2 = 0;
    for(i=0; i<=state->m-1; i++)
    {
        state->b.ptr.p_double[i] = b->ptr.p_double[i];
        state->bnorm2 = state->bnorm2+b->ptr.p_double[i]*b->ptr.p_double[i];
    }
}


/*************************************************************************
This  function  changes  preconditioning  settings of LinLSQQSolveSparse()
function. By default, SolveSparse() uses diagonal preconditioner,  but  if
you want to use solver without preconditioning, you can call this function
which forces solver to use unit matrix for preconditioning.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecunit(linlsqrstate* state, ae_state *_state)
{


    ae_assert(!state->running, "LinLSQRSetPrecUnit: you can not change preconditioner, because function LinLSQRIteration is running!", _state);
    state->prectype = -1;
}


/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function.  LinCGSolveSparse() will use diagonal of the  system  matrix  as
preconditioner. This preconditioning mode is active by default.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecdiag(linlsqrstate* state, ae_state *_state)
{


    ae_assert(!state->running, "LinLSQRSetPrecDiag: you can not change preconditioner, because function LinCGIteration is running!", _state);
    state->prectype = 0;
}


/*************************************************************************
This function sets optional Tikhonov regularization coefficient.
It is zero by default.

INPUT PARAMETERS:
    LambdaI -   regularization factor, LambdaI>=0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state
    
  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetlambdai(linlsqrstate* state,
     double lambdai,
     ae_state *_state)
{


    ae_assert(!state->running, "LinLSQRSetLambdaI: you can not set LambdaI, because function LinLSQRIteration is running", _state);
    ae_assert(ae_isfinite(lambdai, _state)&&ae_fp_greater_eq(lambdai,0), "LinLSQRSetLambdaI: LambdaI is infinite or NaN", _state);
    state->lambdai = lambdai;
}


/*************************************************************************

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
ae_bool linlsqriteration(linlsqrstate* state, ae_state *_state)
{
    ae_int_t summn;
    double bnorm;
    ae_int_t i;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        summn = state->rstate.ia.ptr.p_int[0];
        i = state->rstate.ia.ptr.p_int[1];
        bnorm = state->rstate.ra.ptr.p_double[0];
    }
    else
    {
        summn = -983;
        i = -989;
        bnorm = -834;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    if( state->rstate.stage==5 )
    {
        goto lbl_5;
    }
    if( state->rstate.stage==6 )
    {
        goto lbl_6;
    }
    
    /*
     * Routine body
     */
    ae_assert(state->b.cnt>0, "LinLSQRIteration: using non-allocated array B", _state);
    bnorm = ae_sqrt(state->bnorm2, _state);
    state->running = ae_true;
    state->repnmv = 0;
    linlsqr_clearrfields(state, _state);
    state->repiterationscount = 0;
    summn = state->m+state->n;
    state->r2 = state->bnorm2;
    
    /*
     *estimate for ANorm
     */
    normestimatorrestart(&state->nes, _state);
lbl_7:
    if( !normestimatoriteration(&state->nes, _state) )
    {
        goto lbl_8;
    }
    if( !state->nes.needmv )
    {
        goto lbl_9;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->nes.x.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmv = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needmv = ae_false;
    ae_v_move(&state->nes.mv.ptr.p_double[0], 1, &state->mv.ptr.p_double[0], 1, ae_v_len(0,state->m-1));
    goto lbl_7;
lbl_9:
    if( !state->nes.needmtv )
    {
        goto lbl_11;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->nes.x.ptr.p_double[0], 1, ae_v_len(0,state->m-1));
    
    /*
     *matrix-vector multiplication
     */
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmtv = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needmtv = ae_false;
    ae_v_move(&state->nes.mtv.ptr.p_double[0], 1, &state->mtv.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    goto lbl_7;
lbl_11:
    goto lbl_7;
lbl_8:
    normestimatorresults(&state->nes, &state->anorm, _state);
    
    /*
     *initialize .RX by zeros
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->rx.ptr.p_double[i] = 0;
    }
    
    /*
     *output first report
     */
    if( !state->xrep )
    {
        goto lbl_13;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    linlsqr_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->xupdated = ae_false;
lbl_13:
    
    /*
     * LSQR, Step 0.
     *
     * Algorithm outline corresponds to one which was described at p.50 of
     * "LSQR - an algorithm for sparse linear equations and sparse least 
     * squares" by C.Paige and M.Saunders with one small addition - we
     * explicitly extend system matrix by additional N lines in order 
     * to handle non-zero lambda, i.e. original A is replaced by
     *         [ A        ]
     * A_mod = [          ]
     *         [ lambda*I ].
     *
     * Step 0:
     *     x[0]          = 0
     *     beta[1]*u[1]  = b
     *     alpha[1]*v[1] = A_mod'*u[1]
     *     w[1]          = v[1]
     *     phiBar[1]     = beta[1]
     *     rhoBar[1]     = alpha[1]
     *     d[0]          = 0
     *
     * NOTE:
     * There are three criteria for stopping:
     * (S0) maximum number of iterations
     * (S1) ||Rk||<=EpsB*||B||;
     * (S2) ||A^T*Rk||/(||A||*||Rk||)<=EpsA.
     * It is very important that S2 always checked AFTER S1. It is necessary
     * to avoid division by zero when Rk=0.
     */
    state->betai = bnorm;
    if( ae_fp_eq(state->betai,0) )
    {
        
        /*
         * Zero right part
         */
        state->running = ae_false;
        state->repterminationtype = 1;
        result = ae_false;
        return result;
    }
    for(i=0; i<=summn-1; i++)
    {
        if( i<state->m )
        {
            state->ui.ptr.p_double[i] = state->b.ptr.p_double[i]/state->betai;
        }
        else
        {
            state->ui.ptr.p_double[i] = 0;
        }
        state->x.ptr.p_double[i] = state->ui.ptr.p_double[i];
    }
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmtv = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needmtv = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->mtv.ptr.p_double[i] = state->mtv.ptr.p_double[i]+state->lambdai*state->ui.ptr.p_double[state->m+i];
    }
    state->alphai = 0;
    for(i=0; i<=state->n-1; i++)
    {
        state->alphai = state->alphai+state->mtv.ptr.p_double[i]*state->mtv.ptr.p_double[i];
    }
    state->alphai = ae_sqrt(state->alphai, _state);
    if( ae_fp_eq(state->alphai,0) )
    {
        
        /*
         * Orthogonality stopping criterion is met
         */
        state->running = ae_false;
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    for(i=0; i<=state->n-1; i++)
    {
        state->vi.ptr.p_double[i] = state->mtv.ptr.p_double[i]/state->alphai;
        state->omegai.ptr.p_double[i] = state->vi.ptr.p_double[i];
    }
    state->phibari = state->betai;
    state->rhobari = state->alphai;
    for(i=0; i<=state->n-1; i++)
    {
        state->d.ptr.p_double[i] = 0;
    }
    state->dnorm = 0;
    
    /*
     * Steps I=1, 2, ...
     */
lbl_15:
    if( ae_false )
    {
        goto lbl_16;
    }
    
    /*
     * At I-th step State.RepIterationsCount=I.
     */
    state->repiterationscount = state->repiterationscount+1;
    
    /*
     * Bidiagonalization part:
     *     beta[i+1]*u[i+1]  = A_mod*v[i]-alpha[i]*u[i]
     *     alpha[i+1]*v[i+1] = A_mod'*u[i+1] - beta[i+1]*v[i]
     *     
     * NOTE:  beta[i+1]=0 or alpha[i+1]=0 will lead to successful termination
     *        in the end of the current iteration. In this case u/v are zero.
     * NOTE2: algorithm won't fail on zero alpha or beta (there will be no
     *        division by zero because it will be stopped BEFORE division
     *        occurs). However, near-zero alpha and beta won't stop algorithm
     *        and, although no division by zero will happen, orthogonality 
     *        in U and V will be lost.
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->vi.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmv = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needmv = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->mv.ptr.p_double[state->m+i] = state->lambdai*state->vi.ptr.p_double[i];
    }
    state->betaip1 = 0;
    for(i=0; i<=summn-1; i++)
    {
        state->uip1.ptr.p_double[i] = state->mv.ptr.p_double[i]-state->alphai*state->ui.ptr.p_double[i];
        state->betaip1 = state->betaip1+state->uip1.ptr.p_double[i]*state->uip1.ptr.p_double[i];
    }
    if( ae_fp_neq(state->betaip1,0) )
    {
        state->betaip1 = ae_sqrt(state->betaip1, _state);
        for(i=0; i<=summn-1; i++)
        {
            state->uip1.ptr.p_double[i] = state->uip1.ptr.p_double[i]/state->betaip1;
        }
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->uip1.ptr.p_double[0], 1, ae_v_len(0,state->m-1));
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmtv = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->needmtv = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->mtv.ptr.p_double[i] = state->mtv.ptr.p_double[i]+state->lambdai*state->uip1.ptr.p_double[state->m+i];
    }
    state->alphaip1 = 0;
    for(i=0; i<=state->n-1; i++)
    {
        state->vip1.ptr.p_double[i] = state->mtv.ptr.p_double[i]-state->betaip1*state->vi.ptr.p_double[i];
        state->alphaip1 = state->alphaip1+state->vip1.ptr.p_double[i]*state->vip1.ptr.p_double[i];
    }
    if( ae_fp_neq(state->alphaip1,0) )
    {
        state->alphaip1 = ae_sqrt(state->alphaip1, _state);
        for(i=0; i<=state->n-1; i++)
        {
            state->vip1.ptr.p_double[i] = state->vip1.ptr.p_double[i]/state->alphaip1;
        }
    }
    
    /*
     * Build next orthogonal transformation
     */
    state->rhoi = safepythag2(state->rhobari, state->betaip1, _state);
    state->ci = state->rhobari/state->rhoi;
    state->si = state->betaip1/state->rhoi;
    state->theta = state->si*state->alphaip1;
    state->rhobarip1 = -state->ci*state->alphaip1;
    state->phii = state->ci*state->phibari;
    state->phibarip1 = state->si*state->phibari;
    
    /*
     * Update .RNorm
     *
     * This tricky  formula  is  necessary  because  simply  writing
     * State.R2:=State.PhiBarIP1*State.PhiBarIP1 does NOT guarantees
     * monotonic decrease of R2. Roundoff error combined with 80-bit
     * precision used internally by Intel chips allows R2 to increase
     * slightly in some rare, but possible cases. This property is
     * undesirable, so we prefer to guard against R increase.
     */
    state->r2 = ae_minreal(state->r2, state->phibarip1*state->phibarip1, _state);
    
    /*
     * Update d and DNorm, check condition-related stopping criteria
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->d.ptr.p_double[i] = 1/state->rhoi*(state->vi.ptr.p_double[i]-state->theta*state->d.ptr.p_double[i]);
        state->dnorm = state->dnorm+state->d.ptr.p_double[i]*state->d.ptr.p_double[i];
    }
    if( ae_fp_greater_eq(ae_sqrt(state->dnorm, _state)*state->anorm,state->epsc) )
    {
        state->running = ae_false;
        state->repterminationtype = 7;
        result = ae_false;
        return result;
    }
    
    /*
     * Update x, output report
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->rx.ptr.p_double[i] = state->rx.ptr.p_double[i]+state->phii/state->rhoi*state->omegai.ptr.p_double[i];
    }
    if( !state->xrep )
    {
        goto lbl_17;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    linlsqr_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->xupdated = ae_false;
lbl_17:
    
    /*
     * Check stopping criteria
     * 1. achieved required number of iterations;
     * 2. ||Rk||<=EpsB*||B||;
     * 3. ||A^T*Rk||/(||A||*||Rk||)<=EpsA;
     */
    if( state->maxits>0&&state->repiterationscount>=state->maxits )
    {
        
        /*
         * Achieved required number of iterations
         */
        state->running = ae_false;
        state->repterminationtype = 5;
        result = ae_false;
        return result;
    }
    if( ae_fp_less_eq(state->phibarip1,state->epsb*bnorm) )
    {
        
        /*
         * ||Rk||<=EpsB*||B||, here ||Rk||=PhiBar
         */
        state->running = ae_false;
        state->repterminationtype = 1;
        result = ae_false;
        return result;
    }
    if( ae_fp_less_eq(state->alphaip1*ae_fabs(state->ci, _state)/state->anorm,state->epsa) )
    {
        
        /*
         * ||A^T*Rk||/(||A||*||Rk||)<=EpsA, here ||A^T*Rk||=PhiBar*Alpha[i+1]*|.C|
         */
        state->running = ae_false;
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    
    /*
     * Update omega
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->omegaip1.ptr.p_double[i] = state->vip1.ptr.p_double[i]-state->theta/state->rhoi*state->omegai.ptr.p_double[i];
    }
    
    /*
     * Prepare for the next iteration - rename variables:
     * u[i]   := u[i+1]
     * v[i]   := v[i+1]
     * rho[i] := rho[i+1]
     * ...
     */
    ae_v_move(&state->ui.ptr.p_double[0], 1, &state->uip1.ptr.p_double[0], 1, ae_v_len(0,summn-1));
    ae_v_move(&state->vi.ptr.p_double[0], 1, &state->vip1.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_v_move(&state->omegai.ptr.p_double[0], 1, &state->omegaip1.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->alphai = state->alphaip1;
    state->betai = state->betaip1;
    state->phibari = state->phibarip1;
    state->rhobari = state->rhobarip1;
    goto lbl_15;
lbl_16:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = summn;
    state->rstate.ia.ptr.p_int[1] = i;
    state->rstate.ra.ptr.p_double[0] = bnorm;
    return result;
}


/*************************************************************************
Procedure for solution of A*x=b with sparse A.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse M*N matrix in the CRS format (you MUST contvert  it 
                to CRS format  by  calling  SparseConvertToCRS()  function
                BEFORE you pass it to this function).
    B       -   right part, array[M]

RESULT:
    This function returns no result.
    You can get solution by calling LinCGResults()
    
NOTE: this function uses lightweight preconditioning -  multiplication  by
      inverse of diag(A). If you want, you can turn preconditioning off by
      calling LinLSQRSetPrecUnit(). However, preconditioning cost is   low
      and preconditioner is very important for solution  of  badly  scaled
      problems.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsolvesparse(linlsqrstate* state,
     sparsematrix* a,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t t0;
    ae_int_t t1;
    double v;


    n = state->n;
    ae_assert(!state->running, "LinLSQRSolveSparse: you can not call this function when LinLSQRIteration is running", _state);
    ae_assert(b->cnt>=state->m, "LinLSQRSolveSparse: Length(B)<M", _state);
    ae_assert(isfinitevector(b, state->m, _state), "LinLSQRSolveSparse: B contains infinite or NaN values", _state);
    
    /*
     * Allocate temporaries
     */
    rvectorsetlengthatleast(&state->tmpd, n, _state);
    rvectorsetlengthatleast(&state->tmpx, n, _state);
    
    /*
     * Compute diagonal scaling matrix D
     */
    if( state->prectype==0 )
    {
        
        /*
         * Default preconditioner - inverse of column norms
         */
        for(i=0; i<=n-1; i++)
        {
            state->tmpd.ptr.p_double[i] = 0;
        }
        t0 = 0;
        t1 = 0;
        while(sparseenumerate(a, &t0, &t1, &i, &j, &v, _state))
        {
            state->tmpd.ptr.p_double[j] = state->tmpd.ptr.p_double[j]+ae_sqr(v, _state);
        }
        for(i=0; i<=n-1; i++)
        {
            if( ae_fp_greater(state->tmpd.ptr.p_double[i],0) )
            {
                state->tmpd.ptr.p_double[i] = 1/ae_sqrt(state->tmpd.ptr.p_double[i], _state);
            }
            else
            {
                state->tmpd.ptr.p_double[i] = 1;
            }
        }
    }
    else
    {
        
        /*
         * No diagonal scaling
         */
        for(i=0; i<=n-1; i++)
        {
            state->tmpd.ptr.p_double[i] = 1;
        }
    }
    
    /*
     * Solve.
     *
     * Instead of solving A*x=b we solve preconditioned system (A*D)*(inv(D)*x)=b.
     * Transformed A is not calculated explicitly, we just modify multiplication
     * by A or A'. After solution we modify State.RX so it will store untransformed
     * variables
     */
    linlsqrsetb(state, b, _state);
    linlsqrrestart(state, _state);
    while(linlsqriteration(state, _state))
    {
        if( state->needmv )
        {
            for(i=0; i<=n-1; i++)
            {
                state->tmpx.ptr.p_double[i] = state->tmpd.ptr.p_double[i]*state->x.ptr.p_double[i];
            }
            sparsemv(a, &state->tmpx, &state->mv, _state);
        }
        if( state->needmtv )
        {
            sparsemtv(a, &state->x, &state->mtv, _state);
            for(i=0; i<=n-1; i++)
            {
                state->mtv.ptr.p_double[i] = state->tmpd.ptr.p_double[i]*state->mtv.ptr.p_double[i];
            }
        }
    }
    for(i=0; i<=n-1; i++)
    {
        state->rx.ptr.p_double[i] = state->tmpd.ptr.p_double[i]*state->rx.ptr.p_double[i];
    }
}


/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsA    -   algorithm will be stopped if ||A^T*Rk||/(||A||*||Rk||)<=EpsA.
    EpsB    -   algorithm will be stopped if ||Rk||<=EpsB*||B||
    MaxIts  -   algorithm will be stopped if number of iterations
                more than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE: if EpsA,EpsB,EpsC and MaxIts are zero then these variables will
be setted as default values.
    
  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetcond(linlsqrstate* state,
     double epsa,
     double epsb,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(!state->running, "LinLSQRSetCond: you can not call this function when LinLSQRIteration is running", _state);
    ae_assert(ae_isfinite(epsa, _state)&&ae_fp_greater_eq(epsa,0), "LinLSQRSetCond: EpsA is negative, INF or NAN", _state);
    ae_assert(ae_isfinite(epsb, _state)&&ae_fp_greater_eq(epsb,0), "LinLSQRSetCond: EpsB is negative, INF or NAN", _state);
    ae_assert(maxits>=0, "LinLSQRSetCond: MaxIts is negative", _state);
    if( (ae_fp_eq(epsa,0)&&ae_fp_eq(epsb,0))&&maxits==0 )
    {
        state->epsa = linlsqr_atol;
        state->epsb = linlsqr_btol;
        state->maxits = state->n;
    }
    else
    {
        state->epsa = epsa;
        state->epsb = epsb;
        state->maxits = maxits;
    }
}


/*************************************************************************
LSQR solver: results.

This function must be called after LinLSQRSolve

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    *  1    ||Rk||<=EpsB*||B||
                    *  4    ||A^T*Rk||/(||A||*||Rk||)<=EpsA
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            X contains best point found so far.
                            (sometimes returned on singular systems)
                * Rep.IterationsCount contains iterations count
                * NMV countains number of matrix-vector calculations
                
  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrresults(linlsqrstate* state,
     /* Real    */ ae_vector* x,
     linlsqrreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _linlsqrreport_clear(rep);

    ae_assert(!state->running, "LinLSQRResult: you can not call this function when LinLSQRIteration is running", _state);
    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nmv = state->repnmv;
    rep->terminationtype = state->repterminationtype;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetxrep(linlsqrstate* state,
     ae_bool needxrep,
     ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function restarts LinLSQRIteration

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrrestart(linlsqrstate* state, ae_state *_state)
{


    ae_vector_set_length(&state->rstate.ia, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 0+1, _state);
    state->rstate.stage = -1;
    linlsqr_clearrfields(state, _state);
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void linlsqr_clearrfields(linlsqrstate* state, ae_state *_state)
{


    state->xupdated = ae_false;
    state->needmv = ae_false;
    state->needmtv = ae_false;
    state->needmv2 = ae_false;
    state->needvmv = ae_false;
    state->needprec = ae_false;
}


ae_bool _linlsqrstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    linlsqrstate *p = (linlsqrstate*)_p;
    ae_touch_ptr((void*)p);
    if( !_normestimatorstate_init(&p->nes, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rx, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->ui, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->uip1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->vi, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->vip1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->omegai, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->omegaip1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->mv, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->mtv, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmpd, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmpx, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init(&p->rstate, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _linlsqrstate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    linlsqrstate *dst = (linlsqrstate*)_dst;
    linlsqrstate *src = (linlsqrstate*)_src;
    if( !_normestimatorstate_init_copy(&dst->nes, &src->nes, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rx, &src->rx, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic) )
        return ae_false;
    dst->n = src->n;
    dst->m = src->m;
    dst->prectype = src->prectype;
    if( !ae_vector_init_copy(&dst->ui, &src->ui, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->uip1, &src->uip1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->vi, &src->vi, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->vip1, &src->vip1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->omegai, &src->omegai, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->omegaip1, &src->omegaip1, _state, make_automatic) )
        return ae_false;
    dst->alphai = src->alphai;
    dst->alphaip1 = src->alphaip1;
    dst->betai = src->betai;
    dst->betaip1 = src->betaip1;
    dst->phibari = src->phibari;
    dst->phibarip1 = src->phibarip1;
    dst->phii = src->phii;
    dst->rhobari = src->rhobari;
    dst->rhobarip1 = src->rhobarip1;
    dst->rhoi = src->rhoi;
    dst->ci = src->ci;
    dst->si = src->si;
    dst->theta = src->theta;
    dst->lambdai = src->lambdai;
    if( !ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic) )
        return ae_false;
    dst->anorm = src->anorm;
    dst->bnorm2 = src->bnorm2;
    dst->dnorm = src->dnorm;
    dst->r2 = src->r2;
    if( !ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->mv, &src->mv, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->mtv, &src->mtv, _state, make_automatic) )
        return ae_false;
    dst->epsa = src->epsa;
    dst->epsb = src->epsb;
    dst->epsc = src->epsc;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->xupdated = src->xupdated;
    dst->needmv = src->needmv;
    dst->needmtv = src->needmtv;
    dst->needmv2 = src->needmv2;
    dst->needvmv = src->needvmv;
    dst->needprec = src->needprec;
    dst->repiterationscount = src->repiterationscount;
    dst->repnmv = src->repnmv;
    dst->repterminationtype = src->repterminationtype;
    dst->running = src->running;
    if( !ae_vector_init_copy(&dst->tmpd, &src->tmpd, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmpx, &src->tmpx, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _linlsqrstate_clear(void* _p)
{
    linlsqrstate *p = (linlsqrstate*)_p;
    ae_touch_ptr((void*)p);
    _normestimatorstate_clear(&p->nes);
    ae_vector_clear(&p->rx);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->ui);
    ae_vector_clear(&p->uip1);
    ae_vector_clear(&p->vi);
    ae_vector_clear(&p->vip1);
    ae_vector_clear(&p->omegai);
    ae_vector_clear(&p->omegaip1);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->mv);
    ae_vector_clear(&p->mtv);
    ae_vector_clear(&p->tmpd);
    ae_vector_clear(&p->tmpx);
    _rcommstate_clear(&p->rstate);
}


void _linlsqrstate_destroy(void* _p)
{
    linlsqrstate *p = (linlsqrstate*)_p;
    ae_touch_ptr((void*)p);
    _normestimatorstate_destroy(&p->nes);
    ae_vector_destroy(&p->rx);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->ui);
    ae_vector_destroy(&p->uip1);
    ae_vector_destroy(&p->vi);
    ae_vector_destroy(&p->vip1);
    ae_vector_destroy(&p->omegai);
    ae_vector_destroy(&p->omegaip1);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->mv);
    ae_vector_destroy(&p->mtv);
    ae_vector_destroy(&p->tmpd);
    ae_vector_destroy(&p->tmpx);
    _rcommstate_destroy(&p->rstate);
}


ae_bool _linlsqrreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    linlsqrreport *p = (linlsqrreport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _linlsqrreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    linlsqrreport *dst = (linlsqrreport*)_dst;
    linlsqrreport *src = (linlsqrreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nmv = src->nmv;
    dst->terminationtype = src->terminationtype;
    return ae_true;
}


void _linlsqrreport_clear(void* _p)
{
    linlsqrreport *p = (linlsqrreport*)_p;
    ae_touch_ptr((void*)p);
}


void _linlsqrreport_destroy(void* _p)
{
    linlsqrreport *p = (linlsqrreport*)_p;
    ae_touch_ptr((void*)p);
}




/*************************************************************************
This function initializes linear CG Solver. This solver is used  to  solve
symmetric positive definite problems. If you want  to  solve  nonsymmetric
(or non-positive definite) problem you may use LinLSQR solver provided  by
ALGLIB.

USAGE:
1. User initializes algorithm state with LinCGCreate() call
2. User tunes solver parameters with  LinCGSetCond() and other functions
3. Optionally, user sets starting point with LinCGSetStartingPoint()
4. User  calls LinCGSolveSparse() function which takes algorithm state and
   SparseMatrix object.
5. User calls LinCGResults() to get solution
6. Optionally, user may call LinCGSolveSparse()  again  to  solve  another
   problem  with different matrix and/or right part without reinitializing
   LinCGState structure.
  
INPUT PARAMETERS:
    N       -   problem dimension, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgcreate(ae_int_t n, lincgstate* state, ae_state *_state)
{
    ae_int_t i;

    _lincgstate_clear(state);

    ae_assert(n>0, "LinCGCreate: N<=0", _state);
    state->n = n;
    state->prectype = 0;
    state->itsbeforerestart = n;
    state->itsbeforerupdate = 10;
    state->epsf = lincg_defaultprecision;
    state->maxits = 0;
    state->xrep = ae_false;
    state->running = ae_false;
    
    /*
     * * allocate arrays
     * * set RX to NAN (just for the case user calls Results() without 
     *   calling SolveSparse()
     * * set starting point to zero
     * * we do NOT initialize B here because we assume that user should
     *   initializate it using LinCGSetB() function. In case he forgets
     *   to do so, exception will be thrown in the LinCGIteration().
     */
    ae_vector_set_length(&state->rx, state->n, _state);
    ae_vector_set_length(&state->startx, state->n, _state);
    ae_vector_set_length(&state->b, state->n, _state);
    for(i=0; i<=state->n-1; i++)
    {
        state->rx.ptr.p_double[i] = _state->v_nan;
        state->startx.ptr.p_double[i] = 0.0;
        state->b.ptr.p_double[i] = 0;
    }
    ae_vector_set_length(&state->cx, state->n, _state);
    ae_vector_set_length(&state->p, state->n, _state);
    ae_vector_set_length(&state->r, state->n, _state);
    ae_vector_set_length(&state->cr, state->n, _state);
    ae_vector_set_length(&state->z, state->n, _state);
    ae_vector_set_length(&state->cz, state->n, _state);
    ae_vector_set_length(&state->x, state->n, _state);
    ae_vector_set_length(&state->mv, state->n, _state);
    ae_vector_set_length(&state->pv, state->n, _state);
    lincg_updateitersdata(state, _state);
    ae_vector_set_length(&state->rstate.ia, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************
This function sets starting point.
By default, zero starting point is used.

INPUT PARAMETERS:
    X       -   starting point, array[N]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetstartingpoint(lincgstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetStartingPoint: you can not change starting point because LinCGIteration() function is running", _state);
    ae_assert(state->n<=x->cnt, "LinCGSetStartingPoint: Length(X)<N", _state);
    ae_assert(isfinitevector(x, state->n, _state), "LinCGSetStartingPoint: X contains infinite or NaN values!", _state);
    ae_v_move(&state->startx.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
}


/*************************************************************************
This function sets right part. By default, right part is zero.

INPUT PARAMETERS:
    B       -   right part, array[N].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetb(lincgstate* state,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetB: you can not set B, because function LinCGIteration is running!", _state);
    ae_assert(b->cnt>=state->n, "LinCGSetB: Length(B)<N", _state);
    ae_assert(isfinitevector(b, state->n, _state), "LinCGSetB: B contains infinite or NaN values!", _state);
    ae_v_move(&state->b.ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
}


/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function. By default, SolveSparse() uses diagonal preconditioner,  but  if
you want to use solver without preconditioning, you can call this function
which forces solver to use unit matrix for preconditioning.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void lincgsetprecunit(lincgstate* state, ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetPrecUnit: you can not change preconditioner, because function LinCGIteration is running!", _state);
    state->prectype = -1;
}


/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function.  LinCGSolveSparse() will use diagonal of the  system  matrix  as
preconditioner. This preconditioning mode is active by default.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void lincgsetprecdiag(lincgstate* state, ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetPrecDiag: you can not change preconditioner, because function LinCGIteration is running!", _state);
    state->prectype = 0;
}


/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsF    -   algorithm will be stopped if norm of residual is less than 
                EpsF*||b||.
    MaxIts  -   algorithm will be stopped if number of iterations is  more 
                than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
If  both  EpsF  and  MaxIts  are  zero then small EpsF will be set to small 
value.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetcond(lincgstate* state,
     double epsf,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetCond: you can not change stopping criteria when LinCGIteration() is running", _state);
    ae_assert(ae_isfinite(epsf, _state)&&ae_fp_greater_eq(epsf,0), "LinCGSetCond: EpsF is negative or contains infinite or NaN values", _state);
    ae_assert(maxits>=0, "LinCGSetCond: MaxIts is negative", _state);
    if( ae_fp_eq(epsf,0)&&maxits==0 )
    {
        state->epsf = lincg_defaultprecision;
        state->maxits = maxits;
    }
    else
    {
        state->epsf = epsf;
        state->maxits = maxits;
    }
}


/*************************************************************************
Reverse communication version of linear CG.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
ae_bool lincgiteration(lincgstate* state, ae_state *_state)
{
    ae_int_t i;
    double uvar;
    double bnorm;
    double v;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        i = state->rstate.ia.ptr.p_int[0];
        uvar = state->rstate.ra.ptr.p_double[0];
        bnorm = state->rstate.ra.ptr.p_double[1];
        v = state->rstate.ra.ptr.p_double[2];
    }
    else
    {
        i = -983;
        uvar = -989;
        bnorm = -834;
        v = 900;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    if( state->rstate.stage==5 )
    {
        goto lbl_5;
    }
    if( state->rstate.stage==6 )
    {
        goto lbl_6;
    }
    if( state->rstate.stage==7 )
    {
        goto lbl_7;
    }
    
    /*
     * Routine body
     */
    ae_assert(state->b.cnt>0, "LinCGIteration: B is not initialized (you must initialize B by LinCGSetB() call", _state);
    state->running = ae_true;
    state->repnmv = 0;
    lincg_clearrfields(state, _state);
    lincg_updateitersdata(state, _state);
    
    /*
     * Start 0-th iteration
     */
    ae_v_move(&state->rx.ptr.p_double[0], 1, &state->startx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needvmv = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needvmv = ae_false;
    bnorm = 0;
    state->r2 = 0;
    state->meritfunction = 0;
    for(i=0; i<=state->n-1; i++)
    {
        state->r.ptr.p_double[i] = state->b.ptr.p_double[i]-state->mv.ptr.p_double[i];
        state->r2 = state->r2+state->r.ptr.p_double[i]*state->r.ptr.p_double[i];
        state->meritfunction = state->meritfunction+state->mv.ptr.p_double[i]*state->rx.ptr.p_double[i]-2*state->b.ptr.p_double[i]*state->rx.ptr.p_double[i];
        bnorm = bnorm+state->b.ptr.p_double[i]*state->b.ptr.p_double[i];
    }
    bnorm = ae_sqrt(bnorm, _state);
    
    /*
     * Output first report
     */
    if( !state->xrep )
    {
        goto lbl_8;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    lincg_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
lbl_8:
    
    /*
     * Is x0 a solution?
     */
    if( !ae_isfinite(state->r2, _state)||ae_fp_less_eq(ae_sqrt(state->r2, _state),state->epsf*bnorm) )
    {
        state->running = ae_false;
        if( ae_isfinite(state->r2, _state) )
        {
            state->repterminationtype = 1;
        }
        else
        {
            state->repterminationtype = -4;
        }
        result = ae_false;
        return result;
    }
    
    /*
     * Calculate Z and P
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->r.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needprec = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needprec = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->z.ptr.p_double[i] = state->pv.ptr.p_double[i];
        state->p.ptr.p_double[i] = state->z.ptr.p_double[i];
    }
    
    /*
     * Other iterations(1..N)
     */
    state->repiterationscount = 0;
lbl_10:
    if( ae_false )
    {
        goto lbl_11;
    }
    state->repiterationscount = state->repiterationscount+1;
    
    /*
     * Calculate Alpha
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->p.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needvmv = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needvmv = ae_false;
    if( !ae_isfinite(state->vmv, _state)||ae_fp_less_eq(state->vmv,0) )
    {
        
        /*
         * a) Overflow when calculating VMV
         * b) non-positive VMV (non-SPD matrix)
         */
        state->running = ae_false;
        if( ae_isfinite(state->vmv, _state) )
        {
            state->repterminationtype = -5;
        }
        else
        {
            state->repterminationtype = -4;
        }
        result = ae_false;
        return result;
    }
    state->alpha = 0;
    for(i=0; i<=state->n-1; i++)
    {
        state->alpha = state->alpha+state->r.ptr.p_double[i]*state->z.ptr.p_double[i];
    }
    state->alpha = state->alpha/state->vmv;
    if( !ae_isfinite(state->alpha, _state) )
    {
        
        /*
         * Overflow when calculating Alpha
         */
        state->running = ae_false;
        state->repterminationtype = -4;
        result = ae_false;
        return result;
    }
    
    /*
     * Next step toward solution
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->cx.ptr.p_double[i] = state->rx.ptr.p_double[i]+state->alpha*state->p.ptr.p_double[i];
    }
    
    /*
     * Calculate R:
     * * use recurrent relation to update R
     * * at every ItsBeforeRUpdate-th iteration recalculate it from scratch, using matrix-vector product
     *   in case R grows instead of decreasing, algorithm is terminated with positive completion code
     */
    if( !(state->itsbeforerupdate==0||state->repiterationscount%state->itsbeforerupdate!=0) )
    {
        goto lbl_12;
    }
    
    /*
     * Calculate R using recurrent formula
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->cr.ptr.p_double[i] = state->r.ptr.p_double[i]-state->alpha*state->mv.ptr.p_double[i];
        state->x.ptr.p_double[i] = state->cr.ptr.p_double[i];
    }
    goto lbl_13;
lbl_12:
    
    /*
     * Calculate R using matrix-vector multiplication
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->cx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needmv = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needmv = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->cr.ptr.p_double[i] = state->b.ptr.p_double[i]-state->mv.ptr.p_double[i];
        state->x.ptr.p_double[i] = state->cr.ptr.p_double[i];
    }
    
    /*
     * Calculating merit function
     * Check emergency stopping criterion
     */
    v = 0;
    for(i=0; i<=state->n-1; i++)
    {
        v = v+state->mv.ptr.p_double[i]*state->cx.ptr.p_double[i]-2*state->b.ptr.p_double[i]*state->cx.ptr.p_double[i];
    }
    if( ae_fp_less(v,state->meritfunction) )
    {
        goto lbl_14;
    }
    for(i=0; i<=state->n-1; i++)
    {
        if( !ae_isfinite(state->rx.ptr.p_double[i], _state) )
        {
            state->running = ae_false;
            state->repterminationtype = -4;
            result = ae_false;
            return result;
        }
    }
    
    /*
     *output last report
     */
    if( !state->xrep )
    {
        goto lbl_16;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    lincg_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->xupdated = ae_false;
lbl_16:
    state->running = ae_false;
    state->repterminationtype = 7;
    result = ae_false;
    return result;
lbl_14:
    state->meritfunction = v;
lbl_13:
    ae_v_move(&state->rx.ptr.p_double[0], 1, &state->cx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    
    /*
     * calculating RNorm
     *
     * NOTE: monotonic decrease of R2 is not guaranteed by algorithm.
     */
    state->r2 = 0;
    for(i=0; i<=state->n-1; i++)
    {
        state->r2 = state->r2+state->cr.ptr.p_double[i]*state->cr.ptr.p_double[i];
    }
    
    /*
     *output report
     */
    if( !state->xrep )
    {
        goto lbl_18;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    lincg_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->xupdated = ae_false;
lbl_18:
    
    /*
     *stopping criterion
     *achieved the required precision
     */
    if( !ae_isfinite(state->r2, _state)||ae_fp_less_eq(ae_sqrt(state->r2, _state),state->epsf*bnorm) )
    {
        state->running = ae_false;
        if( ae_isfinite(state->r2, _state) )
        {
            state->repterminationtype = 1;
        }
        else
        {
            state->repterminationtype = -4;
        }
        result = ae_false;
        return result;
    }
    if( state->repiterationscount>=state->maxits&&state->maxits>0 )
    {
        for(i=0; i<=state->n-1; i++)
        {
            if( !ae_isfinite(state->rx.ptr.p_double[i], _state) )
            {
                state->running = ae_false;
                state->repterminationtype = -4;
                result = ae_false;
                return result;
            }
        }
        
        /*
         *if X is finite number
         */
        state->running = ae_false;
        state->repterminationtype = 5;
        result = ae_false;
        return result;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->cr.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    
    /*
     *prepere of parameters for next iteration
     */
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needprec = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->needprec = ae_false;
    ae_v_move(&state->cz.ptr.p_double[0], 1, &state->pv.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    if( state->repiterationscount%state->itsbeforerestart!=0 )
    {
        state->beta = 0;
        uvar = 0;
        for(i=0; i<=state->n-1; i++)
        {
            state->beta = state->beta+state->cz.ptr.p_double[i]*state->cr.ptr.p_double[i];
            uvar = uvar+state->z.ptr.p_double[i]*state->r.ptr.p_double[i];
        }
        
        /*
         *check that UVar is't INF or is't zero
         */
        if( !ae_isfinite(uvar, _state)||ae_fp_eq(uvar,0) )
        {
            state->running = ae_false;
            state->repterminationtype = -4;
            result = ae_false;
            return result;
        }
        
        /*
         *calculate .BETA
         */
        state->beta = state->beta/uvar;
        
        /*
         *check that .BETA neither INF nor NaN
         */
        if( !ae_isfinite(state->beta, _state) )
        {
            state->running = ae_false;
            state->repterminationtype = -1;
            result = ae_false;
            return result;
        }
        for(i=0; i<=state->n-1; i++)
        {
            state->p.ptr.p_double[i] = state->cz.ptr.p_double[i]+state->beta*state->p.ptr.p_double[i];
        }
    }
    else
    {
        ae_v_move(&state->p.ptr.p_double[0], 1, &state->cz.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    }
    
    /*
     *prepere data for next iteration
     */
    for(i=0; i<=state->n-1; i++)
    {
        
        /*
         *write (k+1)th iteration to (k )th iteration
         */
        state->r.ptr.p_double[i] = state->cr.ptr.p_double[i];
        state->z.ptr.p_double[i] = state->cz.ptr.p_double[i];
    }
    goto lbl_10;
lbl_11:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = i;
    state->rstate.ra.ptr.p_double[0] = uvar;
    state->rstate.ra.ptr.p_double[1] = bnorm;
    state->rstate.ra.ptr.p_double[2] = v;
    return result;
}


/*************************************************************************
Procedure for solution of A*x=b with sparse A.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse matrix in the CRS format (you MUST contvert  it  to 
                CRS format by calling SparseConvertToCRS() function).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all 
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get solution by calling LinCGResults()
    
NOTE: this function uses lightweight preconditioning -  multiplication  by
      inverse of diag(A). If you want, you can turn preconditioning off by
      calling LinCGSetPrecUnit(). However, preconditioning cost is low and
      preconditioner  is  very  important  for  solution  of  badly scaled
      problems.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsolvesparse(lincgstate* state,
     sparsematrix* a,
     ae_bool isupper,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double v;
    double vmv;


    n = state->n;
    ae_assert(b->cnt>=state->n, "LinCGSetB: Length(B)<N", _state);
    ae_assert(isfinitevector(b, state->n, _state), "LinCGSetB: B contains infinite or NaN values!", _state);
    
    /*
     * Allocate temporaries
     */
    rvectorsetlengthatleast(&state->tmpd, n, _state);
    
    /*
     * Compute diagonal scaling matrix D
     */
    if( state->prectype==0 )
    {
        
        /*
         * Default preconditioner - inverse of matrix diagonal
         */
        for(i=0; i<=n-1; i++)
        {
            v = sparsegetdiagonal(a, i, _state);
            if( ae_fp_greater(v,0) )
            {
                state->tmpd.ptr.p_double[i] = 1/ae_sqrt(v, _state);
            }
            else
            {
                state->tmpd.ptr.p_double[i] = 1;
            }
        }
    }
    else
    {
        
        /*
         * No diagonal scaling
         */
        for(i=0; i<=n-1; i++)
        {
            state->tmpd.ptr.p_double[i] = 1;
        }
    }
    
    /*
     * Solve
     */
    lincgrestart(state, _state);
    lincgsetb(state, b, _state);
    while(lincgiteration(state, _state))
    {
        
        /*
         * Process different requests from optimizer
         */
        if( state->needmv )
        {
            sparsesmv(a, isupper, &state->x, &state->mv, _state);
        }
        if( state->needvmv )
        {
            sparsesmv(a, isupper, &state->x, &state->mv, _state);
            vmv = ae_v_dotproduct(&state->x.ptr.p_double[0], 1, &state->mv.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
            state->vmv = vmv;
        }
        if( state->needprec )
        {
            for(i=0; i<=n-1; i++)
            {
                state->pv.ptr.p_double[i] = state->x.ptr.p_double[i]*ae_sqr(state->tmpd.ptr.p_double[i], _state);
            }
        }
    }
}


/*************************************************************************
CG-solver: results.

This function must be called after LinCGSolve

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -5    input matrix is either not positive definite,
                            too large or too small                            
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                * Rep.IterationsCount contains iterations count
                * NMV countains number of matrix-vector calculations

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgresults(lincgstate* state,
     /* Real    */ ae_vector* x,
     lincgreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _lincgreport_clear(rep);

    ae_assert(!state->running, "LinCGResult: you can not get result, because function LinCGIteration has been launched!", _state);
    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nmv = state->repnmv;
    rep->terminationtype = state->repterminationtype;
    rep->r2 = state->r2;
}


/*************************************************************************
This function sets restart frequency. By default, algorithm  is  restarted
after N subsequent iterations.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetrestartfreq(lincgstate* state,
     ae_int_t srf,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetRestartFreq: you can not change restart frequency when LinCGIteration() is running", _state);
    ae_assert(srf>0, "LinCGSetRestartFreq: non-positive SRF", _state);
    state->itsbeforerestart = srf;
}


/*************************************************************************
This function sets frequency of residual recalculations.

Algorithm updates residual r_k using iterative formula,  but  recalculates
it from scratch after each 10 iterations. It is done to avoid accumulation
of numerical errors and to stop algorithm when r_k starts to grow.

Such low update frequence (1/10) gives very  little  overhead,  but  makes
algorithm a bit more robust against numerical errors. However, you may
change it 

INPUT PARAMETERS:
    Freq    -   desired update frequency, Freq>=0.
                Zero value means that no updates will be done.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetrupdatefreq(lincgstate* state,
     ae_int_t freq,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetRUpdateFreq: you can not change update frequency when LinCGIteration() is running", _state);
    ae_assert(freq>=0, "LinCGSetRUpdateFreq: non-positive Freq", _state);
    state->itsbeforerupdate = freq;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetxrep(lincgstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
Procedure for restart function LinCGIteration

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgrestart(lincgstate* state, ae_state *_state)
{


    ae_vector_set_length(&state->rstate.ia, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
    lincg_clearrfields(state, _state);
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void lincg_clearrfields(lincgstate* state, ae_state *_state)
{


    state->xupdated = ae_false;
    state->needmv = ae_false;
    state->needmtv = ae_false;
    state->needmv2 = ae_false;
    state->needvmv = ae_false;
    state->needprec = ae_false;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void lincg_updateitersdata(lincgstate* state, ae_state *_state)
{


    state->repiterationscount = 0;
    state->repnmv = 0;
    state->repterminationtype = 0;
}


ae_bool _lincgstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    lincgstate *p = (lincgstate*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_vector_init(&p->rx, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->cx, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->cr, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->cz, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->p, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->r, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->z, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->mv, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->pv, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->startx, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmpd, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init(&p->rstate, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _lincgstate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    lincgstate *dst = (lincgstate*)_dst;
    lincgstate *src = (lincgstate*)_src;
    if( !ae_vector_init_copy(&dst->rx, &src->rx, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic) )
        return ae_false;
    dst->n = src->n;
    dst->prectype = src->prectype;
    if( !ae_vector_init_copy(&dst->cx, &src->cx, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->cr, &src->cr, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->cz, &src->cz, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->p, &src->p, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->r, &src->r, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->z, &src->z, _state, make_automatic) )
        return ae_false;
    dst->alpha = src->alpha;
    dst->beta = src->beta;
    dst->r2 = src->r2;
    dst->meritfunction = src->meritfunction;
    if( !ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->mv, &src->mv, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->pv, &src->pv, _state, make_automatic) )
        return ae_false;
    dst->vmv = src->vmv;
    if( !ae_vector_init_copy(&dst->startx, &src->startx, _state, make_automatic) )
        return ae_false;
    dst->epsf = src->epsf;
    dst->maxits = src->maxits;
    dst->itsbeforerestart = src->itsbeforerestart;
    dst->itsbeforerupdate = src->itsbeforerupdate;
    dst->xrep = src->xrep;
    dst->xupdated = src->xupdated;
    dst->needmv = src->needmv;
    dst->needmtv = src->needmtv;
    dst->needmv2 = src->needmv2;
    dst->needvmv = src->needvmv;
    dst->needprec = src->needprec;
    dst->repiterationscount = src->repiterationscount;
    dst->repnmv = src->repnmv;
    dst->repterminationtype = src->repterminationtype;
    dst->running = src->running;
    if( !ae_vector_init_copy(&dst->tmpd, &src->tmpd, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _lincgstate_clear(void* _p)
{
    lincgstate *p = (lincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->rx);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->cx);
    ae_vector_clear(&p->cr);
    ae_vector_clear(&p->cz);
    ae_vector_clear(&p->p);
    ae_vector_clear(&p->r);
    ae_vector_clear(&p->z);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->mv);
    ae_vector_clear(&p->pv);
    ae_vector_clear(&p->startx);
    ae_vector_clear(&p->tmpd);
    _rcommstate_clear(&p->rstate);
}


void _lincgstate_destroy(void* _p)
{
    lincgstate *p = (lincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->rx);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->cx);
    ae_vector_destroy(&p->cr);
    ae_vector_destroy(&p->cz);
    ae_vector_destroy(&p->p);
    ae_vector_destroy(&p->r);
    ae_vector_destroy(&p->z);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->mv);
    ae_vector_destroy(&p->pv);
    ae_vector_destroy(&p->startx);
    ae_vector_destroy(&p->tmpd);
    _rcommstate_destroy(&p->rstate);
}


ae_bool _lincgreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    lincgreport *p = (lincgreport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _lincgreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    lincgreport *dst = (lincgreport*)_dst;
    lincgreport *src = (lincgreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nmv = src->nmv;
    dst->terminationtype = src->terminationtype;
    dst->r2 = src->r2;
    return ae_true;
}


void _lincgreport_clear(void* _p)
{
    lincgreport *p = (lincgreport*)_p;
    ae_touch_ptr((void*)p);
}


void _lincgreport_destroy(void* _p)
{
    lincgreport *p = (lincgreport*)_p;
    ae_touch_ptr((void*)p);
}




/*************************************************************************
                LEVENBERG-MARQUARDT-LIKE NONLINEAR SOLVER

DESCRIPTION:
This algorithm solves system of nonlinear equations
    F[0](x[0], ..., x[n-1])   = 0
    F[1](x[0], ..., x[n-1])   = 0
    ...
    F[M-1](x[0], ..., x[n-1]) = 0
with M/N do not necessarily coincide.  Algorithm  converges  quadratically
under following conditions:
    * the solution set XS is nonempty
    * for some xs in XS there exist such neighbourhood N(xs) that:
      * vector function F(x) and its Jacobian J(x) are continuously
        differentiable on N
      * ||F(x)|| provides local error bound on N, i.e. there  exists  such
        c1, that ||F(x)||>c1*distance(x,XS)
Note that these conditions are much more weaker than usual non-singularity
conditions. For example, algorithm will converge for any  affine  function
F (whether its Jacobian singular or not).


REQUIREMENTS:
Algorithm will request following information during its operation:
* function vector F[] and Jacobian matrix at given point X
* value of merit function f(x)=F[0]^2(x)+...+F[M-1]^2(x) at given point X


USAGE:
1. User initializes algorithm state with NLEQCreateLM() call
2. User tunes solver parameters with  NLEQSetCond(),  NLEQSetStpMax()  and
   other functions
3. User  calls  NLEQSolve()  function  which  takes  algorithm  state  and
   pointers (delegates, etc.) to callback functions which calculate  merit
   function value and Jacobian.
4. User calls NLEQResults() to get solution
5. Optionally, user may call NLEQRestartFrom() to  solve  another  problem
   with same parameters (N/M) but another starting  point  and/or  another
   function vector. NLEQRestartFrom() allows to reuse already  initialized
   structure.


INPUT PARAMETERS:
    N       -   space dimension, N>1:
                * if provided, only leading N elements of X are used
                * if not provided, determined automatically from size of X
    M       -   system size
    X       -   starting point


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


NOTES:
1. you may tune stopping conditions with NLEQSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use NLEQSetStpMax() function to bound algorithm's steps.
3. this  algorithm  is  a  slightly  modified implementation of the method
   described  in  'Levenberg-Marquardt  method  for constrained  nonlinear
   equations with strong local convergence properties' by Christian Kanzow
   Nobuo Yamashita and Masao Fukushima and further  developed  in  'On the
   convergence of a New Levenberg-Marquardt Method'  by  Jin-yan  Fan  and
   Ya-Xiang Yuan.


  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqcreatelm(ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x,
     nleqstate* state,
     ae_state *_state)
{

    _nleqstate_clear(state);

    ae_assert(n>=1, "NLEQCreateLM: N<1!", _state);
    ae_assert(m>=1, "NLEQCreateLM: M<1!", _state);
    ae_assert(x->cnt>=n, "NLEQCreateLM: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "NLEQCreateLM: X contains infinite or NaN values!", _state);
    
    /*
     * Initialize
     */
    state->n = n;
    state->m = m;
    nleqsetcond(state, 0, 0, _state);
    nleqsetxrep(state, ae_false, _state);
    nleqsetstpmax(state, 0, _state);
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->xbase, n, _state);
    ae_matrix_set_length(&state->j, m, n, _state);
    ae_vector_set_length(&state->fi, m, _state);
    ae_vector_set_length(&state->rightpart, n, _state);
    ae_vector_set_length(&state->candstep, n, _state);
    nleqrestartfrom(state, x, _state);
}


/*************************************************************************
This function sets stopping conditions for the nonlinear solver

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsF    -   >=0
                The subroutine finishes  its work if on k+1-th iteration
                the condition ||F||<=EpsF is satisfied
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsF=0 and MaxIts=0 simultaneously will lead to  automatic
stopping criterion selection (small EpsF).

NOTES:

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetcond(nleqstate* state,
     double epsf,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsf, _state), "NLEQSetCond: EpsF is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsf,0), "NLEQSetCond: negative EpsF!", _state);
    ae_assert(maxits>=0, "NLEQSetCond: negative MaxIts!", _state);
    if( ae_fp_eq(epsf,0)&&maxits==0 )
    {
        epsf = 1.0E-6;
    }
    state->epsf = epsf;
    state->maxits = maxits;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to NLEQSolve().

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetxrep(nleqstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when target function  contains  exp()  or  other  fast
growing functions, and algorithm makes  too  large  steps  which  lead  to
overflow. This function allows us to reject steps that are too large  (and
therefore expose us to the possible overflow) without actually calculating
function value at the x+stp*d.

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetstpmax(nleqstate* state, double stpmax, ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "NLEQSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,0), "NLEQSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool nleqiteration(nleqstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    double lambdaup;
    double lambdadown;
    double lambdav;
    double rho;
    double mu;
    double stepnorm;
    ae_bool b;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        n = state->rstate.ia.ptr.p_int[0];
        m = state->rstate.ia.ptr.p_int[1];
        i = state->rstate.ia.ptr.p_int[2];
        b = state->rstate.ba.ptr.p_bool[0];
        lambdaup = state->rstate.ra.ptr.p_double[0];
        lambdadown = state->rstate.ra.ptr.p_double[1];
        lambdav = state->rstate.ra.ptr.p_double[2];
        rho = state->rstate.ra.ptr.p_double[3];
        mu = state->rstate.ra.ptr.p_double[4];
        stepnorm = state->rstate.ra.ptr.p_double[5];
    }
    else
    {
        n = -983;
        m = -989;
        i = -834;
        b = ae_false;
        lambdaup = -287;
        lambdadown = 364;
        lambdav = 214;
        rho = -338;
        mu = -686;
        stepnorm = 912;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    
    /*
     * Routine body
     */
    
    /*
     * Prepare
     */
    n = state->n;
    m = state->m;
    state->repterminationtype = 0;
    state->repiterationscount = 0;
    state->repnfunc = 0;
    state->repnjac = 0;
    
    /*
     * Calculate F/G, initialize algorithm
     */
    nleq_clearrequestfields(state, _state);
    state->needf = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needf = ae_false;
    state->repnfunc = state->repnfunc+1;
    ae_v_move(&state->xbase.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fbase = state->f;
    state->fprev = ae_maxrealnumber;
    if( !state->xrep )
    {
        goto lbl_5;
    }
    
    /*
     * progress report
     */
    nleq_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
lbl_5:
    if( ae_fp_less_eq(state->f,ae_sqr(state->epsf, _state)) )
    {
        state->repterminationtype = 1;
        result = ae_false;
        return result;
    }
    
    /*
     * Main cycle
     */
    lambdaup = 10;
    lambdadown = 0.3;
    lambdav = 0.001;
    rho = 1;
lbl_7:
    if( ae_false )
    {
        goto lbl_8;
    }
    
    /*
     * Get Jacobian;
     * before we get to this point we already have State.XBase filled
     * with current point and State.FBase filled with function value
     * at XBase
     */
    nleq_clearrequestfields(state, _state);
    state->needfij = ae_true;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfij = ae_false;
    state->repnfunc = state->repnfunc+1;
    state->repnjac = state->repnjac+1;
    rmatrixmv(n, m, &state->j, 0, 0, 1, &state->fi, 0, &state->rightpart, 0, _state);
    ae_v_muld(&state->rightpart.ptr.p_double[0], 1, ae_v_len(0,n-1), -1);
    
    /*
     * Inner cycle: find good lambda
     */
lbl_9:
    if( ae_false )
    {
        goto lbl_10;
    }
    
    /*
     * Solve (J^T*J + (Lambda+Mu)*I)*y = J^T*F
     * to get step d=-y where:
     * * Mu=||F|| - is damping parameter for nonlinear system
     * * Lambda   - is additional Levenberg-Marquardt parameter
     *              for better convergence when far away from minimum
     */
    for(i=0; i<=n-1; i++)
    {
        state->candstep.ptr.p_double[i] = 0;
    }
    fblssolvecgx(&state->j, m, n, lambdav, &state->rightpart, &state->candstep, &state->cgbuf, _state);
    
    /*
     * Normalize step (it must be no more than StpMax)
     */
    stepnorm = 0;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(state->candstep.ptr.p_double[i],0) )
        {
            stepnorm = 1;
            break;
        }
    }
    linminnormalized(&state->candstep, &stepnorm, n, _state);
    if( ae_fp_neq(state->stpmax,0) )
    {
        stepnorm = ae_minreal(stepnorm, state->stpmax, _state);
    }
    
    /*
     * Test new step - is it good enough?
     * * if not, Lambda is increased and we try again.
     * * if step is good, we decrease Lambda and move on.
     *
     * We can break this cycle on two occasions:
     * * step is so small that x+step==x (in floating point arithmetics)
     * * lambda is so large
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_addd(&state->x.ptr.p_double[0], 1, &state->candstep.ptr.p_double[0], 1, ae_v_len(0,n-1), stepnorm);
    b = ae_true;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(state->x.ptr.p_double[i],state->xbase.ptr.p_double[i]) )
        {
            b = ae_false;
            break;
        }
    }
    if( b )
    {
        
        /*
         * Step is too small, force zero step and break
         */
        stepnorm = 0;
        ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->f = state->fbase;
        goto lbl_10;
    }
    nleq_clearrequestfields(state, _state);
    state->needf = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needf = ae_false;
    state->repnfunc = state->repnfunc+1;
    if( ae_fp_less(state->f,state->fbase) )
    {
        
        /*
         * function value decreased, move on
         */
        nleq_decreaselambda(&lambdav, &rho, lambdadown, _state);
        goto lbl_10;
    }
    if( !nleq_increaselambda(&lambdav, &rho, lambdaup, _state) )
    {
        
        /*
         * Lambda is too large (near overflow), force zero step and break
         */
        stepnorm = 0;
        ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->f = state->fbase;
        goto lbl_10;
    }
    goto lbl_9;
lbl_10:
    
    /*
     * Accept step:
     * * new position
     * * new function value
     */
    state->fbase = state->f;
    ae_v_addd(&state->xbase.ptr.p_double[0], 1, &state->candstep.ptr.p_double[0], 1, ae_v_len(0,n-1), stepnorm);
    state->repiterationscount = state->repiterationscount+1;
    
    /*
     * Report new iteration
     */
    if( !state->xrep )
    {
        goto lbl_11;
    }
    nleq_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->f = state->fbase;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->xupdated = ae_false;
lbl_11:
    
    /*
     * Test stopping conditions on F, step (zero/non-zero) and MaxIts;
     * If one of the conditions is met, RepTerminationType is changed.
     */
    if( ae_fp_less_eq(ae_sqrt(state->f, _state),state->epsf) )
    {
        state->repterminationtype = 1;
    }
    if( ae_fp_eq(stepnorm,0)&&state->repterminationtype==0 )
    {
        state->repterminationtype = -4;
    }
    if( state->repiterationscount>=state->maxits&&state->maxits>0 )
    {
        state->repterminationtype = 5;
    }
    if( state->repterminationtype!=0 )
    {
        goto lbl_8;
    }
    
    /*
     * Now, iteration is finally over
     */
    goto lbl_7;
lbl_8:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = m;
    state->rstate.ia.ptr.p_int[2] = i;
    state->rstate.ba.ptr.p_bool[0] = b;
    state->rstate.ra.ptr.p_double[0] = lambdaup;
    state->rstate.ra.ptr.p_double[1] = lambdadown;
    state->rstate.ra.ptr.p_double[2] = lambdav;
    state->rstate.ra.ptr.p_double[3] = rho;
    state->rstate.ra.ptr.p_double[4] = mu;
    state->rstate.ra.ptr.p_double[5] = stepnorm;
    return result;
}


/*************************************************************************
NLEQ solver results

INPUT PARAMETERS:
    State   -   algorithm state.

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -4    ERROR:  algorithm   has   converged   to   the
                            stationary point Xf which is local minimum  of
                            f=F[0]^2+...+F[m-1]^2, but is not solution  of
                            nonlinear system.
                    *  1    sqrt(f)<=EpsF.
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations
                * ActiveConstraints contains number of active constraints

  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqresults(nleqstate* state,
     /* Real    */ ae_vector* x,
     nleqreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _nleqreport_clear(rep);

    nleqresultsbuf(state, x, rep, _state);
}


/*************************************************************************
NLEQ solver results

Buffered implementation of NLEQResults(), which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqresultsbuf(nleqstate* state,
     /* Real    */ ae_vector* x,
     nleqreport* rep,
     ae_state *_state)
{


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nfunc = state->repnfunc;
    rep->njac = state->repnjac;
    rep->terminationtype = state->repterminationtype;
}


/*************************************************************************
This  subroutine  restarts  CG  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used for reverse communication previously
                allocated with MinCGCreate call.
    X       -   new starting point.
    BndL    -   new lower bounds
    BndU    -   new upper bounds

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void nleqrestartfrom(nleqstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{


    ae_assert(x->cnt>=state->n, "NLEQRestartFrom: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, state->n, _state), "NLEQRestartFrom: X contains infinite or NaN values!", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_vector_set_length(&state->rstate.ia, 2+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 5+1, _state);
    state->rstate.stage = -1;
    nleq_clearrequestfields(state, _state);
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void nleq_clearrequestfields(nleqstate* state, ae_state *_state)
{


    state->needf = ae_false;
    state->needfij = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Increases lambda, returns False when there is a danger of overflow
*************************************************************************/
static ae_bool nleq_increaselambda(double* lambdav,
     double* nu,
     double lambdaup,
     ae_state *_state)
{
    double lnlambda;
    double lnnu;
    double lnlambdaup;
    double lnmax;
    ae_bool result;


    result = ae_false;
    lnlambda = ae_log(*lambdav, _state);
    lnlambdaup = ae_log(lambdaup, _state);
    lnnu = ae_log(*nu, _state);
    lnmax = 0.5*ae_log(ae_maxrealnumber, _state);
    if( ae_fp_greater(lnlambda+lnlambdaup+lnnu,lnmax) )
    {
        return result;
    }
    if( ae_fp_greater(lnnu+ae_log(2, _state),lnmax) )
    {
        return result;
    }
    *lambdav = *lambdav*lambdaup*(*nu);
    *nu = *nu*2;
    result = ae_true;
    return result;
}


/*************************************************************************
Decreases lambda, but leaves it unchanged when there is danger of underflow.
*************************************************************************/
static void nleq_decreaselambda(double* lambdav,
     double* nu,
     double lambdadown,
     ae_state *_state)
{


    *nu = 1;
    if( ae_fp_less(ae_log(*lambdav, _state)+ae_log(lambdadown, _state),ae_log(ae_minrealnumber, _state)) )
    {
        *lambdav = ae_minrealnumber;
    }
    else
    {
        *lambdav = *lambdav*lambdadown;
    }
}


ae_bool _nleqstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nleqstate *p = (nleqstate*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init(&p->rstate, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xbase, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->candstep, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rightpart, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->cgbuf, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _nleqstate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    nleqstate *dst = (nleqstate*)_dst;
    nleqstate *src = (nleqstate*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->epsf = src->epsf;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->stpmax = src->stpmax;
    if( !ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic) )
        return ae_false;
    dst->f = src->f;
    if( !ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic) )
        return ae_false;
    dst->needf = src->needf;
    dst->needfij = src->needfij;
    dst->xupdated = src->xupdated;
    if( !_rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic) )
        return ae_false;
    dst->repiterationscount = src->repiterationscount;
    dst->repnfunc = src->repnfunc;
    dst->repnjac = src->repnjac;
    dst->repterminationtype = src->repterminationtype;
    if( !ae_vector_init_copy(&dst->xbase, &src->xbase, _state, make_automatic) )
        return ae_false;
    dst->fbase = src->fbase;
    dst->fprev = src->fprev;
    if( !ae_vector_init_copy(&dst->candstep, &src->candstep, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rightpart, &src->rightpart, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->cgbuf, &src->cgbuf, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _nleqstate_clear(void* _p)
{
    nleqstate *p = (nleqstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->xbase);
    ae_vector_clear(&p->candstep);
    ae_vector_clear(&p->rightpart);
    ae_vector_clear(&p->cgbuf);
}


void _nleqstate_destroy(void* _p)
{
    nleqstate *p = (nleqstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->xbase);
    ae_vector_destroy(&p->candstep);
    ae_vector_destroy(&p->rightpart);
    ae_vector_destroy(&p->cgbuf);
}


ae_bool _nleqreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nleqreport *p = (nleqreport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _nleqreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    nleqreport *dst = (nleqreport*)_dst;
    nleqreport *src = (nleqreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfunc = src->nfunc;
    dst->njac = src->njac;
    dst->terminationtype = src->terminationtype;
    return ae_true;
}


void _nleqreport_clear(void* _p)
{
    nleqreport *p = (nleqreport*)_p;
    ae_touch_ptr((void*)p);
}


void _nleqreport_destroy(void* _p)
{
    nleqreport *p = (nleqreport*)_p;
    ae_touch_ptr((void*)p);
}



}

