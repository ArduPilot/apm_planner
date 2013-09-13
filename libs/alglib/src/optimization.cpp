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
#include "optimization.h"

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
This object stores state of the nonlinear CG optimizer.

You should use ALGLIB functions to work with this object.
*************************************************************************/
_mincgstate_owner::_mincgstate_owner()
{
    p_struct = (alglib_impl::mincgstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::mincgstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_mincgstate_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_mincgstate_owner::_mincgstate_owner(const _mincgstate_owner &rhs)
{
    p_struct = (alglib_impl::mincgstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::mincgstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_mincgstate_init_copy(p_struct, const_cast<alglib_impl::mincgstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_mincgstate_owner& _mincgstate_owner::operator=(const _mincgstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_mincgstate_clear(p_struct);
    if( !alglib_impl::_mincgstate_init_copy(p_struct, const_cast<alglib_impl::mincgstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_mincgstate_owner::~_mincgstate_owner()
{
    alglib_impl::_mincgstate_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::mincgstate* _mincgstate_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::mincgstate* _mincgstate_owner::c_ptr() const
{
    return const_cast<alglib_impl::mincgstate*>(p_struct);
}
mincgstate::mincgstate() : _mincgstate_owner() ,needf(p_struct->needf),needfg(p_struct->needfg),xupdated(p_struct->xupdated),f(p_struct->f),g(&p_struct->g),x(&p_struct->x)
{
}

mincgstate::mincgstate(const mincgstate &rhs):_mincgstate_owner(rhs) ,needf(p_struct->needf),needfg(p_struct->needfg),xupdated(p_struct->xupdated),f(p_struct->f),g(&p_struct->g),x(&p_struct->x)
{
}

mincgstate& mincgstate::operator=(const mincgstate &rhs)
{
    if( this==&rhs )
        return *this;
    _mincgstate_owner::operator=(rhs);
    return *this;
}

mincgstate::~mincgstate()
{
}


/*************************************************************************

*************************************************************************/
_mincgreport_owner::_mincgreport_owner()
{
    p_struct = (alglib_impl::mincgreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::mincgreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_mincgreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_mincgreport_owner::_mincgreport_owner(const _mincgreport_owner &rhs)
{
    p_struct = (alglib_impl::mincgreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::mincgreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_mincgreport_init_copy(p_struct, const_cast<alglib_impl::mincgreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_mincgreport_owner& _mincgreport_owner::operator=(const _mincgreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_mincgreport_clear(p_struct);
    if( !alglib_impl::_mincgreport_init_copy(p_struct, const_cast<alglib_impl::mincgreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_mincgreport_owner::~_mincgreport_owner()
{
    alglib_impl::_mincgreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::mincgreport* _mincgreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::mincgreport* _mincgreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::mincgreport*>(p_struct);
}
mincgreport::mincgreport() : _mincgreport_owner() ,iterationscount(p_struct->iterationscount),nfev(p_struct->nfev),varidx(p_struct->varidx),terminationtype(p_struct->terminationtype)
{
}

mincgreport::mincgreport(const mincgreport &rhs):_mincgreport_owner(rhs) ,iterationscount(p_struct->iterationscount),nfev(p_struct->nfev),varidx(p_struct->varidx),terminationtype(p_struct->terminationtype)
{
}

mincgreport& mincgreport::operator=(const mincgreport &rhs)
{
    if( this==&rhs )
        return *this;
    _mincgreport_owner::operator=(rhs);
    return *this;
}

mincgreport::~mincgreport()
{
}

/*************************************************************************
        NONLINEAR CONJUGATE GRADIENT METHOD

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by using one of  the
nonlinear conjugate gradient methods.

These CG methods are globally convergent (even on non-convex functions) as
long as grad(f) is Lipschitz continuous in  a  some  neighborhood  of  the
L = { x : f(x)<=f(x0) }.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinCGCreate() call
2. User tunes solver parameters with MinCGSetCond(), MinCGSetStpMax() and
   other functions
3. User calls MinCGOptimize() function which takes algorithm  state   and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinCGResults() to get solution
5. Optionally, user may call MinCGRestartFrom() to solve another  problem
   with same N but another starting point and/or another function.
   MinCGRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void mincgcreate(const ae_int_t n, const real_1d_array &x, mincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgcreate(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::mincgstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
        NONLINEAR CONJUGATE GRADIENT METHOD

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by using one of  the
nonlinear conjugate gradient methods.

These CG methods are globally convergent (even on non-convex functions) as
long as grad(f) is Lipschitz continuous in  a  some  neighborhood  of  the
L = { x : f(x)<=f(x0) }.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinCGCreate() call
2. User tunes solver parameters with MinCGSetCond(), MinCGSetStpMax() and
   other functions
3. User calls MinCGOptimize() function which takes algorithm  state   and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinCGResults() to get solution
5. Optionally, user may call MinCGRestartFrom() to solve another  problem
   with same N but another starting point and/or another function.
   MinCGRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void mincgcreate(const real_1d_array &x, mincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgcreate(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::mincgstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
The subroutine is finite difference variant of MinCGCreate(). It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this function
only. We recommend to read comments on MinCGCreate() in order to get more
information about creation of CG optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinCGSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust  and  precise.  L-BFGS  needs  exact  gradient values.
   Imprecise  gradient may slow down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void mincgcreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, mincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgcreatef(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), diffstep, const_cast<alglib_impl::mincgstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
The subroutine is finite difference variant of MinCGCreate(). It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this function
only. We recommend to read comments on MinCGCreate() in order to get more
information about creation of CG optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinCGSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust  and  precise.  L-BFGS  needs  exact  gradient values.
   Imprecise  gradient may slow down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void mincgcreatef(const real_1d_array &x, const double diffstep, mincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgcreatef(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), diffstep, const_cast<alglib_impl::mincgstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets stopping conditions for CG optimization algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinCGSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinCGSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcond(const mincgstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsetcond(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), epsg, epsf, epsx, maxits, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets scaling coefficients for CG optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of CG optimizer  -  step
along I-th axis is equal to DiffStep*S[I].

In   most   optimizers  (and  in  the  CG  too)  scaling is NOT a form  of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner by separate call to one of the MinCGSetPrec...() functions.

There  is  special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void mincgsetscale(const mincgstate &state, const real_1d_array &s)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsetscale(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(s.c_ptr()), &_alglib_env_state);
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
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetxrep(const mincgstate &state, const bool needxrep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsetxrep(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), needxrep, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets CG algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    CGType  -   algorithm type:
                * -1    automatic selection of the best algorithm
                * 0     DY (Dai and Yuan) algorithm
                * 1     Hybrid DY-HS algorithm

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcgtype(const mincgstate &state, const ae_int_t cgtype)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsetcgtype(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), cgtype, &_alglib_env_state);
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

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetstpmax(const mincgstate &state, const double stpmax)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsetstpmax(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), stpmax, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function allows to suggest initial step length to the CG algorithm.

Suggested  step  length  is used as starting point for the line search. It
can be useful when you have  badly  scaled  problem,  i.e.  when  ||grad||
(which is used as initial estimate for the first step) is many  orders  of
magnitude different from the desired step.

Line search  may  fail  on  such problems without good estimate of initial
step length. Imagine, for example, problem with ||grad||=10^50 and desired
step equal to 0.1 Line  search function will use 10^50  as  initial  step,
then  it  will  decrease step length by 2 (up to 20 attempts) and will get
10^44, which is still too large.

This function allows us to tell than line search should  be  started  from
some moderate step length, like 1.0, so algorithm will be able  to  detect
desired step length in a several searches.

Default behavior (when no step is suggested) is to use preconditioner,  if
it is available, to generate initial estimate of step length.

This function influences only first iteration of algorithm. It  should  be
called between MinCGCreate/MinCGRestartFrom() call and MinCGOptimize call.
Suggested step is ignored if you have preconditioner.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    Stp     -   initial estimate of the step length.
                Can be zero (no estimate).

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsuggeststep(const mincgstate &state, const double stp)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsuggeststep(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), stp, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdefault(const mincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsetprecdefault(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2: D[i] should be positive. Exception will be thrown otherwise.

NOTE 3: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdiag(const mincgstate &state, const real_1d_array &d)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsetprecdiag(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(d.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables with MinCGSetScale() call
(before or after MinCGSetPrecScale() call). Without knowledge of the scale
of your variables scale-based preconditioner will be just unit matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecscale(const mincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsetprecscale(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), &_alglib_env_state);
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
bool mincgiteration(const mincgstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        ae_bool result = alglib_impl::mincgiteration(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return *(reinterpret_cast<bool*>(&result));
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void mincgoptimize(mincgstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( func==NULL )
        throw ap_error("ALGLIB: error in 'mincgoptimize()' (func is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::mincgiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needf )
            {
                func(state.x, state.f, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'mincgoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void mincgoptimize(mincgstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( grad==NULL )
        throw ap_error("ALGLIB: error in 'mincgoptimize()' (grad is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::mincgiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needfg )
            {
                grad(state.x, state.f, state.g, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'mincgoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}



/*************************************************************************
Conjugate gradient results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -7    gradient verification failed.
                            See MinCGSetGradientCheck() for more information.
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible,
                            we return best X found so far
                    *  8    terminated by user
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresults(const mincgstate &state, real_1d_array &x, mincgreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgresults(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::mincgreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Conjugate gradient results

Buffered implementation of MinCGResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresultsbuf(const mincgstate &state, real_1d_array &x, mincgreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgresultsbuf(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::mincgreport*>(rep.c_ptr()), &_alglib_env_state);
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
    State   -   structure used to store algorithm state.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgrestartfrom(const mincgstate &state, const real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgrestartfrom(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************

This  subroutine  turns  on  verification  of  the  user-supplied analytic
gradient:
* user calls this subroutine before optimization begins
* MinCGOptimize() is called
* prior to  actual  optimization, for each component  of  parameters being
  optimized X[i] algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point
  * in case difference between prediction and actual value is higher  than
    some predetermined threshold, algorithm stops with completion code -7;
    Rep.VarIdx is set to index of the parameter with incorrect derivative.
* after verification is over, algorithm proceeds to the actual optimization.

NOTE 1: verification  needs  N (parameters count) gradient evaluations. It
        is very costly and you should use  it  only  for  low  dimensional
        problems,  when  you  want  to  be  sure  that  you've   correctly
        calculated  analytic  derivatives.  You  should  not use it in the
        production code (unless you want to check derivatives provided  by
        some third party).

NOTE 2: you  should  carefully  choose  TestStep. Value which is too large
        (so large that function behaviour is significantly non-cubic) will
        lead to false alarms. You may use  different  step  for  different
        parameters by means of setting scale with MinCGSetScale().

NOTE 3: this function may lead to false positives. In case it reports that
        I-th  derivative was calculated incorrectly, you may decrease test
        step  and  try  one  more  time  - maybe your function changes too
        sharply  and  your  step  is  too  large for such rapidly chanding
        function.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification

  -- ALGLIB --
     Copyright 31.05.2012 by Bochkanov Sergey
*************************************************************************/
void mincgsetgradientcheck(const mincgstate &state, const double teststep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::mincgsetgradientcheck(const_cast<alglib_impl::mincgstate*>(state.c_ptr()), teststep, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinBLEIC subpackage to work with this
object
*************************************************************************/
_minbleicstate_owner::_minbleicstate_owner()
{
    p_struct = (alglib_impl::minbleicstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minbleicstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minbleicstate_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minbleicstate_owner::_minbleicstate_owner(const _minbleicstate_owner &rhs)
{
    p_struct = (alglib_impl::minbleicstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minbleicstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minbleicstate_init_copy(p_struct, const_cast<alglib_impl::minbleicstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minbleicstate_owner& _minbleicstate_owner::operator=(const _minbleicstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minbleicstate_clear(p_struct);
    if( !alglib_impl::_minbleicstate_init_copy(p_struct, const_cast<alglib_impl::minbleicstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minbleicstate_owner::~_minbleicstate_owner()
{
    alglib_impl::_minbleicstate_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minbleicstate* _minbleicstate_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minbleicstate* _minbleicstate_owner::c_ptr() const
{
    return const_cast<alglib_impl::minbleicstate*>(p_struct);
}
minbleicstate::minbleicstate() : _minbleicstate_owner() ,needf(p_struct->needf),needfg(p_struct->needfg),xupdated(p_struct->xupdated),f(p_struct->f),g(&p_struct->g),x(&p_struct->x)
{
}

minbleicstate::minbleicstate(const minbleicstate &rhs):_minbleicstate_owner(rhs) ,needf(p_struct->needf),needfg(p_struct->needfg),xupdated(p_struct->xupdated),f(p_struct->f),g(&p_struct->g),x(&p_struct->x)
{
}

minbleicstate& minbleicstate::operator=(const minbleicstate &rhs)
{
    if( this==&rhs )
        return *this;
    _minbleicstate_owner::operator=(rhs);
    return *this;
}

minbleicstate::~minbleicstate()
{
}


/*************************************************************************
This structure stores optimization report:
* IterationsCount           number of iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -7    gradient verification failed.
        See MinBLEICSetGradientCheck() for more information.
  -3    inconsistent constraints. Feasible point is
        either nonexistent or too hard to find. Try to
        restart optimizer with better initial approximation
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

ADDITIONAL FIELDS

There are additional fields which can be used for debugging:
* DebugEqErr                error in the equality constraints (2-norm)
* DebugFS                   f, calculated at projection of initial point
                            to the feasible set
* DebugFF                   f, calculated at the final point
* DebugDX                   |X_start-X_final|
*************************************************************************/
_minbleicreport_owner::_minbleicreport_owner()
{
    p_struct = (alglib_impl::minbleicreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minbleicreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minbleicreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minbleicreport_owner::_minbleicreport_owner(const _minbleicreport_owner &rhs)
{
    p_struct = (alglib_impl::minbleicreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minbleicreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minbleicreport_init_copy(p_struct, const_cast<alglib_impl::minbleicreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minbleicreport_owner& _minbleicreport_owner::operator=(const _minbleicreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minbleicreport_clear(p_struct);
    if( !alglib_impl::_minbleicreport_init_copy(p_struct, const_cast<alglib_impl::minbleicreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minbleicreport_owner::~_minbleicreport_owner()
{
    alglib_impl::_minbleicreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minbleicreport* _minbleicreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minbleicreport* _minbleicreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::minbleicreport*>(p_struct);
}
minbleicreport::minbleicreport() : _minbleicreport_owner() ,iterationscount(p_struct->iterationscount),nfev(p_struct->nfev),varidx(p_struct->varidx),terminationtype(p_struct->terminationtype),debugeqerr(p_struct->debugeqerr),debugfs(p_struct->debugfs),debugff(p_struct->debugff),debugdx(p_struct->debugdx),debugfeasqpits(p_struct->debugfeasqpits),debugfeasgpaits(p_struct->debugfeasgpaits),inneriterationscount(p_struct->inneriterationscount),outeriterationscount(p_struct->outeriterationscount)
{
}

minbleicreport::minbleicreport(const minbleicreport &rhs):_minbleicreport_owner(rhs) ,iterationscount(p_struct->iterationscount),nfev(p_struct->nfev),varidx(p_struct->varidx),terminationtype(p_struct->terminationtype),debugeqerr(p_struct->debugeqerr),debugfs(p_struct->debugfs),debugff(p_struct->debugff),debugdx(p_struct->debugdx),debugfeasqpits(p_struct->debugfeasqpits),debugfeasgpaits(p_struct->debugfeasgpaits),inneriterationscount(p_struct->inneriterationscount),outeriterationscount(p_struct->outeriterationscount)
{
}

minbleicreport& minbleicreport::operator=(const minbleicreport &rhs)
{
    if( this==&rhs )
        return *this;
    _minbleicreport_owner::operator=(rhs);
    return *this;
}

minbleicreport::~minbleicreport()
{
}

/*************************************************************************
                     BOUND CONSTRAINED OPTIMIZATION
       WITH ADDITIONAL LINEAR EQUALITY AND INEQUALITY CONSTRAINTS

DESCRIPTION:
The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints

REQUIREMENTS:
* user must provide function value and gradient
* starting point X0 must be feasible or
  not too far away from the feasible set
* grad(f) must be Lipschitz continuous on a level set:
  L = { x : f(x)<=f(x0) }
* function must be defined everywhere on the feasible set F

USAGE:

Constrained optimization if far more complex than the unconstrained one.
Here we give very brief outline of the BLEIC optimizer. We strongly recommend
you to read examples in the ALGLIB Reference Manual and to read ALGLIB User Guide
on optimization, which is available at http://www.alglib.net/optimization/

1. User initializes algorithm state with MinBLEICCreate() call

2. USer adds boundary and/or linear constraints by calling
   MinBLEICSetBC() and MinBLEICSetLC() functions.

3. User sets stopping conditions with MinBLEICSetCond().

4. User calls MinBLEICOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.

5. User calls MinBLEICResults() to get solution

6. Optionally user may call MinBLEICRestartFrom() to solve another problem
   with same N but another starting point.
   MinBLEICRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleiccreate(const ae_int_t n, const real_1d_array &x, minbleicstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleiccreate(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
                     BOUND CONSTRAINED OPTIMIZATION
       WITH ADDITIONAL LINEAR EQUALITY AND INEQUALITY CONSTRAINTS

DESCRIPTION:
The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints

REQUIREMENTS:
* user must provide function value and gradient
* starting point X0 must be feasible or
  not too far away from the feasible set
* grad(f) must be Lipschitz continuous on a level set:
  L = { x : f(x)<=f(x0) }
* function must be defined everywhere on the feasible set F

USAGE:

Constrained optimization if far more complex than the unconstrained one.
Here we give very brief outline of the BLEIC optimizer. We strongly recommend
you to read examples in the ALGLIB Reference Manual and to read ALGLIB User Guide
on optimization, which is available at http://www.alglib.net/optimization/

1. User initializes algorithm state with MinBLEICCreate() call

2. USer adds boundary and/or linear constraints by calling
   MinBLEICSetBC() and MinBLEICSetLC() functions.

3. User sets stopping conditions with MinBLEICSetCond().

4. User calls MinBLEICOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.

5. User calls MinBLEICResults() to get solution

6. Optionally user may call MinBLEICRestartFrom() to solve another problem
   with same N but another starting point.
   MinBLEICRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleiccreate(const real_1d_array &x, minbleicstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleiccreate(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
The subroutine is finite difference variant of MinBLEICCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinBLEICCreate() in  order  to  get
more information about creation of BLEIC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinBLEICSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust and precise. CG needs exact gradient values. Imprecise
   gradient may slow  down  convergence, especially  on  highly  nonlinear
   problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minbleiccreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, minbleicstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleiccreatef(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), diffstep, const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
The subroutine is finite difference variant of MinBLEICCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinBLEICCreate() in  order  to  get
more information about creation of BLEIC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinBLEICSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust and precise. CG needs exact gradient values. Imprecise
   gradient may slow  down  convergence, especially  on  highly  nonlinear
   problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minbleiccreatef(const real_1d_array &x, const double diffstep, minbleicstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleiccreatef(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), diffstep, const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets boundary constraints for BLEIC optimizer.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by  bound  constraints,
  even  when  numerical  differentiation is used (algorithm adjusts  nodes
  according to boundary constraints)

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbc(const minbleicstate &state, const real_1d_array &bndl, const real_1d_array &bndu)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetbc(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndl.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndu.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets linear constraints for BLEIC optimizer.

Linear constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately:
* there always exists some minor violation (about Epsilon in magnitude)
  due to rounding errors
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.
If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetlc(const minbleicstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetlc(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), const_cast<alglib_impl::ae_matrix*>(c.c_ptr()), const_cast<alglib_impl::ae_vector*>(ct.c_ptr()), k, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets linear constraints for BLEIC optimizer.

Linear constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately:
* there always exists some minor violation (about Epsilon in magnitude)
  due to rounding errors
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.
If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetlc(const minbleicstate &state, const real_2d_array &c, const integer_1d_array &ct)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t k;
    if( (c.rows()!=ct.length()))
        throw ap_error("Error while calling 'minbleicsetlc': looks like one of arguments has wrong size");
    k = c.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetlc(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), const_cast<alglib_impl::ae_matrix*>(c.c_ptr()), const_cast<alglib_impl::ae_vector*>(ct.c_ptr()), k, &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets stopping conditions for the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinBLEICSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinBLEICSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection.

NOTE: when SetCond() called with non-zero MaxIts, BLEIC solver may perform
      slightly more than MaxIts iterations. I.e., MaxIts  sets  non-strict
      limit on iterations count.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetcond(const minbleicstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetcond(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), epsg, epsf, epsx, maxits, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets scaling coefficients for BLEIC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  BLEIC  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinBLEICSetPrec...()
functions.

There is a special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minbleicsetscale(const minbleicstate &state, const real_1d_array &s)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetscale(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(s.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecdefault(const minbleicstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetprecdefault(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE 1: D[i] should be positive. Exception will be thrown otherwise.

NOTE 2: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecdiag(const minbleicstate &state, const real_1d_array &d)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetprecdiag(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(d.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinBLEICSetScale()
call  (before  or after MinBLEICSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecscale(const minbleicstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetprecscale(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), &_alglib_env_state);
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
provided to MinBLEICOptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetxrep(const minbleicstate &state, const bool needxrep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetxrep(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), needxrep, &_alglib_env_state);
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

IMPORTANT: this feature is hard to combine with preconditioning. You can't
set upper limit on step length, when you solve optimization  problem  with
linear (non-boundary) constraints AND preconditioner turned on.

When  non-boundary  constraints  are  present,  you  have to either a) use
preconditioner, or b) use upper limit on step length.  YOU CAN'T USE BOTH!
In this case algorithm will terminate with appropriate error code.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  lead   to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetstpmax(const minbleicstate &state, const double stpmax)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetstpmax(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), stpmax, &_alglib_env_state);
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
bool minbleiciteration(const minbleicstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        ae_bool result = alglib_impl::minbleiciteration(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return *(reinterpret_cast<bool*>(&result));
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minbleicoptimize(minbleicstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( func==NULL )
        throw ap_error("ALGLIB: error in 'minbleicoptimize()' (func is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minbleiciteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needf )
            {
                func(state.x, state.f, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'minbleicoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minbleicoptimize(minbleicstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( grad==NULL )
        throw ap_error("ALGLIB: error in 'minbleicoptimize()' (grad is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minbleiciteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needfg )
            {
                grad(state.x, state.f, state.g, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'minbleicoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}



/*************************************************************************
BLEIC results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -7   gradient verification failed.
                       See MinBLEICSetGradientCheck() for more information.
                * -3   inconsistent constraints. Feasible point is
                       either nonexistent or too hard to find. Try to
                       restart optimizer with better initial approximation
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken
                More information about fields of this  structure  can  be
                found in the comments on MinBLEICReport datatype.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresults(const minbleicstate &state, real_1d_array &x, minbleicreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicresults(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minbleicreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
BLEIC results

Buffered implementation of MinBLEICResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresultsbuf(const minbleicstate &state, real_1d_array &x, minbleicreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicresultsbuf(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minbleicreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicrestartfrom(const minbleicstate &state, const real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicrestartfrom(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  subroutine  turns  on  verification  of  the  user-supplied analytic
gradient:
* user calls this subroutine before optimization begins
* MinBLEICOptimize() is called
* prior to  actual  optimization, for each component  of  parameters being
  optimized X[i] algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * if needed, steps are bounded with respect to constraints on X[]
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point
  * in case difference between prediction and actual value is higher  than
    some predetermined threshold, algorithm stops with completion code -7;
    Rep.VarIdx is set to index of the parameter with incorrect derivative.
* after verification is over, algorithm proceeds to the actual optimization.

NOTE 1: verification  needs  N (parameters count) gradient evaluations. It
        is very costly and you should use  it  only  for  low  dimensional
        problems,  when  you  want  to  be  sure  that  you've   correctly
        calculated  analytic  derivatives.  You  should  not use it in the
        production code (unless you want to check derivatives provided  by
        some third party).

NOTE 2: you  should  carefully  choose  TestStep. Value which is too large
        (so large that function behaviour is significantly non-cubic) will
        lead to false alarms. You may use  different  step  for  different
        parameters by means of setting scale with MinBLEICSetScale().

NOTE 3: this function may lead to false positives. In case it reports that
        I-th  derivative was calculated incorrectly, you may decrease test
        step  and  try  one  more  time  - maybe your function changes too
        sharply  and  your  step  is  too  large for such rapidly chanding
        function.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification

  -- ALGLIB --
     Copyright 15.06.2012 by Bochkanov Sergey
*************************************************************************/
void minbleicsetgradientcheck(const minbleicstate &state, const double teststep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetgradientcheck(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), teststep, &_alglib_env_state);
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
_minlbfgsstate_owner::_minlbfgsstate_owner()
{
    p_struct = (alglib_impl::minlbfgsstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minlbfgsstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minlbfgsstate_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minlbfgsstate_owner::_minlbfgsstate_owner(const _minlbfgsstate_owner &rhs)
{
    p_struct = (alglib_impl::minlbfgsstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minlbfgsstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minlbfgsstate_init_copy(p_struct, const_cast<alglib_impl::minlbfgsstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minlbfgsstate_owner& _minlbfgsstate_owner::operator=(const _minlbfgsstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minlbfgsstate_clear(p_struct);
    if( !alglib_impl::_minlbfgsstate_init_copy(p_struct, const_cast<alglib_impl::minlbfgsstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minlbfgsstate_owner::~_minlbfgsstate_owner()
{
    alglib_impl::_minlbfgsstate_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minlbfgsstate* _minlbfgsstate_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minlbfgsstate* _minlbfgsstate_owner::c_ptr() const
{
    return const_cast<alglib_impl::minlbfgsstate*>(p_struct);
}
minlbfgsstate::minlbfgsstate() : _minlbfgsstate_owner() ,needf(p_struct->needf),needfg(p_struct->needfg),xupdated(p_struct->xupdated),f(p_struct->f),g(&p_struct->g),x(&p_struct->x)
{
}

minlbfgsstate::minlbfgsstate(const minlbfgsstate &rhs):_minlbfgsstate_owner(rhs) ,needf(p_struct->needf),needfg(p_struct->needfg),xupdated(p_struct->xupdated),f(p_struct->f),g(&p_struct->g),x(&p_struct->x)
{
}

minlbfgsstate& minlbfgsstate::operator=(const minlbfgsstate &rhs)
{
    if( this==&rhs )
        return *this;
    _minlbfgsstate_owner::operator=(rhs);
    return *this;
}

minlbfgsstate::~minlbfgsstate()
{
}


/*************************************************************************

*************************************************************************/
_minlbfgsreport_owner::_minlbfgsreport_owner()
{
    p_struct = (alglib_impl::minlbfgsreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minlbfgsreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minlbfgsreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minlbfgsreport_owner::_minlbfgsreport_owner(const _minlbfgsreport_owner &rhs)
{
    p_struct = (alglib_impl::minlbfgsreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minlbfgsreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minlbfgsreport_init_copy(p_struct, const_cast<alglib_impl::minlbfgsreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minlbfgsreport_owner& _minlbfgsreport_owner::operator=(const _minlbfgsreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minlbfgsreport_clear(p_struct);
    if( !alglib_impl::_minlbfgsreport_init_copy(p_struct, const_cast<alglib_impl::minlbfgsreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minlbfgsreport_owner::~_minlbfgsreport_owner()
{
    alglib_impl::_minlbfgsreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minlbfgsreport* _minlbfgsreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minlbfgsreport* _minlbfgsreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::minlbfgsreport*>(p_struct);
}
minlbfgsreport::minlbfgsreport() : _minlbfgsreport_owner() ,iterationscount(p_struct->iterationscount),nfev(p_struct->nfev),varidx(p_struct->varidx),terminationtype(p_struct->terminationtype)
{
}

minlbfgsreport::minlbfgsreport(const minlbfgsreport &rhs):_minlbfgsreport_owner(rhs) ,iterationscount(p_struct->iterationscount),nfev(p_struct->nfev),varidx(p_struct->varidx),terminationtype(p_struct->terminationtype)
{
}

minlbfgsreport& minlbfgsreport::operator=(const minlbfgsreport &rhs)
{
    if( this==&rhs )
        return *this;
    _minlbfgsreport_owner::operator=(rhs);
    return *this;
}

minlbfgsreport::~minlbfgsreport()
{
}

/*************************************************************************
        LIMITED MEMORY BFGS METHOD FOR LARGE SCALE OPTIMIZATION

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by  using  a  quasi-
Newton method (LBFGS scheme) which is optimized to use  a  minimum  amount
of memory.
The subroutine generates the approximation of an inverse Hessian matrix by
using information about the last M steps of the algorithm  (instead of N).
It lessens a required amount of memory from a value  of  order  N^2  to  a
value of order 2*N*M.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinLBFGSCreate() call
2. User tunes solver parameters with MinLBFGSSetCond() MinLBFGSSetStpMax()
   and other functions
3. User calls MinLBFGSOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinLBFGSResults() to get solution
5. Optionally user may call MinLBFGSRestartFrom() to solve another problem
   with same N/M but another starting point and/or another function.
   MinLBFGSRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension. N>0
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   initial solution approximation, array[0..N-1].


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


NOTES:
1. you may tune stopping conditions with MinLBFGSSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLBFGSSetStpMax() function to bound algorithm's  steps.  However,
   L-BFGS rarely needs such a tuning.


  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreate(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlbfgsstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgscreate(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
        LIMITED MEMORY BFGS METHOD FOR LARGE SCALE OPTIMIZATION

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by  using  a  quasi-
Newton method (LBFGS scheme) which is optimized to use  a  minimum  amount
of memory.
The subroutine generates the approximation of an inverse Hessian matrix by
using information about the last M steps of the algorithm  (instead of N).
It lessens a required amount of memory from a value  of  order  N^2  to  a
value of order 2*N*M.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinLBFGSCreate() call
2. User tunes solver parameters with MinLBFGSSetCond() MinLBFGSSetStpMax()
   and other functions
3. User calls MinLBFGSOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinLBFGSResults() to get solution
5. Optionally user may call MinLBFGSRestartFrom() to solve another problem
   with same N/M but another starting point and/or another function.
   MinLBFGSRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension. N>0
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   initial solution approximation, array[0..N-1].


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


NOTES:
1. you may tune stopping conditions with MinLBFGSSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLBFGSSetStpMax() function to bound algorithm's  steps.  However,
   L-BFGS rarely needs such a tuning.


  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreate(const ae_int_t m, const real_1d_array &x, minlbfgsstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgscreate(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
The subroutine is finite difference variant of MinLBFGSCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinLBFGSCreate() in  order  to  get
more information about creation of LBFGS optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinLBFGSSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is   less  robust  and  precise.  LBFGS  needs  exact  gradient values.
   Imprecise gradient may slow  down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreatef(const ae_int_t n, const ae_int_t m, const real_1d_array &x, const double diffstep, minlbfgsstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgscreatef(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), diffstep, const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
The subroutine is finite difference variant of MinLBFGSCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinLBFGSCreate() in  order  to  get
more information about creation of LBFGS optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinLBFGSSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is   less  robust  and  precise.  LBFGS  needs  exact  gradient values.
   Imprecise gradient may slow  down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreatef(const ae_int_t m, const real_1d_array &x, const double diffstep, minlbfgsstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgscreatef(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), diffstep, const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets stopping conditions for L-BFGS optimization algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinLBFGSSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinLBFGSSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetcond(const minlbfgsstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetcond(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), epsg, epsf, epsx, maxits, &_alglib_env_state);
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
provided to MinLBFGSOptimize().


  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetxrep(const minlbfgsstate &state, const bool needxrep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetxrep(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), needxrep, &_alglib_env_state);
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
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0 (default),  if
                you don't want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetstpmax(const minlbfgsstate &state, const double stpmax)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetstpmax(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), stpmax, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets scaling coefficients for LBFGS optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  LBFGS  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinLBFGSSetPrec...()
functions.

There  is  special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetscale(const minlbfgsstate &state, const real_1d_array &s)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetscale(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(s.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification  of  the  preconditioner:  default  preconditioner    (simple
scaling, same for all elements of X) is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecdefault(const minlbfgsstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetprecdefault(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification of the preconditioner: Cholesky factorization of  approximate
Hessian is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    P       -   triangular preconditioner, Cholesky factorization of
                the approximate Hessian. array[0..N-1,0..N-1],
                (if larger, only leading N elements are used).
    IsUpper -   whether upper or lower triangle of P is given
                (other triangle is not referenced)

After call to this function preconditioner is changed to P  (P  is  copied
into the internal buffer).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2:  P  should  be nonsingular. Exception will be thrown otherwise.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetpreccholesky(const minlbfgsstate &state, const real_2d_array &p, const bool isupper)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetpreccholesky(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), const_cast<alglib_impl::ae_matrix*>(p.c_ptr()), isupper, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2: D[i] should be positive. Exception will be thrown otherwise.

NOTE 3: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecdiag(const minlbfgsstate &state, const real_1d_array &d)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetprecdiag(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(d.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinLBFGSSetScale()
call  (before  or after MinLBFGSSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecscale(const minlbfgsstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetprecscale(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), &_alglib_env_state);
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
bool minlbfgsiteration(const minlbfgsstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        ae_bool result = alglib_impl::minlbfgsiteration(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return *(reinterpret_cast<bool*>(&result));
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minlbfgsoptimize(minlbfgsstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( func==NULL )
        throw ap_error("ALGLIB: error in 'minlbfgsoptimize()' (func is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minlbfgsiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needf )
            {
                func(state.x, state.f, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'minlbfgsoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minlbfgsoptimize(minlbfgsstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( grad==NULL )
        throw ap_error("ALGLIB: error in 'minlbfgsoptimize()' (grad is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minlbfgsiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needfg )
            {
                grad(state.x, state.f, state.g, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'minlbfgsoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}



/*************************************************************************
L-BFGS algorithm results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -7    gradient verification failed.
                            See MinLBFGSSetGradientCheck() for more information.
                    * -2    rounding errors prevent further improvement.
                            X contains best point found.
                    * -1    incorrect parameters were specified
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresults(const minlbfgsstate &state, real_1d_array &x, minlbfgsreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgsresults(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlbfgsreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
L-BFGS algorithm results

Buffered implementation of MinLBFGSResults which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresultsbuf(const minlbfgsstate &state, real_1d_array &x, minlbfgsreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgsresultsbuf(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlbfgsreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  subroutine restarts LBFGS algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsrestartfrom(const minlbfgsstate &state, const real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgsrestartfrom(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  subroutine  turns  on  verification  of  the  user-supplied analytic
gradient:
* user calls this subroutine before optimization begins
* MinLBFGSOptimize() is called
* prior to  actual  optimization, for each component  of  parameters being
  optimized X[i] algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * if needed, steps are bounded with respect to constraints on X[]
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point
  * in case difference between prediction and actual value is higher  than
    some predetermined threshold, algorithm stops with completion code -7;
    Rep.VarIdx is set to index of the parameter with incorrect derivative.
* after verification is over, algorithm proceeds to the actual optimization.

NOTE 1: verification  needs  N (parameters count) gradient evaluations. It
        is very costly and you should use  it  only  for  low  dimensional
        problems,  when  you  want  to  be  sure  that  you've   correctly
        calculated  analytic  derivatives.  You  should  not use it in the
        production code (unless you want to check derivatives provided  by
        some third party).

NOTE 2: you  should  carefully  choose  TestStep. Value which is too large
        (so large that function behaviour is significantly non-cubic) will
        lead to false alarms. You may use  different  step  for  different
        parameters by means of setting scale with MinLBFGSSetScale().

NOTE 3: this function may lead to false positives. In case it reports that
        I-th  derivative was calculated incorrectly, you may decrease test
        step  and  try  one  more  time  - maybe your function changes too
        sharply  and  your  step  is  too  large for such rapidly chanding
        function.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification

  -- ALGLIB --
     Copyright 24.05.2012 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetgradientcheck(const minlbfgsstate &state, const double teststep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetgradientcheck(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), teststep, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinQP subpackage to work with this
object
*************************************************************************/
_minqpstate_owner::_minqpstate_owner()
{
    p_struct = (alglib_impl::minqpstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minqpstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minqpstate_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minqpstate_owner::_minqpstate_owner(const _minqpstate_owner &rhs)
{
    p_struct = (alglib_impl::minqpstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minqpstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minqpstate_init_copy(p_struct, const_cast<alglib_impl::minqpstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minqpstate_owner& _minqpstate_owner::operator=(const _minqpstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minqpstate_clear(p_struct);
    if( !alglib_impl::_minqpstate_init_copy(p_struct, const_cast<alglib_impl::minqpstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minqpstate_owner::~_minqpstate_owner()
{
    alglib_impl::_minqpstate_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minqpstate* _minqpstate_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minqpstate* _minqpstate_owner::c_ptr() const
{
    return const_cast<alglib_impl::minqpstate*>(p_struct);
}
minqpstate::minqpstate() : _minqpstate_owner() 
{
}

minqpstate::minqpstate(const minqpstate &rhs):_minqpstate_owner(rhs) 
{
}

minqpstate& minqpstate::operator=(const minqpstate &rhs)
{
    if( this==&rhs )
        return *this;
    _minqpstate_owner::operator=(rhs);
    return *this;
}

minqpstate::~minqpstate()
{
}


/*************************************************************************
This structure stores optimization report:
* InnerIterationsCount      number of inner iterations
* OuterIterationsCount      number of outer iterations
* NCholesky                 number of Cholesky decomposition
* NMV                       number of matrix-vector products
                            (only products calculated as part of iterative
                            process are counted)
* TerminationType           completion code (see below)

Completion codes:
* -5    inappropriate solver was used:
        * Cholesky solver for semidefinite or indefinite problems
        * Cholesky solver for problems with non-boundary constraints
* -4    BLEIC-QP algorithm found unconstrained direction
        of negative curvature (function is unbounded from
        below  even  under  constraints),  no  meaningful
        minimum can be found.
* -3    inconsistent constraints (or, maybe, feasible point is
        too hard to find). If you are sure that constraints are feasible,
        try to restart optimizer with better initial approximation.
* -1    solver error
*  4    successful completion
*  5    MaxIts steps was taken
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
*************************************************************************/
_minqpreport_owner::_minqpreport_owner()
{
    p_struct = (alglib_impl::minqpreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minqpreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minqpreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minqpreport_owner::_minqpreport_owner(const _minqpreport_owner &rhs)
{
    p_struct = (alglib_impl::minqpreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minqpreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minqpreport_init_copy(p_struct, const_cast<alglib_impl::minqpreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minqpreport_owner& _minqpreport_owner::operator=(const _minqpreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minqpreport_clear(p_struct);
    if( !alglib_impl::_minqpreport_init_copy(p_struct, const_cast<alglib_impl::minqpreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minqpreport_owner::~_minqpreport_owner()
{
    alglib_impl::_minqpreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minqpreport* _minqpreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minqpreport* _minqpreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::minqpreport*>(p_struct);
}
minqpreport::minqpreport() : _minqpreport_owner() ,inneriterationscount(p_struct->inneriterationscount),outeriterationscount(p_struct->outeriterationscount),nmv(p_struct->nmv),ncholesky(p_struct->ncholesky),terminationtype(p_struct->terminationtype)
{
}

minqpreport::minqpreport(const minqpreport &rhs):_minqpreport_owner(rhs) ,inneriterationscount(p_struct->inneriterationscount),outeriterationscount(p_struct->outeriterationscount),nmv(p_struct->nmv),ncholesky(p_struct->ncholesky),terminationtype(p_struct->terminationtype)
{
}

minqpreport& minqpreport::operator=(const minqpreport &rhs)
{
    if( this==&rhs )
        return *this;
    _minqpreport_owner::operator=(rhs);
    return *this;
}

minqpreport::~minqpreport()
{
}

/*************************************************************************
                    CONSTRAINED QUADRATIC PROGRAMMING

The subroutine creates QP optimizer. After initial creation,  it  contains
default optimization problem with zero quadratic and linear terms  and  no
constraints. You should set quadratic/linear terms with calls to functions
provided by MinQP subpackage.

INPUT PARAMETERS:
    N       -   problem size

OUTPUT PARAMETERS:
    State   -   optimizer with zero quadratic/linear terms
                and no constraints

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpcreate(const ae_int_t n, minqpstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpcreate(n, const_cast<alglib_impl::minqpstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets linear term for QP solver.

By default, linear term is zero.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    B       -   linear term, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetlinearterm(const minqpstate &state, const real_1d_array &b)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetlinearterm(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(b.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  function  sets  dense  quadratic  term  for  QP solver. By  default,
quadratic term is zero.

SUPPORT BY ALGLIB QP ALGORITHMS:

Dense quadratic term can be handled by any of the QP algorithms  supported
by ALGLIB QP Solver.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   (optional) storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn’t used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn’t used
                * if not given, both lower and upper  triangles  must  be
                  filled.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadraticterm(const minqpstate &state, const real_2d_array &a, const bool isupper)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetquadraticterm(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), isupper, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  function  sets  dense  quadratic  term  for  QP solver. By  default,
quadratic term is zero.

SUPPORT BY ALGLIB QP ALGORITHMS:

Dense quadratic term can be handled by any of the QP algorithms  supported
by ALGLIB QP Solver.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   (optional) storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn’t used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn’t used
                * if not given, both lower and upper  triangles  must  be
                  filled.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadraticterm(const minqpstate &state, const real_2d_array &a)
{
    alglib_impl::ae_state _alglib_env_state;    
    bool isupper;
    if( !alglib_impl::ae_is_symmetric(const_cast<alglib_impl::ae_matrix*>(a.c_ptr())) )
        throw ap_error("'a' parameter is not symmetric matrix");
    isupper = false;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetquadraticterm(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_matrix*>(a.c_ptr()), isupper, &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  function  sets  sparse  quadratic  term  for  QP solver. By default,
quadratic term is zero.

SUPPORT BY ALGLIB QP ALGORITHMS:

Sparse quadratic term is supported only by BLEIC-based QP  algorithm  (one
which is activated by MinQPSetAlgoBLEIC function). Cholesky-based QP  algo
won't be able to deal  with  sparse  quadratic  term  and  will  terminate
abnormally.

IF YOU CALLED THIS FUNCTION, YOU MUST SWITCH TO BLEIC-BASED  QP  ALGORITHM
BEFORE CALLING MINQPOPTIMIZE() FUNCTION.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   (optional) storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn’t used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn’t used
                * if not given, both lower and upper  triangles  must  be
                  filled.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadratictermsparse(const minqpstate &state, const sparsematrix &a, const bool isupper)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetquadratictermsparse(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::sparsematrix*>(a.c_ptr()), isupper, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets starting point for QP solver. It is useful to have
good initial approximation to the solution, because it will increase
speed of convergence and identification of active constraints.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    X       -   starting point, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetstartingpoint(const minqpstate &state, const real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetstartingpoint(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  function sets origin for QP solver. By default, following QP program
is solved:

    min(0.5*x'*A*x+b'*x)

This function allows to solve different problem:

    min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    XOrigin -   origin, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetorigin(const minqpstate &state, const real_1d_array &xorigin)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetorigin(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(xorigin.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets scaling coefficients.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

BLEIC-based QP solver uses scale for two purposes:
* to evaluate stopping conditions
* for preconditioning of the underlying BLEIC solver

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetscale(const minqpstate &state, const real_1d_array &s)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetscale(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(s.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function tells solver to use Cholesky-based algorithm. This algorithm
is active by default.

DESCRIPTION:

Cholesky-based algorithm can be used only for problems which:
* have dense quadratic term, set  by  MinQPSetQuadraticTerm(),  sparse  or
  structured problems are not supported.
* are strictly convex, i.e. quadratic term is symmetric positive definite,
  indefinite or semidefinite problems are not supported by this algorithm.

If anything of what listed above is violated, you may use  BLEIC-based  QP
algorithm which can be activated by MinQPSetAlgoBLEIC().

BENEFITS AND DRAWBACKS:

This  algorithm  gives  best  precision amongst all QP solvers provided by
ALGLIB (Newton iterations  have  much  higher  precision  than  any  other
optimization algorithm). This solver also gracefully handles problems with
very large amount of constraints.

Performance of the algorithm is good because internally  it  uses  Level 3
Dense BLAS for its performance-critical parts.


From the other side, algorithm has  O(N^3)  complexity  for  unconstrained
problems and up to orders of  magnitude  slower  on  constrained  problems
(these additional iterations are needed to identify  active  constraints).
So, its running time depends on number of constraints active  at solution.

Furthermore, this algorithm can not solve problems with sparse matrices or
problems with semidefinite/indefinite matrices of any kind (dense/sparse).

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgocholesky(const minqpstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetalgocholesky(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function tells solver to use BLEIC-based algorithm and sets  stopping
criteria for the algorithm.

DESCRIPTION:

BLEIC-based QP algorithm can be used for any kind of QP problems:
* problems with both dense and sparse quadratic terms
* problems with positive definite, semidefinite, indefinite terms

BLEIC-based algorithm can solve even indefinite problems - as long as they
are bounded from below on the feasible set. Of course, global  minimum  is
found only  for  positive  definite  and  semidefinite  problems.  As  for
indefinite ones - only local minimum is found.

BENEFITS AND DRAWBACKS:

This algorithm can be used to solve both convex and indefinite QP problems
and it can utilize sparsity of the quadratic  term  (algorithm  calculates
matrix-vector products, which can be  performed  efficiently  in  case  of
sparse matrix).

Algorithm has iteration cost, which (assuming fixed amount of non-boundary
linear constraints) linearly depends on problem size. Boundary constraints
does not significantly change iteration cost.

Thus, it outperforms Cholesky-based QP algorithm (CQP) on high-dimensional
sparse problems with moderate amount of constraints.


From the other side, unlike CQP solver, this algorithm does NOT  make  use
of Level 3 Dense BLAS. Thus, its performance on dense problems is inferior
to that of CQP solver.

Its precision is also inferior to that of CQP. CQP performs  Newton  steps
which are know to achieve very good  precision. In many cases Newton  step
leads us exactly to the solution. BLEIC-QP performs LBFGS steps, which are
good at detecting neighborhood of the solution, buy need  many  iterations
to find solution with 6 digits of precision.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled constrained gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinQPSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if exploratory steepest
                descent  step  on  k+1-th  iteration  satisfies   following
                condition:  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
    EpsX    -   >=0
                The  subroutine  finishes  its work if exploratory steepest
                descent  step  on  k+1-th  iteration  satisfies   following
                condition:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinQPSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection (presently it is  small    step
length, but it may change in the future versions of ALGLIB).

IT IS VERY IMPORTANT THAT YOU CALL MinQPSetScale() WHEN YOU USE THIS ALGO!

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgobleic(const minqpstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetalgobleic(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), epsg, epsf, epsx, maxits, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets boundary constraints for QP solver

Boundary constraints are inactive by default (after initial creation).
After  being  set,  they  are  preserved  until explicitly turned off with
another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).

NOTE: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbc(const minqpstate &state, const real_1d_array &bndl, const real_1d_array &bndu)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetbc(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndl.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndu.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets linear constraints for QP optimizer.

Linear constraints are inactive by default (after initial creation).

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately  -
        there always exists some minor violation (about 10^-10...10^-13)
        due to numerical errors.

  -- ALGLIB --
     Copyright 19.06.2012 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc(const minqpstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetlc(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_matrix*>(c.c_ptr()), const_cast<alglib_impl::ae_vector*>(ct.c_ptr()), k, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets linear constraints for QP optimizer.

Linear constraints are inactive by default (after initial creation).

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately  -
        there always exists some minor violation (about 10^-10...10^-13)
        due to numerical errors.

  -- ALGLIB --
     Copyright 19.06.2012 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc(const minqpstate &state, const real_2d_array &c, const integer_1d_array &ct)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t k;
    if( (c.rows()!=ct.length()))
        throw ap_error("Error while calling 'minqpsetlc': looks like one of arguments has wrong size");
    k = c.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpsetlc(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_matrix*>(c.c_ptr()), const_cast<alglib_impl::ae_vector*>(ct.c_ptr()), k, &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function solves quadratic programming problem.
You should call it after setting solver options with MinQPSet...() calls.

INPUT PARAMETERS:
    State   -   algorithm state

You should use MinQPResults() function to access results after calls
to this function.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey.
     Special thanks to Elvira Illarionova  for  important  suggestions  on
     the linearly constrained QP algorithm.
*************************************************************************/
void minqpoptimize(const minqpstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpoptimize(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
QP solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution.
                This array is allocated and initialized only when
                Rep.TerminationType parameter is positive (success).
    Rep     -   optimization report. You should check Rep.TerminationType,
                which contains completion code, and you may check  another
                fields which contain another information  about  algorithm
                functioning.

                Failure codes returned by algorithm are:
                * -5    inappropriate solver was used:
                        * Cholesky solver for (semi)indefinite problems
                        * Cholesky solver for problems with sparse matrix
                * -4    BLEIC-QP algorithm found unconstrained direction
                        of negative curvature (function is unbounded from
                        below  even  under  constraints),  no  meaningful
                        minimum can be found.
                * -3    inconsistent constraints (or maybe  feasible point
                        is too  hard  to  find).  If  you  are  sure  that
                        constraints are feasible, try to restart optimizer
                        with better initial approximation.

                Completion codes specific for Cholesky algorithm:
                *  4   successful completion

                Completion codes specific for BLEIC-based algorithm:
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresults(const minqpstate &state, real_1d_array &x, minqpreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpresults(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minqpreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
QP results

Buffered implementation of MinQPResults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresultsbuf(const minqpstate &state, real_1d_array &x, minqpreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minqpresultsbuf(const_cast<alglib_impl::minqpstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minqpreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Levenberg-Marquardt optimizer.

This structure should be created using one of the MinLMCreate???()
functions. You should not access its fields directly; use ALGLIB functions
to work with it.
*************************************************************************/
_minlmstate_owner::_minlmstate_owner()
{
    p_struct = (alglib_impl::minlmstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minlmstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minlmstate_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minlmstate_owner::_minlmstate_owner(const _minlmstate_owner &rhs)
{
    p_struct = (alglib_impl::minlmstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minlmstate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minlmstate_init_copy(p_struct, const_cast<alglib_impl::minlmstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minlmstate_owner& _minlmstate_owner::operator=(const _minlmstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minlmstate_clear(p_struct);
    if( !alglib_impl::_minlmstate_init_copy(p_struct, const_cast<alglib_impl::minlmstate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minlmstate_owner::~_minlmstate_owner()
{
    alglib_impl::_minlmstate_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minlmstate* _minlmstate_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minlmstate* _minlmstate_owner::c_ptr() const
{
    return const_cast<alglib_impl::minlmstate*>(p_struct);
}
minlmstate::minlmstate() : _minlmstate_owner() ,needf(p_struct->needf),needfg(p_struct->needfg),needfgh(p_struct->needfgh),needfi(p_struct->needfi),needfij(p_struct->needfij),xupdated(p_struct->xupdated),f(p_struct->f),fi(&p_struct->fi),g(&p_struct->g),h(&p_struct->h),j(&p_struct->j),x(&p_struct->x)
{
}

minlmstate::minlmstate(const minlmstate &rhs):_minlmstate_owner(rhs) ,needf(p_struct->needf),needfg(p_struct->needfg),needfgh(p_struct->needfgh),needfi(p_struct->needfi),needfij(p_struct->needfij),xupdated(p_struct->xupdated),f(p_struct->f),fi(&p_struct->fi),g(&p_struct->g),h(&p_struct->h),j(&p_struct->j),x(&p_struct->x)
{
}

minlmstate& minlmstate::operator=(const minlmstate &rhs)
{
    if( this==&rhs )
        return *this;
    _minlmstate_owner::operator=(rhs);
    return *this;
}

minlmstate::~minlmstate()
{
}


/*************************************************************************
Optimization report, filled by MinLMResults() function

FIELDS:
* TerminationType, completetion code:
    * -7    derivative correctness check failed;
            see Rep.WrongNum, Rep.WrongI, Rep.WrongJ for
            more information.
    *  1    relative function improvement is no more than
            EpsF.
    *  2    relative step is no more than EpsX.
    *  4    gradient is no more than EpsG.
    *  5    MaxIts steps was taken
    *  7    stopping conditions are too stringent,
            further improvement is impossible
* IterationsCount, contains iterations count
* NFunc, number of function calculations
* NJac, number of Jacobi matrix calculations
* NGrad, number of gradient calculations
* NHess, number of Hessian calculations
* NCholesky, number of Cholesky decomposition calculations
*************************************************************************/
_minlmreport_owner::_minlmreport_owner()
{
    p_struct = (alglib_impl::minlmreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minlmreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minlmreport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minlmreport_owner::_minlmreport_owner(const _minlmreport_owner &rhs)
{
    p_struct = (alglib_impl::minlmreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minlmreport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minlmreport_init_copy(p_struct, const_cast<alglib_impl::minlmreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minlmreport_owner& _minlmreport_owner::operator=(const _minlmreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minlmreport_clear(p_struct);
    if( !alglib_impl::_minlmreport_init_copy(p_struct, const_cast<alglib_impl::minlmreport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minlmreport_owner::~_minlmreport_owner()
{
    alglib_impl::_minlmreport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minlmreport* _minlmreport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minlmreport* _minlmreport_owner::c_ptr() const
{
    return const_cast<alglib_impl::minlmreport*>(p_struct);
}
minlmreport::minlmreport() : _minlmreport_owner() ,iterationscount(p_struct->iterationscount),terminationtype(p_struct->terminationtype),funcidx(p_struct->funcidx),varidx(p_struct->varidx),nfunc(p_struct->nfunc),njac(p_struct->njac),ngrad(p_struct->ngrad),nhess(p_struct->nhess),ncholesky(p_struct->ncholesky)
{
}

minlmreport::minlmreport(const minlmreport &rhs):_minlmreport_owner(rhs) ,iterationscount(p_struct->iterationscount),terminationtype(p_struct->terminationtype),funcidx(p_struct->funcidx),varidx(p_struct->varidx),nfunc(p_struct->nfunc),njac(p_struct->njac),ngrad(p_struct->ngrad),nhess(p_struct->nhess),ncholesky(p_struct->ncholesky)
{
}

minlmreport& minlmreport::operator=(const minlmreport &rhs)
{
    if( this==&rhs )
        return *this;
    _minlmreport_owner::operator=(rhs);
    return *this;
}

minlmreport::~minlmreport()
{
}

/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] and Jacobian of f[].


REQUIREMENTS:
This algorithm will request following information during its operation:

* function vector f[] at given point X
* function vector f[] and Jacobian of f[] (simultaneously) at given point

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec()  and jac() callbacks.
First  one  is used to calculate f[] at given point, second one calculates
f[] and Jacobian df[i]/dx[j].

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works  with  general  form function and does not provide Jacobian), but it
will  lead  to  exception  being  thrown  after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateVJ() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevj(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatevj(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] and Jacobian of f[].


REQUIREMENTS:
This algorithm will request following information during its operation:

* function vector f[] at given point X
* function vector f[] and Jacobian of f[] (simultaneously) at given point

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec()  and jac() callbacks.
First  one  is used to calculate f[] at given point, second one calculates
f[] and Jacobian df[i]/dx[j].

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works  with  general  form function and does not provide Jacobian), but it
will  lead  to  exception  being  thrown  after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateVJ() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevj(const ae_int_t m, const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatevj(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] only. Finite differences  are  used  to
calculate Jacobian.


REQUIREMENTS:
This algorithm will request following information during its operation:
* function vector f[] at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec() callback.

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works with general form function and does not accept function vector), but
it will  lead  to  exception being thrown after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateV() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

See also MinLMIteration, MinLMResults.

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatev(const ae_int_t n, const ae_int_t m, const real_1d_array &x, const double diffstep, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatev(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), diffstep, const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] only. Finite differences  are  used  to
calculate Jacobian.


REQUIREMENTS:
This algorithm will request following information during its operation:
* function vector f[] at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec() callback.

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works with general form function and does not accept function vector), but
it will  lead  to  exception being thrown after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateV() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

See also MinLMIteration, MinLMResults.

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatev(const ae_int_t m, const real_1d_array &x, const double diffstep, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatev(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), diffstep, const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
    LEVENBERG-MARQUARDT-LIKE METHOD FOR NON-LINEAR OPTIMIZATION

DESCRIPTION:
This  function  is  used  to  find  minimum  of general form (not "sum-of-
-squares") function
    F = F(x[0], ..., x[n-1])
using  its  gradient  and  Hessian.  Levenberg-Marquardt modification with
L-BFGS pre-optimization and internal pre-conditioned  L-BFGS  optimization
after each Levenberg-Marquardt step is used.


REQUIREMENTS:
This algorithm will request following information during its operation:

* function value F at given point X
* F and gradient G (simultaneously) at given point X
* F, G and Hessian H (simultaneously) at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts func(),  grad()  and  hess()
function pointers. First pointer is used to calculate F  at  given  point,
second  one  calculates  F(x)  and  grad F(x),  third one calculates F(x),
grad F(x), hess F(x).

You can try to initialize MinLMState structure with FGH-function and  then
use incorrect version of MinLMOptimize() (for example, version which  does
not provide Hessian matrix), but it will lead to  exception  being  thrown
after first attempt to calculate Hessian.


USAGE:
1. User initializes algorithm state with MinLMCreateFGH() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   pointers (delegates, etc.) to callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefgh(const ae_int_t n, const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatefgh(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
    LEVENBERG-MARQUARDT-LIKE METHOD FOR NON-LINEAR OPTIMIZATION

DESCRIPTION:
This  function  is  used  to  find  minimum  of general form (not "sum-of-
-squares") function
    F = F(x[0], ..., x[n-1])
using  its  gradient  and  Hessian.  Levenberg-Marquardt modification with
L-BFGS pre-optimization and internal pre-conditioned  L-BFGS  optimization
after each Levenberg-Marquardt step is used.


REQUIREMENTS:
This algorithm will request following information during its operation:

* function value F at given point X
* F and gradient G (simultaneously) at given point X
* F, G and Hessian H (simultaneously) at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts func(),  grad()  and  hess()
function pointers. First pointer is used to calculate F  at  given  point,
second  one  calculates  F(x)  and  grad F(x),  third one calculates F(x),
grad F(x), hess F(x).

You can try to initialize MinLMState structure with FGH-function and  then
use incorrect version of MinLMOptimize() (for example, version which  does
not provide Hessian matrix), but it will lead to  exception  being  thrown
after first attempt to calculate Hessian.


USAGE:
1. User initializes algorithm state with MinLMCreateFGH() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   pointers (delegates, etc.) to callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefgh(const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatefgh(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets stopping conditions for Levenberg-Marquardt optimization
algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinLMSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinLMSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations   is    unlimited.   Only   Levenberg-Marquardt
                iterations  are  counted  (L-BFGS/CG  iterations  are  NOT
                counted because their cost is very low compared to that of
                LM).

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetcond(const minlmstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmsetcond(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), epsg, epsf, epsx, maxits, &_alglib_env_state);
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
provided to MinLMOptimize(). Both Levenberg-Marquardt and internal  L-BFGS
iterations are reported.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetxrep(const minlmstate &state, const bool needxrep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmsetxrep(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), needxrep, &_alglib_env_state);
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

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

NOTE: non-zero StpMax leads to moderate  performance  degradation  because
intermediate  step  of  preconditioned L-BFGS optimization is incompatible
with limits on step size.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetstpmax(const minlmstate &state, const double stpmax)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmsetstpmax(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), stpmax, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets scaling coefficients for LM optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Generally, scale is NOT considered to be a form of preconditioner.  But LM
optimizer is unique in that it uses scaling matrix both  in  the  stopping
condition tests and as Marquardt damping factor.

Proper scaling is very important for the algorithm performance. It is less
important for the quality of results, but still has some influence (it  is
easier  to  converge  when  variables  are  properly  scaled, so premature
stopping is possible when very badly scalled variables are  combined  with
relaxed stopping conditions).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetscale(const minlmstate &state, const real_1d_array &s)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmsetscale(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(s.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function sets boundary constraints for LM optimizer

Boundary constraints are inactive by default (after initial creation).
They are preserved until explicitly turned off with another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by bound constraints
  or at its boundary

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetbc(const minlmstate &state, const real_1d_array &bndl, const real_1d_array &bndu)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmsetbc(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndl.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndu.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function is used to change acceleration settings

You can choose between three acceleration strategies:
* AccType=0, no acceleration.
* AccType=1, secant updates are used to update quadratic model after  each
  iteration. After fixed number of iterations (or after  model  breakdown)
  we  recalculate  quadratic  model  using  analytic  Jacobian  or  finite
  differences. Number of secant-based iterations depends  on  optimization
  settings: about 3 iterations - when we have analytic Jacobian, up to 2*N
  iterations - when we use finite differences to calculate Jacobian.

AccType=1 is recommended when Jacobian  calculation  cost  is  prohibitive
high (several Mx1 function vector calculations  followed  by  several  NxN
Cholesky factorizations are faster than calculation of one M*N  Jacobian).
It should also be used when we have no Jacobian, because finite difference
approximation takes too much time to compute.

Table below list  optimization  protocols  (XYZ  protocol  corresponds  to
MinLMCreateXYZ) and acceleration types they support (and use by  default).

ACCELERATION TYPES SUPPORTED BY OPTIMIZATION PROTOCOLS:

protocol    0   1   comment
V           +   +
VJ          +   +
FGH         +

DAFAULT VALUES:

protocol    0   1   comment
V               x   without acceleration it is so slooooooooow
VJ          x
FGH         x

NOTE: this  function should be called before optimization. Attempt to call
it during algorithm iterations may result in unexpected behavior.

NOTE: attempt to call this function with unsupported protocol/acceleration
combination will result in exception being thrown.

  -- ALGLIB --
     Copyright 14.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetacctype(const minlmstate &state, const ae_int_t acctype)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmsetacctype(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), acctype, &_alglib_env_state);
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
bool minlmiteration(const minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        ae_bool result = alglib_impl::minlmiteration(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return *(reinterpret_cast<bool*>(&result));
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minlmoptimize(minlmstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( fvec==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (fvec is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minlmiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needfi )
            {
                fvec(state.x, state.fi, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'minlmoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minlmoptimize(minlmstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( fvec==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (fvec is NULL)");
    if( jac==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (jac is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minlmiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needfi )
            {
                fvec(state.x, state.fi, ptr);
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
            throw ap_error("ALGLIB: error in 'minlmoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minlmoptimize(minlmstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void (*hess)(const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( func==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (func is NULL)");
    if( grad==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (grad is NULL)");
    if( hess==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (hess is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minlmiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needf )
            {
                func(state.x, state.f, ptr);
                continue;
            }
            if( state.needfg )
            {
                grad(state.x, state.f, state.g, ptr);
                continue;
            }
            if( state.needfgh )
            {
                hess(state.x, state.f, state.g, state.h, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'minlmoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minlmoptimize(minlmstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( func==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (func is NULL)");
    if( jac==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (jac is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minlmiteration(state.c_ptr(), &_alglib_env_state) )
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
            throw ap_error("ALGLIB: error in 'minlmoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minlmoptimize(minlmstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( func==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (func is NULL)");
    if( grad==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (grad is NULL)");
    if( jac==NULL )
        throw ap_error("ALGLIB: error in 'minlmoptimize()' (jac is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minlmiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needf )
            {
                func(state.x, state.f, ptr);
                continue;
            }
            if( state.needfg )
            {
                grad(state.x, state.f, state.g, ptr);
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
            throw ap_error("ALGLIB: error in 'minlmoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}



/*************************************************************************
Levenberg-Marquardt algorithm results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report;
                see comments for this structure for more info.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresults(const minlmstate &state, real_1d_array &x, minlmreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmresults(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Levenberg-Marquardt algorithm results

Buffered implementation of MinLMResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresultsbuf(const minlmstate &state, real_1d_array &x, minlmreport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmresultsbuf(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmreport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  subroutine  restarts  LM  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used for reverse communication previously
                allocated with MinLMCreateXXX call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minlmrestartfrom(const minlmstate &state, const real_1d_array &x)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmrestartfrom(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This is obsolete function.

Since ALGLIB 3.3 it is equivalent to MinLMCreateVJ().

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevgj(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatevgj(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This is obsolete function.

Since ALGLIB 3.3 it is equivalent to MinLMCreateVJ().

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevgj(const ae_int_t m, const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatevgj(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This is obsolete function.

Since ALGLIB 3.3 it is equivalent to MinLMCreateFJ().

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefgj(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatefgj(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This is obsolete function.

Since ALGLIB 3.3 it is equivalent to MinLMCreateFJ().

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefgj(const ae_int_t m, const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatefgj(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function is considered obsolete since ALGLIB 3.1.0 and is present for
backward  compatibility  only.  We  recommend  to use MinLMCreateVJ, which
provides similar, but more consistent and feature-rich interface.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefj(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatefj(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This function is considered obsolete since ALGLIB 3.1.0 and is present for
backward  compatibility  only.  We  recommend  to use MinLMCreateVJ, which
provides similar, but more consistent and feature-rich interface.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefj(const ae_int_t m, const real_1d_array &x, minlmstate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmcreatefj(n, m, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minlmstate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This  subroutine  turns  on  verification  of  the  user-supplied analytic
gradient:
* user calls this subroutine before optimization begins
* MinLMOptimize() is called
* prior to actual optimization, for  each  function Fi and each  component
  of parameters  being  optimized X[j] algorithm performs following steps:
  * two trial steps are made to X[j]-TestStep*S[j] and X[j]+TestStep*S[j],
    where X[j] is j-th parameter and S[j] is a scale of j-th parameter
  * if needed, steps are bounded with respect to constraints on X[]
  * Fi(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point
  * in case difference between prediction and actual value is higher  than
    some predetermined threshold, algorithm stops with completion code -7;
    Rep.VarIdx is set to index of the parameter with incorrect derivative,
    Rep.FuncIdx is set to index of the function.
* after verification is over, algorithm proceeds to the actual optimization.

NOTE 1: verification  needs  N (parameters count) Jacobian evaluations. It
        is  very  costly  and  you  should use it only for low dimensional
        problems,  when  you  want  to  be  sure  that  you've   correctly
        calculated  analytic  derivatives.  You should not  use  it in the
        production code  (unless  you  want  to check derivatives provided
        by some third party).

NOTE 2: you  should  carefully  choose  TestStep. Value which is too large
        (so large that function behaviour is significantly non-cubic) will
        lead to false alarms. You may use  different  step  for  different
        parameters by means of setting scale with MinLMSetScale().

NOTE 3: this function may lead to false positives. In case it reports that
        I-th  derivative was calculated incorrectly, you may decrease test
        step  and  try  one  more  time  - maybe your function changes too
        sharply  and  your  step  is  too  large for such rapidly chanding
        function.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification

  -- ALGLIB --
     Copyright 15.06.2012 by Bochkanov Sergey
*************************************************************************/
void minlmsetgradientcheck(const minlmstate &state, const double teststep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlmsetgradientcheck(const_cast<alglib_impl::minlmstate*>(state.c_ptr()), teststep, &_alglib_env_state);
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
_minasastate_owner::_minasastate_owner()
{
    p_struct = (alglib_impl::minasastate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minasastate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minasastate_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minasastate_owner::_minasastate_owner(const _minasastate_owner &rhs)
{
    p_struct = (alglib_impl::minasastate*)alglib_impl::ae_malloc(sizeof(alglib_impl::minasastate), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minasastate_init_copy(p_struct, const_cast<alglib_impl::minasastate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minasastate_owner& _minasastate_owner::operator=(const _minasastate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minasastate_clear(p_struct);
    if( !alglib_impl::_minasastate_init_copy(p_struct, const_cast<alglib_impl::minasastate*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minasastate_owner::~_minasastate_owner()
{
    alglib_impl::_minasastate_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minasastate* _minasastate_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minasastate* _minasastate_owner::c_ptr() const
{
    return const_cast<alglib_impl::minasastate*>(p_struct);
}
minasastate::minasastate() : _minasastate_owner() ,needfg(p_struct->needfg),xupdated(p_struct->xupdated),f(p_struct->f),g(&p_struct->g),x(&p_struct->x)
{
}

minasastate::minasastate(const minasastate &rhs):_minasastate_owner(rhs) ,needfg(p_struct->needfg),xupdated(p_struct->xupdated),f(p_struct->f),g(&p_struct->g),x(&p_struct->x)
{
}

minasastate& minasastate::operator=(const minasastate &rhs)
{
    if( this==&rhs )
        return *this;
    _minasastate_owner::operator=(rhs);
    return *this;
}

minasastate::~minasastate()
{
}


/*************************************************************************

*************************************************************************/
_minasareport_owner::_minasareport_owner()
{
    p_struct = (alglib_impl::minasareport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minasareport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minasareport_init(p_struct, NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minasareport_owner::_minasareport_owner(const _minasareport_owner &rhs)
{
    p_struct = (alglib_impl::minasareport*)alglib_impl::ae_malloc(sizeof(alglib_impl::minasareport), NULL);
    if( p_struct==NULL )
        throw ap_error("ALGLIB: malloc error");
    if( !alglib_impl::_minasareport_init_copy(p_struct, const_cast<alglib_impl::minasareport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
}

_minasareport_owner& _minasareport_owner::operator=(const _minasareport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    alglib_impl::_minasareport_clear(p_struct);
    if( !alglib_impl::_minasareport_init_copy(p_struct, const_cast<alglib_impl::minasareport*>(rhs.p_struct), NULL, ae_false) )
        throw ap_error("ALGLIB: malloc error");
    return *this;
}

_minasareport_owner::~_minasareport_owner()
{
    alglib_impl::_minasareport_clear(p_struct);
    ae_free(p_struct);
}

alglib_impl::minasareport* _minasareport_owner::c_ptr()
{
    return p_struct;
}

alglib_impl::minasareport* _minasareport_owner::c_ptr() const
{
    return const_cast<alglib_impl::minasareport*>(p_struct);
}
minasareport::minasareport() : _minasareport_owner() ,iterationscount(p_struct->iterationscount),nfev(p_struct->nfev),terminationtype(p_struct->terminationtype),activeconstraints(p_struct->activeconstraints)
{
}

minasareport::minasareport(const minasareport &rhs):_minasareport_owner(rhs) ,iterationscount(p_struct->iterationscount),nfev(p_struct->nfev),terminationtype(p_struct->terminationtype),activeconstraints(p_struct->activeconstraints)
{
}

minasareport& minasareport::operator=(const minasareport &rhs)
{
    if( this==&rhs )
        return *this;
    _minasareport_owner::operator=(rhs);
    return *this;
}

minasareport::~minasareport()
{
}

/*************************************************************************
Obsolete function, use MinLBFGSSetPrecDefault() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetdefaultpreconditioner(const minlbfgsstate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetdefaultpreconditioner(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Obsolete function, use MinLBFGSSetCholeskyPreconditioner() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetcholeskypreconditioner(const minlbfgsstate &state, const real_2d_array &p, const bool isupper)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minlbfgssetcholeskypreconditioner(const_cast<alglib_impl::minlbfgsstate*>(state.c_ptr()), const_cast<alglib_impl::ae_matrix*>(p.c_ptr()), isupper, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierwidth(const minbleicstate &state, const double mu)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetbarrierwidth(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), mu, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierdecay(const minbleicstate &state, const double mudecay)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minbleicsetbarrierdecay(const_cast<alglib_impl::minbleicstate*>(state.c_ptr()), mudecay, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void minasacreate(const ae_int_t n, const real_1d_array &x, const real_1d_array &bndl, const real_1d_array &bndu, minasastate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minasacreate(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndl.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndu.c_ptr()), const_cast<alglib_impl::minasastate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void minasacreate(const real_1d_array &x, const real_1d_array &bndl, const real_1d_array &bndu, minasastate &state)
{
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (x.length()!=bndl.length()) || (x.length()!=bndu.length()))
        throw ap_error("Error while calling 'minasacreate': looks like one of arguments has wrong size");
    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minasacreate(n, const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndl.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndu.c_ptr()), const_cast<alglib_impl::minasastate*>(state.c_ptr()), &_alglib_env_state);

        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetcond(const minasastate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minasasetcond(const_cast<alglib_impl::minasastate*>(state.c_ptr()), epsg, epsf, epsx, maxits, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetxrep(const minasastate &state, const bool needxrep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minasasetxrep(const_cast<alglib_impl::minasastate*>(state.c_ptr()), needxrep, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetalgorithm(const minasastate &state, const ae_int_t algotype)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minasasetalgorithm(const_cast<alglib_impl::minasastate*>(state.c_ptr()), algotype, &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetstpmax(const minasastate &state, const double stpmax)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minasasetstpmax(const_cast<alglib_impl::minasastate*>(state.c_ptr()), stpmax, &_alglib_env_state);
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
bool minasaiteration(const minasastate &state)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        ae_bool result = alglib_impl::minasaiteration(const_cast<alglib_impl::minasastate*>(state.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return *(reinterpret_cast<bool*>(&result));
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}


void minasaoptimize(minasastate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr)
{
    alglib_impl::ae_state _alglib_env_state;
    if( grad==NULL )
        throw ap_error("ALGLIB: error in 'minasaoptimize()' (grad is NULL)");
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        while( alglib_impl::minasaiteration(state.c_ptr(), &_alglib_env_state) )
        {
            if( state.needfg )
            {
                grad(state.x, state.f, state.g, ptr);
                continue;
            }
            if( state.xupdated )
            {
                if( rep!=NULL )
                    rep(state.x, state.f, ptr);
                continue;
            }
            throw ap_error("ALGLIB: error in 'minasaoptimize' (some derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&_alglib_env_state);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}



/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresults(const minasastate &state, real_1d_array &x, minasareport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minasaresults(const_cast<alglib_impl::minasastate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minasareport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresultsbuf(const minasastate &state, real_1d_array &x, minasareport &rep)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minasaresultsbuf(const_cast<alglib_impl::minasastate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::minasareport*>(rep.c_ptr()), &_alglib_env_state);
        alglib_impl::ae_state_clear(&_alglib_env_state);
        return;
    }
    catch(alglib_impl::ae_error_type)
    {
        throw ap_error(_alglib_env_state.error_msg);
    }
}

/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minasarestartfrom(const minasastate &state, const real_1d_array &x, const real_1d_array &bndl, const real_1d_array &bndu)
{
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    try
    {
        alglib_impl::minasarestartfrom(const_cast<alglib_impl::minasastate*>(state.c_ptr()), const_cast<alglib_impl::ae_vector*>(x.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndl.c_ptr()), const_cast<alglib_impl::ae_vector*>(bndu.c_ptr()), &_alglib_env_state);
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


static ae_int_t cqmodels_newtonrefinementits = 3;
static ae_bool cqmodels_cqmrebuild(convexquadraticmodel* s,
     ae_state *_state);
static void cqmodels_cqmsolveea(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);


static ae_int_t snnls_iterativerefinementits = 3;
static ae_bool snnls_boundedstepandactivation(/* Real    */ ae_vector* xc,
     /* Real    */ ae_vector* xn,
     /* Boolean */ ae_vector* nnc,
     ae_int_t n,
     ae_state *_state);


static void sactivesets_constraineddescent(sactiveset* state,
     /* Real    */ ae_vector* g,
     /* Real    */ ae_vector* h,
     /* Real    */ ae_matrix* ha,
     ae_bool normalize,
     /* Real    */ ae_vector* d,
     ae_state *_state);
static void sactivesets_reactivateconstraints(sactiveset* state,
     /* Real    */ ae_vector* gc,
     /* Real    */ ae_vector* h,
     ae_state *_state);


static ae_int_t mincg_rscountdownlen = 10;
static double mincg_gtol = 0.3;
static void mincg_clearrequestfields(mincgstate* state, ae_state *_state);
static void mincg_preconditionedmultiply(mincgstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* work0,
     /* Real    */ ae_vector* work1,
     ae_state *_state);
static double mincg_preconditionedmultiply2(mincgstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* work0,
     /* Real    */ ae_vector* work1,
     ae_state *_state);
static void mincg_mincginitinternal(ae_int_t n,
     double diffstep,
     mincgstate* state,
     ae_state *_state);


static double minbleic_gtol = 0.4;
static double minbleic_maxnonmonotoniclen = 1.0E-5;
static double minbleic_initialdecay = 0.5;
static double minbleic_mindecay = 0.1;
static double minbleic_decaycorrection = 0.8;
static double minbleic_penaltyfactor = 100;
static void minbleic_clearrequestfields(minbleicstate* state,
     ae_state *_state);
static void minbleic_minbleicinitinternal(ae_int_t n,
     /* Real    */ ae_vector* x,
     double diffstep,
     minbleicstate* state,
     ae_state *_state);
static void minbleic_updateestimateofgoodstep(double* estimate,
     double newstep,
     ae_state *_state);


static double minlbfgs_gtol = 0.4;
static void minlbfgs_clearrequestfields(minlbfgsstate* state,
     ae_state *_state);


static ae_int_t minqp_maxlagrangeits = 10;
static ae_int_t minqp_maxbadnewtonits = 7;
static double minqp_penaltyfactor = 100.0;
static ae_int_t minqp_minqpboundedstepandactivation(minqpstate* state,
     /* Real    */ ae_vector* xn,
     /* Real    */ ae_vector* buf,
     ae_state *_state);
static double minqp_minqpmodelvalue(convexquadraticmodel* a,
     /* Real    */ ae_vector* b,
     /* Real    */ ae_vector* xc,
     ae_int_t n,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);
static ae_bool minqp_minqpconstrainedoptimum(minqpstate* state,
     convexquadraticmodel* a,
     double anorm,
     /* Real    */ ae_vector* b,
     /* Real    */ ae_vector* xn,
     /* Real    */ ae_vector* tmp,
     /* Boolean */ ae_vector* tmpb,
     /* Real    */ ae_vector* lagrangec,
     ae_state *_state);


static double minlm_lambdaup = 2.0;
static double minlm_lambdadown = 0.33;
static double minlm_suspiciousnu = 16;
static ae_int_t minlm_smallmodelage = 3;
static ae_int_t minlm_additers = 5;
static void minlm_lmprepare(ae_int_t n,
     ae_int_t m,
     ae_bool havegrad,
     minlmstate* state,
     ae_state *_state);
static void minlm_clearrequestfields(minlmstate* state, ae_state *_state);
static ae_bool minlm_increaselambda(double* lambdav,
     double* nu,
     ae_state *_state);
static void minlm_decreaselambda(double* lambdav,
     double* nu,
     ae_state *_state);
static double minlm_boundedscaledantigradnorm(minlmstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* g,
     ae_state *_state);


static ae_int_t mincomp_n1 = 2;
static ae_int_t mincomp_n2 = 2;
static double mincomp_stpmin = 1.0E-300;
static double mincomp_gtol = 0.3;
static double mincomp_gpaftol = 0.0001;
static double mincomp_gpadecay = 0.5;
static double mincomp_asarho = 0.5;
static double mincomp_asaboundedantigradnorm(minasastate* state,
     ae_state *_state);
static double mincomp_asaginorm(minasastate* state, ae_state *_state);
static double mincomp_asad1norm(minasastate* state, ae_state *_state);
static ae_bool mincomp_asauisempty(minasastate* state, ae_state *_state);
static void mincomp_clearrequestfields(minasastate* state,
     ae_state *_state);





/*************************************************************************
This subroutine is used to prepare threshold value which will be used for
trimming of the target function (see comments on TrimFunction() for more
information).

This function accepts only one parameter: function value at the starting
point. It returns threshold which will be used for trimming.

  -- ALGLIB --
     Copyright 10.05.2011 by Bochkanov Sergey
*************************************************************************/
void trimprepare(double f, double* threshold, ae_state *_state)
{

    *threshold = 0;

    *threshold = 10*(ae_fabs(f, _state)+1);
}


/*************************************************************************
This subroutine is used to "trim" target function, i.e. to do following
transformation:

                   { {F,G}          if F<Threshold
    {F_tr, G_tr} = {
                   { {Threshold, 0} if F>=Threshold
                   
Such transformation allows us to  solve  problems  with  singularities  by
redefining function in such way that it becomes bounded from above.

  -- ALGLIB --
     Copyright 10.05.2011 by Bochkanov Sergey
*************************************************************************/
void trimfunction(double* f,
     /* Real    */ ae_vector* g,
     ae_int_t n,
     double threshold,
     ae_state *_state)
{
    ae_int_t i;


    if( ae_fp_greater_eq(*f,threshold) )
    {
        *f = threshold;
        for(i=0; i<=n-1; i++)
        {
            g->ptr.p_double[i] = 0.0;
        }
    }
}


/*************************************************************************
This function enforces boundary constraints in the X.

This function correctly (although a bit inefficient) handles BL[i] which
are -INF and BU[i] which are +INF.

We have NMain+NSlack  dimensional  X,  with first NMain components bounded
by BL/BU, and next NSlack ones bounded by non-negativity constraints.

INPUT PARAMETERS
    X       -   array[NMain+NSlack], point
    BL      -   array[NMain], lower bounds
                (may contain -INF, when bound is not present)
    HaveBL  -   array[NMain], if HaveBL[i] is False,
                then i-th bound is not present
    BU      -   array[NMain], upper bounds
                (may contain +INF, when bound is not present)
    HaveBU  -   array[NMain], if HaveBU[i] is False,
                then i-th bound is not present

OUTPUT PARAMETERS
    X       -   X with all constraints being enforced

It returns True when constraints are consistent,
False - when constraints are inconsistent.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool enforceboundaryconstraints(/* Real    */ ae_vector* x,
     /* Real    */ ae_vector* bl,
     /* Boolean */ ae_vector* havebl,
     /* Real    */ ae_vector* bu,
     /* Boolean */ ae_vector* havebu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool result;


    result = ae_false;
    for(i=0; i<=nmain-1; i++)
    {
        if( (havebl->ptr.p_bool[i]&&havebu->ptr.p_bool[i])&&ae_fp_greater(bl->ptr.p_double[i],bu->ptr.p_double[i]) )
        {
            return result;
        }
        if( havebl->ptr.p_bool[i]&&ae_fp_less(x->ptr.p_double[i],bl->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = bl->ptr.p_double[i];
        }
        if( havebu->ptr.p_bool[i]&&ae_fp_greater(x->ptr.p_double[i],bu->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = bu->ptr.p_double[i];
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_less(x->ptr.p_double[nmain+i],0) )
        {
            x->ptr.p_double[nmain+i] = 0;
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function projects gradient into feasible area of boundary constrained
optimization  problem.  X  can  be  infeasible  with  respect  to boundary
constraints.  We  have  NMain+NSlack  dimensional  X,   with  first  NMain 
components bounded by BL/BU, and next NSlack ones bounded by non-negativity
constraints.

INPUT PARAMETERS
    X       -   array[NMain+NSlack], point
    G       -   array[NMain+NSlack], gradient
    BL      -   lower bounds (may contain -INF, when bound is not present)
    HaveBL  -   if HaveBL[i] is False, then i-th bound is not present
    BU      -   upper bounds (may contain +INF, when bound is not present)
    HaveBU  -   if HaveBU[i] is False, then i-th bound is not present

OUTPUT PARAMETERS
    G       -   projection of G. Components of G which satisfy one of the
                following
                    (1) (X[I]<=BndL[I]) and (G[I]>0), OR
                    (2) (X[I]>=BndU[I]) and (G[I]<0)
                are replaced by zeros.

NOTE 1: this function assumes that constraints are feasible. It throws
exception otherwise.

NOTE 2: in fact, projection of ANTI-gradient is calculated,  because  this
function trims components of -G which points outside of the feasible area.
However, working with -G is considered confusing, because all optimization
source work with G.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
void projectgradientintobc(/* Real    */ ae_vector* x,
     /* Real    */ ae_vector* g,
     /* Real    */ ae_vector* bl,
     /* Boolean */ ae_vector* havebl,
     /* Real    */ ae_vector* bu,
     /* Boolean */ ae_vector* havebu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=nmain-1; i++)
    {
        ae_assert((!havebl->ptr.p_bool[i]||!havebu->ptr.p_bool[i])||ae_fp_less_eq(bl->ptr.p_double[i],bu->ptr.p_double[i]), "ProjectGradientIntoBC: internal error (infeasible constraints)", _state);
        if( (havebl->ptr.p_bool[i]&&ae_fp_less_eq(x->ptr.p_double[i],bl->ptr.p_double[i]))&&ae_fp_greater(g->ptr.p_double[i],0) )
        {
            g->ptr.p_double[i] = 0;
        }
        if( (havebu->ptr.p_bool[i]&&ae_fp_greater_eq(x->ptr.p_double[i],bu->ptr.p_double[i]))&&ae_fp_less(g->ptr.p_double[i],0) )
        {
            g->ptr.p_double[i] = 0;
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_less_eq(x->ptr.p_double[nmain+i],0)&&ae_fp_greater(g->ptr.p_double[nmain+i],0) )
        {
            g->ptr.p_double[nmain+i] = 0;
        }
    }
}


/*************************************************************************
Given
    a) initial point X0[NMain+NSlack]
       (feasible with respect to bound constraints)
    b) step vector alpha*D[NMain+NSlack]
    c) boundary constraints BndL[NMain], BndU[NMain]
    d) implicit non-negativity constraints for slack variables
this  function  calculates  bound  on  the step length subject to boundary
constraints.

It returns:
    *  MaxStepLen - such step length that X0+MaxStepLen*alpha*D is exactly
       at the boundary given by constraints
    *  VariableToFreeze - index of the constraint to be activated,
       0 <= VariableToFreeze < NMain+NSlack
    *  ValueToFreeze - value of the corresponding constraint.

Notes:
    * it is possible that several constraints can be activated by the step
      at once. In such cases only one constraint is returned. It is caller
      responsibility to check other constraints. This function makes  sure
      that we activate at least one constraint, and everything else is the
      responsibility of the caller.
    * steps smaller than MaxStepLen still can activate constraints due  to
      numerical errors. Thus purpose of this  function  is  not  to  guard 
      against accidental activation of the constraints - quite the reverse, 
      its purpose is to activate at least constraint upon performing  step
      which is too long.
    * in case there is no constraints to activate, we return negative
      VariableToFreeze and zero MaxStepLen and ValueToFreeze.
    * this function assumes that constraints are consistent; it throws
      exception otherwise.

INPUT PARAMETERS
    X           -   array[NMain+NSlack], point. Must be feasible with respect 
                    to bound constraints (exception will be thrown otherwise)
    D           -   array[NMain+NSlack], step direction
    alpha       -   scalar multiplier before D, alpha<>0
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    
OUTPUT PARAMETERS
    VariableToFreeze:
                    * negative value     = step is unbounded, ValueToFreeze=0,
                                           MaxStepLen=0.
                    * non-negative value = at least one constraint, given by
                                           this parameter, will  be  activated
                                           upon performing maximum step.
    ValueToFreeze-  value of the variable which will be constrained
    MaxStepLen  -   maximum length of the step. Can be zero when step vector
                    looks outside of the feasible area.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
void calculatestepbound(/* Real    */ ae_vector* x,
     /* Real    */ ae_vector* d,
     double alpha,
     /* Real    */ ae_vector* bndl,
     /* Boolean */ ae_vector* havebndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t* variabletofreeze,
     double* valuetofreeze,
     double* maxsteplen,
     ae_state *_state)
{
    ae_int_t i;
    double prevmax;
    double initval;

    *variabletofreeze = 0;
    *valuetofreeze = 0;
    *maxsteplen = 0;

    ae_assert(ae_fp_neq(alpha,0), "CalculateStepBound: zero alpha", _state);
    *variabletofreeze = -1;
    initval = ae_maxrealnumber;
    *maxsteplen = initval;
    for(i=0; i<=nmain-1; i++)
    {
        if( havebndl->ptr.p_bool[i]&&ae_fp_less(alpha*d->ptr.p_double[i],0) )
        {
            ae_assert(ae_fp_greater_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]), "CalculateStepBound: infeasible X", _state);
            prevmax = *maxsteplen;
            *maxsteplen = safeminposrv(x->ptr.p_double[i]-bndl->ptr.p_double[i], -alpha*d->ptr.p_double[i], *maxsteplen, _state);
            if( ae_fp_less(*maxsteplen,prevmax) )
            {
                *variabletofreeze = i;
                *valuetofreeze = bndl->ptr.p_double[i];
            }
        }
        if( havebndu->ptr.p_bool[i]&&ae_fp_greater(alpha*d->ptr.p_double[i],0) )
        {
            ae_assert(ae_fp_less_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]), "CalculateStepBound: infeasible X", _state);
            prevmax = *maxsteplen;
            *maxsteplen = safeminposrv(bndu->ptr.p_double[i]-x->ptr.p_double[i], alpha*d->ptr.p_double[i], *maxsteplen, _state);
            if( ae_fp_less(*maxsteplen,prevmax) )
            {
                *variabletofreeze = i;
                *valuetofreeze = bndu->ptr.p_double[i];
            }
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_less(alpha*d->ptr.p_double[nmain+i],0) )
        {
            ae_assert(ae_fp_greater_eq(x->ptr.p_double[nmain+i],0), "CalculateStepBound: infeasible X", _state);
            prevmax = *maxsteplen;
            *maxsteplen = safeminposrv(x->ptr.p_double[nmain+i], -alpha*d->ptr.p_double[nmain+i], *maxsteplen, _state);
            if( ae_fp_less(*maxsteplen,prevmax) )
            {
                *variabletofreeze = nmain+i;
                *valuetofreeze = 0;
            }
        }
    }
    if( ae_fp_eq(*maxsteplen,initval) )
    {
        *valuetofreeze = 0;
        *maxsteplen = 0;
    }
}


/*************************************************************************
This function postprocesses bounded step by:
* analysing step length (whether it is equal to MaxStepLen) and activating 
  constraint given by VariableToFreeze if needed
* checking for additional bound constraints to activate

This function uses final point of the step, quantities calculated  by  the
CalculateStepBound()  function.  As  result,  it  returns  point  which is 
exactly feasible with respect to boundary constraints.

NOTE 1: this function does NOT handle and check linear equality constraints
NOTE 2: when StepTaken=MaxStepLen we always activate at least one constraint

INPUT PARAMETERS
    X           -   array[NMain+NSlack], final point to postprocess
    XPrev       -   array[NMain+NSlack], initial point
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    VariableToFreeze-result of CalculateStepBound()
    ValueToFreeze-  result of CalculateStepBound()
    StepTaken   -   actual step length (actual step is equal to the possibly 
                    non-unit step direction vector times this parameter).
                    StepTaken<=MaxStepLen.
    MaxStepLen  -   result of CalculateStepBound()
    
OUTPUT PARAMETERS
    X           -   point bounded with respect to constraints.
                    components corresponding to active constraints are exactly
                    equal to the boundary values.
                    
RESULT:
    number of constraints activated in addition to previously active ones.
    Constraints which were DEACTIVATED are ignored (do not influence
    function value).

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t postprocessboundedstep(/* Real    */ ae_vector* x,
     /* Real    */ ae_vector* xprev,
     /* Real    */ ae_vector* bndl,
     /* Boolean */ ae_vector* havebndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t variabletofreeze,
     double valuetofreeze,
     double steptaken,
     double maxsteplen,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool wasactivated;
    ae_int_t result;


    ae_assert(variabletofreeze<0||ae_fp_less_eq(steptaken,maxsteplen), "Assertion failed", _state);
    
    /*
     * Activate constraints
     */
    if( variabletofreeze>=0&&ae_fp_eq(steptaken,maxsteplen) )
    {
        x->ptr.p_double[variabletofreeze] = valuetofreeze;
    }
    for(i=0; i<=nmain-1; i++)
    {
        if( havebndl->ptr.p_bool[i]&&ae_fp_less(x->ptr.p_double[i],bndl->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = bndl->ptr.p_double[i];
        }
        if( havebndu->ptr.p_bool[i]&&ae_fp_greater(x->ptr.p_double[i],bndu->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = bndu->ptr.p_double[i];
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_less_eq(x->ptr.p_double[nmain+i],0) )
        {
            x->ptr.p_double[nmain+i] = 0;
        }
    }
    
    /*
     * Calculate number of constraints being activated
     */
    result = 0;
    for(i=0; i<=nmain-1; i++)
    {
        wasactivated = ae_fp_neq(x->ptr.p_double[i],xprev->ptr.p_double[i])&&((havebndl->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]))||(havebndu->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i])));
        wasactivated = wasactivated||variabletofreeze==i;
        if( wasactivated )
        {
            result = result+1;
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        wasactivated = ae_fp_neq(x->ptr.p_double[nmain+i],xprev->ptr.p_double[nmain+i])&&ae_fp_eq(x->ptr.p_double[nmain+i],0.0);
        wasactivated = wasactivated||variabletofreeze==nmain+i;
        if( wasactivated )
        {
            result = result+1;
        }
    }
    return result;
}


/*************************************************************************
The  purpose  of  this  function is to prevent algorithm from "unsticking" 
from  the  active  bound  constraints  because  of  numerical noise in the
gradient or Hessian.

It is done by zeroing some components of the search direction D.  D[i]  is
zeroed when both (a) and (b) are true:
a) corresponding X[i] is exactly at the boundary
b) |D[i]*S[i]| <= DropTol*Sqrt(SUM(D[i]^2*S[I]^2))

D  can  be  step  direction , antigradient, gradient, or anything similar. 
Sign of D does not matter, nor matters step length.

NOTE 1: boundary constraints are expected to be consistent, as well as X
        is expected to be feasible. Exception will be thrown otherwise.

INPUT PARAMETERS
    D           -   array[NMain+NSlack], direction
    X           -   array[NMain+NSlack], current point
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    S           -   array[NMain+NSlack], scaling of the variables
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    DropTol     -   drop tolerance, >=0
    
OUTPUT PARAMETERS
    X           -   point bounded with respect to constraints.
                    components corresponding to active constraints are exactly
                    equal to the boundary values.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
void filterdirection(/* Real    */ ae_vector* d,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* bndl,
     /* Boolean */ ae_vector* havebndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* havebndu,
     /* Real    */ ae_vector* s,
     ae_int_t nmain,
     ae_int_t nslack,
     double droptol,
     ae_state *_state)
{
    ae_int_t i;
    double scalednorm;
    ae_bool isactive;


    scalednorm = 0.0;
    for(i=0; i<=nmain+nslack-1; i++)
    {
        scalednorm = scalednorm+ae_sqr(d->ptr.p_double[i]*s->ptr.p_double[i], _state);
    }
    scalednorm = ae_sqrt(scalednorm, _state);
    for(i=0; i<=nmain-1; i++)
    {
        ae_assert(!havebndl->ptr.p_bool[i]||ae_fp_greater_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]), "FilterDirection: infeasible point", _state);
        ae_assert(!havebndu->ptr.p_bool[i]||ae_fp_less_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]), "FilterDirection: infeasible point", _state);
        isactive = (havebndl->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]))||(havebndu->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]));
        if( isactive&&ae_fp_less_eq(ae_fabs(d->ptr.p_double[i]*s->ptr.p_double[i], _state),droptol*scalednorm) )
        {
            d->ptr.p_double[i] = 0.0;
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        ae_assert(ae_fp_greater_eq(x->ptr.p_double[nmain+i],0), "FilterDirection: infeasible point", _state);
        if( ae_fp_eq(x->ptr.p_double[nmain+i],0)&&ae_fp_less_eq(ae_fabs(d->ptr.p_double[nmain+i]*s->ptr.p_double[nmain+i], _state),droptol*scalednorm) )
        {
            d->ptr.p_double[nmain+i] = 0.0;
        }
    }
}


/*************************************************************************
This function returns number of bound constraints whose state was  changed
(either activated or deactivated) when making step from XPrev to X.

Constraints are considered:
* active - when we are exactly at the boundary
* inactive - when we are not at the boundary

You should note that antigradient direction is NOT taken into account when
we make decions on the constraint status.

INPUT PARAMETERS
    X           -   array[NMain+NSlack], final point.
                    Must be feasible with respect to bound constraints.
    XPrev       -   array[NMain+NSlack], initial point.
                    Must be feasible with respect to bound constraints.
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    
RESULT:
    number of constraints whose state was changed.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t numberofchangedconstraints(/* Real    */ ae_vector* x,
     /* Real    */ ae_vector* xprev,
     /* Real    */ ae_vector* bndl,
     /* Boolean */ ae_vector* havebndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool statuschanged;
    ae_int_t result;


    result = 0;
    for(i=0; i<=nmain-1; i++)
    {
        if( ae_fp_neq(x->ptr.p_double[i],xprev->ptr.p_double[i]) )
        {
            statuschanged = ae_false;
            if( havebndl->ptr.p_bool[i]&&(ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i])||ae_fp_eq(xprev->ptr.p_double[i],bndl->ptr.p_double[i])) )
            {
                statuschanged = ae_true;
            }
            if( havebndu->ptr.p_bool[i]&&(ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i])||ae_fp_eq(xprev->ptr.p_double[i],bndu->ptr.p_double[i])) )
            {
                statuschanged = ae_true;
            }
            if( statuschanged )
            {
                result = result+1;
            }
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_neq(x->ptr.p_double[nmain+i],xprev->ptr.p_double[nmain+i])&&(ae_fp_eq(x->ptr.p_double[nmain+i],0)||ae_fp_eq(xprev->ptr.p_double[nmain+i],0)) )
        {
            result = result+1;
        }
    }
    return result;
}


/*************************************************************************
This function finds feasible point of  (NMain+NSlack)-dimensional  problem
subject to NMain explicit boundary constraints (some  constraints  can  be
omitted), NSlack implicit non-negativity constraints,  K  linear  equality
constraints.

INPUT PARAMETERS
    X           -   array[NMain+NSlack], initial point.
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    CE          -   array[K,NMain+NSlack+1], equality  constraints CE*x=b.
                    Rows contain constraints, first  NMain+NSlack  columns
                    contain coefficients before X[], last  column  contain
                    right part.
    K           -   number of linear constraints
    EpsI        -   infeasibility (error in the right part) allowed in the
                    solution

OUTPUT PARAMETERS:
    X           -   feasible point or best infeasible point found before
                    algorithm termination
    QPIts       -   number of QP iterations (for debug purposes)
    GPAIts      -   number of GPA iterations (for debug purposes)
    
RESULT:
    True in case X is feasible, False - if it is infeasible.

  -- ALGLIB --
     Copyright 20.01.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool findfeasiblepoint(/* Real    */ ae_vector* x,
     /* Real    */ ae_vector* bndl,
     /* Boolean */ ae_vector* havebndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     /* Real    */ ae_matrix* ce,
     ae_int_t k,
     double epsi,
     ae_int_t* qpits,
     ae_int_t* gpaits,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix _ce;
    ae_int_t i;
    ae_int_t j;
    ae_int_t idx0;
    ae_int_t idx1;
    ae_vector permx;
    ae_vector xn;
    ae_vector xa;
    ae_vector newtonstep;
    ae_vector g;
    ae_vector pg;
    ae_matrix a;
    double armijostep;
    double armijobeststep;
    double armijobestfeas;
    double v;
    double mx;
    double feaserr;
    double feasold;
    double feasnew;
    double pgnorm;
    double vn;
    double vd;
    double stp;
    ae_int_t vartofreeze;
    double valtofreeze;
    double maxsteplen;
    ae_bool werechangesinconstraints;
    ae_bool stage1isover;
    ae_bool converged;
    ae_vector activeconstraints;
    ae_vector tmpk;
    ae_vector colnorms;
    ae_int_t nactive;
    ae_int_t nfree;
    ae_int_t nsvd;
    ae_vector p1;
    ae_vector p2;
    apbuffers buf;
    ae_vector w;
    ae_vector s;
    ae_matrix u;
    ae_matrix vt;
    ae_int_t itscount;
    ae_int_t itswithintolerance;
    ae_int_t maxitswithintolerance;
    ae_int_t gparuns;
    ae_int_t maxarmijoruns;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    ae_matrix_init_copy(&_ce, ce, _state, ae_true);
    ce = &_ce;
    *qpits = 0;
    *gpaits = 0;
    ae_vector_init(&permx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xn, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xa, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&newtonstep, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&g, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&pg, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&a, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&activeconstraints, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpk, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&colnorms, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&p1, 0, DT_INT, _state, ae_true);
    ae_vector_init(&p2, 0, DT_INT, _state, ae_true);
    _apbuffers_init(&buf, _state, ae_true);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&s, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&u, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&vt, 0, 0, DT_REAL, _state, ae_true);

    maxitswithintolerance = 3;
    maxarmijoruns = 5;
    *qpits = 0;
    *gpaits = 0;
    
    /*
     * Initial enforcement of the feasibility with respect to boundary constraints
     * NOTE: after this block we assume that boundary constraints are consistent.
     */
    if( !enforceboundaryconstraints(x, bndl, havebndl, bndu, havebndu, nmain, nslack, _state) )
    {
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    if( k==0 )
    {
        
        /*
         * No linear constraints, we can exit right now
         */
        result = ae_true;
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * Scale rows of CE in such way that max(CE[i,0..nmain+nslack-1])=1 for any i=0..k-1
     */
    for(i=0; i<=k-1; i++)
    {
        v = 0.0;
        for(j=0; j<=nmain+nslack-1; j++)
        {
            v = ae_maxreal(v, ae_fabs(ce->ptr.pp_double[i][j], _state), _state);
        }
        if( ae_fp_neq(v,0) )
        {
            v = 1/v;
            ae_v_muld(&ce->ptr.pp_double[i][0], 1, ae_v_len(0,nmain+nslack), v);
        }
    }
    
    /*
     * Allocate temporaries
     */
    ae_vector_set_length(&xn, nmain+nslack, _state);
    ae_vector_set_length(&xa, nmain+nslack, _state);
    ae_vector_set_length(&permx, nmain+nslack, _state);
    ae_vector_set_length(&g, nmain+nslack, _state);
    ae_vector_set_length(&pg, nmain+nslack, _state);
    ae_vector_set_length(&tmpk, k, _state);
    ae_matrix_set_length(&a, k, nmain+nslack, _state);
    ae_vector_set_length(&activeconstraints, nmain+nslack, _state);
    ae_vector_set_length(&newtonstep, nmain+nslack, _state);
    ae_vector_set_length(&s, nmain+nslack, _state);
    ae_vector_set_length(&colnorms, nmain+nslack, _state);
    for(i=0; i<=nmain+nslack-1; i++)
    {
        s.ptr.p_double[i] = 1.0;
        colnorms.ptr.p_double[i] = 0.0;
        for(j=0; j<=k-1; j++)
        {
            colnorms.ptr.p_double[i] = colnorms.ptr.p_double[i]+ae_sqr(ce->ptr.pp_double[j][i], _state);
        }
    }
    
    /*
     * K>0, we have linear equality constraints combined with bound constraints.
     *
     * Try to find feasible point as minimizer of the quadratic function
     *     F(x) = 0.5*||CE*x-b||^2 = 0.5*x'*(CE'*CE)*x - (b'*CE)*x + 0.5*b'*b
     * subject to boundary constraints given by BL, BU and non-negativity of
     * the slack variables. BTW, we drop constant term because it does not
     * actually influences on the solution.
     *
     * Below we will assume that K>0.
     */
    itswithintolerance = 0;
    itscount = 0;
    for(;;)
    {
        
        /*
         * Stage 0: check for exact convergence
         */
        converged = ae_true;
        feaserr = 0;
        for(i=0; i<=k-1; i++)
        {
            
            /*
             * Calculate:
             * * V - error in the right part
             * * MX - maximum term in the left part
             *
             * Terminate if error in the right part is not greater than 100*Eps*MX.
             *
             * IMPORTANT: we must perform check for non-strict inequality, i.e. to use <= instead of <.
             *            it will allow us to easily handle situations with zero rows of CE.
             */
            mx = 0;
            v = -ce->ptr.pp_double[i][nmain+nslack];
            for(j=0; j<=nmain+nslack-1; j++)
            {
                mx = ae_maxreal(mx, ae_fabs(ce->ptr.pp_double[i][j]*x->ptr.p_double[j], _state), _state);
                v = v+ce->ptr.pp_double[i][j]*x->ptr.p_double[j];
            }
            feaserr = feaserr+ae_sqr(v, _state);
            converged = converged&&ae_fp_less_eq(ae_fabs(v, _state),100*ae_machineepsilon*mx);
        }
        feaserr = ae_sqrt(feaserr, _state);
        if( converged )
        {
            result = ae_fp_less_eq(feaserr,epsi);
            ae_frame_leave(_state);
            return result;
        }
        
        /*
         * Stage 1: equality constrained quadratic programming
         *
         * * treat active bound constraints as equality ones (constraint is considered 
         *   active when we are at the boundary, independently of the antigradient direction)
         * * calculate unrestricted Newton step to point XM (which may be infeasible)
         *   calculate MaxStepLen = largest step in direction of XM which retains feasibility.
         * * perform bounded step from X to XN:
         *   a) XN=XM                  (if XM is feasible)
         *   b) XN=X-MaxStepLen*(XM-X) (otherwise)
         * * X := XN
         * * if XM (Newton step subject to currently active constraints) was feasible, goto Stage 2
         * * repeat Stage 1
         *
         * NOTE 1: in order to solve constrained qudratic subproblem we will have to reorder
         *         variables in such way that ones corresponding to inactive constraints will
         *         be first, and active ones will be last in the list. CE and X are now
         *                                                       [ xi ]
         *         separated into two parts: CE = [CEi CEa], x = [    ], where CEi/Xi correspond
         *                                                       [ xa ]
         *         to INACTIVE constraints, and CEa/Xa correspond to the ACTIVE ones.
         *
         *         Now, instead of F=0.5*x'*(CE'*CE)*x - (b'*CE)*x + 0.5*b'*b, we have
         *         F(xi) = 0.5*(CEi*xi,CEi*xi) + (CEa*xa-b,CEi*xi) + (0.5*CEa*xa-b,CEa*xa).
         *         Here xa is considered constant, i.e. we optimize with respect to xi, leaving xa fixed.
         *
         *         We can solve it by performing SVD of CEi and calculating pseudoinverse of the
         *         Hessian matrix. Of course, we do NOT calculate pseudoinverse explicitly - we
         *         just use singular vectors to perform implicit multiplication by it.
         *
         */
        for(;;)
        {
            
            /*
             * Calculate G - gradient subject to equality constraints,
             * multiply it by inverse of the Hessian diagonal to obtain initial
             * step vector.
             *
             * Bound step subject to constraints which can be activated,
             * run Armijo search with increasing step size.
             * Search is terminated when feasibility error stops to decrease.
             *
             * NOTE: it is important to test for "stops to decrease" instead
             * of "starts to increase" in order to correctly handle cases with
             * zero CE.
             */
            armijobeststep = 0.0;
            armijobestfeas = 0.0;
            for(i=0; i<=nmain+nslack-1; i++)
            {
                g.ptr.p_double[i] = 0;
            }
            for(i=0; i<=k-1; i++)
            {
                v = ae_v_dotproduct(&ce->ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                v = v-ce->ptr.pp_double[i][nmain+nslack];
                armijobestfeas = armijobestfeas+ae_sqr(v, _state);
                ae_v_addd(&g.ptr.p_double[0], 1, &ce->ptr.pp_double[i][0], 1, ae_v_len(0,nmain+nslack-1), v);
            }
            armijobestfeas = ae_sqrt(armijobestfeas, _state);
            for(i=0; i<=nmain-1; i++)
            {
                if( havebndl->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]) )
                {
                    g.ptr.p_double[i] = 0.0;
                }
                if( havebndu->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]) )
                {
                    g.ptr.p_double[i] = 0.0;
                }
            }
            for(i=0; i<=nslack-1; i++)
            {
                if( ae_fp_eq(x->ptr.p_double[nmain+i],0.0) )
                {
                    g.ptr.p_double[nmain+i] = 0.0;
                }
            }
            v = 0.0;
            for(i=0; i<=nmain+nslack-1; i++)
            {
                if( ae_fp_neq(ae_sqr(colnorms.ptr.p_double[i], _state),0) )
                {
                    newtonstep.ptr.p_double[i] = -g.ptr.p_double[i]/ae_sqr(colnorms.ptr.p_double[i], _state);
                }
                else
                {
                    newtonstep.ptr.p_double[i] = 0.0;
                }
                v = v+ae_sqr(newtonstep.ptr.p_double[i], _state);
            }
            if( ae_fp_eq(v,0) )
            {
                
                /*
                 * Constrained gradient is zero, QP iterations are over
                 */
                break;
            }
            calculatestepbound(x, &newtonstep, 1.0, bndl, havebndl, bndu, havebndu, nmain, nslack, &vartofreeze, &valtofreeze, &maxsteplen, _state);
            if( vartofreeze>=0&&ae_fp_eq(maxsteplen,0) )
            {
                
                /*
                 * Can not perform step, QP iterations are over
                 */
                break;
            }
            if( vartofreeze>=0 )
            {
                armijostep = ae_minreal(1.0, maxsteplen, _state);
            }
            else
            {
                armijostep = 1;
            }
            for(;;)
            {
                ae_v_move(&xa.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                ae_v_addd(&xa.ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), armijostep);
                enforceboundaryconstraints(&xa, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
                feaserr = 0.0;
                for(i=0; i<=k-1; i++)
                {
                    v = ae_v_dotproduct(&ce->ptr.pp_double[i][0], 1, &xa.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                    v = v-ce->ptr.pp_double[i][nmain+nslack];
                    feaserr = feaserr+ae_sqr(v, _state);
                }
                feaserr = ae_sqrt(feaserr, _state);
                if( ae_fp_greater_eq(feaserr,armijobestfeas) )
                {
                    break;
                }
                armijobestfeas = feaserr;
                armijobeststep = armijostep;
                armijostep = 2.0*armijostep;
            }
            ae_v_addd(&x->ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), armijobeststep);
            enforceboundaryconstraints(x, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
            
            /*
             * Determine number of active and free constraints
             */
            nactive = 0;
            for(i=0; i<=nmain-1; i++)
            {
                activeconstraints.ptr.p_double[i] = 0;
                if( havebndl->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]) )
                {
                    activeconstraints.ptr.p_double[i] = 1;
                }
                if( havebndu->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]) )
                {
                    activeconstraints.ptr.p_double[i] = 1;
                }
                if( ae_fp_greater(activeconstraints.ptr.p_double[i],0) )
                {
                    nactive = nactive+1;
                }
            }
            for(i=0; i<=nslack-1; i++)
            {
                activeconstraints.ptr.p_double[nmain+i] = 0;
                if( ae_fp_eq(x->ptr.p_double[nmain+i],0.0) )
                {
                    activeconstraints.ptr.p_double[nmain+i] = 1;
                }
                if( ae_fp_greater(activeconstraints.ptr.p_double[nmain+i],0) )
                {
                    nactive = nactive+1;
                }
            }
            nfree = nmain+nslack-nactive;
            if( nfree==0 )
            {
                break;
            }
            *qpits = *qpits+1;
            
            /*
             * Reorder variables
             */
            tagsortbuf(&activeconstraints, nmain+nslack, &p1, &p2, &buf, _state);
            for(i=0; i<=k-1; i++)
            {
                for(j=0; j<=nmain+nslack-1; j++)
                {
                    a.ptr.pp_double[i][j] = ce->ptr.pp_double[i][j];
                }
            }
            for(j=0; j<=nmain+nslack-1; j++)
            {
                permx.ptr.p_double[j] = x->ptr.p_double[j];
            }
            for(j=0; j<=nmain+nslack-1; j++)
            {
                if( p2.ptr.p_int[j]!=j )
                {
                    idx0 = p2.ptr.p_int[j];
                    idx1 = j;
                    for(i=0; i<=k-1; i++)
                    {
                        v = a.ptr.pp_double[i][idx0];
                        a.ptr.pp_double[i][idx0] = a.ptr.pp_double[i][idx1];
                        a.ptr.pp_double[i][idx1] = v;
                    }
                    v = permx.ptr.p_double[idx0];
                    permx.ptr.p_double[idx0] = permx.ptr.p_double[idx1];
                    permx.ptr.p_double[idx1] = v;
                }
            }
            
            /*
             * Calculate (unprojected) gradient:
             * G(xi) = CEi'*(CEi*xi + CEa*xa - b)
             */
            for(i=0; i<=nfree-1; i++)
            {
                g.ptr.p_double[i] = 0;
            }
            for(i=0; i<=k-1; i++)
            {
                v = ae_v_dotproduct(&a.ptr.pp_double[i][0], 1, &permx.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                tmpk.ptr.p_double[i] = v-ce->ptr.pp_double[i][nmain+nslack];
            }
            for(i=0; i<=k-1; i++)
            {
                v = tmpk.ptr.p_double[i];
                ae_v_addd(&g.ptr.p_double[0], 1, &a.ptr.pp_double[i][0], 1, ae_v_len(0,nfree-1), v);
            }
            
            /*
             * Calculate Newton step using SVD of CEi:
             *     F(xi)  = 0.5*xi'*H*xi + g'*xi    (Taylor decomposition)
             *     XN     = -H^(-1)*g               (new point, solution of the QP subproblem)
             *     H      = CEi'*CEi                
             *     CEi    = U*W*V'                  (SVD of CEi)
             *     H      = V*W^2*V'                 
             *     H^(-1) = V*W^(-2)*V'
             *     step     = -V*W^(-2)*V'*g          (it is better to perform multiplication from right to left)
             *
             * NOTE 1: we do NOT need left singular vectors to perform Newton step.
             */
            nsvd = ae_minint(k, nfree, _state);
            if( !rmatrixsvd(&a, k, nfree, 0, 1, 2, &w, &u, &vt, _state) )
            {
                result = ae_false;
                ae_frame_leave(_state);
                return result;
            }
            for(i=0; i<=nsvd-1; i++)
            {
                v = ae_v_dotproduct(&vt.ptr.pp_double[i][0], 1, &g.ptr.p_double[0], 1, ae_v_len(0,nfree-1));
                tmpk.ptr.p_double[i] = v;
            }
            for(i=0; i<=nsvd-1; i++)
            {
                
                /*
                 * It is important to have strict ">" in order to correctly 
                 * handle zero singular values.
                 */
                if( ae_fp_greater(ae_sqr(w.ptr.p_double[i], _state),ae_sqr(w.ptr.p_double[0], _state)*(nmain+nslack)*ae_machineepsilon) )
                {
                    tmpk.ptr.p_double[i] = tmpk.ptr.p_double[i]/ae_sqr(w.ptr.p_double[i], _state);
                }
                else
                {
                    tmpk.ptr.p_double[i] = 0;
                }
            }
            for(i=0; i<=nmain+nslack-1; i++)
            {
                newtonstep.ptr.p_double[i] = 0;
            }
            for(i=0; i<=nsvd-1; i++)
            {
                v = tmpk.ptr.p_double[i];
                ae_v_subd(&newtonstep.ptr.p_double[0], 1, &vt.ptr.pp_double[i][0], 1, ae_v_len(0,nfree-1), v);
            }
            for(j=nmain+nslack-1; j>=0; j--)
            {
                if( p2.ptr.p_int[j]!=j )
                {
                    idx0 = p2.ptr.p_int[j];
                    idx1 = j;
                    v = newtonstep.ptr.p_double[idx0];
                    newtonstep.ptr.p_double[idx0] = newtonstep.ptr.p_double[idx1];
                    newtonstep.ptr.p_double[idx1] = v;
                }
            }
            
            /*
             * NewtonStep contains Newton step subject to active bound constraints.
             *
             * Such step leads us to the minimizer of the equality constrained F,
             * but such minimizer may be infeasible because some constraints which
             * are inactive at the initial point can be violated at the solution.
             *
             * Thus, we perform optimization in two stages:
             * a) perform bounded Newton step, i.e. step in the Newton direction
             *    until activation of the first constraint
             * b) in case (MaxStepLen>0)and(MaxStepLen<1), perform additional iteration
             *    of the Armijo line search in the rest of the Newton direction.
             */
            calculatestepbound(x, &newtonstep, 1.0, bndl, havebndl, bndu, havebndu, nmain, nslack, &vartofreeze, &valtofreeze, &maxsteplen, _state);
            if( vartofreeze>=0&&ae_fp_eq(maxsteplen,0) )
            {
                
                /*
                 * Activation of the constraints prevent us from performing step,
                 * QP iterations are over
                 */
                break;
            }
            if( vartofreeze>=0 )
            {
                v = ae_minreal(1.0, maxsteplen, _state);
            }
            else
            {
                v = 1.0;
            }
            ae_v_moved(&xn.ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), v);
            ae_v_add(&xn.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            postprocessboundedstep(&xn, x, bndl, havebndl, bndu, havebndu, nmain, nslack, vartofreeze, valtofreeze, v, maxsteplen, _state);
            if( ae_fp_greater(maxsteplen,0)&&ae_fp_less(maxsteplen,1) )
            {
                
                /*
                 * Newton step was restricted by activation of the constraints,
                 * perform Armijo iteration.
                 *
                 * Initial estimate for best step is zero step. We try different
                 * step sizes, from the 1-MaxStepLen (residual of the full Newton
                 * step) to progressively smaller and smaller steps.
                 */
                armijobeststep = 0.0;
                armijobestfeas = 0.0;
                for(i=0; i<=k-1; i++)
                {
                    v = ae_v_dotproduct(&ce->ptr.pp_double[i][0], 1, &xn.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                    v = v-ce->ptr.pp_double[i][nmain+nslack];
                    armijobestfeas = armijobestfeas+ae_sqr(v, _state);
                }
                armijobestfeas = ae_sqrt(armijobestfeas, _state);
                armijostep = 1-maxsteplen;
                for(j=0; j<=maxarmijoruns-1; j++)
                {
                    ae_v_move(&xa.ptr.p_double[0], 1, &xn.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                    ae_v_addd(&xa.ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), armijostep);
                    enforceboundaryconstraints(&xa, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
                    feaserr = 0.0;
                    for(i=0; i<=k-1; i++)
                    {
                        v = ae_v_dotproduct(&ce->ptr.pp_double[i][0], 1, &xa.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                        v = v-ce->ptr.pp_double[i][nmain+nslack];
                        feaserr = feaserr+ae_sqr(v, _state);
                    }
                    feaserr = ae_sqrt(feaserr, _state);
                    if( ae_fp_less(feaserr,armijobestfeas) )
                    {
                        armijobestfeas = feaserr;
                        armijobeststep = armijostep;
                    }
                    armijostep = 0.5*armijostep;
                }
                ae_v_move(&xa.ptr.p_double[0], 1, &xn.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                ae_v_addd(&xa.ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), armijobeststep);
                enforceboundaryconstraints(&xa, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
            }
            else
            {
                
                /*
                 * Armijo iteration is not performed
                 */
                ae_v_move(&xa.ptr.p_double[0], 1, &xn.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            }
            stage1isover = ae_fp_greater_eq(maxsteplen,1)||ae_fp_eq(maxsteplen,0);
            
            /*
             * Calculate feasibility errors for old and new X.
             * These quantinies are used for debugging purposes only.
             * However, we can leave them in release code because performance impact is insignificant.
             *
             * Update X. Exit if needed.
             */
            feasold = 0;
            feasnew = 0;
            for(i=0; i<=k-1; i++)
            {
                v = ae_v_dotproduct(&ce->ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                feasold = feasold+ae_sqr(v-ce->ptr.pp_double[i][nmain+nslack], _state);
                v = ae_v_dotproduct(&ce->ptr.pp_double[i][0], 1, &xa.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                feasnew = feasnew+ae_sqr(v-ce->ptr.pp_double[i][nmain+nslack], _state);
            }
            feasold = ae_sqrt(feasold, _state);
            feasnew = ae_sqrt(feasnew, _state);
            if( ae_fp_greater_eq(feasnew,feasold) )
            {
                break;
            }
            ae_v_move(&x->ptr.p_double[0], 1, &xa.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            if( stage1isover )
            {
                break;
            }
        }
        
        /*
         * Stage 2: gradient projection algorithm (GPA)
         *
         * * calculate feasibility error (with respect to linear equality constraints)
         * * calculate gradient G of F, project it into feasible area (G => PG)
         * * exit if norm(PG) is exactly zero or feasibility error is smaller than EpsC
         * * let XM be exact minimum of F along -PG (XM may be infeasible).
         *   calculate MaxStepLen = largest step in direction of -PG which retains feasibility.
         * * perform bounded step from X to XN:
         *   a) XN=XM              (if XM is feasible)
         *   b) XN=X-MaxStepLen*PG (otherwise)
         * * X := XN
         * * stop after specified number of iterations or when no new constraints was activated
         *
         * NOTES:
         * * grad(F) = (CE'*CE)*x - (b'*CE)^T
         * * CE[i] denotes I-th row of CE
         * * XM = X+stp*(-PG) where stp=(grad(F(X)),PG)/(CE*PG,CE*PG).
         *   Here PG is a projected gradient, but in fact it can be arbitrary non-zero 
         *   direction vector - formula for minimum of F along PG still will be correct.
         */
        werechangesinconstraints = ae_false;
        for(gparuns=1; gparuns<=k; gparuns++)
        {
            
            /*
             * calculate feasibility error and G
             */
            feaserr = 0;
            for(i=0; i<=nmain+nslack-1; i++)
            {
                g.ptr.p_double[i] = 0;
            }
            for(i=0; i<=k-1; i++)
            {
                
                /*
                 * G += CE[i]^T * (CE[i]*x-b[i])
                 */
                v = ae_v_dotproduct(&ce->ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                v = v-ce->ptr.pp_double[i][nmain+nslack];
                feaserr = feaserr+ae_sqr(v, _state);
                ae_v_addd(&g.ptr.p_double[0], 1, &ce->ptr.pp_double[i][0], 1, ae_v_len(0,nmain+nslack-1), v);
            }
            
            /*
             * project G, filter it (strip numerical noise)
             */
            ae_v_move(&pg.ptr.p_double[0], 1, &g.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            projectgradientintobc(x, &pg, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
            filterdirection(&pg, x, bndl, havebndl, bndu, havebndu, &s, nmain, nslack, 1.0E-9, _state);
            for(i=0; i<=nmain+nslack-1; i++)
            {
                if( ae_fp_neq(ae_sqr(colnorms.ptr.p_double[i], _state),0) )
                {
                    pg.ptr.p_double[i] = pg.ptr.p_double[i]/ae_sqr(colnorms.ptr.p_double[i], _state);
                }
                else
                {
                    pg.ptr.p_double[i] = 0.0;
                }
            }
            
            /*
             * Check GNorm and feasibility.
             * Exit when GNorm is exactly zero.
             */
            pgnorm = ae_v_dotproduct(&pg.ptr.p_double[0], 1, &pg.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            feaserr = ae_sqrt(feaserr, _state);
            pgnorm = ae_sqrt(pgnorm, _state);
            if( ae_fp_eq(pgnorm,0) )
            {
                result = ae_fp_less_eq(feaserr,epsi);
                ae_frame_leave(_state);
                return result;
            }
            
            /*
             * calculate planned step length
             */
            vn = ae_v_dotproduct(&g.ptr.p_double[0], 1, &pg.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            vd = 0;
            for(i=0; i<=k-1; i++)
            {
                v = ae_v_dotproduct(&ce->ptr.pp_double[i][0], 1, &pg.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                vd = vd+ae_sqr(v, _state);
            }
            stp = vn/vd;
            
            /*
             * Calculate step bound.
             * Perform bounded step and post-process it
             */
            calculatestepbound(x, &pg, -1.0, bndl, havebndl, bndu, havebndu, nmain, nslack, &vartofreeze, &valtofreeze, &maxsteplen, _state);
            if( vartofreeze>=0&&ae_fp_eq(maxsteplen,0) )
            {
                result = ae_false;
                ae_frame_leave(_state);
                return result;
            }
            if( vartofreeze>=0 )
            {
                v = ae_minreal(stp, maxsteplen, _state);
            }
            else
            {
                v = stp;
            }
            ae_v_move(&xn.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            ae_v_subd(&xn.ptr.p_double[0], 1, &pg.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), v);
            postprocessboundedstep(&xn, x, bndl, havebndl, bndu, havebndu, nmain, nslack, vartofreeze, valtofreeze, v, maxsteplen, _state);
            
            /*
             * update X
             * check stopping criteria
             */
            werechangesinconstraints = werechangesinconstraints||numberofchangedconstraints(&xn, x, bndl, havebndl, bndu, havebndu, nmain, nslack, _state)>0;
            ae_v_move(&x->ptr.p_double[0], 1, &xn.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            *gpaits = *gpaits+1;
            if( !werechangesinconstraints )
            {
                break;
            }
        }
        
        /*
         * Stage 3: decide to stop algorithm or not to stop
         *
         * 1. we can stop when last GPA run did NOT changed constraints status.
         *    It means that we've found final set of the active constraints even
         *    before GPA made its run. And it means that Newton step moved us to
         *    the minimum subject to the present constraints.
         *    Depending on feasibility error, True or False is returned.
         */
        feaserr = 0;
        for(i=0; i<=k-1; i++)
        {
            v = ae_v_dotproduct(&ce->ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            v = v-ce->ptr.pp_double[i][nmain+nslack];
            feaserr = feaserr+ae_sqr(v, _state);
        }
        feaserr = ae_sqrt(feaserr, _state);
        if( ae_fp_less_eq(feaserr,epsi) )
        {
            itswithintolerance = itswithintolerance+1;
        }
        else
        {
            itswithintolerance = 0;
        }
        if( !werechangesinconstraints||itswithintolerance>=maxitswithintolerance )
        {
            result = ae_fp_less_eq(feaserr,epsi);
            ae_frame_leave(_state);
            return result;
        }
        itscount = itscount+1;
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
    This function check, that input derivatives are right. First it scale
parameters DF0 and DF1 from segment [A;B] to [0;1]. Than it build Hermite
spline and derivative of it in 0,5. Search scale as Max(DF0,DF1, |F0-F1|).
Right derivative has to satisfy condition:
    |H-F|/S<=0,01, |H'-F'|/S<=0,01.
    
INPUT PARAMETERS:
    F0  -   function's value in X-TestStep point;
    DF0 -   derivative's value in X-TestStep point;
    F1  -   function's value in X+TestStep point;
    DF1 -   derivative's value in X+TestStep point;
    F   -   testing function's value;
    DF  -   testing derivative's value;
   Width-   width of verification segment.

RESULT:
    If input derivatives is right then function returns true, else 
    function returns false.
    
  -- ALGLIB --
     Copyright 29.05.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool derivativecheck(double f0,
     double df0,
     double f1,
     double df1,
     double f,
     double df,
     double width,
     ae_state *_state)
{
    double s;
    double h;
    double dh;
    ae_bool result;


    df = width*df;
    df0 = width*df0;
    df1 = width*df1;
    s = ae_maxreal(ae_maxreal(ae_fabs(df0, _state), ae_fabs(df1, _state), _state), ae_fabs(f1-f0, _state), _state);
    h = 0.5*f0+0.125*df0+0.5*f1-0.125*df1;
    dh = -1.5*f0-0.25*df0+1.5*f1-0.25*df1;
    if( ae_fp_neq(s,0) )
    {
        if( ae_fp_greater(ae_fabs(h-f, _state)/s,0.001)||ae_fp_greater(ae_fabs(dh-df, _state)/s,0.001) )
        {
            result = ae_false;
            return result;
        }
    }
    else
    {
        if( ae_fp_neq(h-f,0.0)||ae_fp_neq(dh-df,0.0) )
        {
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    return result;
}




/*************************************************************************
This subroutine is used to initialize CQM. By default, empty NxN model  is
generated, with Alpha=Lambda=Theta=0.0 and zero b.

Previously allocated buffer variables are reused as much as possible.

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqminit(ae_int_t n, convexquadraticmodel* s, ae_state *_state)
{
    ae_int_t i;


    s->n = n;
    s->k = 0;
    s->nfree = n;
    s->ecakind = -1;
    s->alpha = 0.0;
    s->tau = 0.0;
    s->theta = 0.0;
    s->ismaintermchanged = ae_true;
    s->issecondarytermchanged = ae_true;
    s->islineartermchanged = ae_true;
    s->isactivesetchanged = ae_true;
    bvectorsetlengthatleast(&s->activeset, n, _state);
    rvectorsetlengthatleast(&s->xc, n, _state);
    rvectorsetlengthatleast(&s->eb, n, _state);
    rvectorsetlengthatleast(&s->tq1, n, _state);
    rvectorsetlengthatleast(&s->txc, n, _state);
    rvectorsetlengthatleast(&s->tb, n, _state);
    rvectorsetlengthatleast(&s->b, s->n, _state);
    rvectorsetlengthatleast(&s->tk1, s->n, _state);
    for(i=0; i<=n-1; i++)
    {
        s->activeset.ptr.p_bool[i] = ae_false;
        s->xc.ptr.p_double[i] = 0.0;
        s->b.ptr.p_double[i] = 0.0;
    }
}


/*************************************************************************
This subroutine changes main quadratic term of the model.

INPUT PARAMETERS:
    S       -   model
    A       -   NxN matrix, only upper or lower triangle is referenced
    IsUpper -   True, when matrix is stored in upper triangle
    Alpha   -   multiplier; when Alpha=0, A is not referenced at all

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmseta(convexquadraticmodel* s,
     /* Real    */ ae_matrix* a,
     ae_bool isupper,
     double alpha,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;


    ae_assert(ae_isfinite(alpha, _state)&&ae_fp_greater_eq(alpha,0), "CQMSetA: Alpha<0 or is not finite number", _state);
    ae_assert(ae_fp_eq(alpha,0)||isfinitertrmatrix(a, s->n, isupper, _state), "CQMSetA: A is not finite NxN matrix", _state);
    s->alpha = alpha;
    if( ae_fp_greater(alpha,0) )
    {
        rmatrixsetlengthatleast(&s->a, s->n, s->n, _state);
        rmatrixsetlengthatleast(&s->ecadense, s->n, s->n, _state);
        rmatrixsetlengthatleast(&s->tq2dense, s->n, s->n, _state);
        for(i=0; i<=s->n-1; i++)
        {
            for(j=i; j<=s->n-1; j++)
            {
                if( isupper )
                {
                    v = a->ptr.pp_double[i][j];
                }
                else
                {
                    v = a->ptr.pp_double[j][i];
                }
                s->a.ptr.pp_double[i][j] = v;
                s->a.ptr.pp_double[j][i] = v;
            }
        }
    }
    s->ismaintermchanged = ae_true;
}


/*************************************************************************
This subroutine rewrites diagonal of the main quadratic term of the  model
(dense  A)  by  vector  Z/Alpha (current value of the Alpha coefficient is
used).

IMPORTANT: in  case  model  has  no  dense  quadratic  term, this function
           allocates N*N dense matrix of zeros, and fills its diagonal  by
           non-zero values.

INPUT PARAMETERS:
    S       -   model
    Z       -   new diagonal, array[N]

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmrewritedensediagonal(convexquadraticmodel* s,
     /* Real    */ ae_vector* z,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;


    n = s->n;
    if( ae_fp_eq(s->alpha,0) )
    {
        rmatrixsetlengthatleast(&s->a, s->n, s->n, _state);
        rmatrixsetlengthatleast(&s->ecadense, s->n, s->n, _state);
        rmatrixsetlengthatleast(&s->tq2dense, s->n, s->n, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                s->a.ptr.pp_double[i][j] = 0.0;
            }
        }
        s->alpha = 1.0;
    }
    for(i=0; i<=s->n-1; i++)
    {
        s->a.ptr.pp_double[i][i] = z->ptr.p_double[i]/s->alpha;
    }
    s->ismaintermchanged = ae_true;
}


/*************************************************************************
This subroutine changes diagonal quadratic term of the model.

INPUT PARAMETERS:
    S       -   model
    D       -   array[N], semidefinite diagonal matrix
    Tau     -   multiplier; when Tau=0, D is not referenced at all

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmsetd(convexquadraticmodel* s,
     /* Real    */ ae_vector* d,
     double tau,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(ae_isfinite(tau, _state)&&ae_fp_greater_eq(tau,0), "CQMSetD: Tau<0 or is not finite number", _state);
    ae_assert(ae_fp_eq(tau,0)||isfinitevector(d, s->n, _state), "CQMSetD: D is not finite Nx1 vector", _state);
    s->tau = tau;
    if( ae_fp_greater(tau,0) )
    {
        rvectorsetlengthatleast(&s->d, s->n, _state);
        rvectorsetlengthatleast(&s->ecadiag, s->n, _state);
        rvectorsetlengthatleast(&s->tq2diag, s->n, _state);
        for(i=0; i<=s->n-1; i++)
        {
            ae_assert(ae_fp_greater_eq(d->ptr.p_double[i],0), "CQMSetD: D[i]<0", _state);
            s->d.ptr.p_double[i] = d->ptr.p_double[i];
        }
    }
    s->ismaintermchanged = ae_true;
}


/*************************************************************************
This subroutine drops main quadratic term A from the model. It is same  as
call  to  CQMSetA()  with  zero  A,   but gives better performance because
algorithm  knows  that  matrix  is  zero  and  can  optimize    subsequent
calculations.

INPUT PARAMETERS:
    S       -   model

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmdropa(convexquadraticmodel* s, ae_state *_state)
{


    s->alpha = 0.0;
    s->ismaintermchanged = ae_true;
}


/*************************************************************************
This subroutine changes linear term of the model

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmsetb(convexquadraticmodel* s,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(isfinitevector(b, s->n, _state), "CQMSetB: B is not finite vector", _state);
    rvectorsetlengthatleast(&s->b, s->n, _state);
    for(i=0; i<=s->n-1; i++)
    {
        s->b.ptr.p_double[i] = b->ptr.p_double[i];
    }
    s->islineartermchanged = ae_true;
}


/*************************************************************************
This subroutine changes linear term of the model

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmsetq(convexquadraticmodel* s,
     /* Real    */ ae_matrix* q,
     /* Real    */ ae_vector* r,
     ae_int_t k,
     double theta,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    ae_assert(k>=0, "CQMSetQ: K<0", _state);
    ae_assert((k==0||ae_fp_eq(theta,0))||apservisfinitematrix(q, k, s->n, _state), "CQMSetQ: Q is not finite matrix", _state);
    ae_assert((k==0||ae_fp_eq(theta,0))||isfinitevector(r, k, _state), "CQMSetQ: R is not finite vector", _state);
    ae_assert(ae_isfinite(theta, _state)&&ae_fp_greater_eq(theta,0), "CQMSetQ: Theta<0 or is not finite number", _state);
    
    /*
     * degenerate case: K=0 or Theta=0
     */
    if( k==0||ae_fp_eq(theta,0) )
    {
        s->k = 0;
        s->theta = 0;
        s->issecondarytermchanged = ae_true;
        return;
    }
    
    /*
     * General case: both Theta>0 and K>0
     */
    s->k = k;
    s->theta = theta;
    rmatrixsetlengthatleast(&s->q, s->k, s->n, _state);
    rvectorsetlengthatleast(&s->r, s->k, _state);
    rmatrixsetlengthatleast(&s->eq, s->k, s->n, _state);
    rmatrixsetlengthatleast(&s->eccm, s->k, s->k, _state);
    rmatrixsetlengthatleast(&s->tk2, s->k, s->n, _state);
    for(i=0; i<=s->k-1; i++)
    {
        for(j=0; j<=s->n-1; j++)
        {
            s->q.ptr.pp_double[i][j] = q->ptr.pp_double[i][j];
        }
        s->r.ptr.p_double[i] = r->ptr.p_double[i];
    }
    s->issecondarytermchanged = ae_true;
}


/*************************************************************************
This subroutine changes active set

INPUT PARAMETERS
    S       -   model
    X       -   array[N], constraint values
    ActiveSet-  array[N], active set. If ActiveSet[I]=True, then I-th
                variables is constrained to X[I].

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmsetactiveset(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     /* Boolean */ ae_vector* activeset,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(x->cnt>=s->n, "CQMSetActiveSet: Length(X)<N", _state);
    ae_assert(activeset->cnt>=s->n, "CQMSetActiveSet: Length(ActiveSet)<N", _state);
    for(i=0; i<=s->n-1; i++)
    {
        s->isactivesetchanged = s->isactivesetchanged||(s->activeset.ptr.p_bool[i]&&!activeset->ptr.p_bool[i]);
        s->isactivesetchanged = s->isactivesetchanged||(activeset->ptr.p_bool[i]&&!s->activeset.ptr.p_bool[i]);
        s->activeset.ptr.p_bool[i] = activeset->ptr.p_bool[i];
        if( activeset->ptr.p_bool[i] )
        {
            ae_assert(ae_isfinite(x->ptr.p_double[i], _state), "CQMSetActiveSet: X[] contains infinite constraints", _state);
            s->isactivesetchanged = s->isactivesetchanged||ae_fp_neq(s->xc.ptr.p_double[i],x->ptr.p_double[i]);
            s->xc.ptr.p_double[i] = x->ptr.p_double[i];
        }
    }
}


/*************************************************************************
This subroutine evaluates model at X. Active constraints are ignored.

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
double cqmeval(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    double v;
    double result;


    n = s->n;
    ae_assert(isfinitevector(x, n, _state), "CQMEval: X is not finite vector", _state);
    result = 0.0;
    
    /*
     * main quadratic term
     */
    if( ae_fp_greater(s->alpha,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                result = result+s->alpha*0.5*x->ptr.p_double[i]*s->a.ptr.pp_double[i][j]*x->ptr.p_double[j];
            }
        }
    }
    if( ae_fp_greater(s->tau,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            result = result+0.5*ae_sqr(x->ptr.p_double[i], _state)*s->tau*s->d.ptr.p_double[i];
        }
    }
    
    /*
     * secondary quadratic term
     */
    if( ae_fp_greater(s->theta,0) )
    {
        for(i=0; i<=s->k-1; i++)
        {
            v = ae_v_dotproduct(&s->q.ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
            result = result+0.5*s->theta*ae_sqr(v-s->r.ptr.p_double[i], _state);
        }
    }
    
    /*
     * linear term
     */
    for(i=0; i<=s->n-1; i++)
    {
        result = result+x->ptr.p_double[i]*s->b.ptr.p_double[i];
    }
    return result;
}


/*************************************************************************
This subroutine evaluates model at X. Active constraints are ignored.
It returns:
    R   -   model value
    Noise-  estimate of the numerical noise in data

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmevalx(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     double* r,
     double* noise,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    double v;
    double v2;
    double mxq;
    double eps;

    *r = 0;
    *noise = 0;

    n = s->n;
    ae_assert(isfinitevector(x, n, _state), "CQMEval: X is not finite vector", _state);
    *r = 0.0;
    *noise = 0.0;
    eps = 2*ae_machineepsilon;
    mxq = 0.0;
    
    /*
     * Main quadratic term.
     *
     * Noise from the main quadratic term is equal to the
     * maximum summand in the term.
     */
    if( ae_fp_greater(s->alpha,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                v = s->alpha*0.5*x->ptr.p_double[i]*s->a.ptr.pp_double[i][j]*x->ptr.p_double[j];
                *r = *r+v;
                *noise = ae_maxreal(*noise, eps*ae_fabs(v, _state), _state);
            }
        }
    }
    if( ae_fp_greater(s->tau,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            v = 0.5*ae_sqr(x->ptr.p_double[i], _state)*s->tau*s->d.ptr.p_double[i];
            *r = *r+v;
            *noise = ae_maxreal(*noise, eps*ae_fabs(v, _state), _state);
        }
    }
    
    /*
     * secondary quadratic term
     *
     * Noise from the secondary quadratic term is estimated as follows:
     * * noise in qi*x-r[i] is estimated as
     *   Eps*MXQ = Eps*max(|r[i]|, |q[i,j]*x[j]|)
     * * noise in (qi*x-r[i])^2 is estimated as
     *   NOISE = (|qi*x-r[i]|+Eps*MXQ)^2-(|qi*x-r[i]|)^2
     *         = Eps*MXQ*(2*|qi*x-r[i]|+Eps*MXQ)
     */
    if( ae_fp_greater(s->theta,0) )
    {
        for(i=0; i<=s->k-1; i++)
        {
            v = 0.0;
            mxq = ae_fabs(s->r.ptr.p_double[i], _state);
            for(j=0; j<=n-1; j++)
            {
                v2 = s->q.ptr.pp_double[i][j]*x->ptr.p_double[j];
                v = v+v2;
                mxq = ae_maxreal(mxq, ae_fabs(v2, _state), _state);
            }
            *r = *r+0.5*s->theta*ae_sqr(v-s->r.ptr.p_double[i], _state);
            *noise = ae_maxreal(*noise, eps*mxq*(2*ae_fabs(v-s->r.ptr.p_double[i], _state)+eps*mxq), _state);
        }
    }
    
    /*
     * linear term
     */
    for(i=0; i<=s->n-1; i++)
    {
        *r = *r+x->ptr.p_double[i]*s->b.ptr.p_double[i];
        *noise = ae_maxreal(*noise, eps*ae_fabs(x->ptr.p_double[i]*s->b.ptr.p_double[i], _state), _state);
    }
    
    /*
     * Final update of the noise
     */
    *noise = n*(*noise);
}


/*************************************************************************
This  subroutine  evaluates  gradient of the model; active constraints are
ignored.

INPUT PARAMETERS:
    S       -   convex model
    X       -   point, array[N]
    G       -   possibly preallocated buffer; resized, if too small

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmgradunconstrained(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* g,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    double v;


    n = s->n;
    ae_assert(isfinitevector(x, n, _state), "CQMEvalGradUnconstrained: X is not finite vector", _state);
    rvectorsetlengthatleast(g, n, _state);
    for(i=0; i<=n-1; i++)
    {
        g->ptr.p_double[i] = 0;
    }
    
    /*
     * main quadratic term
     */
    if( ae_fp_greater(s->alpha,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            v = 0.0;
            for(j=0; j<=n-1; j++)
            {
                v = v+s->alpha*s->a.ptr.pp_double[i][j]*x->ptr.p_double[j];
            }
            g->ptr.p_double[i] = g->ptr.p_double[i]+v;
        }
    }
    if( ae_fp_greater(s->tau,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            g->ptr.p_double[i] = g->ptr.p_double[i]+x->ptr.p_double[i]*s->tau*s->d.ptr.p_double[i];
        }
    }
    
    /*
     * secondary quadratic term
     */
    if( ae_fp_greater(s->theta,0) )
    {
        for(i=0; i<=s->k-1; i++)
        {
            v = ae_v_dotproduct(&s->q.ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
            v = s->theta*(v-s->r.ptr.p_double[i]);
            ae_v_addd(&g->ptr.p_double[0], 1, &s->q.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
        }
    }
    
    /*
     * linear term
     */
    for(i=0; i<=n-1; i++)
    {
        g->ptr.p_double[i] = g->ptr.p_double[i]+s->b.ptr.p_double[i];
    }
}


/*************************************************************************
This subroutine evaluates x'*(0.5*alpha*A+tau*D)*x

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
double cqmxtadx2(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    double result;


    n = s->n;
    ae_assert(isfinitevector(x, n, _state), "CQMEval: X is not finite vector", _state);
    result = 0.0;
    
    /*
     * main quadratic term
     */
    if( ae_fp_greater(s->alpha,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                result = result+s->alpha*0.5*x->ptr.p_double[i]*s->a.ptr.pp_double[i][j]*x->ptr.p_double[j];
            }
        }
    }
    if( ae_fp_greater(s->tau,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            result = result+0.5*ae_sqr(x->ptr.p_double[i], _state)*s->tau*s->d.ptr.p_double[i];
        }
    }
    return result;
}


/*************************************************************************
This subroutine evaluates (0.5*alpha*A+tau*D)*x

Y is automatically resized if needed

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmadx(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double v;


    n = s->n;
    ae_assert(isfinitevector(x, n, _state), "CQMEval: X is not finite vector", _state);
    rvectorsetlengthatleast(y, n, _state);
    
    /*
     * main quadratic term
     */
    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = 0;
    }
    if( ae_fp_greater(s->alpha,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            v = ae_v_dotproduct(&s->a.ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->alpha*v;
        }
    }
    if( ae_fp_greater(s->tau,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+x->ptr.p_double[i]*s->tau*s->d.ptr.p_double[i];
        }
    }
}


/*************************************************************************
This subroutine finds optimum of the model. It returns  False  on  failure
(indefinite/semidefinite matrix).  Optimum  is  found  subject  to  active
constraints.

INPUT PARAMETERS
    S       -   model
    X       -   possibly preallocated buffer; automatically resized, if
                too small enough.

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool cqmconstrainedoptimum(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nfree;
    ae_int_t k;
    ae_int_t i;
    double v;
    ae_int_t cidx0;
    ae_int_t itidx;
    ae_bool result;


    
    /*
     * Rebuild internal structures
     */
    if( !cqmodels_cqmrebuild(s, _state) )
    {
        result = ae_false;
        return result;
    }
    n = s->n;
    k = s->k;
    nfree = s->nfree;
    result = ae_true;
    
    /*
     * Calculate initial point for the iterative refinement:
     * * free components are set to zero
     * * constrained components are set to their constrained values
     */
    rvectorsetlengthatleast(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( s->activeset.ptr.p_bool[i] )
        {
            x->ptr.p_double[i] = s->xc.ptr.p_double[i];
        }
        else
        {
            x->ptr.p_double[i] = 0;
        }
    }
    
    /*
     * Iterative refinement.
     *
     * In an ideal world without numerical errors it would be enough
     * to make just one Newton step from initial point:
     *   x_new = -H^(-1)*grad(x=0)
     * However, roundoff errors can significantly deteriorate quality
     * of the solution. So we have to recalculate gradient and to
     * perform Newton steps several times.
     *
     * Below we perform fixed number of Newton iterations.
     */
    for(itidx=0; itidx<=cqmodels_newtonrefinementits-1; itidx++)
    {
        
        /*
         * Calculate gradient at the current point.
         * Move free components of the gradient in the beginning.
         */
        cqmgradunconstrained(s, x, &s->tmpg, _state);
        cidx0 = 0;
        for(i=0; i<=n-1; i++)
        {
            if( !s->activeset.ptr.p_bool[i] )
            {
                s->tmpg.ptr.p_double[cidx0] = s->tmpg.ptr.p_double[i];
                cidx0 = cidx0+1;
            }
        }
        
        /*
         * Free components of the extrema are calculated in the first NFree elements of TXC.
         *
         * First, we have to calculate original Newton step, without rank-K perturbations
         */
        ae_v_moveneg(&s->txc.ptr.p_double[0], 1, &s->tmpg.ptr.p_double[0], 1, ae_v_len(0,nfree-1));
        cqmodels_cqmsolveea(s, &s->txc, &s->tmp0, _state);
        
        /*
         * Then, we account for rank-K correction.
         * Woodbury matrix identity is used.
         */
        if( s->k>0&&ae_fp_greater(s->theta,0) )
        {
            rvectorsetlengthatleast(&s->tmp0, ae_maxint(nfree, k, _state), _state);
            rvectorsetlengthatleast(&s->tmp1, ae_maxint(nfree, k, _state), _state);
            ae_v_moveneg(&s->tmp1.ptr.p_double[0], 1, &s->tmpg.ptr.p_double[0], 1, ae_v_len(0,nfree-1));
            cqmodels_cqmsolveea(s, &s->tmp1, &s->tmp0, _state);
            for(i=0; i<=k-1; i++)
            {
                v = ae_v_dotproduct(&s->eq.ptr.pp_double[i][0], 1, &s->tmp1.ptr.p_double[0], 1, ae_v_len(0,nfree-1));
                s->tmp0.ptr.p_double[i] = v;
            }
            fblscholeskysolve(&s->eccm, 1.0, k, ae_true, &s->tmp0, &s->tmp1, _state);
            for(i=0; i<=nfree-1; i++)
            {
                s->tmp1.ptr.p_double[i] = 0.0;
            }
            for(i=0; i<=k-1; i++)
            {
                v = s->tmp0.ptr.p_double[i];
                ae_v_addd(&s->tmp1.ptr.p_double[0], 1, &s->eq.ptr.pp_double[i][0], 1, ae_v_len(0,nfree-1), v);
            }
            cqmodels_cqmsolveea(s, &s->tmp1, &s->tmp0, _state);
            ae_v_sub(&s->txc.ptr.p_double[0], 1, &s->tmp1.ptr.p_double[0], 1, ae_v_len(0,nfree-1));
        }
        
        /*
         * Unpack components from TXC into X. We pass through all
         * free components of X and add our step.
         */
        cidx0 = 0;
        for(i=0; i<=n-1; i++)
        {
            if( !s->activeset.ptr.p_bool[i] )
            {
                x->ptr.p_double[i] = x->ptr.p_double[i]+s->txc.ptr.p_double[cidx0];
                cidx0 = cidx0+1;
            }
        }
    }
    return result;
}


/*************************************************************************
This function scales vector  by  multiplying it by inverse of the diagonal
of the Hessian matrix. It should be used to  accelerate  steepest  descent
phase of the QP solver.

Although  it  is  called  "scale-grad",  it  can be called for any vector,
whether it is gradient, anti-gradient, or just some vector.

This function does NOT takes into account current set of  constraints,  it
just performs matrix-vector multiplication  without  taking  into  account
constraints.

INPUT PARAMETERS:
    S       -   model
    X       -   vector to scale

OUTPUT PARAMETERS:
    X       -   scaled vector
    
NOTE:
    when called for non-SPD matrices, it silently skips components of X
    which correspond to zero or negative diagonal elements.
    
NOTE:
    this function uses diagonals of A and D; it ignores Q - rank-K term of
    the quadratic model.

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
void cqmscalevector(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double v;


    n = s->n;
    for(i=0; i<=n-1; i++)
    {
        v = 0.0;
        if( ae_fp_greater(s->alpha,0) )
        {
            v = v+s->a.ptr.pp_double[i][i];
        }
        if( ae_fp_greater(s->tau,0) )
        {
            v = v+s->d.ptr.p_double[i];
        }
        if( ae_fp_greater(v,0) )
        {
            x->ptr.p_double[i] = x->ptr.p_double[i]/v;
        }
    }
}


/*************************************************************************
This subroutine calls CQMRebuild() and evaluates model at X subject to
active constraints.

It  is  intended  for  debug  purposes only, because it evaluates model by
means of temporaries, which were calculated  by  CQMRebuild().  The   only
purpose of this function  is  to  check  correctness  of  CQMRebuild()  by
comparing results of this function with ones obtained by CQMEval(),  which
is  used  as  reference  point. The  idea is that significant deviation in
results  of  these  two  functions  is  evidence  of  some  error  in  the
CQMRebuild().

NOTE: suffix T denotes that temporaries marked by T-prefix are used. There
      is one more variant of this function, which uses  "effective"  model
      built by CQMRebuild().

NOTE2: in case CQMRebuild() fails (due to model non-convexity), this
      function returns NAN.

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
double cqmdebugconstrainedevalt(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nfree;
    ae_int_t i;
    ae_int_t j;
    double v;
    double result;


    n = s->n;
    ae_assert(isfinitevector(x, n, _state), "CQMDebugConstrainedEvalT: X is not finite vector", _state);
    if( !cqmodels_cqmrebuild(s, _state) )
    {
        result = _state->v_nan;
        return result;
    }
    result = 0.0;
    nfree = s->nfree;
    
    /*
     * Reorder variables
     */
    j = 0;
    for(i=0; i<=n-1; i++)
    {
        if( !s->activeset.ptr.p_bool[i] )
        {
            ae_assert(j<nfree, "CQMDebugConstrainedEvalT: internal error", _state);
            s->txc.ptr.p_double[j] = x->ptr.p_double[i];
            j = j+1;
        }
    }
    
    /*
     * TQ2, TQ1, TQ0
     *
     */
    if( ae_fp_greater(s->alpha,0) )
    {
        
        /*
         * Dense TQ2
         */
        for(i=0; i<=nfree-1; i++)
        {
            for(j=0; j<=nfree-1; j++)
            {
                result = result+0.5*s->txc.ptr.p_double[i]*s->tq2dense.ptr.pp_double[i][j]*s->txc.ptr.p_double[j];
            }
        }
    }
    else
    {
        
        /*
         * Diagonal TQ2
         */
        for(i=0; i<=nfree-1; i++)
        {
            result = result+0.5*s->tq2diag.ptr.p_double[i]*ae_sqr(s->txc.ptr.p_double[i], _state);
        }
    }
    for(i=0; i<=nfree-1; i++)
    {
        result = result+s->tq1.ptr.p_double[i]*s->txc.ptr.p_double[i];
    }
    result = result+s->tq0;
    
    /*
     * TK2, TK1, TK0
     */
    if( s->k>0&&ae_fp_greater(s->theta,0) )
    {
        for(i=0; i<=s->k-1; i++)
        {
            v = 0;
            for(j=0; j<=nfree-1; j++)
            {
                v = v+s->tk2.ptr.pp_double[i][j]*s->txc.ptr.p_double[j];
            }
            result = result+0.5*ae_sqr(v, _state);
        }
        for(i=0; i<=nfree-1; i++)
        {
            result = result+s->tk1.ptr.p_double[i]*s->txc.ptr.p_double[i];
        }
        result = result+s->tk0;
    }
    
    /*
     * TB (Bf and Bc parts)
     */
    for(i=0; i<=n-1; i++)
    {
        result = result+s->tb.ptr.p_double[i]*s->txc.ptr.p_double[i];
    }
    return result;
}


/*************************************************************************
This subroutine calls CQMRebuild() and evaluates model at X subject to
active constraints.

It  is  intended  for  debug  purposes only, because it evaluates model by
means of "effective" matrices built by CQMRebuild(). The only  purpose  of
this function is to check correctness of CQMRebuild() by comparing results
of this function with  ones  obtained  by  CQMEval(),  which  is  used  as
reference  point.  The  idea  is  that significant deviation in results of
these two functions is evidence of some error in the CQMRebuild().

NOTE: suffix E denotes that effective matrices. There is one more  variant
      of this function, which uses temporary matrices built by
      CQMRebuild().

NOTE2: in case CQMRebuild() fails (due to model non-convexity), this
      function returns NAN.

  -- ALGLIB --
     Copyright 12.06.2012 by Bochkanov Sergey
*************************************************************************/
double cqmdebugconstrainedevale(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nfree;
    ae_int_t i;
    ae_int_t j;
    double v;
    double result;


    n = s->n;
    ae_assert(isfinitevector(x, n, _state), "CQMDebugConstrainedEvalE: X is not finite vector", _state);
    if( !cqmodels_cqmrebuild(s, _state) )
    {
        result = _state->v_nan;
        return result;
    }
    result = 0.0;
    nfree = s->nfree;
    
    /*
     * Reorder variables
     */
    j = 0;
    for(i=0; i<=n-1; i++)
    {
        if( !s->activeset.ptr.p_bool[i] )
        {
            ae_assert(j<nfree, "CQMDebugConstrainedEvalE: internal error", _state);
            s->txc.ptr.p_double[j] = x->ptr.p_double[i];
            j = j+1;
        }
    }
    
    /*
     * ECA
     */
    ae_assert((s->ecakind==0||s->ecakind==1)||(s->ecakind==-1&&nfree==0), "CQMDebugConstrainedEvalE: unexpected ECAKind", _state);
    if( s->ecakind==0 )
    {
        
        /*
         * Dense ECA
         */
        for(i=0; i<=nfree-1; i++)
        {
            v = 0.0;
            for(j=i; j<=nfree-1; j++)
            {
                v = v+s->ecadense.ptr.pp_double[i][j]*s->txc.ptr.p_double[j];
            }
            result = result+0.5*ae_sqr(v, _state);
        }
    }
    if( s->ecakind==1 )
    {
        
        /*
         * Diagonal ECA
         */
        for(i=0; i<=nfree-1; i++)
        {
            result = result+0.5*ae_sqr(s->ecadiag.ptr.p_double[i]*s->txc.ptr.p_double[i], _state);
        }
    }
    
    /*
     * EQ
     */
    for(i=0; i<=s->k-1; i++)
    {
        v = 0.0;
        for(j=0; j<=nfree-1; j++)
        {
            v = v+s->eq.ptr.pp_double[i][j]*s->txc.ptr.p_double[j];
        }
        result = result+0.5*ae_sqr(v, _state);
    }
    
    /*
     * EB
     */
    for(i=0; i<=nfree-1; i++)
    {
        result = result+s->eb.ptr.p_double[i]*s->txc.ptr.p_double[i];
    }
    
    /*
     * EC
     */
    result = result+s->ec;
    return result;
}


/*************************************************************************
Internal function, rebuilds "effective" model subject to constraints.
Returns False on failure (non-SPD main quadratic term)

  -- ALGLIB --
     Copyright 10.05.2011 by Bochkanov Sergey
*************************************************************************/
static ae_bool cqmodels_cqmrebuild(convexquadraticmodel* s,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nfree;
    ae_int_t k;
    ae_int_t i;
    ae_int_t j;
    ae_int_t ridx0;
    ae_int_t ridx1;
    ae_int_t cidx0;
    ae_int_t cidx1;
    double v;
    ae_bool result;


    if( ae_fp_eq(s->alpha,0)&&ae_fp_eq(s->tau,0) )
    {
        
        /*
         * Non-SPD model, quick exit
         */
        result = ae_false;
        return result;
    }
    result = ae_true;
    n = s->n;
    k = s->k;
    
    /*
     * Determine number of free variables.
     * Fill TXC - array whose last N-NFree elements store constraints.
     */
    if( s->isactivesetchanged )
    {
        s->nfree = 0;
        for(i=0; i<=n-1; i++)
        {
            if( !s->activeset.ptr.p_bool[i] )
            {
                s->nfree = s->nfree+1;
            }
        }
        j = s->nfree;
        for(i=0; i<=n-1; i++)
        {
            if( s->activeset.ptr.p_bool[i] )
            {
                s->txc.ptr.p_double[j] = s->xc.ptr.p_double[i];
                j = j+1;
            }
        }
    }
    nfree = s->nfree;
    
    /*
     * Re-evaluate TQ2/TQ1/TQ0, if needed
     */
    if( s->isactivesetchanged||s->ismaintermchanged )
    {
        
        /*
         * Handle cases Alpha>0 and Alpha=0 separately:
         * * in the first case we have dense matrix
         * * in the second one we have diagonal matrix, which can be
         *   handled more efficiently
         */
        if( ae_fp_greater(s->alpha,0) )
        {
            
            /*
             * Alpha>0, dense QP
             *
             * Split variables into two groups - free (F) and constrained (C). Reorder
             * variables in such way that free vars come first, constrained are last:
             * x = [xf, xc].
             * 
             * Main quadratic term x'*(alpha*A+tau*D)*x now splits into quadratic part,
             * linear part and constant part:
             *                   ( alpha*Aff+tau*Df  alpha*Afc        ) ( xf )              
             *   0.5*( xf' xc' )*(                                    )*(    ) =
             *                   ( alpha*Acf         alpha*Acc+tau*Dc ) ( xc )
             *
             *   = 0.5*xf'*(alpha*Aff+tau*Df)*xf + (alpha*Afc*xc)'*xf + 0.5*xc'(alpha*Acc+tau*Dc)*xc
             *                    
             * We store these parts into temporary variables:
             * * alpha*Aff+tau*Df, alpha*Afc, alpha*Acc+tau*Dc are stored into upper
             *   triangle of TQ2
             * * alpha*Afc*xc is stored into TQ1
             * * 0.5*xc'(alpha*Acc+tau*Dc)*xc is stored into TQ0
             *
             * Below comes first part of the work - generation of TQ2:
             * * we pass through rows of A and copy I-th row into upper block (Aff/Afc) or
             *   lower one (Acf/Acc) of TQ2, depending on presence of X[i] in the active set.
             *   RIdx0 variable contains current position for insertion into upper block,
             *   RIdx1 contains current position for insertion into lower one.
             * * within each row, we copy J-th element into left half (Aff/Acf) or right
             *   one (Afc/Acc), depending on presence of X[j] in the active set. CIdx0
             *   contains current position for insertion into left block, CIdx1 contains
             *   position for insertion into right one.
             * * during copying, we multiply elements by alpha and add diagonal matrix D.
             */
            ridx0 = 0;
            ridx1 = s->nfree;
            for(i=0; i<=n-1; i++)
            {
                cidx0 = 0;
                cidx1 = s->nfree;
                for(j=0; j<=n-1; j++)
                {
                    if( !s->activeset.ptr.p_bool[i]&&!s->activeset.ptr.p_bool[j] )
                    {
                        
                        /*
                         * Element belongs to Aff
                         */
                        v = s->alpha*s->a.ptr.pp_double[i][j];
                        if( i==j&&ae_fp_greater(s->tau,0) )
                        {
                            v = v+s->tau*s->d.ptr.p_double[i];
                        }
                        s->tq2dense.ptr.pp_double[ridx0][cidx0] = v;
                    }
                    if( !s->activeset.ptr.p_bool[i]&&s->activeset.ptr.p_bool[j] )
                    {
                        
                        /*
                         * Element belongs to Afc
                         */
                        s->tq2dense.ptr.pp_double[ridx0][cidx1] = s->alpha*s->a.ptr.pp_double[i][j];
                    }
                    if( s->activeset.ptr.p_bool[i]&&!s->activeset.ptr.p_bool[j] )
                    {
                        
                        /*
                         * Element belongs to Acf
                         */
                        s->tq2dense.ptr.pp_double[ridx1][cidx0] = s->alpha*s->a.ptr.pp_double[i][j];
                    }
                    if( s->activeset.ptr.p_bool[i]&&s->activeset.ptr.p_bool[j] )
                    {
                        
                        /*
                         * Element belongs to Acc
                         */
                        v = s->alpha*s->a.ptr.pp_double[i][j];
                        if( i==j&&ae_fp_greater(s->tau,0) )
                        {
                            v = v+s->tau*s->d.ptr.p_double[i];
                        }
                        s->tq2dense.ptr.pp_double[ridx1][cidx1] = v;
                    }
                    if( s->activeset.ptr.p_bool[j] )
                    {
                        cidx1 = cidx1+1;
                    }
                    else
                    {
                        cidx0 = cidx0+1;
                    }
                }
                if( s->activeset.ptr.p_bool[i] )
                {
                    ridx1 = ridx1+1;
                }
                else
                {
                    ridx0 = ridx0+1;
                }
            }
            
            /*
             * Now we have TQ2, and we can evaluate TQ1.
             * In the special case when we have Alpha=0, NFree=0 or NFree=N,
             * TQ1 is filled by zeros.
             */
            for(i=0; i<=n-1; i++)
            {
                s->tq1.ptr.p_double[i] = 0.0;
            }
            if( s->nfree>0&&s->nfree<n )
            {
                rmatrixmv(s->nfree, n-s->nfree, &s->tq2dense, 0, s->nfree, 0, &s->txc, s->nfree, &s->tq1, 0, _state);
            }
            
            /*
             * And finally, we evaluate TQ0.
             */
            v = 0.0;
            for(i=s->nfree; i<=n-1; i++)
            {
                for(j=s->nfree; j<=n-1; j++)
                {
                    v = v+0.5*s->txc.ptr.p_double[i]*s->tq2dense.ptr.pp_double[i][j]*s->txc.ptr.p_double[j];
                }
            }
            s->tq0 = v;
        }
        else
        {
            
            /*
             * Alpha=0, diagonal QP
             *
             * Split variables into two groups - free (F) and constrained (C). Reorder
             * variables in such way that free vars come first, constrained are last:
             * x = [xf, xc].
             * 
             * Main quadratic term x'*(tau*D)*x now splits into quadratic and constant
             * parts:
             *                   ( tau*Df        ) ( xf )              
             *   0.5*( xf' xc' )*(               )*(    ) =
             *                   (        tau*Dc ) ( xc )
             *
             *   = 0.5*xf'*(tau*Df)*xf + 0.5*xc'(tau*Dc)*xc
             *                    
             * We store these parts into temporary variables:
             * * tau*Df is stored in TQ2Diag
             * * 0.5*xc'(tau*Dc)*xc is stored into TQ0
             */
            s->tq0 = 0.0;
            ridx0 = 0;
            for(i=0; i<=n-1; i++)
            {
                if( !s->activeset.ptr.p_bool[i] )
                {
                    s->tq2diag.ptr.p_double[ridx0] = s->tau*s->d.ptr.p_double[i];
                    ridx0 = ridx0+1;
                }
                else
                {
                    s->tq0 = s->tq0+0.5*s->tau*s->d.ptr.p_double[i]*ae_sqr(s->xc.ptr.p_double[i], _state);
                }
            }
            for(i=0; i<=n-1; i++)
            {
                s->tq1.ptr.p_double[i] = 0.0;
            }
        }
    }
    
    /*
     * Re-evaluate TK2/TK1/TK0, if needed
     */
    if( s->isactivesetchanged||s->issecondarytermchanged )
    {
        
        /*
         * Split variables into two groups - free (F) and constrained (C). Reorder
         * variables in such way that free vars come first, constrained are last:
         * x = [xf, xc].
         * 
         * Secondary term theta*(Q*x-r)'*(Q*x-r) now splits into quadratic part,
         * linear part and constant part:
         *             (          ( xf )     )'  (          ( xf )     )
         *   0.5*theta*( (Qf Qc)'*(    ) - r ) * ( (Qf Qc)'*(    ) - r ) =
         *             (          ( xc )     )   (          ( xc )     )
         *
         *   = 0.5*theta*xf'*(Qf'*Qf)*xf + theta*((Qc*xc-r)'*Qf)*xf + 
         *     + theta*(-r'*(Qc*xc-r)-0.5*r'*r+0.5*xc'*Qc'*Qc*xc)
         *                    
         * We store these parts into temporary variables:
         * * sqrt(theta)*Qf is stored into TK2
         * * theta*((Qc*xc-r)'*Qf) is stored into TK1
         * * theta*(-r'*(Qc*xc-r)-0.5*r'*r+0.5*xc'*Qc'*Qc*xc) is stored into TK0
         *
         * We use several other temporaries to store intermediate results:
         * * Tmp0 - to store Qc*xc-r
         * * Tmp1 - to store Qc*xc
         *
         * Generation of TK2/TK1/TK0 is performed as follows:
         * * we fill TK2/TK1/TK0 (to handle K=0 or Theta=0)
         * * other steps are performed only for K>0 and Theta>0
         * * we pass through columns of Q and copy I-th column into left block (Qf) or
         *   right one (Qc) of TK2, depending on presence of X[i] in the active set.
         *   CIdx0 variable contains current position for insertion into upper block,
         *   CIdx1 contains current position for insertion into lower one.
         * * we calculate Qc*xc-r and store it into Tmp0
         * * we calculate TK0 and TK1
         * * we multiply leading part of TK2 which stores Qf by sqrt(theta)
         *   it is important to perform this step AFTER calculation of TK0 and TK1,
         *   because we need original (non-modified) Qf to calculate TK0 and TK1.
         */
        for(j=0; j<=n-1; j++)
        {
            for(i=0; i<=k-1; i++)
            {
                s->tk2.ptr.pp_double[i][j] = 0.0;
            }
            s->tk1.ptr.p_double[j] = 0.0;
        }
        s->tk0 = 0.0;
        if( s->k>0&&ae_fp_greater(s->theta,0) )
        {
            
            /*
             * Split Q into Qf and Qc
             * Calculate Qc*xc-r, store in Tmp0
             */
            rvectorsetlengthatleast(&s->tmp0, k, _state);
            rvectorsetlengthatleast(&s->tmp1, k, _state);
            cidx0 = 0;
            cidx1 = nfree;
            for(i=0; i<=k-1; i++)
            {
                s->tmp1.ptr.p_double[i] = 0.0;
            }
            for(j=0; j<=n-1; j++)
            {
                if( s->activeset.ptr.p_bool[j] )
                {
                    for(i=0; i<=k-1; i++)
                    {
                        s->tk2.ptr.pp_double[i][cidx1] = s->q.ptr.pp_double[i][j];
                        s->tmp1.ptr.p_double[i] = s->tmp1.ptr.p_double[i]+s->q.ptr.pp_double[i][j]*s->txc.ptr.p_double[cidx1];
                    }
                    cidx1 = cidx1+1;
                }
                else
                {
                    for(i=0; i<=k-1; i++)
                    {
                        s->tk2.ptr.pp_double[i][cidx0] = s->q.ptr.pp_double[i][j];
                    }
                    cidx0 = cidx0+1;
                }
            }
            for(i=0; i<=k-1; i++)
            {
                s->tmp0.ptr.p_double[i] = s->tmp1.ptr.p_double[i]-s->r.ptr.p_double[i];
            }
            
            /*
             * Calculate TK0
             */
            v = 0.0;
            for(i=0; i<=k-1; i++)
            {
                v = v+s->theta*(0.5*ae_sqr(s->tmp1.ptr.p_double[i], _state)-s->r.ptr.p_double[i]*s->tmp0.ptr.p_double[i]-0.5*ae_sqr(s->r.ptr.p_double[i], _state));
            }
            s->tk0 = v;
            
            /*
             * Calculate TK1
             */
            if( nfree>0 )
            {
                for(i=0; i<=k-1; i++)
                {
                    v = s->theta*s->tmp0.ptr.p_double[i];
                    ae_v_addd(&s->tk1.ptr.p_double[0], 1, &s->tk2.ptr.pp_double[i][0], 1, ae_v_len(0,nfree-1), v);
                }
            }
            
            /*
             * Calculate TK2
             */
            if( nfree>0 )
            {
                v = ae_sqrt(s->theta, _state);
                for(i=0; i<=k-1; i++)
                {
                    ae_v_muld(&s->tk2.ptr.pp_double[i][0], 1, ae_v_len(0,nfree-1), v);
                }
            }
        }
    }
    
    /*
     * Re-evaluate TB
     */
    if( s->isactivesetchanged||s->islineartermchanged )
    {
        ridx0 = 0;
        ridx1 = nfree;
        for(i=0; i<=n-1; i++)
        {
            if( s->activeset.ptr.p_bool[i] )
            {
                s->tb.ptr.p_double[ridx1] = s->b.ptr.p_double[i];
                ridx1 = ridx1+1;
            }
            else
            {
                s->tb.ptr.p_double[ridx0] = s->b.ptr.p_double[i];
                ridx0 = ridx0+1;
            }
        }
    }
    
    /*
     * Compose ECA: either dense ECA or diagonal ECA
     */
    if( (s->isactivesetchanged||s->ismaintermchanged)&&nfree>0 )
    {
        if( ae_fp_greater(s->alpha,0) )
        {
            
            /*
             * Dense ECA
             */
            s->ecakind = 0;
            for(i=0; i<=nfree-1; i++)
            {
                for(j=i; j<=nfree-1; j++)
                {
                    s->ecadense.ptr.pp_double[i][j] = s->tq2dense.ptr.pp_double[i][j];
                }
            }
            if( !spdmatrixcholeskyrec(&s->ecadense, 0, nfree, ae_true, &s->tmp0, _state) )
            {
                result = ae_false;
                return result;
            }
        }
        else
        {
            
            /*
             * Diagonal ECA
             */
            s->ecakind = 1;
            for(i=0; i<=nfree-1; i++)
            {
                if( ae_fp_less(s->tq2diag.ptr.p_double[i],0) )
                {
                    result = ae_false;
                    return result;
                }
                s->ecadiag.ptr.p_double[i] = ae_sqrt(s->tq2diag.ptr.p_double[i], _state);
            }
        }
    }
    
    /*
     * Compose EQ
     */
    if( s->isactivesetchanged||s->issecondarytermchanged )
    {
        for(i=0; i<=k-1; i++)
        {
            for(j=0; j<=nfree-1; j++)
            {
                s->eq.ptr.pp_double[i][j] = s->tk2.ptr.pp_double[i][j];
            }
        }
    }
    
    /*
     * Calculate ECCM
     */
    if( ((((s->isactivesetchanged||s->ismaintermchanged)||s->issecondarytermchanged)&&s->k>0)&&ae_fp_greater(s->theta,0))&&nfree>0 )
    {
        
        /*
         * Calculate ECCM - Cholesky factor of the "effective" capacitance
         * matrix CM = I + EQ*inv(EffectiveA)*EQ'.
         *
         * We calculate CM as follows:
         *   CM = I + EQ*inv(EffectiveA)*EQ'
         *      = I + EQ*ECA^(-1)*ECA^(-T)*EQ'
         *      = I + (EQ*ECA^(-1))*(EQ*ECA^(-1))'
         *
         * Then we perform Cholesky decomposition of CM.
         */
        rmatrixsetlengthatleast(&s->tmp2, k, n, _state);
        rmatrixcopy(k, nfree, &s->eq, 0, 0, &s->tmp2, 0, 0, _state);
        ae_assert(s->ecakind==0||s->ecakind==1, "CQMRebuild: unexpected ECAKind", _state);
        if( s->ecakind==0 )
        {
            rmatrixrighttrsm(k, nfree, &s->ecadense, 0, 0, ae_true, ae_false, 0, &s->tmp2, 0, 0, _state);
        }
        if( s->ecakind==1 )
        {
            for(i=0; i<=k-1; i++)
            {
                for(j=0; j<=nfree-1; j++)
                {
                    s->tmp2.ptr.pp_double[i][j] = s->tmp2.ptr.pp_double[i][j]/s->ecadiag.ptr.p_double[j];
                }
            }
        }
        for(i=0; i<=k-1; i++)
        {
            for(j=0; j<=k-1; j++)
            {
                s->eccm.ptr.pp_double[i][j] = 0.0;
            }
            s->eccm.ptr.pp_double[i][i] = 1.0;
        }
        rmatrixsyrk(k, nfree, 1.0, &s->tmp2, 0, 0, 0, 1.0, &s->eccm, 0, 0, ae_true, _state);
        if( !spdmatrixcholeskyrec(&s->eccm, 0, k, ae_true, &s->tmp0, _state) )
        {
            result = ae_false;
            return result;
        }
    }
    
    /*
     * Compose EB and EC
     *
     * NOTE: because these quantities are cheap to compute, we do not
     * use caching here.
     */
    for(i=0; i<=nfree-1; i++)
    {
        s->eb.ptr.p_double[i] = s->tq1.ptr.p_double[i]+s->tk1.ptr.p_double[i]+s->tb.ptr.p_double[i];
    }
    s->ec = s->tq0+s->tk0;
    for(i=nfree; i<=n-1; i++)
    {
        s->ec = s->ec+s->tb.ptr.p_double[i]*s->txc.ptr.p_double[i];
    }
    
    /*
     * Change cache status - everything is cached 
     */
    s->ismaintermchanged = ae_false;
    s->issecondarytermchanged = ae_false;
    s->islineartermchanged = ae_false;
    s->isactivesetchanged = ae_false;
    return result;
}


/*************************************************************************
Internal function, solves system Effective_A*x = b.
It should be called after successful completion of CQMRebuild().

INPUT PARAMETERS:
    S       -   quadratic model, after call to CQMRebuild()
    X       -   right part B, array[S.NFree]
    Tmp     -   temporary array, automatically reallocated if needed

OUTPUT PARAMETERS:
    X       -   solution, array[S.NFree]
    
NOTE: when called with zero S.NFree, returns silently
NOTE: this function assumes that EA is non-degenerate

  -- ALGLIB --
     Copyright 10.05.2011 by Bochkanov Sergey
*************************************************************************/
static void cqmodels_cqmsolveea(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert((s->ecakind==0||s->ecakind==1)||(s->ecakind==-1&&s->nfree==0), "CQMSolveEA: unexpected ECAKind", _state);
    if( s->ecakind==0 )
    {
        
        /*
         * Dense ECA, use FBLSCholeskySolve() dense solver.
         */
        fblscholeskysolve(&s->ecadense, 1.0, s->nfree, ae_true, x, tmp, _state);
    }
    if( s->ecakind==1 )
    {
        
        /*
         * Diagonal ECA
         */
        for(i=0; i<=s->nfree-1; i++)
        {
            x->ptr.p_double[i] = x->ptr.p_double[i]/ae_sqr(s->ecadiag.ptr.p_double[i], _state);
        }
    }
}


ae_bool _convexquadraticmodel_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    convexquadraticmodel *p = (convexquadraticmodel*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_matrix_init(&p->a, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->q, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->r, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xc, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->activeset, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->tq2dense, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->tk2, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tq2diag, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tq1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tk1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->txc, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tb, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->ecadense, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->eq, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->eccm, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->ecadiag, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->eb, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmpg, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->tmp2, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _convexquadraticmodel_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    convexquadraticmodel *dst = (convexquadraticmodel*)_dst;
    convexquadraticmodel *src = (convexquadraticmodel*)_src;
    dst->n = src->n;
    dst->k = src->k;
    dst->alpha = src->alpha;
    dst->tau = src->tau;
    dst->theta = src->theta;
    if( !ae_matrix_init_copy(&dst->a, &src->a, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->q, &src->q, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->r, &src->r, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->xc, &src->xc, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->activeset, &src->activeset, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->tq2dense, &src->tq2dense, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->tk2, &src->tk2, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tq2diag, &src->tq2diag, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tq1, &src->tq1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tk1, &src->tk1, _state, make_automatic) )
        return ae_false;
    dst->tq0 = src->tq0;
    dst->tk0 = src->tk0;
    if( !ae_vector_init_copy(&dst->txc, &src->txc, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tb, &src->tb, _state, make_automatic) )
        return ae_false;
    dst->nfree = src->nfree;
    dst->ecakind = src->ecakind;
    if( !ae_matrix_init_copy(&dst->ecadense, &src->ecadense, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->eq, &src->eq, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->eccm, &src->eccm, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->ecadiag, &src->ecadiag, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->eb, &src->eb, _state, make_automatic) )
        return ae_false;
    dst->ec = src->ec;
    if( !ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmpg, &src->tmpg, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic) )
        return ae_false;
    dst->ismaintermchanged = src->ismaintermchanged;
    dst->issecondarytermchanged = src->issecondarytermchanged;
    dst->islineartermchanged = src->islineartermchanged;
    dst->isactivesetchanged = src->isactivesetchanged;
    return ae_true;
}


void _convexquadraticmodel_clear(void* _p)
{
    convexquadraticmodel *p = (convexquadraticmodel*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->a);
    ae_matrix_clear(&p->q);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->r);
    ae_vector_clear(&p->xc);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->activeset);
    ae_matrix_clear(&p->tq2dense);
    ae_matrix_clear(&p->tk2);
    ae_vector_clear(&p->tq2diag);
    ae_vector_clear(&p->tq1);
    ae_vector_clear(&p->tk1);
    ae_vector_clear(&p->txc);
    ae_vector_clear(&p->tb);
    ae_matrix_clear(&p->ecadense);
    ae_matrix_clear(&p->eq);
    ae_matrix_clear(&p->eccm);
    ae_vector_clear(&p->ecadiag);
    ae_vector_clear(&p->eb);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmpg);
    ae_matrix_clear(&p->tmp2);
}


void _convexquadraticmodel_destroy(void* _p)
{
    convexquadraticmodel *p = (convexquadraticmodel*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->a);
    ae_matrix_destroy(&p->q);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->r);
    ae_vector_destroy(&p->xc);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->activeset);
    ae_matrix_destroy(&p->tq2dense);
    ae_matrix_destroy(&p->tk2);
    ae_vector_destroy(&p->tq2diag);
    ae_vector_destroy(&p->tq1);
    ae_vector_destroy(&p->tk1);
    ae_vector_destroy(&p->txc);
    ae_vector_destroy(&p->tb);
    ae_matrix_destroy(&p->ecadense);
    ae_matrix_destroy(&p->eq);
    ae_matrix_destroy(&p->eccm);
    ae_vector_destroy(&p->ecadiag);
    ae_vector_destroy(&p->eb);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmpg);
    ae_matrix_destroy(&p->tmp2);
}




/*************************************************************************
This subroutine is used to initialize SNNLS solver.

By default, empty NNLS problem is produced, but we allocated enough  space
to store problems with NSMax+NDMax columns and  NRMax  rows.  It  is  good
place to provide algorithm with initial estimate of the space requirements,
although you may underestimate problem size or even pass zero estimates  -
in this case buffer variables will be resized automatically  when  you set
NNLS problem.

Previously allocated buffer variables are reused as much as possible. This
function does not clear structure completely, it tries to preserve as much
dynamically allocated memory as possible.

  -- ALGLIB --
     Copyright 10.10.2012 by Bochkanov Sergey
*************************************************************************/
void snnlsinit(ae_int_t nsmax,
     ae_int_t ndmax,
     ae_int_t nrmax,
     snnlssolver* s,
     ae_state *_state)
{


    s->ns = 0;
    s->nd = 0;
    s->nr = 0;
    rmatrixsetlengthatleast(&s->densea, nrmax, ndmax, _state);
    rmatrixsetlengthatleast(&s->tmpca, nrmax, ndmax, _state);
    rmatrixsetlengthatleast(&s->tmpz, ndmax, ndmax, _state);
    rvectorsetlengthatleast(&s->b, nrmax, _state);
    bvectorsetlengthatleast(&s->nnc, nsmax+ndmax, _state);
    s->debugflops = 0.0;
    s->debugmaxnewton = 0;
    s->refinementits = snnls_iterativerefinementits;
}


/*************************************************************************
This subroutine is used to set NNLS problem:

        ( [ 1     |      ]   [   ]   [   ] )^2
        ( [   1   |      ]   [   ]   [   ] )
    min ( [     1 |  Ad  ] * [ x ] - [ b ] )    s.t. x>=0
        ( [       |      ]   [   ]   [   ] )
        ( [       |      ]   [   ]   [   ] )

where:
* identity matrix has NS*NS size (NS<=NR, NS can be zero)
* dense matrix Ad has NR*ND size
* b is NR*1 vector
* x is (NS+ND)*1 vector
* all elements of x are non-negative (this constraint can be removed later
  by calling SNNLSDropNNC() function)

Previously allocated buffer variables are reused as much as possible.
After you set problem, you can solve it with SNNLSSolve().

INPUT PARAMETERS:
    S   -   SNNLS solver, must be initialized with SNNLSInit() call
    A   -   array[NR,ND], dense part of the system
    B   -   array[NR], right part
    NS  -   size of the sparse part of the system, 0<=NS<=NR
    ND  -   size of the dense part of the system, ND>=0
    NR  -   rows count, NR>0

NOTE:
    1. You can have NS+ND=0, solver will correctly accept such combination
       and return empty array as problem solution.
    
  -- ALGLIB --
     Copyright 10.10.2012 by Bochkanov Sergey
*************************************************************************/
void snnlssetproblem(snnlssolver* s,
     /* Real    */ ae_matrix* a,
     /* Real    */ ae_vector* b,
     ae_int_t ns,
     ae_int_t nd,
     ae_int_t nr,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(nd>=0, "SNNLSSetProblem: ND<0", _state);
    ae_assert(ns>=0, "SNNLSSetProblem: NS<0", _state);
    ae_assert(nr>0, "SNNLSSetProblem: NR<=0", _state);
    ae_assert(ns<=nr, "SNNLSSetProblem: NS>NR", _state);
    ae_assert(a->rows>=nr||nd==0, "SNNLSSetProblem: rows(A)<NR", _state);
    ae_assert(a->cols>=nd, "SNNLSSetProblem: cols(A)<ND", _state);
    ae_assert(b->cnt>=nr, "SNNLSSetProblem: length(B)<NR", _state);
    ae_assert(apservisfinitematrix(a, nr, nd, _state), "SNNLSSetProblem: A contains INF/NAN", _state);
    ae_assert(isfinitevector(b, nr, _state), "SNNLSSetProblem: B contains INF/NAN", _state);
    
    /*
     * Copy problem
     */
    s->ns = ns;
    s->nd = nd;
    s->nr = nr;
    if( nd>0 )
    {
        rmatrixsetlengthatleast(&s->densea, nr, nd, _state);
        for(i=0; i<=nr-1; i++)
        {
            ae_v_move(&s->densea.ptr.pp_double[i][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,nd-1));
        }
    }
    rvectorsetlengthatleast(&s->b, nr, _state);
    ae_v_move(&s->b.ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,nr-1));
    bvectorsetlengthatleast(&s->nnc, ns+nd, _state);
    for(i=0; i<=ns+nd-1; i++)
    {
        s->nnc.ptr.p_bool[i] = ae_true;
    }
}


/*************************************************************************
This subroutine drops non-negativity constraint from the  problem  set  by
SNNLSSetProblem() call. This function must be called AFTER problem is set,
because each SetProblem() call resets constraints to their  default  state
(all constraints are present).

INPUT PARAMETERS:
    S   -   SNNLS solver, must be initialized with SNNLSInit() call,
            problem must be set with SNNLSSetProblem() call.
    Idx -   constraint index, 0<=IDX<NS+ND
    
  -- ALGLIB --
     Copyright 10.10.2012 by Bochkanov Sergey
*************************************************************************/
void snnlsdropnnc(snnlssolver* s, ae_int_t idx, ae_state *_state)
{


    ae_assert(idx>=0, "SNNLSDropNNC: Idx<0", _state);
    ae_assert(idx<s->ns+s->nd, "SNNLSDropNNC: Idx>=NS+ND", _state);
    s->nnc.ptr.p_bool[idx] = ae_false;
}


/*************************************************************************
This subroutine is used to solve NNLS problem.

INPUT PARAMETERS:
    S   -   SNNLS solver, must be initialized with SNNLSInit() call and
            problem must be set up with SNNLSSetProblem() call.
    X   -   possibly preallocated buffer, automatically resized if needed

OUTPUT PARAMETERS:
    X   -   array[NS+ND], solution
    
NOTE:
    1. You can have NS+ND=0, solver will correctly accept such combination
       and return empty array as problem solution.
    
    2. Internal field S.DebugFLOPS contains rough estimate of  FLOPs  used
       to solve problem. It can be used for debugging purposes. This field
       is real-valued.
    
  -- ALGLIB --
     Copyright 10.10.2012 by Bochkanov Sergey
*************************************************************************/
void snnlssolve(snnlssolver* s,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t ns;
    ae_int_t nd;
    ae_int_t nr;
    ae_int_t nsc;
    ae_int_t ndc;
    ae_int_t newtoncnt;
    ae_bool terminationneeded;
    double eps;
    double fcur;
    double fprev;
    double fcand;
    double noiselevel;
    double noisetolerance;
    double stplen;
    double d2;
    double d1;
    double d0;
    ae_bool wasactivation;
    ae_int_t rfsits;
    double lambdav;
    double v0;
    double v1;
    double v;


    
    /*
     * Prepare
     */
    ns = s->ns;
    nd = s->nd;
    nr = s->nr;
    s->debugflops = 0.0;
    
    /*
     * Handle special cases:
     * * NS+ND=0
     * * ND=0
     */
    if( ns+nd==0 )
    {
        return;
    }
    if( nd==0 )
    {
        rvectorsetlengthatleast(x, ns, _state);
        for(i=0; i<=ns-1; i++)
        {
            x->ptr.p_double[i] = s->b.ptr.p_double[i];
            if( s->nnc.ptr.p_bool[i] )
            {
                x->ptr.p_double[i] = ae_maxreal(x->ptr.p_double[i], 0.0, _state);
            }
        }
        return;
    }
    
    /*
     * Main cycle of BLEIC-SNNLS algorithm.
     * Below we assume that ND>0.
     */
    rvectorsetlengthatleast(x, ns+nd, _state);
    rvectorsetlengthatleast(&s->xn, ns+nd, _state);
    rvectorsetlengthatleast(&s->g, ns+nd, _state);
    rvectorsetlengthatleast(&s->d, ns+nd, _state);
    rvectorsetlengthatleast(&s->r, nr, _state);
    rvectorsetlengthatleast(&s->diagaa, nd, _state);
    rvectorsetlengthatleast(&s->dx, ns+nd, _state);
    for(i=0; i<=ns+nd-1; i++)
    {
        x->ptr.p_double[i] = 0.0;
    }
    eps = 2*ae_machineepsilon;
    noisetolerance = 10.0;
    lambdav = 1.0E6*ae_machineepsilon;
    newtoncnt = 0;
    for(;;)
    {
        
        /*
         * Phase 1: perform steepest descent step.
         *
         * TerminationNeeded control variable is set on exit from this loop:
         * * TerminationNeeded=False in case we have to proceed to Phase 2 (Newton step)
         * * TerminationNeeded=True in case we found solution (step along projected gradient is small enough)
         *
         * Temporaries used:
         * * R      (I|A)*x-b
         *
         * NOTE 1. It is assumed that initial point X is feasible. This feasibility
         *         is retained during all iterations.
         */
        terminationneeded = ae_false;
        for(;;)
        {
            
            /*
             * Calculate gradient G and constrained descent direction D
             */
            for(i=0; i<=nr-1; i++)
            {
                v = ae_v_dotproduct(&s->densea.ptr.pp_double[i][0], 1, &x->ptr.p_double[ns], 1, ae_v_len(0,nd-1));
                if( i<ns )
                {
                    v = v+x->ptr.p_double[i];
                }
                s->r.ptr.p_double[i] = v-s->b.ptr.p_double[i];
            }
            for(i=0; i<=ns-1; i++)
            {
                s->g.ptr.p_double[i] = s->r.ptr.p_double[i];
            }
            for(i=ns; i<=ns+nd-1; i++)
            {
                s->g.ptr.p_double[i] = 0.0;
            }
            for(i=0; i<=nr-1; i++)
            {
                v = s->r.ptr.p_double[i];
                ae_v_addd(&s->g.ptr.p_double[ns], 1, &s->densea.ptr.pp_double[i][0], 1, ae_v_len(ns,ns+nd-1), v);
            }
            for(i=0; i<=ns+nd-1; i++)
            {
                if( (s->nnc.ptr.p_bool[i]&&ae_fp_less_eq(x->ptr.p_double[i],0))&&ae_fp_greater(s->g.ptr.p_double[i],0) )
                {
                    s->d.ptr.p_double[i] = 0.0;
                }
                else
                {
                    s->d.ptr.p_double[i] = -s->g.ptr.p_double[i];
                }
            }
            s->debugflops = s->debugflops+2*2*nr*nd;
            
            /*
             * Build quadratic model of F along descent direction:
             *     F(x+alpha*d) = D2*alpha^2 + D1*alpha + D0
             *
             * Estimate numerical noise in the X (noise level is used
             * to classify step as singificant or insignificant). Noise
             * comes from two sources:
             * * noise when calculating rows of (I|A)*x
             * * noise when calculating norm of residual
             *
             * In case function curvature is negative or product of descent
             * direction and gradient is non-negative, iterations are terminated.
             *
             * NOTE: D0 is not actually used, but we prefer to maintain it.
             */
            fprev = ae_v_dotproduct(&s->r.ptr.p_double[0], 1, &s->r.ptr.p_double[0], 1, ae_v_len(0,nr-1));
            fprev = fprev/2;
            noiselevel = 0.0;
            for(i=0; i<=nr-1; i++)
            {
                
                /*
                 * Estimate noise introduced by I-th row of (I|A)*x
                 */
                v = 0.0;
                if( i<ns )
                {
                    v = eps*x->ptr.p_double[i];
                }
                for(j=0; j<=nd-1; j++)
                {
                    v = ae_maxreal(v, eps*ae_fabs(s->densea.ptr.pp_double[i][j]*x->ptr.p_double[ns+j], _state), _state);
                }
                v = 2*ae_fabs(s->r.ptr.p_double[i]*v, _state)+v*v;
                
                /*
                 * Add to summary noise in the model
                 */
                noiselevel = noiselevel+v;
            }
            noiselevel = ae_maxreal(noiselevel, eps*fprev, _state);
            d2 = 0.0;
            for(i=0; i<=nr-1; i++)
            {
                v = ae_v_dotproduct(&s->densea.ptr.pp_double[i][0], 1, &s->d.ptr.p_double[ns], 1, ae_v_len(0,nd-1));
                if( i<ns )
                {
                    v = v+s->d.ptr.p_double[i];
                }
                d2 = d2+0.5*ae_sqr(v, _state);
            }
            v = ae_v_dotproduct(&s->d.ptr.p_double[0], 1, &s->g.ptr.p_double[0], 1, ae_v_len(0,ns+nd-1));
            d1 = v;
            d0 = fprev;
            if( ae_fp_less_eq(d2,0)||ae_fp_greater_eq(d1,0) )
            {
                terminationneeded = ae_true;
                break;
            }
            s->debugflops = s->debugflops+2*nr*nd;
            touchreal(&d0, _state);
            
            /*
             * Perform full (unconstrained) step with length StpLen in direction D.
             *
             * We can terminate iterations in case one of two criteria is met:
             * 1. function change is dominated by noise (or function actually increased
             *    instead of decreasing)
             * 2. relative change in X is small enough
             *
             * First condition is not enough to guarantee algorithm termination because
             * sometimes our noise estimate is too optimistic (say, in situations when
             * function value at solition is zero).
             */
            stplen = -d1/(2*d2);
            ae_v_move(&s->xn.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,ns+nd-1));
            ae_v_addd(&s->xn.ptr.p_double[0], 1, &s->d.ptr.p_double[0], 1, ae_v_len(0,ns+nd-1), stplen);
            fcand = 0.0;
            for(i=0; i<=nr-1; i++)
            {
                v = ae_v_dotproduct(&s->densea.ptr.pp_double[i][0], 1, &s->xn.ptr.p_double[ns], 1, ae_v_len(0,nd-1));
                if( i<ns )
                {
                    v = v+s->xn.ptr.p_double[i];
                }
                fcand = fcand+0.5*ae_sqr(v-s->b.ptr.p_double[i], _state);
            }
            s->debugflops = s->debugflops+2*nr*nd;
            if( ae_fp_greater_eq(fcand,fprev-noiselevel*noisetolerance) )
            {
                terminationneeded = ae_true;
                break;
            }
            v = 0;
            for(i=0; i<=ns+nd-1; i++)
            {
                v0 = ae_fabs(x->ptr.p_double[i], _state);
                v1 = ae_fabs(s->xn.ptr.p_double[i], _state);
                if( ae_fp_neq(v0,0)||ae_fp_neq(v1,0) )
                {
                    v = ae_maxreal(v, ae_fabs(x->ptr.p_double[i]-s->xn.ptr.p_double[i], _state)/ae_maxreal(v0, v1, _state), _state);
                }
            }
            if( ae_fp_less_eq(v,eps*noisetolerance) )
            {
                terminationneeded = ae_true;
                break;
            }
            
            /*
             * Perform step one more time, now with non-negativity constraints.
             *
             * NOTE: complicated code below which deals with VarIdx temporary makes
             *       sure that in case unconstrained step leads us outside of feasible
             *       area, we activate at least one constraint.
             */
            wasactivation = snnls_boundedstepandactivation(x, &s->xn, &s->nnc, ns+nd, _state);
            fcur = 0.0;
            for(i=0; i<=nr-1; i++)
            {
                v = ae_v_dotproduct(&s->densea.ptr.pp_double[i][0], 1, &x->ptr.p_double[ns], 1, ae_v_len(0,nd-1));
                if( i<ns )
                {
                    v = v+x->ptr.p_double[i];
                }
                fcur = fcur+0.5*ae_sqr(v-s->b.ptr.p_double[i], _state);
            }
            s->debugflops = s->debugflops+2*nr*nd;
            
            /*
             * Depending on results, decide what to do:
             * 1. In case step was performed without activation of constraints,
             *    we proceed to Newton method
             * 2. In case there was activated at least one constraint, we repeat
             *    steepest descent step.
             */
            if( !wasactivation )
            {
                
                /*
                 * Step without activation, proceed to Newton
                 */
                break;
            }
        }
        if( terminationneeded )
        {
            break;
        }
        
        /*
         * Phase 2: Newton method.
         */
        rvectorsetlengthatleast(&s->cx, ns+nd, _state);
        ivectorsetlengthatleast(&s->columnmap, ns+nd, _state);
        ivectorsetlengthatleast(&s->rowmap, nr, _state);
        rmatrixsetlengthatleast(&s->tmpca, nr, nd, _state);
        rmatrixsetlengthatleast(&s->tmpz, nd, nd, _state);
        rvectorsetlengthatleast(&s->cborg, nr, _state);
        rvectorsetlengthatleast(&s->cb, nr, _state);
        terminationneeded = ae_false;
        for(;;)
        {
            
            /*
             * Prepare equality constrained subproblem with NSC<=NS "sparse"
             * variables and NDC<=ND "dense" variables.
             *
             * First, we reorder variables (columns) and move all unconstrained
             * variables "to the left", ColumnMap stores this permutation.
             *
             * Then, we reorder first NS rows of A and first NS elements of B in
             * such way that we still have identity matrix in first NSC columns
             * of problem. This permutation is stored in RowMap.
             */
            nsc = 0;
            ndc = 0;
            for(i=0; i<=ns-1; i++)
            {
                if( !(s->nnc.ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],0)) )
                {
                    s->columnmap.ptr.p_int[nsc] = i;
                    nsc = nsc+1;
                }
            }
            for(i=ns; i<=ns+nd-1; i++)
            {
                if( !(s->nnc.ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],0)) )
                {
                    s->columnmap.ptr.p_int[nsc+ndc] = i;
                    ndc = ndc+1;
                }
            }
            for(i=0; i<=nsc-1; i++)
            {
                s->rowmap.ptr.p_int[i] = s->columnmap.ptr.p_int[i];
            }
            j = nsc;
            for(i=0; i<=ns-1; i++)
            {
                if( s->nnc.ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],0) )
                {
                    s->rowmap.ptr.p_int[j] = i;
                    j = j+1;
                }
            }
            for(i=ns; i<=nr-1; i++)
            {
                s->rowmap.ptr.p_int[i] = i;
            }
            
            /*
             * Now, permutations are ready, and we can copy/reorder
             * A, B and X to CA, CB and CX.
             */
            for(i=0; i<=nsc+ndc-1; i++)
            {
                s->cx.ptr.p_double[i] = x->ptr.p_double[s->columnmap.ptr.p_int[i]];
            }
            for(i=0; i<=nr-1; i++)
            {
                for(j=0; j<=ndc-1; j++)
                {
                    s->tmpca.ptr.pp_double[i][j] = s->densea.ptr.pp_double[s->rowmap.ptr.p_int[i]][s->columnmap.ptr.p_int[nsc+j]-ns];
                }
                s->cb.ptr.p_double[i] = s->b.ptr.p_double[s->rowmap.ptr.p_int[i]];
            }
            
            /*
             * Solve equality constrained subproblem.
             */
            if( ndc>0 )
            {
                
                /*
                 * NDC>0.
                 *
                 * Solve subproblem using Newton-type algorithm. We have a
                 * NR*(NSC+NDC) linear least squares subproblem
                 *
                 *         | ( I  AU )   ( XU )   ( BU ) |^2
                 *     min | (       ) * (    ) - (    ) |
                 *         | ( 0  AL )   ( XL )   ( BL ) |
                 *
                 * where:
                 * * I is a NSC*NSC identity matrix
                 * * AU is NSC*NDC dense matrix (first NSC rows of CA)
                 * * AL is (NR-NSC)*NDC dense matrix (next NR-NSC rows of CA)
                 * * BU and BL are correspondingly sized parts of CB
                 *
                 * After conversion to normal equations and small regularization,
                 * we get:
                 *
                 *     ( I   AU ) (  XU )   ( BU            )
                 *     (        )*(     ) = (               )
                 *     ( AU' Y  ) (  XL )   ( AU'*BU+AL'*BL )
                 *
                 * where Y = AU'*AU + AL'*AL + lambda*diag(AU'*AU+AL'*AL).
                 *
                 * With Schur Complement Method this system can be solved in
                 * O(NR*NDC^2+NDC^3) operations. In order to solve it we multiply
                 * first row by AU' and subtract it from the second one. As result,
                 * we get system
                 *
                 *     Z*XL = AL'*BL, where Z=AL'*AL+lambda*diag(AU'*AU+AL'*AL)
                 *
                 * We can easily solve it for XL, and we can get XU as XU = BU-AU*XL.
                 *
                 * We will start solution from calculating Cholesky decomposition of Z.
                 */
                for(i=0; i<=nr-1; i++)
                {
                    s->cborg.ptr.p_double[i] = s->cb.ptr.p_double[i];
                }
                for(i=0; i<=ndc-1; i++)
                {
                    s->diagaa.ptr.p_double[i] = 0;
                }
                for(i=0; i<=nr-1; i++)
                {
                    for(j=0; j<=ndc-1; j++)
                    {
                        s->diagaa.ptr.p_double[j] = s->diagaa.ptr.p_double[j]+ae_sqr(s->tmpca.ptr.pp_double[i][j], _state);
                    }
                }
                for(j=0; j<=ndc-1; j++)
                {
                    if( ae_fp_eq(s->diagaa.ptr.p_double[j],0) )
                    {
                        s->diagaa.ptr.p_double[j] = 1;
                    }
                }
                for(;;)
                {
                    
                    /*
                     * NOTE: we try to factorize Z. In case of failure we increase
                     *       regularization parameter and try again.
                     */
                    s->debugflops = s->debugflops+2*(nr-nsc)*ae_sqr(ndc, _state)+ae_pow(ndc, 3, _state)/3;
                    for(i=0; i<=ndc-1; i++)
                    {
                        for(j=0; j<=ndc-1; j++)
                        {
                            s->tmpz.ptr.pp_double[i][j] = 0.0;
                        }
                    }
                    rmatrixsyrk(ndc, nr-nsc, 1.0, &s->tmpca, nsc, 0, 2, 0.0, &s->tmpz, 0, 0, ae_true, _state);
                    for(i=0; i<=ndc-1; i++)
                    {
                        s->tmpz.ptr.pp_double[i][i] = s->tmpz.ptr.pp_double[i][i]+lambdav*s->diagaa.ptr.p_double[i];
                    }
                    if( spdmatrixcholeskyrec(&s->tmpz, 0, ndc, ae_true, &s->tmpcholesky, _state) )
                    {
                        break;
                    }
                    lambdav = lambdav*10;
                }
                
                /*
                 * We have Cholesky decomposition of Z, now we can solve system:
                 * * we start from initial point CX
                 * * we perform several iterations of refinement:
                 *   * BU_new := BU_orig - XU_cur - AU*XL_cur
                 *   * BL_new := BL_orig - AL*XL_cur
                 *   * solve for BU_new/BL_new, obtain solution dx
                 *   * XU_cur := XU_cur + dx_u
                 *   * XL_cur := XL_cur + dx_l
                 * * BU_new/BL_new are stored in CB, original right part is
                 *   stored in CBOrg, correction to X is stored in DX, current
                 *   X is stored in CX
                 */
                for(rfsits=1; rfsits<=s->refinementits; rfsits++)
                {
                    for(i=0; i<=nr-1; i++)
                    {
                        v = ae_v_dotproduct(&s->tmpca.ptr.pp_double[i][0], 1, &s->cx.ptr.p_double[nsc], 1, ae_v_len(0,ndc-1));
                        s->cb.ptr.p_double[i] = s->cborg.ptr.p_double[i]-v;
                        if( i<nsc )
                        {
                            s->cb.ptr.p_double[i] = s->cb.ptr.p_double[i]-s->cx.ptr.p_double[i];
                        }
                    }
                    s->debugflops = s->debugflops+2*nr*ndc;
                    for(i=0; i<=ndc-1; i++)
                    {
                        s->dx.ptr.p_double[i] = 0.0;
                    }
                    for(i=nsc; i<=nr-1; i++)
                    {
                        v = s->cb.ptr.p_double[i];
                        ae_v_addd(&s->dx.ptr.p_double[0], 1, &s->tmpca.ptr.pp_double[i][0], 1, ae_v_len(0,ndc-1), v);
                    }
                    fblscholeskysolve(&s->tmpz, 1.0, ndc, ae_true, &s->dx, &s->tmpcholesky, _state);
                    s->debugflops = s->debugflops+2*ndc*ndc;
                    ae_v_add(&s->cx.ptr.p_double[nsc], 1, &s->dx.ptr.p_double[0], 1, ae_v_len(nsc,nsc+ndc-1));
                    for(i=0; i<=nsc-1; i++)
                    {
                        v = ae_v_dotproduct(&s->tmpca.ptr.pp_double[i][0], 1, &s->dx.ptr.p_double[0], 1, ae_v_len(0,ndc-1));
                        s->cx.ptr.p_double[i] = s->cx.ptr.p_double[i]+s->cb.ptr.p_double[i]-v;
                    }
                    s->debugflops = s->debugflops+2*nsc*ndc;
                }
            }
            else
            {
                
                /*
                 * NDC=0.
                 *
                 * We have a NR*NSC linear least squares subproblem
                 *
                 *     min |XU-BU|^2
                 *
                 * solution is easy to find - it is XU=BU!
                 */
                for(i=0; i<=nsc-1; i++)
                {
                    s->cx.ptr.p_double[i] = s->cb.ptr.p_double[i];
                }
            }
            for(i=0; i<=ns+nd-1; i++)
            {
                s->xn.ptr.p_double[i] = x->ptr.p_double[i];
            }
            for(i=0; i<=nsc+ndc-1; i++)
            {
                s->xn.ptr.p_double[s->columnmap.ptr.p_int[i]] = s->cx.ptr.p_double[i];
            }
            newtoncnt = newtoncnt+1;
            
            /*
             * Step to candidate point.
             * If no constraints was added, accept candidate point XN and move to next phase.
             * Terminate, if number of Newton iterations exceeded DebugMaxNewton counter.
             */
            terminationneeded = s->debugmaxnewton>0&&newtoncnt>=s->debugmaxnewton;
            if( !snnls_boundedstepandactivation(x, &s->xn, &s->nnc, ns+nd, _state) )
            {
                break;
            }
            if( terminationneeded )
            {
                break;
            }
        }
        if( terminationneeded )
        {
            break;
        }
    }
}


/*************************************************************************
Having feasible current point XC and possibly infeasible candidate   point
XN,  this  function  performs  longest  step  from  XC to XN which retains
feasibility. In case XN is found to be infeasible, at least one constraint
is activated.

For example, if we have:
  XC=0.5
  XN=-1.2
  x>=0
then this function will move us to X=0 and activate constraint "x>=0".

INPUT PARAMETERS:
    XC      -   current point, must be feasible with respect to
                all constraints
    XN      -   candidate point, can be infeasible with respect to some
                constraints
    NNC     -   NNC[i] is True when I-th variable is non-negatively
                constrained
    N       -   variable count

OUTPUT PARAMETERS:
    XC      -   new position

RESULT:
    True in case at least one constraint was activated by step

  -- ALGLIB --
     Copyright 19.10.2012 by Bochkanov Sergey
*************************************************************************/
static ae_bool snnls_boundedstepandactivation(/* Real    */ ae_vector* xc,
     /* Real    */ ae_vector* xn,
     /* Boolean */ ae_vector* nnc,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t varidx;
    double vmax;
    double v;
    double stplen;
    ae_bool result;


    
    /*
     * Check constraints.
     *
     * NOTE: it is important to test for XN[i]<XC[i] (strict inequality,
     *       allows to handle correctly situations with XC[i]=0 without
     *       activating already active constraints), but to check for
     *       XN[i]<=0 (non-strict inequality, correct handling of some
     *       special cases when unconstrained step ends at the boundary).
     */
    result = ae_false;
    varidx = -1;
    vmax = ae_maxrealnumber;
    for(i=0; i<=n-1; i++)
    {
        if( (nnc->ptr.p_bool[i]&&ae_fp_less(xn->ptr.p_double[i],xc->ptr.p_double[i]))&&ae_fp_less_eq(xn->ptr.p_double[i],0.0) )
        {
            v = vmax;
            vmax = safeminposrv(xc->ptr.p_double[i], xc->ptr.p_double[i]-xn->ptr.p_double[i], vmax, _state);
            if( ae_fp_less(vmax,v) )
            {
                varidx = i;
            }
        }
    }
    stplen = ae_minreal(vmax, 1.0, _state);
    
    /*
     * Perform step with activation.
     *
     * NOTE: it is important to use (1-StpLen)*XC + StpLen*XN because
     *       it allows us to step exactly to XN when StpLen=1, even in
     *       the presence of numerical errors.
     */
    for(i=0; i<=n-1; i++)
    {
        xc->ptr.p_double[i] = (1-stplen)*xc->ptr.p_double[i]+stplen*xn->ptr.p_double[i];
    }
    if( varidx>=0 )
    {
        xc->ptr.p_double[varidx] = 0.0;
        result = ae_true;
    }
    for(i=0; i<=n-1; i++)
    {
        if( nnc->ptr.p_bool[i]&&ae_fp_less(xc->ptr.p_double[i],0.0) )
        {
            xc->ptr.p_double[i] = 0.0;
            result = ae_true;
        }
    }
    return result;
}


ae_bool _snnlssolver_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    snnlssolver *p = (snnlssolver*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_matrix_init(&p->densea, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->nnc, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xn, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->tmpz, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->tmpca, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->dx, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->diagaa, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->cb, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->cx, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->cborg, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->columnmap, 0, DT_INT, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rowmap, 0, DT_INT, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmpcholesky, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->r, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _snnlssolver_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    snnlssolver *dst = (snnlssolver*)_dst;
    snnlssolver *src = (snnlssolver*)_src;
    dst->ns = src->ns;
    dst->nd = src->nd;
    dst->nr = src->nr;
    if( !ae_matrix_init_copy(&dst->densea, &src->densea, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->nnc, &src->nnc, _state, make_automatic) )
        return ae_false;
    dst->refinementits = src->refinementits;
    dst->debugflops = src->debugflops;
    dst->debugmaxnewton = src->debugmaxnewton;
    if( !ae_vector_init_copy(&dst->xn, &src->xn, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->tmpz, &src->tmpz, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->tmpca, &src->tmpca, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->dx, &src->dx, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->diagaa, &src->diagaa, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->cb, &src->cb, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->cx, &src->cx, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->cborg, &src->cborg, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->columnmap, &src->columnmap, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rowmap, &src->rowmap, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmpcholesky, &src->tmpcholesky, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->r, &src->r, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _snnlssolver_clear(void* _p)
{
    snnlssolver *p = (snnlssolver*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->densea);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->nnc);
    ae_vector_clear(&p->xn);
    ae_matrix_clear(&p->tmpz);
    ae_matrix_clear(&p->tmpca);
    ae_vector_clear(&p->g);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->dx);
    ae_vector_clear(&p->diagaa);
    ae_vector_clear(&p->cb);
    ae_vector_clear(&p->cx);
    ae_vector_clear(&p->cborg);
    ae_vector_clear(&p->columnmap);
    ae_vector_clear(&p->rowmap);
    ae_vector_clear(&p->tmpcholesky);
    ae_vector_clear(&p->r);
}


void _snnlssolver_destroy(void* _p)
{
    snnlssolver *p = (snnlssolver*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->densea);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->nnc);
    ae_vector_destroy(&p->xn);
    ae_matrix_destroy(&p->tmpz);
    ae_matrix_destroy(&p->tmpca);
    ae_vector_destroy(&p->g);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->dx);
    ae_vector_destroy(&p->diagaa);
    ae_vector_destroy(&p->cb);
    ae_vector_destroy(&p->cx);
    ae_vector_destroy(&p->cborg);
    ae_vector_destroy(&p->columnmap);
    ae_vector_destroy(&p->rowmap);
    ae_vector_destroy(&p->tmpcholesky);
    ae_vector_destroy(&p->r);
}




/*************************************************************************
This   subroutine   is   used  to initialize active set. By default, empty
N-variable model with no constraints is  generated.  Previously  allocated
buffer variables are reused as much as possible.

Two use cases for this object are described below.

CASE 1 - STEEPEST DESCENT:

    SASInit()
    repeat:
        SASReactivateConstraints()
        SASDescentDirection()
        SASExploreDirection()
        SASMoveTo()
    until convergence

CASE 1 - PRECONDITIONED STEEPEST DESCENT:

    SASInit()
    repeat:
        SASReactivateConstraintsPrec()
        SASDescentDirectionPrec()
        SASExploreDirection()
        SASMoveTo()
    until convergence

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sasinit(ae_int_t n, sactiveset* s, ae_state *_state)
{
    ae_int_t i;


    s->n = n;
    s->algostate = 0;
    
    /*
     * Constraints
     */
    s->constraintschanged = ae_true;
    s->nec = 0;
    s->nic = 0;
    rvectorsetlengthatleast(&s->bndl, n, _state);
    bvectorsetlengthatleast(&s->hasbndl, n, _state);
    rvectorsetlengthatleast(&s->bndu, n, _state);
    bvectorsetlengthatleast(&s->hasbndu, n, _state);
    for(i=0; i<=n-1; i++)
    {
        s->bndl.ptr.p_double[i] = _state->v_neginf;
        s->bndu.ptr.p_double[i] = _state->v_posinf;
        s->hasbndl.ptr.p_bool[i] = ae_false;
        s->hasbndu.ptr.p_bool[i] = ae_false;
    }
    
    /*
     * current point, scale
     */
    s->hasxc = ae_false;
    rvectorsetlengthatleast(&s->xc, n, _state);
    rvectorsetlengthatleast(&s->s, n, _state);
    rvectorsetlengthatleast(&s->h, n, _state);
    for(i=0; i<=n-1; i++)
    {
        s->xc.ptr.p_double[i] = 0.0;
        s->s.ptr.p_double[i] = 1.0;
        s->h.ptr.p_double[i] = 1.0;
    }
    
    /*
     * Other
     */
    rvectorsetlengthatleast(&s->unitdiagonal, n, _state);
    for(i=0; i<=n-1; i++)
    {
        s->unitdiagonal.ptr.p_double[i] = 1.0;
    }
}


/*************************************************************************
This function sets scaling coefficients for SAS object.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

During orthogonalization phase, scale is used to calculate drop tolerances
(whether vector is significantly non-zero or not).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sassetscale(sactiveset* state,
     /* Real    */ ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(state->algostate==0, "SASSetScale: you may change scale only in modification mode", _state);
    ae_assert(s->cnt>=state->n, "SASSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "SASSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],0), "SASSetScale: S contains zero elements", _state);
    }
    for(i=0; i<=state->n-1; i++)
    {
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE 1: D[i] should be positive. Exception will be thrown otherwise.

NOTE 2: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sassetprecdiag(sactiveset* state,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(state->algostate==0, "SASSetPrecDiag: you may change preconditioner only in modification mode", _state);
    ae_assert(d->cnt>=state->n, "SASSetPrecDiag: D is too short", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(d->ptr.p_double[i], _state), "SASSetPrecDiag: D contains infinite or NAN elements", _state);
        ae_assert(ae_fp_greater(d->ptr.p_double[i],0), "SASSetPrecDiag: D contains non-positive elements", _state);
    }
    for(i=0; i<=state->n-1; i++)
    {
        state->h.ptr.p_double[i] = d->ptr.p_double[i];
    }
}


/*************************************************************************
This function sets/changes boundary constraints.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sassetbc(sactiveset* state,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    ae_assert(state->algostate==0, "SASSetBC: you may change constraints only in modification mode", _state);
    n = state->n;
    ae_assert(bndl->cnt>=n, "SASSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "SASSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "SASSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "SASSetBC: BndL contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
    state->constraintschanged = ae_true;
}


/*************************************************************************
This function sets linear constraints for SAS object.

Linear constraints are inactive by default (after initial creation).

INPUT PARAMETERS:
    State   -   SAS structure
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0

NOTE 1: linear (non-bound) constraints are satisfied only approximately:
* there always exists some minor violation (about Epsilon in magnitude)
  due to rounding errors
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.
If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void sassetlc(sactiveset* state,
     /* Real    */ ae_matrix* c,
     /* Integer */ ae_vector* ct,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;


    ae_assert(state->algostate==0, "SASSetLC: you may change constraints only in modification mode", _state);
    n = state->n;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(k>=0, "SASSetLC: K<0", _state);
    ae_assert(c->cols>=n+1||k==0, "SASSetLC: Cols(C)<N+1", _state);
    ae_assert(c->rows>=k, "SASSetLC: Rows(C)<K", _state);
    ae_assert(ct->cnt>=k, "SASSetLC: Length(CT)<K", _state);
    ae_assert(apservisfinitematrix(c, k, n+1, _state), "SASSetLC: C contains infinite or NaN values!", _state);
    
    /*
     * Handle zero K
     */
    if( k==0 )
    {
        state->nec = 0;
        state->nic = 0;
        state->constraintschanged = ae_true;
        return;
    }
    
    /*
     * Equality constraints are stored first, in the upper
     * NEC rows of State.CLEIC matrix. Inequality constraints
     * are stored in the next NIC rows.
     *
     * NOTE: we convert inequality constraints to the form
     * A*x<=b before copying them.
     */
    rmatrixsetlengthatleast(&state->cleic, k, n+1, _state);
    state->nec = 0;
    state->nic = 0;
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]==0 )
        {
            ae_v_move(&state->cleic.ptr.pp_double[state->nec][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            state->nec = state->nec+1;
        }
    }
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]!=0 )
        {
            if( ct->ptr.p_int[i]>0 )
            {
                ae_v_moveneg(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            else
            {
                ae_v_move(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            state->nic = state->nic+1;
        }
    }
    
    /*
     * Mark state as changed
     */
    state->constraintschanged = ae_true;
}


/*************************************************************************
Another variation of SASSetLC(), which accepts  linear  constraints  using
another representation.

Linear constraints are inactive by default (after initial creation).

INPUT PARAMETERS:
    State   -   SAS structure
    CLEIC   -   linear constraints, array[NEC+NIC,N+1].
                Each row of C represents one constraint:
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                First NEC rows store equality constraints, next NIC -  are
                inequality ones.
                All elements of C (including right part) must be finite.
    NEC     -   number of equality constraints, NEC>=0
    NIC     -   number of inequality constraints, NIC>=0

NOTE 1: linear (non-bound) constraints are satisfied only approximately:
* there always exists some minor violation (about Epsilon in magnitude)
  due to rounding errors
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.
If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void sassetlcx(sactiveset* state,
     /* Real    */ ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;


    ae_assert(state->algostate==0, "SASSetLCX: you may change constraints only in modification mode", _state);
    n = state->n;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(nec>=0, "SASSetLCX: NEC<0", _state);
    ae_assert(nic>=0, "SASSetLCX: NIC<0", _state);
    ae_assert(cleic->cols>=n+1||nec+nic==0, "SASSetLCX: Cols(CLEIC)<N+1", _state);
    ae_assert(cleic->rows>=nec+nic, "SASSetLCX: Rows(CLEIC)<NEC+NIC", _state);
    ae_assert(apservisfinitematrix(cleic, nec+nic, n+1, _state), "SASSetLCX: CLEIC contains infinite or NaN values!", _state);
    
    /*
     * Store constraints
     */
    rmatrixsetlengthatleast(&state->cleic, nec+nic, n+1, _state);
    state->nec = nec;
    state->nic = nic;
    for(i=0; i<=nec+nic-1; i++)
    {
        for(j=0; j<=n; j++)
        {
            state->cleic.ptr.pp_double[i][j] = cleic->ptr.pp_double[i][j];
        }
    }
    
    /*
     * Mark state as changed
     */
    state->constraintschanged = ae_true;
}


/*************************************************************************
This subroutine turns on optimization mode:
1. feasibility in X is enforced  (in case X=S.XC and constraints  have not
   changed, algorithm just uses X without any modifications at all)
2. constraints are marked as "candidate" or "inactive"

INPUT PARAMETERS:
    S   -   active set object
    X   -   initial point (candidate), array[N]. It is expected that X
            contains only finite values (we do not check it).
    
OUTPUT PARAMETERS:
    S   -   state is changed
    X   -   initial point can be changed to enforce feasibility
    
RESULT:
    True in case feasible point was found (mode was changed to "optimization")
    False in case no feasible point was found (mode was not changed)

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool sasstartoptimization(sactiveset* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_bool result;


    ae_assert(state->algostate==0, "SASStartOptimization: already in optimization mode", _state);
    result = ae_false;
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    
    /*
     * Enforce feasibility and calculate set of "candidate"/"active" constraints.
     * Always active equality constraints are marked as "active", all other constraints
     * are marked as "candidate".
     */
    ivectorsetlengthatleast(&state->activeset, n+nec+nic, _state);
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i] )
        {
            if( ae_fp_greater(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                return result;
            }
        }
    }
    ae_v_move(&state->xc.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
    if( state->nec+state->nic>0 )
    {
        
        /*
         * General linear constraints are present; general code is used.
         */
        rvectorsetlengthatleast(&state->tmp0, n, _state);
        rvectorsetlengthatleast(&state->tmpfeas, n+state->nic, _state);
        rmatrixsetlengthatleast(&state->tmpm0, state->nec+state->nic, n+state->nic+1, _state);
        for(i=0; i<=state->nec+state->nic-1; i++)
        {
            ae_v_move(&state->tmpm0.ptr.pp_double[i][0], 1, &state->cleic.ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
            for(j=n; j<=n+state->nic-1; j++)
            {
                state->tmpm0.ptr.pp_double[i][j] = 0;
            }
            if( i>=state->nec )
            {
                state->tmpm0.ptr.pp_double[i][n+i-state->nec] = 1.0;
            }
            state->tmpm0.ptr.pp_double[i][n+state->nic] = state->cleic.ptr.pp_double[i][n];
        }
        ae_v_move(&state->tmpfeas.ptr.p_double[0], 1, &state->xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
        for(i=0; i<=state->nic-1; i++)
        {
            v = ae_v_dotproduct(&state->cleic.ptr.pp_double[i+state->nec][0], 1, &state->xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
            state->tmpfeas.ptr.p_double[i+n] = ae_maxreal(state->cleic.ptr.pp_double[i+state->nec][n]-v, 0.0, _state);
        }
        if( !findfeasiblepoint(&state->tmpfeas, &state->bndl, &state->hasbndl, &state->bndu, &state->hasbndu, n, state->nic, &state->tmpm0, state->nec+state->nic, 1.0E-6, &i, &j, _state) )
        {
            return result;
        }
        ae_v_move(&state->xc.ptr.p_double[0], 1, &state->tmpfeas.ptr.p_double[0], 1, ae_v_len(0,n-1));
        for(i=0; i<=n-1; i++)
        {
            if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->activeset.ptr.p_int[i] = 1;
                continue;
            }
            if( (state->hasbndl.ptr.p_bool[i]&&ae_fp_eq(state->xc.ptr.p_double[i],state->bndl.ptr.p_double[i]))||(state->hasbndu.ptr.p_bool[i]&&ae_fp_eq(state->xc.ptr.p_double[i],state->bndu.ptr.p_double[i])) )
            {
                state->activeset.ptr.p_int[i] = 0;
                continue;
            }
            state->activeset.ptr.p_int[i] = -1;
        }
        for(i=0; i<=state->nec-1; i++)
        {
            state->activeset.ptr.p_int[n+i] = 1;
        }
        for(i=0; i<=state->nic-1; i++)
        {
            if( ae_fp_eq(state->tmpfeas.ptr.p_double[n+i],0) )
            {
                state->activeset.ptr.p_int[n+state->nec+i] = 0;
            }
            else
            {
                state->activeset.ptr.p_int[n+state->nec+i] = -1;
            }
        }
    }
    else
    {
        
        /*
         * Only bound constraints are present, quick code can be used
         */
        for(i=0; i<=n-1; i++)
        {
            state->activeset.ptr.p_int[i] = -1;
            if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->activeset.ptr.p_int[i] = 1;
                state->xc.ptr.p_double[i] = state->bndl.ptr.p_double[i];
                continue;
            }
            if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less_eq(state->xc.ptr.p_double[i],state->bndl.ptr.p_double[i]) )
            {
                state->xc.ptr.p_double[i] = state->bndl.ptr.p_double[i];
                state->activeset.ptr.p_int[i] = 0;
                continue;
            }
            if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater_eq(state->xc.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->xc.ptr.p_double[i] = state->bndu.ptr.p_double[i];
                state->activeset.ptr.p_int[i] = 0;
                continue;
            }
        }
    }
    
    /*
     * Change state, allocate temporaries
     */
    result = ae_true;
    state->algostate = 1;
    state->basisisready = ae_false;
    state->hasxc = ae_true;
    rmatrixsetlengthatleast(&state->pbasis, ae_minint(nec+nic, n, _state), n+1, _state);
    rmatrixsetlengthatleast(&state->ibasis, ae_minint(nec+nic, n, _state), n+1, _state);
    rmatrixsetlengthatleast(&state->sbasis, ae_minint(nec+nic, n, _state), n+1, _state);
    return result;
}


/*************************************************************************
This function explores search direction and calculates bound for  step  as
well as information for activation of constraints.

INPUT PARAMETERS:
    State       -   SAS structure which stores current point and all other
                    active set related information
    D           -   descent direction to explore

OUTPUT PARAMETERS:
    StpMax      -   upper  limit  on  step  length imposed by yet inactive
                    constraints. Can be  zero  in  case  some  constraints
                    can be activated by zero step.  Equal  to  some  large
                    value in case step is unlimited.
    CIdx        -   -1 for unlimited step, in [0,N+NEC+NIC) in case of
                    limited step.
    VVal        -   value which is assigned to X[CIdx] during activation.
                    For CIdx<0 or CIdx>=N some dummy value is assigned to
                    this parameter.
*************************************************************************/
void sasexploredirection(sactiveset* state,
     /* Real    */ ae_vector* d,
     double* stpmax,
     ae_int_t* cidx,
     double* vval,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t i;
    double prevmax;
    double vc;
    double vd;

    *stpmax = 0;
    *cidx = 0;
    *vval = 0;

    ae_assert(state->algostate==1, "SASExploreDirection: is not in optimization mode", _state);
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    *cidx = -1;
    *vval = 0;
    *stpmax = 1.0E50;
    for(i=0; i<=n-1; i++)
    {
        if( state->activeset.ptr.p_int[i]<=0 )
        {
            ae_assert(!state->hasbndl.ptr.p_bool[i]||ae_fp_greater_eq(state->xc.ptr.p_double[i],state->bndl.ptr.p_double[i]), "SASExploreDirection: internal error - infeasible X", _state);
            ae_assert(!state->hasbndu.ptr.p_bool[i]||ae_fp_less_eq(state->xc.ptr.p_double[i],state->bndu.ptr.p_double[i]), "SASExploreDirection: internal error - infeasible X", _state);
            if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(d->ptr.p_double[i],0) )
            {
                prevmax = *stpmax;
                *stpmax = safeminposrv(state->xc.ptr.p_double[i]-state->bndl.ptr.p_double[i], -d->ptr.p_double[i], *stpmax, _state);
                if( ae_fp_less(*stpmax,prevmax) )
                {
                    *cidx = i;
                    *vval = state->bndl.ptr.p_double[i];
                }
            }
            if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(d->ptr.p_double[i],0) )
            {
                prevmax = *stpmax;
                *stpmax = safeminposrv(state->bndu.ptr.p_double[i]-state->xc.ptr.p_double[i], d->ptr.p_double[i], *stpmax, _state);
                if( ae_fp_less(*stpmax,prevmax) )
                {
                    *cidx = i;
                    *vval = state->bndu.ptr.p_double[i];
                }
            }
        }
    }
    for(i=nec; i<=nec+nic-1; i++)
    {
        if( state->activeset.ptr.p_int[n+i]<=0 )
        {
            vc = ae_v_dotproduct(&state->cleic.ptr.pp_double[i][0], 1, &state->xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
            vc = vc-state->cleic.ptr.pp_double[i][n];
            vd = ae_v_dotproduct(&state->cleic.ptr.pp_double[i][0], 1, &d->ptr.p_double[0], 1, ae_v_len(0,n-1));
            if( ae_fp_less_eq(vd,0) )
            {
                continue;
            }
            if( ae_fp_less(vc,0) )
            {
                
                /*
                 * XC is strictly feasible with respect to I-th constraint,
                 * we can perform non-zero step because there is non-zero distance
                 * between XC and bound.
                 */
                prevmax = *stpmax;
                *stpmax = safeminposrv(-vc, vd, *stpmax, _state);
                if( ae_fp_less(*stpmax,prevmax) )
                {
                    *cidx = n+i;
                }
            }
            else
            {
                
                /*
                 * XC is at the boundary (or slightly beyond it), and step vector
                 * points beyond the boundary.
                 *
                 * The only thing we can do is to perform zero step and activate
                 * I-th constraint.
                 */
                *stpmax = 0;
                *cidx = n+i;
            }
        }
    }
}


/*************************************************************************
This subroutine moves current point to XN,  in  the  direction  previously
explored with SASExploreDirection() function.

Step may activate one constraint. It is assumed than XN  is  approximately
feasible (small error as  large  as several  ulps  is  possible).   Strict
feasibility  with  respect  to  bound  constraints  is  enforced    during
activation, feasibility with respect to general linear constraints is  not
enforced.

INPUT PARAMETERS:
    S       -   active set object
    XN      -   new point.
    NeedAct -   True in case one constraint needs activation
    CIdx    -   index of constraint, in [0,N+NEC+NIC).
                Ignored if NeedAct is false.
                This value is calculated by SASExploreDirection().
    CVal    -   for CIdx in [0,N) this field stores value which is
                assigned to XC[CIdx] during activation. CVal is ignored in
                other cases.
                This value is calculated by SASExploreDirection().
    
OUTPUT PARAMETERS:
    S       -   current point and list of active constraints are changed.

RESULT:
    >0, in case at least one inactive non-candidate constraint was activated
    =0, in case only "candidate" constraints were activated
    <0, in case no constraints were activated by the step

NOTE: in general case State.XC<>XN because activation of  constraints  may
      slightly change current point (to enforce feasibility).

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t sasmoveto(sactiveset* state,
     /* Real    */ ae_vector* xn,
     ae_bool needact,
     ae_int_t cidx,
     double cval,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t i;
    ae_bool wasactivation;
    ae_int_t result;


    ae_assert(state->algostate==1, "SASMoveTo: is not in optimization mode", _state);
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    
    /*
     * Save previous state, update current point
     */
    rvectorsetlengthatleast(&state->mtx, n, _state);
    ivectorsetlengthatleast(&state->mtas, n+nec+nic, _state);
    for(i=0; i<=n-1; i++)
    {
        state->mtx.ptr.p_double[i] = state->xc.ptr.p_double[i];
        state->xc.ptr.p_double[i] = xn->ptr.p_double[i];
    }
    for(i=0; i<=n+nec+nic-1; i++)
    {
        state->mtas.ptr.p_int[i] = state->activeset.ptr.p_int[i];
    }
    
    /*
     * Activate constraints
     */
    wasactivation = ae_false;
    if( needact )
    {
        
        /*
         * Activation
         */
        ae_assert(cidx>=0&&cidx<n+nec+nic, "SASMoveTo: incorrect CIdx", _state);
        if( cidx<n )
        {
            
            /*
             * CIdx in [0,N-1] means that bound constraint was activated.
             * We activate it explicitly to avoid situation when roundoff-error
             * prevents us from moving EXACTLY to x=CVal.
             */
            state->xc.ptr.p_double[cidx] = cval;
        }
        state->activeset.ptr.p_int[cidx] = 1;
        wasactivation = ae_true;
    }
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Post-check (some constraints may be activated because of numerical errors)
         */
        if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(state->xc.ptr.p_double[i],state->bndl.ptr.p_double[i]) )
        {
            state->xc.ptr.p_double[i] = state->bndl.ptr.p_double[i];
            state->activeset.ptr.p_int[i] = 1;
            wasactivation = ae_true;
        }
        if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(state->xc.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            state->xc.ptr.p_double[i] = state->bndu.ptr.p_double[i];
            state->activeset.ptr.p_int[i] = 1;
            wasactivation = ae_true;
        }
    }
    
    /*
     * Determine return status:
     * * -1 in case no constraints were activated
     * *  0 in case only "candidate" constraints were activated
     * * +1 in case at least one "non-candidate" constraint was activated
     */
    if( wasactivation )
    {
        
        /*
         * Step activated one/several constraints, but sometimes it is spurious
         * activation - RecalculateConstraints() tells us that constraint is
         * inactive (negative Largrange multiplier), but step activates it
         * because of numerical noise.
         *
         * This block of code checks whether step activated truly new constraints
         * (ones which were not in the active set at the solution):
         *
         * * for non-boundary constraint it is enough to check that previous value
         *   of ActiveSet[i] is negative (=far from boundary), and new one is
         *   positive (=we are at the boundary, constraint is activated).
         *
         * * for boundary constraints previous criterion won't work. Each variable
         *   has two constraints, and simply checking their status is not enough -
         *   we have to correctly identify cases when we leave one boundary
         *   (PrevActiveSet[i]=0) and move to another boundary (ActiveSet[i]>0).
         *   Such cases can be identified if we compare previous X with new X.
         *
         * In case only "candidate" constraints were activated, result variable
         * is set to 0. In case at least one new constraint was activated, result
         * is set to 1.
         */
        result = 0;
        for(i=0; i<=n-1; i++)
        {
            if( state->activeset.ptr.p_int[i]>0&&ae_fp_neq(state->xc.ptr.p_double[i],state->mtx.ptr.p_double[i]) )
            {
                result = 1;
            }
        }
        for(i=n; i<=n+state->nec+state->nic-1; i++)
        {
            if( state->mtas.ptr.p_int[i]<0&&state->activeset.ptr.p_int[i]>0 )
            {
                result = 1;
            }
        }
    }
    else
    {
        
        /*
         * No activation, return -1
         */
        result = -1;
    }
    
    /*
     * Invalidate basis
     */
    state->basisisready = ae_false;
    return result;
}


/*************************************************************************
This subroutine performs immediate activation of one constraint:
* "immediate" means that we do not have to move to activate it
* in case boundary constraint is activated, we enforce current point to be
  exactly at the boundary

INPUT PARAMETERS:
    S       -   active set object
    CIdx    -   index of constraint, in [0,N+NEC+NIC).
                This value is calculated by SASExploreDirection().
    CVal    -   for CIdx in [0,N) this field stores value which is
                assigned to XC[CIdx] during activation. CVal is ignored in
                other cases.
                This value is calculated by SASExploreDirection().

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sasimmediateactivation(sactiveset* state,
     ae_int_t cidx,
     double cval,
     ae_state *_state)
{


    ae_assert(state->algostate==1, "SASMoveTo: is not in optimization mode", _state);
    if( cidx<state->n )
    {
        state->xc.ptr.p_double[cidx] = cval;
    }
    state->activeset.ptr.p_int[cidx] = 1;
    state->basisisready = ae_false;
}


/*************************************************************************
This subroutine calculates descent direction subject to current active set.

INPUT PARAMETERS:
    S       -   active set object
    G       -   array[N], gradient
    D       -   possibly prealocated buffer;
                automatically resized if needed.
    
OUTPUT PARAMETERS:
    D       -   descent direction projected onto current active set.
                Components of D which correspond to active boundary
                constraints are forced to be exactly zero.
                In case D is non-zero, it is normalized to have unit norm.
                
NOTE: in  case active set has N  active  constraints  (or  more),  descent
      direction is forced to be exactly zero.

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sasconstraineddescent(sactiveset* state,
     /* Real    */ ae_vector* g,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{


    ae_assert(state->algostate==1, "SASConstrainedDescent: is not in optimization mode", _state);
    sasrebuildbasis(state, _state);
    sactivesets_constraineddescent(state, g, &state->unitdiagonal, &state->ibasis, ae_true, d, _state);
}


/*************************************************************************
This  subroutine  calculates  preconditioned  descent direction subject to
current active set.

INPUT PARAMETERS:
    S       -   active set object
    G       -   array[N], gradient
    D       -   possibly prealocated buffer;
                automatically resized if needed.
    
OUTPUT PARAMETERS:
    D       -   descent direction projected onto current active set.
                Components of D which correspond to active boundary
                constraints are forced to be exactly zero.
                In case D is non-zero, it is normalized to have unit norm.
                
NOTE: in  case active set has N  active  constraints  (or  more),  descent
      direction is forced to be exactly zero.

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sasconstraineddescentprec(sactiveset* state,
     /* Real    */ ae_vector* g,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{


    ae_assert(state->algostate==1, "SASConstrainedDescentPrec: is not in optimization mode", _state);
    sasrebuildbasis(state, _state);
    sactivesets_constraineddescent(state, g, &state->h, &state->pbasis, ae_true, d, _state);
}


/*************************************************************************
This subroutine calculates product of direction vector and  preconditioner
multiplied subject to current active set.

INPUT PARAMETERS:
    S       -   active set object
    D       -   array[N], direction
    
OUTPUT PARAMETERS:
    D       -   preconditioned direction projected onto current active set.
                Components of D which correspond to active boundary
                constraints are forced to be exactly zero.
                
NOTE: in  case active set has N  active  constraints  (or  more),  descent
      direction is forced to be exactly zero.

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sasconstraineddirection(sactiveset* state,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(state->algostate==1, "SASConstrainedAntigradientPrec: is not in optimization mode", _state);
    sasrebuildbasis(state, _state);
    sactivesets_constraineddescent(state, d, &state->unitdiagonal, &state->ibasis, ae_false, &state->cdtmp, _state);
    for(i=0; i<=state->n-1; i++)
    {
        d->ptr.p_double[i] = -state->cdtmp.ptr.p_double[i];
    }
}


/*************************************************************************
This subroutine calculates product of direction vector and  preconditioner
multiplied subject to current active set.

INPUT PARAMETERS:
    S       -   active set object
    D       -   array[N], direction
    
OUTPUT PARAMETERS:
    D       -   preconditioned direction projected onto current active set.
                Components of D which correspond to active boundary
                constraints are forced to be exactly zero.
                
NOTE: in  case active set has N  active  constraints  (or  more),  descent
      direction is forced to be exactly zero.

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sasconstraineddirectionprec(sactiveset* state,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(state->algostate==1, "SASConstrainedAntigradientPrec: is not in optimization mode", _state);
    sasrebuildbasis(state, _state);
    sactivesets_constraineddescent(state, d, &state->h, &state->pbasis, ae_false, &state->cdtmp, _state);
    for(i=0; i<=state->n-1; i++)
    {
        d->ptr.p_double[i] = -state->cdtmp.ptr.p_double[i];
    }
}


/*************************************************************************
This  subroutine  performs  correction of some (possibly infeasible) point
with respect to a) current active set, b) all boundary  constraints,  both
active and inactive:

0) we calculate L1 penalty term for violation of active linear constraints
   (one which is returned by SASActiveLCPenalty1() function).
1) first, it performs projection (orthogonal with respect to scale  matrix
   S) of X into current active set: X -> X1.
2) next, we perform projection with respect to  ALL  boundary  constraints
   which are violated at X1: X1 -> X2.
3) X is replaced by X2.

The idea is that this function can preserve and enforce feasibility during
optimization, and additional penalty parameter can be used to prevent algo
from leaving feasible set because of rounding errors.

INPUT PARAMETERS:
    S       -   active set object
    X       -   array[N], candidate point
    
OUTPUT PARAMETERS:
    X       -   "improved" candidate point:
                a) feasible with respect to all boundary constraints
                b) feasibility with respect to active set is retained at
                   good level.
    Penalty -   penalty term, which can be added to function value if user
                wants to penalize violation of constraints (recommended).
                
NOTE: this function is not intended to find exact  projection  (i.e.  best
      approximation) of X into feasible set. It just improves situation  a
      bit.
      Regular  use  of   this function will help you to retain feasibility
      - if you already have something to start  with  and  constrain  your
      steps is such way that the only source of infeasibility are roundoff
      errors.

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sascorrection(sactiveset* state,
     /* Real    */ ae_vector* x,
     double* penalty,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    double v;

    *penalty = 0;

    ae_assert(state->algostate==1, "SASCorrection: is not in optimization mode", _state);
    sasrebuildbasis(state, _state);
    n = state->n;
    rvectorsetlengthatleast(&state->corrtmp, n, _state);
    
    /*
     * Calculate penalty term.
     */
    *penalty = sasactivelcpenalty1(state, x, _state);
    
    /*
     * Perform projection 1.
     *
     * This projecton is given by:
     *
     *     x_proj = x - S*S*As'*(As*x-b)
     *
     * where x is original x before projection, S is a scale matrix,
     * As is a matrix of equality constraints (active set) which were
     * orthogonalized with respect to inner product given by S (i.e. we
     * have As*S*S'*As'=I), b is a right part of the orthogonalized
     * constraints.
     *
     * NOTE: you can verify that x_proj is strictly feasible w.r.t.
     *       active set by multiplying it by As - you will get
     *       As*x_proj = As*x - As*x + b = b.
     *
     *       This formula for projection can be obtained by solving
     *       following minimization problem.
     *
     *           min ||inv(S)*(x_proj-x)||^2 s.t. As*x_proj=b
     *       
     */
    ae_v_move(&state->corrtmp.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
    for(i=0; i<=state->basissize-1; i++)
    {
        v = -state->sbasis.ptr.pp_double[i][n];
        for(j=0; j<=n-1; j++)
        {
            v = v+state->sbasis.ptr.pp_double[i][j]*state->corrtmp.ptr.p_double[j];
        }
        for(j=0; j<=n-1; j++)
        {
            state->corrtmp.ptr.p_double[j] = state->corrtmp.ptr.p_double[j]-v*state->sbasis.ptr.pp_double[i][j]*ae_sqr(state->s.ptr.p_double[j], _state);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( state->activeset.ptr.p_int[i]>0 )
        {
            state->corrtmp.ptr.p_double[i] = state->xc.ptr.p_double[i];
        }
    }
    
    /*
     * Perform projection 2
     */
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = state->corrtmp.ptr.p_double[i];
        if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(x->ptr.p_double[i],state->bndl.ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = state->bndl.ptr.p_double[i];
        }
        if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(x->ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = state->bndu.ptr.p_double[i];
        }
    }
}


/*************************************************************************
This  subroutine returns L1 penalty for violation of active general linear
constraints (violation of boundary or inactive linear constraints  is  not
added to penalty).

Penalty term is equal to:
    
    Penalty = SUM( Abs((C_i*x-R_i)/Alpha_i) )
    
Here:
* summation is performed for I=0...NEC+NIC-1, ActiveSet[N+I]>0
  (only for rows of CLEIC which are in active set)
* C_i is I-th row of CLEIC
* R_i is corresponding right part
* S is a scale matrix
* Alpha_i = ||S*C_i|| - is a scaling coefficient which "normalizes"
  I-th summation term according to its scale.

INPUT PARAMETERS:
    S       -   active set object
    X       -   array[N], candidate point

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
double sasactivelcpenalty1(sactiveset* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    double v;
    double alpha;
    double p;
    double result;


    ae_assert(state->algostate==1, "SASActiveLCPenalty1: is not in optimization mode", _state);
    sasrebuildbasis(state, _state);
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    
    /*
     * Calculate penalty term.
     */
    result = 0;
    for(i=0; i<=nec+nic-1; i++)
    {
        if( state->activeset.ptr.p_int[n+i]>0 )
        {
            alpha = 0;
            p = -state->cleic.ptr.pp_double[i][n];
            for(j=0; j<=n-1; j++)
            {
                v = state->cleic.ptr.pp_double[i][j];
                p = p+v*x->ptr.p_double[j];
                alpha = alpha+ae_sqr(v*state->s.ptr.p_double[j], _state);
            }
            alpha = ae_sqrt(alpha, _state);
            if( ae_fp_neq(alpha,0) )
            {
                result = result+ae_fabs(p/alpha, _state);
            }
        }
    }
    return result;
}


/*************************************************************************
This subroutine calculates scaled norm of  vector  after  projection  onto
subspace of active constraints. Most often this function is used  to  test
stopping conditions.

INPUT PARAMETERS:
    S       -   active set object
    D       -   vector whose norm is calculated
    
RESULT:
    Vector norm (after projection and scaling)
    
NOTE: projection is performed first, scaling is performed after projection

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
double sasscaledconstrainednorm(sactiveset* state,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    double v;
    double result;


    ae_assert(state->algostate==1, "SASMoveTo: is not in optimization mode", _state);
    n = state->n;
    rvectorsetlengthatleast(&state->scntmp, n, _state);
    
    /*
     * Prepare basis (if needed)
     */
    sasrebuildbasis(state, _state);
    
    /*
     * Calculate descent direction
     */
    for(i=0; i<=n-1; i++)
    {
        if( state->activeset.ptr.p_int[i]>0 )
        {
            state->scntmp.ptr.p_double[i] = 0;
        }
        else
        {
            state->scntmp.ptr.p_double[i] = d->ptr.p_double[i];
        }
    }
    for(i=0; i<=state->basissize-1; i++)
    {
        v = ae_v_dotproduct(&state->ibasis.ptr.pp_double[i][0], 1, &state->scntmp.ptr.p_double[0], 1, ae_v_len(0,n-1));
        ae_v_subd(&state->scntmp.ptr.p_double[0], 1, &state->ibasis.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
    }
    v = 0.0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->s.ptr.p_double[i]*state->scntmp.ptr.p_double[i], _state);
    }
    result = ae_sqrt(v, _state);
    return result;
}


/*************************************************************************
This subroutine turns off optimization mode.

INPUT PARAMETERS:
    S   -   active set object
    
OUTPUT PARAMETERS:
    S   -   state is changed

NOTE: this function can be called many times for optimizer which was
      already stopped.

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
void sasstopoptimization(sactiveset* state, ae_state *_state)
{


    state->algostate = 0;
}


/*************************************************************************
This function recalculates constraints - activates  and  deactivates  them
according to gradient value at current point. Algorithm  assumes  that  we
want to make steepest descent step from  current  point;  constraints  are
activated and deactivated in such way that we won't violate any constraint
by steepest descent step.

After call to this function active set is ready to  try  steepest  descent
step (SASDescentDirection-SASExploreDirection-SASMoveTo).

Only already "active" and "candidate" elements of ActiveSet are  examined;
constraints which are not active are not examined.

INPUT PARAMETERS:
    State       -   active set object
    GC          -   array[N], gradient at XC
    
OUTPUT PARAMETERS:
    State       -   active set object, with new set of constraint

  -- ALGLIB --
     Copyright 26.09.2012 by Bochkanov Sergey
*************************************************************************/
void sasreactivateconstraints(sactiveset* state,
     /* Real    */ ae_vector* gc,
     ae_state *_state)
{


    ae_assert(state->algostate==1, "SASReactivateConstraints: must be in optimization mode", _state);
    sactivesets_reactivateconstraints(state, gc, &state->unitdiagonal, _state);
}


/*************************************************************************
This function recalculates constraints - activates  and  deactivates  them
according to gradient value at current point.

Algorithm  assumes  that  we  want  to make Quasi-Newton step from current
point with diagonal Quasi-Newton matrix H. Constraints are  activated  and
deactivated in such way that we won't violate any constraint by step.

After call to  this  function  active set is ready to  try  preconditioned
steepest descent step (SASDescentDirection-SASExploreDirection-SASMoveTo).

Only already "active" and "candidate" elements of ActiveSet are  examined;
constraints which are not active are not examined.

INPUT PARAMETERS:
    State       -   active set object
    GC          -   array[N], gradient at XC
    
OUTPUT PARAMETERS:
    State       -   active set object, with new set of constraint

  -- ALGLIB --
     Copyright 26.09.2012 by Bochkanov Sergey
*************************************************************************/
void sasreactivateconstraintsprec(sactiveset* state,
     /* Real    */ ae_vector* gc,
     ae_state *_state)
{


    ae_assert(state->algostate==1, "SASReactivateConstraintsPrec: must be in optimization mode", _state);
    sactivesets_reactivateconstraints(state, gc, &state->h, _state);
}


/*************************************************************************
This function builds three orthonormal basises for current active set:
* P-orthogonal one, which is orthogonalized with inner product
  (x,y) = x'*P*y, where P=inv(H) is current preconditioner
* S-orthogonal one, which is orthogonalized with inner product
  (x,y) = x'*S'*S*y, where S is diagonal scaling matrix
* I-orthogonal one, which is orthogonalized with standard dot product

NOTE: all sets of orthogonal vectors are guaranteed  to  have  same  size.
      P-orthogonal basis is built first, I/S-orthogonal basises are forced
      to have same number of vectors as P-orthogonal one (padded  by  zero
      vectors if needed).
      
NOTE: this function tracks changes in active set; first call  will  result
      in reorthogonalization

INPUT PARAMETERS:
    State   -   active set object
    H       -   diagonal preconditioner, H[i]>0

OUTPUT PARAMETERS:
    State   -   active set object with new basis
    
  -- ALGLIB --
     Copyright 20.06.2012 by Bochkanov Sergey
*************************************************************************/
void sasrebuildbasis(sactiveset* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t i;
    ae_int_t j;
    ae_int_t t;
    ae_int_t nactivelin;
    ae_int_t nactivebnd;
    double v;
    double vmax;
    ae_int_t kmax;


    if( state->basisisready )
    {
        return;
    }
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    rmatrixsetlengthatleast(&state->tmpbasis, nec+nic, n+1, _state);
    state->basissize = 0;
    state->basisisready = ae_true;
    
    /*
     * Determine number of active boundary and non-boundary
     * constraints, move them to TmpBasis. Quick exit if no
     * non-boundary constraints were detected.
     */
    nactivelin = 0;
    nactivebnd = 0;
    for(i=0; i<=nec+nic-1; i++)
    {
        if( state->activeset.ptr.p_int[n+i]>0 )
        {
            nactivelin = nactivelin+1;
        }
    }
    for(j=0; j<=n-1; j++)
    {
        if( state->activeset.ptr.p_int[j]>0 )
        {
            nactivebnd = nactivebnd+1;
        }
    }
    if( nactivelin==0 )
    {
        return;
    }
    
    /*
     * Orthogonalize linear constraints (inner product is given by preconditioner)
     * with respect to each other and boundary ones:
     * * normalize all constraints
     * * orthogonalize with respect to boundary ones
     * * repeat:
     *   * if basisSize+nactivebnd=n - TERMINATE
     *   * choose largest row from TmpBasis
     *   * if row norm is too small  - TERMINATE
     *   * add row to basis, normalize
     *   * remove from TmpBasis, orthogonalize other constraints with respect to this one
     */
    nactivelin = 0;
    for(i=0; i<=nec+nic-1; i++)
    {
        if( state->activeset.ptr.p_int[n+i]>0 )
        {
            ae_v_move(&state->tmpbasis.ptr.pp_double[nactivelin][0], 1, &state->cleic.ptr.pp_double[i][0], 1, ae_v_len(0,n));
            nactivelin = nactivelin+1;
        }
    }
    for(i=0; i<=nactivelin-1; i++)
    {
        v = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = v+ae_sqr(state->tmpbasis.ptr.pp_double[i][j], _state)/state->h.ptr.p_double[j];
        }
        if( ae_fp_greater(v,0) )
        {
            v = 1/ae_sqrt(v, _state);
            for(j=0; j<=n; j++)
            {
                state->tmpbasis.ptr.pp_double[i][j] = state->tmpbasis.ptr.pp_double[i][j]*v;
            }
        }
    }
    for(j=0; j<=n-1; j++)
    {
        if( state->activeset.ptr.p_int[j]>0 )
        {
            for(i=0; i<=nactivelin-1; i++)
            {
                state->tmpbasis.ptr.pp_double[i][n] = state->tmpbasis.ptr.pp_double[i][n]-state->tmpbasis.ptr.pp_double[i][j]*state->xc.ptr.p_double[j];
                state->tmpbasis.ptr.pp_double[i][j] = 0.0;
            }
        }
    }
    while(state->basissize+nactivebnd<n)
    {
        
        /*
         * Find largest vector, add to basis
         */
        vmax = -1;
        kmax = -1;
        for(i=0; i<=nactivelin-1; i++)
        {
            v = 0.0;
            for(j=0; j<=n-1; j++)
            {
                v = v+ae_sqr(state->tmpbasis.ptr.pp_double[i][j], _state)/state->h.ptr.p_double[j];
            }
            v = ae_sqrt(v, _state);
            if( ae_fp_greater(v,vmax) )
            {
                vmax = v;
                kmax = i;
            }
        }
        if( ae_fp_less(vmax,1.0E4*ae_machineepsilon) )
        {
            break;
        }
        v = 1/vmax;
        ae_v_moved(&state->pbasis.ptr.pp_double[state->basissize][0], 1, &state->tmpbasis.ptr.pp_double[kmax][0], 1, ae_v_len(0,n), v);
        state->basissize = state->basissize+1;
        
        /*
         * Reorthogonalize other vectors with respect to chosen one.
         * Remove it from the array.
         */
        for(i=0; i<=nactivelin-1; i++)
        {
            if( i!=kmax )
            {
                v = 0;
                for(j=0; j<=n-1; j++)
                {
                    v = v+state->pbasis.ptr.pp_double[state->basissize-1][j]*state->tmpbasis.ptr.pp_double[i][j]/state->h.ptr.p_double[j];
                }
                ae_v_subd(&state->tmpbasis.ptr.pp_double[i][0], 1, &state->pbasis.ptr.pp_double[state->basissize-1][0], 1, ae_v_len(0,n), v);
            }
        }
        for(j=0; j<=n; j++)
        {
            state->tmpbasis.ptr.pp_double[kmax][j] = 0;
        }
    }
    
    /*
     * Orthogonalize linear constraints using traditional dot product
     * with respect to each other and boundary ones.
     *
     * NOTE: we force basis size to be equal to one which was computed
     *       at the previous step, with preconditioner-based inner product.
     */
    nactivelin = 0;
    for(i=0; i<=nec+nic-1; i++)
    {
        if( state->activeset.ptr.p_int[n+i]>0 )
        {
            ae_v_move(&state->tmpbasis.ptr.pp_double[nactivelin][0], 1, &state->cleic.ptr.pp_double[i][0], 1, ae_v_len(0,n));
            nactivelin = nactivelin+1;
        }
    }
    for(i=0; i<=nactivelin-1; i++)
    {
        v = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = v+ae_sqr(state->tmpbasis.ptr.pp_double[i][j], _state);
        }
        if( ae_fp_greater(v,0) )
        {
            v = 1/ae_sqrt(v, _state);
            for(j=0; j<=n; j++)
            {
                state->tmpbasis.ptr.pp_double[i][j] = state->tmpbasis.ptr.pp_double[i][j]*v;
            }
        }
    }
    for(j=0; j<=n-1; j++)
    {
        if( state->activeset.ptr.p_int[j]>0 )
        {
            for(i=0; i<=nactivelin-1; i++)
            {
                state->tmpbasis.ptr.pp_double[i][n] = state->tmpbasis.ptr.pp_double[i][n]-state->tmpbasis.ptr.pp_double[i][j]*state->xc.ptr.p_double[j];
                state->tmpbasis.ptr.pp_double[i][j] = 0.0;
            }
        }
    }
    for(t=0; t<=state->basissize-1; t++)
    {
        
        /*
         * Find largest vector, add to basis.
         */
        vmax = -1;
        kmax = -1;
        for(i=0; i<=nactivelin-1; i++)
        {
            v = 0.0;
            for(j=0; j<=n-1; j++)
            {
                v = v+ae_sqr(state->tmpbasis.ptr.pp_double[i][j], _state);
            }
            v = ae_sqrt(v, _state);
            if( ae_fp_greater(v,vmax) )
            {
                vmax = v;
                kmax = i;
            }
        }
        if( ae_fp_eq(vmax,0) )
        {
            for(j=0; j<=n; j++)
            {
                state->ibasis.ptr.pp_double[t][j] = 0.0;
            }
            continue;
        }
        v = 1/vmax;
        ae_v_moved(&state->ibasis.ptr.pp_double[t][0], 1, &state->tmpbasis.ptr.pp_double[kmax][0], 1, ae_v_len(0,n), v);
        
        /*
         * Reorthogonalize other vectors with respect to chosen one.
         * Remove it from the array.
         */
        for(i=0; i<=nactivelin-1; i++)
        {
            if( i!=kmax )
            {
                v = 0;
                for(j=0; j<=n-1; j++)
                {
                    v = v+state->ibasis.ptr.pp_double[t][j]*state->tmpbasis.ptr.pp_double[i][j];
                }
                ae_v_subd(&state->tmpbasis.ptr.pp_double[i][0], 1, &state->ibasis.ptr.pp_double[t][0], 1, ae_v_len(0,n), v);
            }
        }
        for(j=0; j<=n; j++)
        {
            state->tmpbasis.ptr.pp_double[kmax][j] = 0;
        }
    }
    
    /*
     * Orthogonalize linear constraints using inner product given by
     * scale matrix.
     *
     * NOTE: we force basis size to be equal to one which was computed
     *       with preconditioner-based inner product.
     */
    nactivelin = 0;
    for(i=0; i<=nec+nic-1; i++)
    {
        if( state->activeset.ptr.p_int[n+i]>0 )
        {
            ae_v_move(&state->tmpbasis.ptr.pp_double[nactivelin][0], 1, &state->cleic.ptr.pp_double[i][0], 1, ae_v_len(0,n));
            nactivelin = nactivelin+1;
        }
    }
    for(i=0; i<=nactivelin-1; i++)
    {
        v = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = v+ae_sqr(state->tmpbasis.ptr.pp_double[i][j]*state->s.ptr.p_double[j], _state);
        }
        if( ae_fp_greater(v,0) )
        {
            v = 1/ae_sqrt(v, _state);
            for(j=0; j<=n; j++)
            {
                state->tmpbasis.ptr.pp_double[i][j] = state->tmpbasis.ptr.pp_double[i][j]*v;
            }
        }
    }
    for(j=0; j<=n-1; j++)
    {
        if( state->activeset.ptr.p_int[j]>0 )
        {
            for(i=0; i<=nactivelin-1; i++)
            {
                state->tmpbasis.ptr.pp_double[i][n] = state->tmpbasis.ptr.pp_double[i][n]-state->tmpbasis.ptr.pp_double[i][j]*state->xc.ptr.p_double[j];
                state->tmpbasis.ptr.pp_double[i][j] = 0.0;
            }
        }
    }
    for(t=0; t<=state->basissize-1; t++)
    {
        
        /*
         * Find largest vector, add to basis.
         */
        vmax = -1;
        kmax = -1;
        for(i=0; i<=nactivelin-1; i++)
        {
            v = 0.0;
            for(j=0; j<=n-1; j++)
            {
                v = v+ae_sqr(state->tmpbasis.ptr.pp_double[i][j]*state->s.ptr.p_double[j], _state);
            }
            v = ae_sqrt(v, _state);
            if( ae_fp_greater(v,vmax) )
            {
                vmax = v;
                kmax = i;
            }
        }
        if( ae_fp_eq(vmax,0) )
        {
            for(j=0; j<=n; j++)
            {
                state->sbasis.ptr.pp_double[t][j] = 0.0;
            }
            continue;
        }
        v = 1/vmax;
        ae_v_moved(&state->sbasis.ptr.pp_double[t][0], 1, &state->tmpbasis.ptr.pp_double[kmax][0], 1, ae_v_len(0,n), v);
        
        /*
         * Reorthogonalize other vectors with respect to chosen one.
         * Remove it from the array.
         */
        for(i=0; i<=nactivelin-1; i++)
        {
            if( i!=kmax )
            {
                v = 0;
                for(j=0; j<=n-1; j++)
                {
                    v = v+state->sbasis.ptr.pp_double[t][j]*state->tmpbasis.ptr.pp_double[i][j]*ae_sqr(state->s.ptr.p_double[j], _state);
                }
                ae_v_subd(&state->tmpbasis.ptr.pp_double[i][0], 1, &state->sbasis.ptr.pp_double[t][0], 1, ae_v_len(0,n), v);
            }
        }
        for(j=0; j<=n; j++)
        {
            state->tmpbasis.ptr.pp_double[kmax][j] = 0;
        }
    }
}


/*************************************************************************
This  subroutine  calculates  preconditioned  descent direction subject to
current active set.

INPUT PARAMETERS:
    State   -   active set object
    G       -   array[N], gradient
    H       -   array[N], Hessian matrix
    HA      -   active constraints orthogonalized in such way
                that HA*inv(H)*HA'= I.
    Normalize-  whether we need normalized descent or not
    D       -   possibly preallocated buffer; automatically resized.
    
OUTPUT PARAMETERS:
    D       -   descent direction projected onto current active set.
                Components of D which correspond to active boundary
                constraints are forced to be exactly zero.
                In case D is non-zero and Normalize is True, it is
                normalized to have unit norm.

  -- ALGLIB --
     Copyright 21.12.2012 by Bochkanov Sergey
*************************************************************************/
static void sactivesets_constraineddescent(sactiveset* state,
     /* Real    */ ae_vector* g,
     /* Real    */ ae_vector* h,
     /* Real    */ ae_matrix* ha,
     ae_bool normalize,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    double v;
    ae_int_t nactive;


    ae_assert(state->algostate==1, "SAS: internal error in ConstrainedDescent() - not in optimization mode", _state);
    ae_assert(state->basisisready, "SAS: internal error in ConstrainedDescent() - no basis", _state);
    n = state->n;
    rvectorsetlengthatleast(d, n, _state);
    
    /*
     * Calculate preconditioned constrained descent direction:
     *
     *     d := -inv(H)*( g - HA'*(HA*inv(H)*g) )
     *
     * Formula above always gives direction which is orthogonal to rows of HA.
     * You can verify it by multiplication of both sides by HA[i] (I-th row),
     * taking into account that HA*inv(H)*HA'= I (by definition of HA - it is
     * orthogonal basis with inner product given by inv(H)).
     */
    nactive = 0;
    for(i=0; i<=n-1; i++)
    {
        if( state->activeset.ptr.p_int[i]>0 )
        {
            d->ptr.p_double[i] = 0;
            nactive = nactive+1;
        }
        else
        {
            d->ptr.p_double[i] = g->ptr.p_double[i];
        }
    }
    for(i=0; i<=state->basissize-1; i++)
    {
        v = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = v+ha->ptr.pp_double[i][j]*d->ptr.p_double[j]/h->ptr.p_double[j];
        }
        ae_v_subd(&d->ptr.p_double[0], 1, &ha->ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
        nactive = nactive+1;
    }
    v = 0.0;
    for(i=0; i<=n-1; i++)
    {
        if( state->activeset.ptr.p_int[i]>0 )
        {
            d->ptr.p_double[i] = 0;
        }
        else
        {
            d->ptr.p_double[i] = -d->ptr.p_double[i]/h->ptr.p_double[i];
            v = v+ae_sqr(d->ptr.p_double[i], _state);
        }
    }
    v = ae_sqrt(v, _state);
    if( nactive>=n )
    {
        v = 0;
        for(i=0; i<=n-1; i++)
        {
            d->ptr.p_double[i] = 0;
        }
    }
    if( normalize&&ae_fp_greater(v,0) )
    {
        for(i=0; i<=n-1; i++)
        {
            d->ptr.p_double[i] = d->ptr.p_double[i]/v;
        }
    }
}


/*************************************************************************
This function recalculates constraints - activates  and  deactivates  them
according to gradient value at current point.

Algorithm  assumes  that  we  want  to make Quasi-Newton step from current
point with diagonal Quasi-Newton matrix H. Constraints are  activated  and
deactivated in such way that we won't violate any constraint by step.

Only already "active" and "candidate" elements of ActiveSet are  examined;
constraints which are not active are not examined.

INPUT PARAMETERS:
    State       -   active set object
    GC          -   array[N], gradient at XC
    H           -   array[N], Hessian matrix
    
OUTPUT PARAMETERS:
    State       -   active set object, with new set of constraint

  -- ALGLIB --
     Copyright 26.09.2012 by Bochkanov Sergey
*************************************************************************/
static void sactivesets_reactivateconstraints(sactiveset* state,
     /* Real    */ ae_vector* gc,
     /* Real    */ ae_vector* h,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t i;
    ae_int_t j;
    ae_int_t idx0;
    ae_int_t idx1;
    double v;
    ae_int_t nactivebnd;
    ae_int_t nactivelin;
    ae_int_t nactiveconstraints;
    double rowscale;


    ae_assert(state->algostate==1, "SASReactivateConstraintsPrec: must be in optimization mode", _state);
    
    /*
     * Prepare
     */
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    state->basisisready = ae_false;
    
    /*
     * Handle important special case - no linear constraints,
     * only boundary constraints are present
     */
    if( nec+nic==0 )
    {
        for(i=0; i<=n-1; i++)
        {
            if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->activeset.ptr.p_int[i] = 1;
                continue;
            }
            if( (state->hasbndl.ptr.p_bool[i]&&ae_fp_eq(state->xc.ptr.p_double[i],state->bndl.ptr.p_double[i]))&&ae_fp_greater_eq(gc->ptr.p_double[i],0) )
            {
                state->activeset.ptr.p_int[i] = 1;
                continue;
            }
            if( (state->hasbndu.ptr.p_bool[i]&&ae_fp_eq(state->xc.ptr.p_double[i],state->bndu.ptr.p_double[i]))&&ae_fp_less_eq(gc->ptr.p_double[i],0) )
            {
                state->activeset.ptr.p_int[i] = 1;
                continue;
            }
            state->activeset.ptr.p_int[i] = -1;
        }
        return;
    }
    
    /*
     * General case.
     * Allocate temporaries.
     */
    rvectorsetlengthatleast(&state->rctmpg, n, _state);
    rvectorsetlengthatleast(&state->rctmprightpart, n, _state);
    rvectorsetlengthatleast(&state->rctmps, n, _state);
    rmatrixsetlengthatleast(&state->rctmpdense0, n, nec+nic, _state);
    rmatrixsetlengthatleast(&state->rctmpdense1, n, nec+nic, _state);
    bvectorsetlengthatleast(&state->rctmpisequality, n+nec+nic, _state);
    ivectorsetlengthatleast(&state->rctmpconstraintidx, n+nec+nic, _state);
    
    /*
     * Calculate descent direction
     */
    ae_v_moveneg(&state->rctmpg.ptr.p_double[0], 1, &gc->ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * Determine candidates to the active set.
     *
     * After this block constraints become either "inactive" (ActiveSet[i]<0)
     * or "candidates" (ActiveSet[i]=0). Previously active constraints always
     * become "candidates".
     */
    for(i=0; i<=n+nec+nic-1; i++)
    {
        if( state->activeset.ptr.p_int[i]>0 )
        {
            state->activeset.ptr.p_int[i] = 0;
        }
        else
        {
            state->activeset.ptr.p_int[i] = -1;
        }
    }
    nactiveconstraints = 0;
    nactivebnd = 0;
    nactivelin = 0;
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Activate boundary constraints:
         * * copy constraint index to RCTmpConstraintIdx
         * * set corresponding element of ActiveSet[] to "candidate"
         * * fill RCTmpS by either +1 (lower bound) or -1 (upper bound)
         * * set RCTmpIsEquality to False (BndL<BndU) or True (BndL=BndU)
         * * increase counters
         */
        if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint is activated
             */
            state->rctmpconstraintidx.ptr.p_int[nactiveconstraints] = i;
            state->activeset.ptr.p_int[i] = 0;
            state->rctmps.ptr.p_double[i] = 1.0;
            state->rctmpisequality.ptr.p_bool[nactiveconstraints] = ae_true;
            nactiveconstraints = nactiveconstraints+1;
            nactivebnd = nactivebnd+1;
            continue;
        }
        if( state->hasbndl.ptr.p_bool[i]&&ae_fp_eq(state->xc.ptr.p_double[i],state->bndl.ptr.p_double[i]) )
        {
            
            /*
             * Lower bound is activated
             */
            state->rctmpconstraintidx.ptr.p_int[nactiveconstraints] = i;
            state->activeset.ptr.p_int[i] = 0;
            state->rctmps.ptr.p_double[i] = -1.0;
            state->rctmpisequality.ptr.p_bool[nactiveconstraints] = ae_false;
            nactiveconstraints = nactiveconstraints+1;
            nactivebnd = nactivebnd+1;
            continue;
        }
        if( state->hasbndu.ptr.p_bool[i]&&ae_fp_eq(state->xc.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            
            /*
             * Upper bound is activated
             */
            state->rctmpconstraintidx.ptr.p_int[nactiveconstraints] = i;
            state->activeset.ptr.p_int[i] = 0;
            state->rctmps.ptr.p_double[i] = 1.0;
            state->rctmpisequality.ptr.p_bool[nactiveconstraints] = ae_false;
            nactiveconstraints = nactiveconstraints+1;
            nactivebnd = nactivebnd+1;
            continue;
        }
    }
    for(i=0; i<=nec+nic-1; i++)
    {
        if( i>=nec )
        {
            
            /*
             * Inequality constraints are skipped if we too far away from
             * the boundary.
             */
            rowscale = 0.0;
            v = -state->cleic.ptr.pp_double[i][n];
            for(j=0; j<=n-1; j++)
            {
                v = v+state->cleic.ptr.pp_double[i][j]*state->xc.ptr.p_double[j];
                rowscale = ae_maxreal(rowscale, ae_fabs(state->cleic.ptr.pp_double[i][j]*state->s.ptr.p_double[j], _state), _state);
            }
            if( ae_fp_less_eq(v,-1.0E5*ae_machineepsilon*rowscale) )
            {
                
                /*
                 * NOTE: it is important to check for non-strict inequality
                 *       because we have to correctly handle zero constraint
                 *       0*x<=0
                 */
                continue;
            }
        }
        ae_v_move(&state->rctmpdense0.ptr.pp_double[0][nactivelin], state->rctmpdense0.stride, &state->cleic.ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
        state->rctmpconstraintidx.ptr.p_int[nactiveconstraints] = n+i;
        state->activeset.ptr.p_int[n+i] = 0;
        state->rctmpisequality.ptr.p_bool[nactiveconstraints] = i<nec;
        nactiveconstraints = nactiveconstraints+1;
        nactivelin = nactivelin+1;
    }
    
    /*
     * Skip if no "candidate" constraints was found
     */
    if( nactiveconstraints==0 )
    {
        for(i=0; i<=n-1; i++)
        {
            if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->activeset.ptr.p_int[i] = 1;
                continue;
            }
            if( (state->hasbndl.ptr.p_bool[i]&&ae_fp_eq(state->xc.ptr.p_double[i],state->bndl.ptr.p_double[i]))&&ae_fp_greater_eq(gc->ptr.p_double[i],0) )
            {
                state->activeset.ptr.p_int[i] = 1;
                continue;
            }
            if( (state->hasbndu.ptr.p_bool[i]&&ae_fp_eq(state->xc.ptr.p_double[i],state->bndu.ptr.p_double[i]))&&ae_fp_less_eq(gc->ptr.p_double[i],0) )
            {
                state->activeset.ptr.p_int[i] = 1;
                continue;
            }
        }
        return;
    }
    
    /*
     * General case.
     *
     * APPROACH TO CONSTRAINTS ACTIVATION/DEACTIVATION
     *
     * We have NActiveConstraints "candidates": NActiveBnd boundary candidates,
     * NActiveLin linear candidates. Indexes of boundary constraints are stored
     * in RCTmpConstraintIdx[0:NActiveBnd-1], indexes of linear ones are stored
     * in RCTmpConstraintIdx[NActiveBnd:NActiveBnd+NActiveLin-1]. Some of the
     * constraints are equality ones, some are inequality - as specified by 
     * RCTmpIsEquality[i].
     *
     * Now we have to determine active subset of "candidates" set. In order to
     * do so we solve following constrained minimization problem:
     *         (                         )^2
     *     min ( SUM(lambda[i]*A[i]) + G )
     *         (                         )
     * Here:
     * * G is a gradient (column vector)
     * * A[i] is a column vector, linear (left) part of I-th constraint.
     *   I=0..NActiveConstraints-1, first NActiveBnd elements of A are just
     *   subset of identity matrix (boundary constraints), next NActiveLin
     *   elements are subset of rows of the matrix of general linear constraints.
     * * lambda[i] is a Lagrange multiplier corresponding to I-th constraint
     *
     * NOTE: for preconditioned setting A is replaced by A*H^(-0.5), G is
     *       replaced by G*H^(-0.5). We apply this scaling at the last stage,
     *       before passing data to NNLS solver.
     *
     * Minimization is performed subject to non-negativity constraints on
     * lambda[i] corresponding to inequality constraints. Inequality constraints
     * which correspond to non-zero lambda are activated, equality constraints
     * are always considered active.
     *
     * Informally speaking, we "decompose" descent direction -G and represent
     * it as sum of constraint vectors and "residual" part (which is equal to
     * the actual descent direction subject to constraints).
     *
     * SOLUTION OF THE NNLS PROBLEM
     *
     * We solve this optimization problem with Non-Negative Least Squares solver,
     * which can efficiently solve least squares problems of the form
     *
     *         ( [ I | AU ]     )^2
     *     min ( [   |    ]*x-b )   s.t. non-negativity constraints on some x[i]
     *         ( [ 0 | AL ]     )
     *
     * In order to use this solver we have to rearrange rows of A[] and G in
     * such way that first NActiveBnd columns of A store identity matrix (before
     * sorting non-zero elements are randomly distributed in the first NActiveBnd
     * columns of A, during sorting we move them to first NActiveBnd rows).
     *
     * Then we create instance of NNLS solver (we reuse instance left from the
     * previous run of the optimization problem) and solve NNLS problem.
     */
    idx0 = 0;
    idx1 = nactivebnd;
    for(i=0; i<=n-1; i++)
    {
        if( state->activeset.ptr.p_int[i]>=0 )
        {
            v = 1/ae_sqrt(h->ptr.p_double[i], _state);
            for(j=0; j<=nactivelin-1; j++)
            {
                state->rctmpdense1.ptr.pp_double[idx0][j] = state->rctmpdense0.ptr.pp_double[i][j]/state->rctmps.ptr.p_double[i]*v;
            }
            state->rctmprightpart.ptr.p_double[idx0] = state->rctmpg.ptr.p_double[i]/state->rctmps.ptr.p_double[i]*v;
            idx0 = idx0+1;
        }
        else
        {
            v = 1/ae_sqrt(h->ptr.p_double[i], _state);
            for(j=0; j<=nactivelin-1; j++)
            {
                state->rctmpdense1.ptr.pp_double[idx1][j] = state->rctmpdense0.ptr.pp_double[i][j]*v;
            }
            state->rctmprightpart.ptr.p_double[idx1] = state->rctmpg.ptr.p_double[i]*v;
            idx1 = idx1+1;
        }
    }
    snnlsinit(n, nec+nic, n, &state->solver, _state);
    snnlssetproblem(&state->solver, &state->rctmpdense1, &state->rctmprightpart, nactivebnd, nactiveconstraints-nactivebnd, n, _state);
    for(i=0; i<=nactiveconstraints-1; i++)
    {
        if( state->rctmpisequality.ptr.p_bool[i] )
        {
            snnlsdropnnc(&state->solver, i, _state);
        }
    }
    snnlssolve(&state->solver, &state->rctmplambdas, _state);
    
    /*
     * After solution of the problem we activate equality constraints (always active)
     * and inequality constraints with non-zero Lagrange multipliers. Then we reorthogonalize
     * active constraints.
     */
    for(i=0; i<=nactiveconstraints-1; i++)
    {
        if( state->rctmpisequality.ptr.p_bool[i]||ae_fp_greater(state->rctmplambdas.ptr.p_double[i],0) )
        {
            state->activeset.ptr.p_int[state->rctmpconstraintidx.ptr.p_int[i]] = 1;
        }
        else
        {
            state->activeset.ptr.p_int[state->rctmpconstraintidx.ptr.p_int[i]] = 0;
        }
    }
    sasrebuildbasis(state, _state);
}


ae_bool _sactiveset_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sactiveset *p = (sactiveset*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_vector_init(&p->xc, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->h, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->activeset, 0, DT_INT, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->sbasis, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->pbasis, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->ibasis, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->cleic, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->mtx, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->mtas, 0, DT_INT, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->cdtmp, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->corrtmp, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->unitdiagonal, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_snnlssolver_init(&p->solver, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->scntmp, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmpfeas, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->tmpm0, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rctmps, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rctmpg, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rctmprightpart, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->rctmpdense0, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->rctmpdense1, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rctmpisequality, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rctmpconstraintidx, 0, DT_INT, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rctmplambdas, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->tmpbasis, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _sactiveset_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    sactiveset *dst = (sactiveset*)_dst;
    sactiveset *src = (sactiveset*)_src;
    dst->n = src->n;
    dst->algostate = src->algostate;
    if( !ae_vector_init_copy(&dst->xc, &src->xc, _state, make_automatic) )
        return ae_false;
    dst->hasxc = src->hasxc;
    if( !ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->h, &src->h, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->activeset, &src->activeset, _state, make_automatic) )
        return ae_false;
    dst->basisisready = src->basisisready;
    if( !ae_matrix_init_copy(&dst->sbasis, &src->sbasis, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->pbasis, &src->pbasis, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->ibasis, &src->ibasis, _state, make_automatic) )
        return ae_false;
    dst->basissize = src->basissize;
    dst->constraintschanged = src->constraintschanged;
    if( !ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->cleic, &src->cleic, _state, make_automatic) )
        return ae_false;
    dst->nec = src->nec;
    dst->nic = src->nic;
    if( !ae_vector_init_copy(&dst->mtx, &src->mtx, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->mtas, &src->mtas, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->cdtmp, &src->cdtmp, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->corrtmp, &src->corrtmp, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->unitdiagonal, &src->unitdiagonal, _state, make_automatic) )
        return ae_false;
    if( !_snnlssolver_init_copy(&dst->solver, &src->solver, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->scntmp, &src->scntmp, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmpfeas, &src->tmpfeas, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->tmpm0, &src->tmpm0, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rctmps, &src->rctmps, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rctmpg, &src->rctmpg, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rctmprightpart, &src->rctmprightpart, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->rctmpdense0, &src->rctmpdense0, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->rctmpdense1, &src->rctmpdense1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rctmpisequality, &src->rctmpisequality, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rctmpconstraintidx, &src->rctmpconstraintidx, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rctmplambdas, &src->rctmplambdas, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->tmpbasis, &src->tmpbasis, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _sactiveset_clear(void* _p)
{
    sactiveset *p = (sactiveset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->xc);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->h);
    ae_vector_clear(&p->activeset);
    ae_matrix_clear(&p->sbasis);
    ae_matrix_clear(&p->pbasis);
    ae_matrix_clear(&p->ibasis);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_matrix_clear(&p->cleic);
    ae_vector_clear(&p->mtx);
    ae_vector_clear(&p->mtas);
    ae_vector_clear(&p->cdtmp);
    ae_vector_clear(&p->corrtmp);
    ae_vector_clear(&p->unitdiagonal);
    _snnlssolver_clear(&p->solver);
    ae_vector_clear(&p->scntmp);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmpfeas);
    ae_matrix_clear(&p->tmpm0);
    ae_vector_clear(&p->rctmps);
    ae_vector_clear(&p->rctmpg);
    ae_vector_clear(&p->rctmprightpart);
    ae_matrix_clear(&p->rctmpdense0);
    ae_matrix_clear(&p->rctmpdense1);
    ae_vector_clear(&p->rctmpisequality);
    ae_vector_clear(&p->rctmpconstraintidx);
    ae_vector_clear(&p->rctmplambdas);
    ae_matrix_clear(&p->tmpbasis);
}


void _sactiveset_destroy(void* _p)
{
    sactiveset *p = (sactiveset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->xc);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->h);
    ae_vector_destroy(&p->activeset);
    ae_matrix_destroy(&p->sbasis);
    ae_matrix_destroy(&p->pbasis);
    ae_matrix_destroy(&p->ibasis);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_matrix_destroy(&p->cleic);
    ae_vector_destroy(&p->mtx);
    ae_vector_destroy(&p->mtas);
    ae_vector_destroy(&p->cdtmp);
    ae_vector_destroy(&p->corrtmp);
    ae_vector_destroy(&p->unitdiagonal);
    _snnlssolver_destroy(&p->solver);
    ae_vector_destroy(&p->scntmp);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmpfeas);
    ae_matrix_destroy(&p->tmpm0);
    ae_vector_destroy(&p->rctmps);
    ae_vector_destroy(&p->rctmpg);
    ae_vector_destroy(&p->rctmprightpart);
    ae_matrix_destroy(&p->rctmpdense0);
    ae_matrix_destroy(&p->rctmpdense1);
    ae_vector_destroy(&p->rctmpisequality);
    ae_vector_destroy(&p->rctmpconstraintidx);
    ae_vector_destroy(&p->rctmplambdas);
    ae_matrix_destroy(&p->tmpbasis);
}




/*************************************************************************
        NONLINEAR CONJUGATE GRADIENT METHOD

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by using one of  the
nonlinear conjugate gradient methods.

These CG methods are globally convergent (even on non-convex functions) as
long as grad(f) is Lipschitz continuous in  a  some  neighborhood  of  the
L = { x : f(x)<=f(x0) }.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinCGCreate() call
2. User tunes solver parameters with MinCGSetCond(), MinCGSetStpMax() and
   other functions
3. User calls MinCGOptimize() function which takes algorithm  state   and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinCGResults() to get solution
5. Optionally, user may call MinCGRestartFrom() to solve another  problem
   with same N but another starting point and/or another function.
   MinCGRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void mincgcreate(ae_int_t n,
     /* Real    */ ae_vector* x,
     mincgstate* state,
     ae_state *_state)
{

    _mincgstate_clear(state);

    ae_assert(n>=1, "MinCGCreate: N too small!", _state);
    ae_assert(x->cnt>=n, "MinCGCreate: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinCGCreate: X contains infinite or NaN values!", _state);
    mincg_mincginitinternal(n, 0.0, state, _state);
    mincgrestartfrom(state, x, _state);
}


/*************************************************************************
The subroutine is finite difference variant of MinCGCreate(). It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this function
only. We recommend to read comments on MinCGCreate() in order to get more
information about creation of CG optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinCGSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust  and  precise.  L-BFGS  needs  exact  gradient values.
   Imprecise  gradient may slow down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void mincgcreatef(ae_int_t n,
     /* Real    */ ae_vector* x,
     double diffstep,
     mincgstate* state,
     ae_state *_state)
{

    _mincgstate_clear(state);

    ae_assert(n>=1, "MinCGCreateF: N too small!", _state);
    ae_assert(x->cnt>=n, "MinCGCreateF: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinCGCreateF: X contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(diffstep, _state), "MinCGCreateF: DiffStep is infinite or NaN!", _state);
    ae_assert(ae_fp_greater(diffstep,0), "MinCGCreateF: DiffStep is non-positive!", _state);
    mincg_mincginitinternal(n, diffstep, state, _state);
    mincgrestartfrom(state, x, _state);
}


/*************************************************************************
This function sets stopping conditions for CG optimization algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinCGSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinCGSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcond(mincgstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinCGSetCond: EpsG is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsg,0), "MinCGSetCond: negative EpsG!", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinCGSetCond: EpsF is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsf,0), "MinCGSetCond: negative EpsF!", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinCGSetCond: EpsX is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsx,0), "MinCGSetCond: negative EpsX!", _state);
    ae_assert(maxits>=0, "MinCGSetCond: negative MaxIts!", _state);
    if( ((ae_fp_eq(epsg,0)&&ae_fp_eq(epsf,0))&&ae_fp_eq(epsx,0))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    state->epsg = epsg;
    state->epsf = epsf;
    state->epsx = epsx;
    state->maxits = maxits;
}


/*************************************************************************
This function sets scaling coefficients for CG optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of CG optimizer  -  step
along I-th axis is equal to DiffStep*S[I].

In   most   optimizers  (and  in  the  CG  too)  scaling is NOT a form  of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner by separate call to one of the MinCGSetPrec...() functions.

There  is  special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void mincgsetscale(mincgstate* state,
     /* Real    */ ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinCGSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinCGSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],0), "MinCGSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
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
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetxrep(mincgstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function turns on/off line search reports.
These reports are described in more details in developer-only  comments on
MinCGState object.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedDRep-   whether line search reports are needed or not

This function is intended for private use only. Turning it on artificially
may cause program failure.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetdrep(mincgstate* state, ae_bool needdrep, ae_state *_state)
{


    state->drep = needdrep;
}


/*************************************************************************
This function sets CG algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    CGType  -   algorithm type:
                * -1    automatic selection of the best algorithm
                * 0     DY (Dai and Yuan) algorithm
                * 1     Hybrid DY-HS algorithm

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcgtype(mincgstate* state, ae_int_t cgtype, ae_state *_state)
{


    ae_assert(cgtype>=-1&&cgtype<=1, "MinCGSetCGType: incorrect CGType!", _state);
    if( cgtype==-1 )
    {
        cgtype = 1;
    }
    state->cgtype = cgtype;
}


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetstpmax(mincgstate* state, double stpmax, ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "MinCGSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,0), "MinCGSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************
This function allows to suggest initial step length to the CG algorithm.

Suggested  step  length  is used as starting point for the line search. It
can be useful when you have  badly  scaled  problem,  i.e.  when  ||grad||
(which is used as initial estimate for the first step) is many  orders  of
magnitude different from the desired step.

Line search  may  fail  on  such problems without good estimate of initial
step length. Imagine, for example, problem with ||grad||=10^50 and desired
step equal to 0.1 Line  search function will use 10^50  as  initial  step,
then  it  will  decrease step length by 2 (up to 20 attempts) and will get
10^44, which is still too large.

This function allows us to tell than line search should  be  started  from
some moderate step length, like 1.0, so algorithm will be able  to  detect
desired step length in a several searches.

Default behavior (when no step is suggested) is to use preconditioner,  if
it is available, to generate initial estimate of step length.

This function influences only first iteration of algorithm. It  should  be
called between MinCGCreate/MinCGRestartFrom() call and MinCGOptimize call.
Suggested step is ignored if you have preconditioner.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    Stp     -   initial estimate of the step length.
                Can be zero (no estimate).

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsuggeststep(mincgstate* state, double stp, ae_state *_state)
{


    ae_assert(ae_isfinite(stp, _state), "MinCGSuggestStep: Stp is infinite or NAN", _state);
    ae_assert(ae_fp_greater_eq(stp,0), "MinCGSuggestStep: Stp<0", _state);
    state->suggestedstep = stp;
}


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdefault(mincgstate* state, ae_state *_state)
{


    state->prectype = 0;
    state->innerresetneeded = ae_true;
}


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2: D[i] should be positive. Exception will be thrown otherwise.

NOTE 3: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdiag(mincgstate* state,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(d->cnt>=state->n, "MinCGSetPrecDiag: D is too short", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(d->ptr.p_double[i], _state), "MinCGSetPrecDiag: D contains infinite or NAN elements", _state);
        ae_assert(ae_fp_greater(d->ptr.p_double[i],0), "MinCGSetPrecDiag: D contains non-positive elements", _state);
    }
    mincgsetprecdiagfast(state, d, _state);
}


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables with MinCGSetScale() call
(before or after MinCGSetPrecScale() call). Without knowledge of the scale
of your variables scale-based preconditioner will be just unit matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecscale(mincgstate* state, ae_state *_state)
{


    state->prectype = 3;
    state->innerresetneeded = ae_true;
}


/*************************************************************************
NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied  gradient, and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.
   
   Depending  on  the  specific  function  used to create optimizer object
   (either MinCGCreate()  for analytical gradient  or  MinCGCreateF()  for
   numerical differentiation) you should  choose  appropriate  variant  of
   MinCGOptimize() - one which accepts function AND gradient or one  which
   accepts function ONLY.

   Be careful to choose variant of MinCGOptimize()  which  corresponds  to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed  to  MinCGOptimize()  and  specific
   function used to create optimizer.
   

                  |         USER PASSED TO MinCGOptimize()
   CREATED WITH   |  function only   |  function and gradient
   ------------------------------------------------------------
   MinCGCreateF() |     work                FAIL
   MinCGCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function and MinCGOptimize() version. Attemps to use  such  combination
   (for  example,  to create optimizer with  MinCGCreateF()  and  to  pass
   gradient information to MinCGOptimize()) will lead to  exception  being
   thrown. Either  you  did  not  pass  gradient when it WAS needed or you
   passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool mincgiteration(mincgstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double betak;
    double v;
    double vv;
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
        i = state->rstate.ia.ptr.p_int[1];
        betak = state->rstate.ra.ptr.p_double[0];
        v = state->rstate.ra.ptr.p_double[1];
        vv = state->rstate.ra.ptr.p_double[2];
    }
    else
    {
        n = -983;
        i = -989;
        betak = -834;
        v = 900;
        vv = -287;
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
    if( state->rstate.stage==8 )
    {
        goto lbl_8;
    }
    if( state->rstate.stage==9 )
    {
        goto lbl_9;
    }
    if( state->rstate.stage==10 )
    {
        goto lbl_10;
    }
    if( state->rstate.stage==11 )
    {
        goto lbl_11;
    }
    if( state->rstate.stage==12 )
    {
        goto lbl_12;
    }
    if( state->rstate.stage==13 )
    {
        goto lbl_13;
    }
    if( state->rstate.stage==14 )
    {
        goto lbl_14;
    }
    if( state->rstate.stage==15 )
    {
        goto lbl_15;
    }
    if( state->rstate.stage==16 )
    {
        goto lbl_16;
    }
    if( state->rstate.stage==17 )
    {
        goto lbl_17;
    }
    if( state->rstate.stage==18 )
    {
        goto lbl_18;
    }
    if( state->rstate.stage==19 )
    {
        goto lbl_19;
    }
    
    /*
     * Routine body
     */
    
    /*
     * Prepare
     */
    n = state->n;
    state->repterminationtype = 0;
    state->repiterationscount = 0;
    state->repvaridx = -1;
    state->repnfev = 0;
    state->debugrestartscount = 0;
    
    /*
     *  Check, that transferred derivative value is right
     */
    mincg_clearrequestfields(state, _state);
    if( !(ae_fp_eq(state->diffstep,0)&&ae_fp_greater(state->teststep,0)) )
    {
        goto lbl_20;
    }
    state->needfg = ae_true;
    i = 0;
lbl_22:
    if( i>n-1 )
    {
        goto lbl_24;
    }
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->teststep*state->s.ptr.p_double[i];
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->fm1 = state->f;
    state->fp1 = state->g.ptr.p_double[i];
    state->x.ptr.p_double[i] = v+state->teststep*state->s.ptr.p_double[i];
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->fm2 = state->f;
    state->fp2 = state->g.ptr.p_double[i];
    state->x.ptr.p_double[i] = v;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    
    /*
     * 2*State.TestStep   -   scale parameter
     * width of segment [Xi-TestStep;Xi+TestStep]
     */
    if( !derivativecheck(state->fm1, state->fp1, state->fm2, state->fp2, state->f, state->g.ptr.p_double[i], 2*state->teststep, _state) )
    {
        state->repvaridx = i;
        state->repterminationtype = -7;
        result = ae_false;
        return result;
    }
    i = i+1;
    goto lbl_22;
lbl_24:
    state->needfg = ae_false;
lbl_20:
    
    /*
     * Preparations continue:
     * * set XK
     * * calculate F/G
     * * set DK to -G
     * * powerup algo (it may change preconditioner)
     * * apply preconditioner to DK
     * * report update of X
     * * check stopping conditions for G
     */
    ae_v_move(&state->xk.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->terminationneeded = ae_false;
    mincg_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,0) )
    {
        goto lbl_25;
    }
    state->needfg = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needfg = ae_false;
    goto lbl_26;
lbl_25:
    state->needf = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->fbase = state->f;
    i = 0;
lbl_27:
    if( i>n-1 )
    {
        goto lbl_29;
    }
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->fp2 = state->f;
    state->x.ptr.p_double[i] = v;
    state->g.ptr.p_double[i] = (8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/(6*state->diffstep*state->s.ptr.p_double[i]);
    i = i+1;
    goto lbl_27;
lbl_29:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_26:
    if( !state->drep )
    {
        goto lbl_30;
    }
    
    /*
     * Report algorithm powerup (if needed)
     */
    mincg_clearrequestfields(state, _state);
    state->algpowerup = ae_true;
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->algpowerup = ae_false;
lbl_30:
    trimprepare(state->f, &state->trimthreshold, _state);
    ae_v_moveneg(&state->dk.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    mincg_preconditionedmultiply(state, &state->dk, &state->work0, &state->work1, _state);
    if( !state->xrep )
    {
        goto lbl_32;
    }
    mincg_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->xupdated = ae_false;
lbl_32:
    if( state->terminationneeded )
    {
        ae_v_move(&state->xn.ptr.p_double[0], 1, &state->xk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    v = 0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->g.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    if( ae_fp_less_eq(ae_sqrt(v, _state),state->epsg) )
    {
        ae_v_move(&state->xn.ptr.p_double[0], 1, &state->xk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    state->repnfev = 1;
    state->k = 0;
    state->fold = state->f;
    
    /*
     * Choose initial step.
     * Apply preconditioner, if we have something other than default.
     */
    if( state->prectype==2||state->prectype==3 )
    {
        
        /*
         * because we use preconditioner, step length must be equal
         * to the norm of DK
         */
        v = ae_v_dotproduct(&state->dk.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->lastgoodstep = ae_sqrt(v, _state);
    }
    else
    {
        
        /*
         * No preconditioner is used, we try to use suggested step
         */
        if( ae_fp_greater(state->suggestedstep,0) )
        {
            state->lastgoodstep = state->suggestedstep;
        }
        else
        {
            state->lastgoodstep = 1.0;
        }
    }
    
    /*
     * Main cycle
     */
    state->rstimer = mincg_rscountdownlen;
lbl_34:
    if( ae_false )
    {
        goto lbl_35;
    }
    
    /*
     * * clear reset flag
     * * clear termination flag
     * * store G[k] for later calculation of Y[k]
     * * prepare starting point and direction and step length for line search
     */
    state->innerresetneeded = ae_false;
    state->terminationneeded = ae_false;
    ae_v_moveneg(&state->yk.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->d.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xk.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->mcstage = 0;
    state->stp = 1.0;
    linminnormalized(&state->d, &state->stp, n, _state);
    if( ae_fp_neq(state->lastgoodstep,0) )
    {
        state->stp = state->lastgoodstep;
    }
    state->curstpmax = state->stpmax;
    
    /*
     * Report beginning of line search (if needed)
     * Terminate algorithm, if user request was detected
     */
    if( !state->drep )
    {
        goto lbl_36;
    }
    mincg_clearrequestfields(state, _state);
    state->lsstart = ae_true;
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->lsstart = ae_false;
lbl_36:
    if( state->terminationneeded )
    {
        ae_v_move(&state->xn.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    
    /*
     * Minimization along D
     */
    mcsrch(n, &state->x, &state->f, &state->g, &state->d, &state->stp, state->curstpmax, mincg_gtol, &state->mcinfo, &state->nfev, &state->work0, &state->lstate, &state->mcstage, _state);
lbl_38:
    if( state->mcstage==0 )
    {
        goto lbl_39;
    }
    
    /*
     * Calculate function/gradient using either
     * analytical gradient supplied by user
     * or finite difference approximation.
     *
     * "Trim" function in order to handle near-singularity points.
     */
    mincg_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,0) )
    {
        goto lbl_40;
    }
    state->needfg = ae_true;
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    state->needfg = ae_false;
    goto lbl_41;
lbl_40:
    state->needf = ae_true;
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->fbase = state->f;
    i = 0;
lbl_42:
    if( i>n-1 )
    {
        goto lbl_44;
    }
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 16;
    goto lbl_rcomm;
lbl_16:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 17;
    goto lbl_rcomm;
lbl_17:
    state->fp2 = state->f;
    state->x.ptr.p_double[i] = v;
    state->g.ptr.p_double[i] = (8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/(6*state->diffstep*state->s.ptr.p_double[i]);
    i = i+1;
    goto lbl_42;
lbl_44:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_41:
    trimfunction(&state->f, &state->g, n, state->trimthreshold, _state);
    
    /*
     * Call MCSRCH again
     */
    mcsrch(n, &state->x, &state->f, &state->g, &state->d, &state->stp, state->curstpmax, mincg_gtol, &state->mcinfo, &state->nfev, &state->work0, &state->lstate, &state->mcstage, _state);
    goto lbl_38;
lbl_39:
    
    /*
     * * report end of line search
     * * store current point to XN
     * * report iteration
     * * terminate algorithm if user request was detected
     */
    if( !state->drep )
    {
        goto lbl_45;
    }
    
    /*
     * Report end of line search (if needed)
     */
    mincg_clearrequestfields(state, _state);
    state->lsend = ae_true;
    state->rstate.stage = 18;
    goto lbl_rcomm;
lbl_18:
    state->lsend = ae_false;
lbl_45:
    ae_v_move(&state->xn.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    if( !state->xrep )
    {
        goto lbl_47;
    }
    mincg_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 19;
    goto lbl_rcomm;
lbl_19:
    state->xupdated = ae_false;
lbl_47:
    if( state->terminationneeded )
    {
        ae_v_move(&state->xn.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    
    /*
     * Line search is finished.
     * * calculate BetaK
     * * calculate DN
     * * update timers
     * * calculate step length:
     *   * LastScaledStep is ALWAYS calculated because it is used in the stopping criteria
     *   * LastGoodStep is updated only when MCINFO is equal to 1 (Wolfe conditions hold).
     *     See below for more explanation.
     */
    if( state->mcinfo==1&&!state->innerresetneeded )
    {
        
        /*
         * Standard Wolfe conditions hold
         * Calculate Y[K] and D[K]'*Y[K]
         */
        ae_v_add(&state->yk.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
        vv = ae_v_dotproduct(&state->yk.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        
        /*
         * Calculate BetaK according to DY formula
         */
        v = mincg_preconditionedmultiply2(state, &state->g, &state->g, &state->work0, &state->work1, _state);
        state->betady = v/vv;
        
        /*
         * Calculate BetaK according to HS formula
         */
        v = mincg_preconditionedmultiply2(state, &state->g, &state->yk, &state->work0, &state->work1, _state);
        state->betahs = v/vv;
        
        /*
         * Choose BetaK
         */
        if( state->cgtype==0 )
        {
            betak = state->betady;
        }
        if( state->cgtype==1 )
        {
            betak = ae_maxreal(0, ae_minreal(state->betady, state->betahs, _state), _state);
        }
    }
    else
    {
        
        /*
         * Something is wrong (may be function is too wild or too flat)
         * or we just have to restart algo.
         *
         * We'll set BetaK=0, which will restart CG algorithm.
         * We can stop later (during normal checks) if stopping conditions are met.
         */
        betak = 0;
        state->debugrestartscount = state->debugrestartscount+1;
    }
    if( state->repiterationscount>0&&state->repiterationscount%(3+n)==0 )
    {
        
        /*
         * clear Beta every N iterations
         */
        betak = 0;
    }
    if( state->mcinfo==1||state->mcinfo==5 )
    {
        state->rstimer = mincg_rscountdownlen;
    }
    else
    {
        state->rstimer = state->rstimer-1;
    }
    ae_v_moveneg(&state->dn.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    mincg_preconditionedmultiply(state, &state->dn, &state->work0, &state->work1, _state);
    ae_v_addd(&state->dn.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1), betak);
    state->lastscaledstep = 0.0;
    for(i=0; i<=n-1; i++)
    {
        state->lastscaledstep = state->lastscaledstep+ae_sqr(state->d.ptr.p_double[i]/state->s.ptr.p_double[i], _state);
    }
    state->lastscaledstep = state->stp*ae_sqrt(state->lastscaledstep, _state);
    if( state->mcinfo==1 )
    {
        
        /*
         * Step is good (Wolfe conditions hold), update LastGoodStep.
         *
         * This check for MCINFO=1 is essential because sometimes in the
         * constrained optimization setting we may take very short steps
         * (like 1E-15) because we were very close to boundary of the
         * feasible area. Such short step does not mean that we've converged
         * to the solution - it was so short because we were close to the
         * boundary and there was a limit on step length.
         *
         * So having such short step is quite normal situation. However, we
         * should NOT start next iteration from step whose initial length is
         * estimated as 1E-15 because it may lead to the failure of the
         * linear minimizer (step is too short, function does not changes,
         * line search stagnates).
         */
        state->lastgoodstep = 0;
        for(i=0; i<=n-1; i++)
        {
            state->lastgoodstep = state->lastgoodstep+ae_sqr(state->d.ptr.p_double[i], _state);
        }
        state->lastgoodstep = state->stp*ae_sqrt(state->lastgoodstep, _state);
    }
    
    /*
     * Update information.
     * Check stopping conditions.
     */
    state->repnfev = state->repnfev+state->nfev;
    state->repiterationscount = state->repiterationscount+1;
    if( state->repiterationscount>=state->maxits&&state->maxits>0 )
    {
        
        /*
         * Too many iterations
         */
        state->repterminationtype = 5;
        result = ae_false;
        return result;
    }
    v = 0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->g.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    if( ae_fp_less_eq(ae_sqrt(v, _state),state->epsg) )
    {
        
        /*
         * Gradient is small enough
         */
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    if( !state->innerresetneeded )
    {
        
        /*
         * These conditions are checked only when no inner reset was requested by user
         */
        if( ae_fp_less_eq(state->fold-state->f,state->epsf*ae_maxreal(ae_fabs(state->fold, _state), ae_maxreal(ae_fabs(state->f, _state), 1.0, _state), _state)) )
        {
            
            /*
             * F(k+1)-F(k) is small enough
             */
            state->repterminationtype = 1;
            result = ae_false;
            return result;
        }
        if( ae_fp_less_eq(state->lastscaledstep,state->epsx) )
        {
            
            /*
             * X(k+1)-X(k) is small enough
             */
            state->repterminationtype = 2;
            result = ae_false;
            return result;
        }
    }
    if( state->rstimer<=0 )
    {
        
        /*
         * Too many subsequent restarts
         */
        state->repterminationtype = 7;
        result = ae_false;
        return result;
    }
    
    /*
     * Shift Xk/Dk, update other information
     */
    ae_v_move(&state->xk.ptr.p_double[0], 1, &state->xn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->dk.ptr.p_double[0], 1, &state->dn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fold = state->f;
    state->k = state->k+1;
    goto lbl_34;
lbl_35:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = i;
    state->rstate.ra.ptr.p_double[0] = betak;
    state->rstate.ra.ptr.p_double[1] = v;
    state->rstate.ra.ptr.p_double[2] = vv;
    return result;
}


/*************************************************************************
Conjugate gradient results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -7    gradient verification failed.
                            See MinCGSetGradientCheck() for more information.
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible,
                            we return best X found so far
                    *  8    terminated by user
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresults(mincgstate* state,
     /* Real    */ ae_vector* x,
     mincgreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _mincgreport_clear(rep);

    mincgresultsbuf(state, x, rep, _state);
}


/*************************************************************************
Conjugate gradient results

Buffered implementation of MinCGResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresultsbuf(mincgstate* state,
     /* Real    */ ae_vector* x,
     mincgreport* rep,
     ae_state *_state)
{


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->xn.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nfev = state->repnfev;
    rep->varidx = state->repvaridx;
    rep->terminationtype = state->repterminationtype;
}


/*************************************************************************
This  subroutine  restarts  CG  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgrestartfrom(mincgstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{


    ae_assert(x->cnt>=state->n, "MinCGRestartFrom: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, state->n, _state), "MinCGCreate: X contains infinite or NaN values!", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    mincgsuggeststep(state, 0.0, _state);
    ae_vector_set_length(&state->rstate.ia, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
    mincg_clearrequestfields(state, _state);
}


/*************************************************************************
Faster version of MinCGSetPrecDiag(), for time-critical parts of code,
without safety checks.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdiagfast(mincgstate* state,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    rvectorsetlengthatleast(&state->diagh, state->n, _state);
    rvectorsetlengthatleast(&state->diaghl2, state->n, _state);
    state->prectype = 2;
    state->vcnt = 0;
    state->innerresetneeded = ae_true;
    for(i=0; i<=state->n-1; i++)
    {
        state->diagh.ptr.p_double[i] = d->ptr.p_double[i];
        state->diaghl2.ptr.p_double[i] = 0.0;
    }
}


/*************************************************************************
This function sets low-rank preconditioner for Hessian matrix  H=D+V'*C*V,
where:
* H is a Hessian matrix, which is approximated by D/V/C
* D=D1+D2 is a diagonal matrix, which includes two positive definite terms:
  * constant term D1 (is not updated or infrequently updated)
  * variable term D2 (can be cheaply updated from iteration to iteration)
* V is a low-rank correction
* C is a diagonal factor of low-rank correction

Preconditioner P is calculated using approximate Woodburry formula:
    P  = D^(-1) - D^(-1)*V'*(C^(-1)+V*D1^(-1)*V')^(-1)*V*D^(-1)
       = D^(-1) - D^(-1)*VC'*VC*D^(-1),
where
    VC = sqrt(B)*V
    B  = (C^(-1)+V*D1^(-1)*V')^(-1)
    
Note that B is calculated using constant term (D1) only,  which  allows us
to update D2 without recalculation of B or   VC.  Such  preconditioner  is
exact when D2 is zero. When D2 is non-zero, it is only approximation,  but
very good and cheap one.

This function accepts D1, V, C.
D2 is set to zero by default.

Cost of this update is O(N*VCnt*VCnt), but D2 can be updated in just O(N)
by MinCGSetPrecVarPart.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetpreclowrankfast(mincgstate* state,
     /* Real    */ ae_vector* d1,
     /* Real    */ ae_vector* c,
     /* Real    */ ae_matrix* v,
     ae_int_t vcnt,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t n;
    double t;
    ae_matrix b;

    ae_frame_make(_state, &_frame_block);
    ae_matrix_init(&b, 0, 0, DT_REAL, _state, ae_true);

    if( vcnt==0 )
    {
        mincgsetprecdiagfast(state, d1, _state);
        ae_frame_leave(_state);
        return;
    }
    n = state->n;
    ae_matrix_set_length(&b, vcnt, vcnt, _state);
    rvectorsetlengthatleast(&state->diagh, n, _state);
    rvectorsetlengthatleast(&state->diaghl2, n, _state);
    rmatrixsetlengthatleast(&state->vcorr, vcnt, n, _state);
    state->prectype = 2;
    state->vcnt = vcnt;
    state->innerresetneeded = ae_true;
    for(i=0; i<=n-1; i++)
    {
        state->diagh.ptr.p_double[i] = d1->ptr.p_double[i];
        state->diaghl2.ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=vcnt-1; i++)
    {
        for(j=i; j<=vcnt-1; j++)
        {
            t = 0;
            for(k=0; k<=n-1; k++)
            {
                t = t+v->ptr.pp_double[i][k]*v->ptr.pp_double[j][k]/d1->ptr.p_double[k];
            }
            b.ptr.pp_double[i][j] = t;
        }
        b.ptr.pp_double[i][i] = b.ptr.pp_double[i][i]+1.0/c->ptr.p_double[i];
    }
    if( !spdmatrixcholeskyrec(&b, 0, vcnt, ae_true, &state->work0, _state) )
    {
        state->vcnt = 0;
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=vcnt-1; i++)
    {
        ae_v_move(&state->vcorr.ptr.pp_double[i][0], 1, &v->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
        for(j=0; j<=i-1; j++)
        {
            t = b.ptr.pp_double[j][i];
            ae_v_subd(&state->vcorr.ptr.pp_double[i][0], 1, &state->vcorr.ptr.pp_double[j][0], 1, ae_v_len(0,n-1), t);
        }
        t = 1/b.ptr.pp_double[i][i];
        ae_v_muld(&state->vcorr.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), t);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This function updates variable part (diagonal matrix D2)
of low-rank preconditioner.

This update is very cheap and takes just O(N) time.

It has no effect with default preconditioner.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecvarpart(mincgstate* state,
     /* Real    */ ae_vector* d2,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    for(i=0; i<=n-1; i++)
    {
        state->diaghl2.ptr.p_double[i] = d2->ptr.p_double[i];
    }
}


/*************************************************************************

This  subroutine  turns  on  verification  of  the  user-supplied analytic
gradient:
* user calls this subroutine before optimization begins
* MinCGOptimize() is called
* prior to  actual  optimization, for each component  of  parameters being
  optimized X[i] algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point
  * in case difference between prediction and actual value is higher  than
    some predetermined threshold, algorithm stops with completion code -7;
    Rep.VarIdx is set to index of the parameter with incorrect derivative.
* after verification is over, algorithm proceeds to the actual optimization.

NOTE 1: verification  needs  N (parameters count) gradient evaluations. It
        is very costly and you should use  it  only  for  low  dimensional
        problems,  when  you  want  to  be  sure  that  you've   correctly
        calculated  analytic  derivatives.  You  should  not use it in the
        production code (unless you want to check derivatives provided  by
        some third party).

NOTE 2: you  should  carefully  choose  TestStep. Value which is too large
        (so large that function behaviour is significantly non-cubic) will
        lead to false alarms. You may use  different  step  for  different
        parameters by means of setting scale with MinCGSetScale().

NOTE 3: this function may lead to false positives. In case it reports that
        I-th  derivative was calculated incorrectly, you may decrease test
        step  and  try  one  more  time  - maybe your function changes too
        sharply  and  your  step  is  too  large for such rapidly chanding
        function.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification

  -- ALGLIB --
     Copyright 31.05.2012 by Bochkanov Sergey
*************************************************************************/
void mincgsetgradientcheck(mincgstate* state,
     double teststep,
     ae_state *_state)
{


    ae_assert(ae_isfinite(teststep, _state), "MinCGSetGradientCheck: TestStep contains NaN or Infinite", _state);
    ae_assert(ae_fp_greater_eq(teststep,0), "MinCGSetGradientCheck: invalid argument TestStep(TestStep<0)", _state);
    state->teststep = teststep;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void mincg_clearrequestfields(mincgstate* state, ae_state *_state)
{


    state->needf = ae_false;
    state->needfg = ae_false;
    state->xupdated = ae_false;
    state->lsstart = ae_false;
    state->lsend = ae_false;
    state->algpowerup = ae_false;
}


/*************************************************************************
This function calculates preconditioned product H^(-1)*x and stores result
back into X. Work0[] and Work1[] are used as temporaries (size must be at
least N; this function doesn't allocate arrays).

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
static void mincg_preconditionedmultiply(mincgstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* work0,
     /* Real    */ ae_vector* work1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t vcnt;
    double v;


    n = state->n;
    vcnt = state->vcnt;
    if( state->prectype==0 )
    {
        return;
    }
    if( state->prectype==3 )
    {
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = x->ptr.p_double[i]*state->s.ptr.p_double[i]*state->s.ptr.p_double[i];
        }
        return;
    }
    ae_assert(state->prectype==2, "MinCG: internal error (unexpected PrecType)", _state);
    
    /*
     * handle part common for VCnt=0 and VCnt<>0
     */
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
    }
    
    /*
     * if VCnt>0
     */
    if( vcnt>0 )
    {
        for(i=0; i<=vcnt-1; i++)
        {
            v = ae_v_dotproduct(&state->vcorr.ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
            work0->ptr.p_double[i] = v;
        }
        for(i=0; i<=n-1; i++)
        {
            work1->ptr.p_double[i] = 0;
        }
        for(i=0; i<=vcnt-1; i++)
        {
            v = work0->ptr.p_double[i];
            ae_v_addd(&state->work1.ptr.p_double[0], 1, &state->vcorr.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
        }
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = x->ptr.p_double[i]-state->work1.ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
        }
    }
}


/*************************************************************************
This function calculates preconditioned product x'*H^(-1)*y. Work0[] and
Work1[] are used as temporaries (size must be at least N; this function
doesn't allocate arrays).

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
static double mincg_preconditionedmultiply2(mincgstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* work0,
     /* Real    */ ae_vector* work1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t vcnt;
    double v0;
    double v1;
    double result;


    n = state->n;
    vcnt = state->vcnt;
    
    /*
     * no preconditioning
     */
    if( state->prectype==0 )
    {
        v0 = ae_v_dotproduct(&x->ptr.p_double[0], 1, &y->ptr.p_double[0], 1, ae_v_len(0,n-1));
        result = v0;
        return result;
    }
    if( state->prectype==3 )
    {
        result = 0;
        for(i=0; i<=n-1; i++)
        {
            result = result+x->ptr.p_double[i]*state->s.ptr.p_double[i]*state->s.ptr.p_double[i]*y->ptr.p_double[i];
        }
        return result;
    }
    ae_assert(state->prectype==2, "MinCG: internal error (unexpected PrecType)", _state);
    
    /*
     * low rank preconditioning
     */
    result = 0.0;
    for(i=0; i<=n-1; i++)
    {
        result = result+x->ptr.p_double[i]*y->ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
    }
    if( vcnt>0 )
    {
        for(i=0; i<=n-1; i++)
        {
            work0->ptr.p_double[i] = x->ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
            work1->ptr.p_double[i] = y->ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
        }
        for(i=0; i<=vcnt-1; i++)
        {
            v0 = ae_v_dotproduct(&work0->ptr.p_double[0], 1, &state->vcorr.ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
            v1 = ae_v_dotproduct(&work1->ptr.p_double[0], 1, &state->vcorr.ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
            result = result-v0*v1;
        }
    }
    return result;
}


/*************************************************************************
Internal initialization subroutine

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
static void mincg_mincginitinternal(ae_int_t n,
     double diffstep,
     mincgstate* state,
     ae_state *_state)
{
    ae_int_t i;


    
    /*
     * Initialize
     */
    state->teststep = 0;
    state->n = n;
    state->diffstep = diffstep;
    mincgsetcond(state, 0, 0, 0, 0, _state);
    mincgsetxrep(state, ae_false, _state);
    mincgsetdrep(state, ae_false, _state);
    mincgsetstpmax(state, 0, _state);
    mincgsetcgtype(state, -1, _state);
    mincgsetprecdefault(state, _state);
    ae_vector_set_length(&state->xk, n, _state);
    ae_vector_set_length(&state->dk, n, _state);
    ae_vector_set_length(&state->xn, n, _state);
    ae_vector_set_length(&state->dn, n, _state);
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->d, n, _state);
    ae_vector_set_length(&state->g, n, _state);
    ae_vector_set_length(&state->work0, n, _state);
    ae_vector_set_length(&state->work1, n, _state);
    ae_vector_set_length(&state->yk, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->s.ptr.p_double[i] = 1.0;
    }
}


ae_bool _mincgstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    mincgstate *p = (mincgstate*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_vector_init(&p->diagh, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->diaghl2, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->vcorr, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xk, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->dk, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xn, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->dn, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->yk, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init(&p->rstate, _state, make_automatic) )
        return ae_false;
    if( !_linminstate_init(&p->lstate, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->work0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->work1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _mincgstate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    mincgstate *dst = (mincgstate*)_dst;
    mincgstate *src = (mincgstate*)_src;
    dst->n = src->n;
    dst->epsg = src->epsg;
    dst->epsf = src->epsf;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->stpmax = src->stpmax;
    dst->suggestedstep = src->suggestedstep;
    dst->xrep = src->xrep;
    dst->drep = src->drep;
    dst->cgtype = src->cgtype;
    dst->prectype = src->prectype;
    if( !ae_vector_init_copy(&dst->diagh, &src->diagh, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->diaghl2, &src->diaghl2, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->vcorr, &src->vcorr, _state, make_automatic) )
        return ae_false;
    dst->vcnt = src->vcnt;
    if( !ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic) )
        return ae_false;
    dst->diffstep = src->diffstep;
    dst->nfev = src->nfev;
    dst->mcstage = src->mcstage;
    dst->k = src->k;
    if( !ae_vector_init_copy(&dst->xk, &src->xk, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->dk, &src->dk, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->xn, &src->xn, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->dn, &src->dn, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic) )
        return ae_false;
    dst->fold = src->fold;
    dst->stp = src->stp;
    dst->curstpmax = src->curstpmax;
    if( !ae_vector_init_copy(&dst->yk, &src->yk, _state, make_automatic) )
        return ae_false;
    dst->lastgoodstep = src->lastgoodstep;
    dst->lastscaledstep = src->lastscaledstep;
    dst->mcinfo = src->mcinfo;
    dst->innerresetneeded = src->innerresetneeded;
    dst->terminationneeded = src->terminationneeded;
    dst->trimthreshold = src->trimthreshold;
    dst->rstimer = src->rstimer;
    if( !ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic) )
        return ae_false;
    dst->f = src->f;
    if( !ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic) )
        return ae_false;
    dst->needf = src->needf;
    dst->needfg = src->needfg;
    dst->xupdated = src->xupdated;
    dst->algpowerup = src->algpowerup;
    dst->lsstart = src->lsstart;
    dst->lsend = src->lsend;
    dst->teststep = src->teststep;
    if( !_rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic) )
        return ae_false;
    dst->repiterationscount = src->repiterationscount;
    dst->repnfev = src->repnfev;
    dst->repvaridx = src->repvaridx;
    dst->repterminationtype = src->repterminationtype;
    dst->debugrestartscount = src->debugrestartscount;
    if( !_linminstate_init_copy(&dst->lstate, &src->lstate, _state, make_automatic) )
        return ae_false;
    dst->fbase = src->fbase;
    dst->fm2 = src->fm2;
    dst->fm1 = src->fm1;
    dst->fp1 = src->fp1;
    dst->fp2 = src->fp2;
    dst->betahs = src->betahs;
    dst->betady = src->betady;
    if( !ae_vector_init_copy(&dst->work0, &src->work0, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->work1, &src->work1, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _mincgstate_clear(void* _p)
{
    mincgstate *p = (mincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->diagh);
    ae_vector_clear(&p->diaghl2);
    ae_matrix_clear(&p->vcorr);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->xk);
    ae_vector_clear(&p->dk);
    ae_vector_clear(&p->xn);
    ae_vector_clear(&p->dn);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->yk);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->g);
    _rcommstate_clear(&p->rstate);
    _linminstate_clear(&p->lstate);
    ae_vector_clear(&p->work0);
    ae_vector_clear(&p->work1);
}


void _mincgstate_destroy(void* _p)
{
    mincgstate *p = (mincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->diagh);
    ae_vector_destroy(&p->diaghl2);
    ae_matrix_destroy(&p->vcorr);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->xk);
    ae_vector_destroy(&p->dk);
    ae_vector_destroy(&p->xn);
    ae_vector_destroy(&p->dn);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->yk);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->g);
    _rcommstate_destroy(&p->rstate);
    _linminstate_destroy(&p->lstate);
    ae_vector_destroy(&p->work0);
    ae_vector_destroy(&p->work1);
}


ae_bool _mincgreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    mincgreport *p = (mincgreport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _mincgreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    mincgreport *dst = (mincgreport*)_dst;
    mincgreport *src = (mincgreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfev = src->nfev;
    dst->varidx = src->varidx;
    dst->terminationtype = src->terminationtype;
    return ae_true;
}


void _mincgreport_clear(void* _p)
{
    mincgreport *p = (mincgreport*)_p;
    ae_touch_ptr((void*)p);
}


void _mincgreport_destroy(void* _p)
{
    mincgreport *p = (mincgreport*)_p;
    ae_touch_ptr((void*)p);
}




/*************************************************************************
                     BOUND CONSTRAINED OPTIMIZATION
       WITH ADDITIONAL LINEAR EQUALITY AND INEQUALITY CONSTRAINTS

DESCRIPTION:
The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints

REQUIREMENTS:
* user must provide function value and gradient
* starting point X0 must be feasible or
  not too far away from the feasible set
* grad(f) must be Lipschitz continuous on a level set:
  L = { x : f(x)<=f(x0) }
* function must be defined everywhere on the feasible set F

USAGE:

Constrained optimization if far more complex than the unconstrained one.
Here we give very brief outline of the BLEIC optimizer. We strongly recommend
you to read examples in the ALGLIB Reference Manual and to read ALGLIB User Guide
on optimization, which is available at http://www.alglib.net/optimization/

1. User initializes algorithm state with MinBLEICCreate() call

2. USer adds boundary and/or linear constraints by calling
   MinBLEICSetBC() and MinBLEICSetLC() functions.

3. User sets stopping conditions with MinBLEICSetCond().

4. User calls MinBLEICOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.

5. User calls MinBLEICResults() to get solution

6. Optionally user may call MinBLEICRestartFrom() to solve another problem
   with same N but another starting point.
   MinBLEICRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleiccreate(ae_int_t n,
     /* Real    */ ae_vector* x,
     minbleicstate* state,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix c;
    ae_vector ct;

    ae_frame_make(_state, &_frame_block);
    _minbleicstate_clear(state);
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);

    ae_assert(n>=1, "MinBLEICCreate: N<1", _state);
    ae_assert(x->cnt>=n, "MinBLEICCreate: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinBLEICCreate: X contains infinite or NaN values!", _state);
    minbleic_minbleicinitinternal(n, x, 0.0, state, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
The subroutine is finite difference variant of MinBLEICCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinBLEICCreate() in  order  to  get
more information about creation of BLEIC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinBLEICSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust and precise. CG needs exact gradient values. Imprecise
   gradient may slow  down  convergence, especially  on  highly  nonlinear
   problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minbleiccreatef(ae_int_t n,
     /* Real    */ ae_vector* x,
     double diffstep,
     minbleicstate* state,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix c;
    ae_vector ct;

    ae_frame_make(_state, &_frame_block);
    _minbleicstate_clear(state);
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);

    ae_assert(n>=1, "MinBLEICCreateF: N<1", _state);
    ae_assert(x->cnt>=n, "MinBLEICCreateF: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinBLEICCreateF: X contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(diffstep, _state), "MinBLEICCreateF: DiffStep is infinite or NaN!", _state);
    ae_assert(ae_fp_greater(diffstep,0), "MinBLEICCreateF: DiffStep is non-positive!", _state);
    minbleic_minbleicinitinternal(n, x, diffstep, state, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function sets boundary constraints for BLEIC optimizer.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by  bound  constraints,
  even  when  numerical  differentiation is used (algorithm adjusts  nodes
  according to boundary constraints)

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbc(minbleicstate* state,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->nmain;
    ae_assert(bndl->cnt>=n, "MinBLEICSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinBLEICSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinBLEICSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinBLEICSetBC: BndL contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
    sassetbc(&state->sas, bndl, bndu, _state);
}


/*************************************************************************
This function sets linear constraints for BLEIC optimizer.

Linear constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately:
* there always exists some minor violation (about Epsilon in magnitude)
  due to rounding errors
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.
If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetlc(minbleicstate* state,
     /* Real    */ ae_matrix* c,
     /* Integer */ ae_vector* ct,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    double v;


    n = state->nmain;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(k>=0, "MinBLEICSetLC: K<0", _state);
    ae_assert(c->cols>=n+1||k==0, "MinBLEICSetLC: Cols(C)<N+1", _state);
    ae_assert(c->rows>=k, "MinBLEICSetLC: Rows(C)<K", _state);
    ae_assert(ct->cnt>=k, "MinBLEICSetLC: Length(CT)<K", _state);
    ae_assert(apservisfinitematrix(c, k, n+1, _state), "MinBLEICSetLC: C contains infinite or NaN values!", _state);
    
    /*
     * Handle zero K
     */
    if( k==0 )
    {
        state->nec = 0;
        state->nic = 0;
        return;
    }
    
    /*
     * Equality constraints are stored first, in the upper
     * NEC rows of State.CLEIC matrix. Inequality constraints
     * are stored in the next NIC rows.
     *
     * NOTE: we convert inequality constraints to the form
     * A*x<=b before copying them.
     */
    rmatrixsetlengthatleast(&state->cleic, k, n+1, _state);
    state->nec = 0;
    state->nic = 0;
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]==0 )
        {
            ae_v_move(&state->cleic.ptr.pp_double[state->nec][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            state->nec = state->nec+1;
        }
    }
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]!=0 )
        {
            if( ct->ptr.p_int[i]>0 )
            {
                ae_v_moveneg(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            else
            {
                ae_v_move(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            state->nic = state->nic+1;
        }
    }
    
    /*
     * Normalize rows of State.CLEIC: each row must have unit norm.
     * Norm is calculated using first N elements (i.e. right part is
     * not counted when we calculate norm).
     */
    for(i=0; i<=k-1; i++)
    {
        v = 0;
        for(j=0; j<=n-1; j++)
        {
            v = v+ae_sqr(state->cleic.ptr.pp_double[i][j], _state);
        }
        if( ae_fp_eq(v,0) )
        {
            continue;
        }
        v = 1/ae_sqrt(v, _state);
        ae_v_muld(&state->cleic.ptr.pp_double[i][0], 1, ae_v_len(0,n), v);
    }
    sassetlc(&state->sas, c, ct, k, _state);
}


/*************************************************************************
This function sets stopping conditions for the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinBLEICSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinBLEICSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection.

NOTE: when SetCond() called with non-zero MaxIts, BLEIC solver may perform
      slightly more than MaxIts iterations. I.e., MaxIts  sets  non-strict
      limit on iterations count.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetcond(minbleicstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinBLEICSetCond: EpsG is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsg,0), "MinBLEICSetCond: negative EpsG", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinBLEICSetCond: EpsF is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsf,0), "MinBLEICSetCond: negative EpsF", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinBLEICSetCond: EpsX is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsx,0), "MinBLEICSetCond: negative EpsX", _state);
    ae_assert(maxits>=0, "MinBLEICSetCond: negative MaxIts!", _state);
    if( ((ae_fp_eq(epsg,0)&&ae_fp_eq(epsf,0))&&ae_fp_eq(epsx,0))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    state->epsg = epsg;
    state->epsf = epsf;
    state->epsx = epsx;
    state->maxits = maxits;
}


/*************************************************************************
This function sets scaling coefficients for BLEIC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  BLEIC  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinBLEICSetPrec...()
functions.

There is a special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minbleicsetscale(minbleicstate* state,
     /* Real    */ ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->nmain, "MinBLEICSetScale: Length(S)<N", _state);
    for(i=0; i<=state->nmain-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinBLEICSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],0), "MinBLEICSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
    sassetscale(&state->sas, s, _state);
}


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecdefault(minbleicstate* state, ae_state *_state)
{


    state->prectype = 0;
}


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE 1: D[i] should be positive. Exception will be thrown otherwise.

NOTE 2: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecdiag(minbleicstate* state,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(d->cnt>=state->nmain, "MinBLEICSetPrecDiag: D is too short", _state);
    for(i=0; i<=state->nmain-1; i++)
    {
        ae_assert(ae_isfinite(d->ptr.p_double[i], _state), "MinBLEICSetPrecDiag: D contains infinite or NAN elements", _state);
        ae_assert(ae_fp_greater(d->ptr.p_double[i],0), "MinBLEICSetPrecDiag: D contains non-positive elements", _state);
    }
    rvectorsetlengthatleast(&state->diagh, state->nmain, _state);
    state->prectype = 2;
    for(i=0; i<=state->nmain-1; i++)
    {
        state->diagh.ptr.p_double[i] = d->ptr.p_double[i];
    }
}


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinBLEICSetScale()
call  (before  or after MinBLEICSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecscale(minbleicstate* state, ae_state *_state)
{


    state->prectype = 3;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinBLEICOptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetxrep(minbleicstate* state,
     ae_bool needxrep,
     ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function turns on/off line search reports.
These reports are described in more details in developer-only  comments on
MinBLEICState object.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedDRep-   whether line search reports are needed or not

This function is intended for private use only. Turning it on artificially
may cause program failure.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetdrep(minbleicstate* state,
     ae_bool needdrep,
     ae_state *_state)
{


    state->drep = needdrep;
}


/*************************************************************************
This function sets maximum step length

IMPORTANT: this feature is hard to combine with preconditioning. You can't
set upper limit on step length, when you solve optimization  problem  with
linear (non-boundary) constraints AND preconditioner turned on.

When  non-boundary  constraints  are  present,  you  have to either a) use
preconditioner, or b) use upper limit on step length.  YOU CAN'T USE BOTH!
In this case algorithm will terminate with appropriate error code.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  lead   to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetstpmax(minbleicstate* state,
     double stpmax,
     ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "MinBLEICSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,0), "MinBLEICSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************
NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either  MinBLEICCreate() for analytical gradient or  MinBLEICCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinBLEICOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinBLEICOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinBLEICOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinBLEICOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinBLEICCreateF() |     work                FAIL
   MinBLEICCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinBLEICOptimize()  version.   Attemps   to   use   such
   combination (for  example,  to  create optimizer with MinBLEICCreateF()
   and  to  pass  gradient  information  to  MinCGOptimize()) will lead to
   exception being thrown. Either  you  did  not pass gradient when it WAS
   needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool minbleiciteration(minbleicstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;
    ae_int_t badbfgsits;
    ae_bool b;
    ae_int_t nextaction;
    ae_int_t mcinfo;
    ae_int_t actstatus;
    ae_int_t ic;
    double penalty;
    double ginit;
    double gdecay;
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
        j = state->rstate.ia.ptr.p_int[3];
        badbfgsits = state->rstate.ia.ptr.p_int[4];
        nextaction = state->rstate.ia.ptr.p_int[5];
        mcinfo = state->rstate.ia.ptr.p_int[6];
        actstatus = state->rstate.ia.ptr.p_int[7];
        ic = state->rstate.ia.ptr.p_int[8];
        b = state->rstate.ba.ptr.p_bool[0];
        v = state->rstate.ra.ptr.p_double[0];
        vv = state->rstate.ra.ptr.p_double[1];
        penalty = state->rstate.ra.ptr.p_double[2];
        ginit = state->rstate.ra.ptr.p_double[3];
        gdecay = state->rstate.ra.ptr.p_double[4];
    }
    else
    {
        n = -983;
        m = -989;
        i = -834;
        j = 900;
        badbfgsits = -287;
        nextaction = 364;
        mcinfo = 214;
        actstatus = -338;
        ic = -686;
        b = ae_false;
        v = 585;
        vv = 497;
        penalty = -271;
        ginit = -581;
        gdecay = 745;
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
    if( state->rstate.stage==8 )
    {
        goto lbl_8;
    }
    if( state->rstate.stage==9 )
    {
        goto lbl_9;
    }
    if( state->rstate.stage==10 )
    {
        goto lbl_10;
    }
    if( state->rstate.stage==11 )
    {
        goto lbl_11;
    }
    if( state->rstate.stage==12 )
    {
        goto lbl_12;
    }
    if( state->rstate.stage==13 )
    {
        goto lbl_13;
    }
    if( state->rstate.stage==14 )
    {
        goto lbl_14;
    }
    if( state->rstate.stage==15 )
    {
        goto lbl_15;
    }
    if( state->rstate.stage==16 )
    {
        goto lbl_16;
    }
    if( state->rstate.stage==17 )
    {
        goto lbl_17;
    }
    if( state->rstate.stage==18 )
    {
        goto lbl_18;
    }
    if( state->rstate.stage==19 )
    {
        goto lbl_19;
    }
    if( state->rstate.stage==20 )
    {
        goto lbl_20;
    }
    if( state->rstate.stage==21 )
    {
        goto lbl_21;
    }
    if( state->rstate.stage==22 )
    {
        goto lbl_22;
    }
    if( state->rstate.stage==23 )
    {
        goto lbl_23;
    }
    if( state->rstate.stage==24 )
    {
        goto lbl_24;
    }
    if( state->rstate.stage==25 )
    {
        goto lbl_25;
    }
    if( state->rstate.stage==26 )
    {
        goto lbl_26;
    }
    if( state->rstate.stage==27 )
    {
        goto lbl_27;
    }
    if( state->rstate.stage==28 )
    {
        goto lbl_28;
    }
    if( state->rstate.stage==29 )
    {
        goto lbl_29;
    }
    if( state->rstate.stage==30 )
    {
        goto lbl_30;
    }
    if( state->rstate.stage==31 )
    {
        goto lbl_31;
    }
    if( state->rstate.stage==32 )
    {
        goto lbl_32;
    }
    if( state->rstate.stage==33 )
    {
        goto lbl_33;
    }
    if( state->rstate.stage==34 )
    {
        goto lbl_34;
    }
    if( state->rstate.stage==35 )
    {
        goto lbl_35;
    }
    if( state->rstate.stage==36 )
    {
        goto lbl_36;
    }
    if( state->rstate.stage==37 )
    {
        goto lbl_37;
    }
    if( state->rstate.stage==38 )
    {
        goto lbl_38;
    }
    if( state->rstate.stage==39 )
    {
        goto lbl_39;
    }
    if( state->rstate.stage==40 )
    {
        goto lbl_40;
    }
    if( state->rstate.stage==41 )
    {
        goto lbl_41;
    }
    
    /*
     * Routine body
     */
    
    /*
     * Algorithm parameters:
     * * M          number of L-BFGS corrections.
     *              This coefficient remains fixed during iterations.
     * * GDecay     desired decrease of constrained gradient during L-BFGS iterations.
     *              This coefficient is decreased after each L-BFGS round until
     *              it reaches minimum decay.
     */
    m = ae_minint(5, state->nmain, _state);
    gdecay = minbleic_initialdecay;
    
    /*
     * Init
     */
    n = state->nmain;
    state->repterminationtype = 0;
    state->repinneriterationscount = 0;
    state->repouteriterationscount = 0;
    state->repnfev = 0;
    state->repvaridx = -1;
    state->repdebugeqerr = 0.0;
    state->repdebugfs = _state->v_nan;
    state->repdebugff = _state->v_nan;
    state->repdebugdx = _state->v_nan;
    if( ae_fp_neq(state->stpmax,0)&&state->prectype!=0 )
    {
        state->repterminationtype = -10;
        result = ae_false;
        return result;
    }
    rvectorsetlengthatleast(&state->rho, m, _state);
    rvectorsetlengthatleast(&state->theta, m, _state);
    rmatrixsetlengthatleast(&state->yk, m, n, _state);
    rmatrixsetlengthatleast(&state->sk, m, n, _state);
    
    /*
     * Fill TmpPrec with current preconditioner
     */
    rvectorsetlengthatleast(&state->tmpprec, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( state->prectype==2 )
        {
            state->tmpprec.ptr.p_double[i] = state->diagh.ptr.p_double[i];
            continue;
        }
        if( state->prectype==3 )
        {
            state->tmpprec.ptr.p_double[i] = 1/ae_sqr(state->s.ptr.p_double[i], _state);
            continue;
        }
        state->tmpprec.ptr.p_double[i] = 1;
    }
    sassetprecdiag(&state->sas, &state->tmpprec, _state);
    
    /*
     * Start optimization
     */
    if( !sasstartoptimization(&state->sas, &state->xstart, _state) )
    {
        state->repterminationtype = -3;
        result = ae_false;
        return result;
    }
    
    /*
     *  Check correctness of user-supplied gradient
     */
    if( !(ae_fp_eq(state->diffstep,0)&&ae_fp_greater(state->teststep,0)) )
    {
        goto lbl_42;
    }
    minbleic_clearrequestfields(state, _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->needfg = ae_true;
    i = 0;
lbl_44:
    if( i>n-1 )
    {
        goto lbl_46;
    }
    ae_assert(!state->hasbndl.ptr.p_bool[i]||ae_fp_greater_eq(state->sas.xc.ptr.p_double[i],state->bndl.ptr.p_double[i]), "MinBLEICIteration: internal error(State.X is out of bounds)", _state);
    ae_assert(!state->hasbndu.ptr.p_bool[i]||ae_fp_less_eq(state->sas.xc.ptr.p_double[i],state->bndu.ptr.p_double[i]), "MinBLEICIteration: internal error(State.X is out of bounds)", _state);
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->teststep*state->s.ptr.p_double[i];
    if( state->hasbndl.ptr.p_bool[i] )
    {
        state->x.ptr.p_double[i] = ae_maxreal(state->x.ptr.p_double[i], state->bndl.ptr.p_double[i], _state);
    }
    state->xm1 = state->x.ptr.p_double[i];
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->fm1 = state->f;
    state->gm1 = state->g.ptr.p_double[i];
    state->x.ptr.p_double[i] = v+state->teststep*state->s.ptr.p_double[i];
    if( state->hasbndu.ptr.p_bool[i] )
    {
        state->x.ptr.p_double[i] = ae_minreal(state->x.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
    }
    state->xp1 = state->x.ptr.p_double[i];
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->fp1 = state->f;
    state->gp1 = state->g.ptr.p_double[i];
    state->x.ptr.p_double[i] = (state->xm1+state->xp1)/2;
    if( state->hasbndl.ptr.p_bool[i] )
    {
        state->x.ptr.p_double[i] = ae_maxreal(state->x.ptr.p_double[i], state->bndl.ptr.p_double[i], _state);
    }
    if( state->hasbndu.ptr.p_bool[i] )
    {
        state->x.ptr.p_double[i] = ae_minreal(state->x.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
    }
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->x.ptr.p_double[i] = v;
    if( !derivativecheck(state->fm1, state->gm1, state->fp1, state->gp1, state->f, state->g.ptr.p_double[i], state->xp1-state->xm1, _state) )
    {
        state->repvaridx = i;
        state->repterminationtype = -7;
        sasstopoptimization(&state->sas, _state);
        result = ae_false;
        return result;
    }
    i = i+1;
    goto lbl_44;
lbl_46:
    state->needfg = ae_false;
lbl_42:
    
    /*
     * Main cycle of BLEIC-PG algorithm
     */
    state->repterminationtype = 4;
    badbfgsits = 0;
    state->lastgoodstep = 0;
    state->lastscaledgoodstep = 0;
    state->maxscaledgrad = 0;
    state->nonmonotoniccnt = n+state->nic;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minbleic_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,0) )
    {
        goto lbl_47;
    }
    state->needfg = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needfg = ae_false;
    goto lbl_48;
lbl_47:
    state->needf = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needf = ae_false;
lbl_48:
    state->fc = state->f;
    trimprepare(state->f, &state->trimthreshold, _state);
    state->repnfev = state->repnfev+1;
    if( !state->xrep )
    {
        goto lbl_49;
    }
    
    /*
     * Report current point
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fc;
    state->xupdated = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->xupdated = ae_false;
lbl_49:
lbl_51:
    if( ae_false )
    {
        goto lbl_52;
    }
    
    /*
     * Phase 1
     *
     * (a) calculate unconstrained gradient
     * (b) determine active set
     * (c) update MaxScaledGrad
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minbleic_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,0) )
    {
        goto lbl_53;
    }
    
    /*
     * Analytic gradient
     */
    state->needfg = ae_true;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->needfg = ae_false;
    goto lbl_54;
lbl_53:
    
    /*
     * Numerical differentiation
     */
    state->needf = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->fbase = state->f;
    i = 0;
lbl_55:
    if( i>n-1 )
    {
        goto lbl_57;
    }
    v = state->x.ptr.p_double[i];
    b = ae_false;
    if( state->hasbndl.ptr.p_bool[i] )
    {
        b = b||ae_fp_less(v-state->diffstep*state->s.ptr.p_double[i],state->bndl.ptr.p_double[i]);
    }
    if( state->hasbndu.ptr.p_bool[i] )
    {
        b = b||ae_fp_greater(v+state->diffstep*state->s.ptr.p_double[i],state->bndu.ptr.p_double[i]);
    }
    if( b )
    {
        goto lbl_58;
    }
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->fp2 = state->f;
    state->g.ptr.p_double[i] = (8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/(6*state->diffstep*state->s.ptr.p_double[i]);
    goto lbl_59;
lbl_58:
    state->xm1 = v-state->diffstep*state->s.ptr.p_double[i];
    state->xp1 = v+state->diffstep*state->s.ptr.p_double[i];
    if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(state->xm1,state->bndl.ptr.p_double[i]) )
    {
        state->xm1 = state->bndl.ptr.p_double[i];
    }
    if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(state->xp1,state->bndu.ptr.p_double[i]) )
    {
        state->xp1 = state->bndu.ptr.p_double[i];
    }
    state->x.ptr.p_double[i] = state->xm1;
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = state->xp1;
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->fp1 = state->f;
    if( ae_fp_neq(state->xm1,state->xp1) )
    {
        state->g.ptr.p_double[i] = (state->fp1-state->fm1)/(state->xp1-state->xm1);
    }
    else
    {
        state->g.ptr.p_double[i] = 0;
    }
lbl_59:
    state->x.ptr.p_double[i] = v;
    i = i+1;
    goto lbl_55;
lbl_57:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_54:
    state->fc = state->f;
    ae_v_move(&state->gc.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    sasreactivateconstraintsprec(&state->sas, &state->gc, _state);
    v = 0.0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->gc.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    state->maxscaledgrad = ae_maxreal(state->maxscaledgrad, ae_sqrt(v, _state), _state);
    
    /*
     * Phase 2: perform steepest descent step.
     *
     * NextAction control variable is set on exit from this loop:
     * * NextAction>0 in case we have to proceed to Phase 3 (L-BFGS step)
     * * NextAction<0 in case we have to proceed to Phase 1 (recalculate active set)
     * * NextAction=0 in case we found solution (step size or function change are small enough)
     */
    nextaction = 0;
lbl_60:
    if( ae_false )
    {
        goto lbl_61;
    }
    
    /*
     * Check gradient-based stopping criteria
     */
    if( ae_fp_less_eq(sasscaledconstrainednorm(&state->sas, &state->gc, _state),state->epsg) )
    {
        
        /*
         * Gradient is small enough, stop iterations
         */
        state->repterminationtype = 4;
        nextaction = 0;
        goto lbl_61;
    }
    
    /*
     * Calculate normalized constrained descent direction, store to D.
     * Try to use previous scaled step length as initial estimate for new step.
     *
     * NOTE: D can be exactly zero, in this case Stp is set to 1.0
     */
    sasconstraineddescentprec(&state->sas, &state->gc, &state->d, _state);
    v = 0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->d.ptr.p_double[i]/state->s.ptr.p_double[i], _state);
    }
    v = ae_sqrt(v, _state);
    if( ae_fp_greater(state->lastscaledgoodstep,0)&&ae_fp_greater(v,0) )
    {
        state->stp = state->lastscaledgoodstep/v;
    }
    else
    {
        state->stp = 1.0;
    }
    
    /*
     * Calculate bound on step length.
     * Enforce user-supplied limit on step length.
     */
    sasexploredirection(&state->sas, &state->d, &state->curstpmax, &state->cidx, &state->cval, _state);
    state->activationstep = state->curstpmax;
    if( state->cidx>=0&&ae_fp_eq(state->activationstep,0) )
    {
        sasimmediateactivation(&state->sas, state->cidx, state->cval, _state);
        goto lbl_60;
    }
    if( ae_fp_greater(state->stpmax,0) )
    {
        state->curstpmax = ae_minreal(state->curstpmax, state->stpmax, _state);
    }
    
    /*
     * Report beginning of line search (if requested by caller).
     * See description of the MinBLEICState for more information
     * about fields accessible to caller.
     *
     * Caller may do following:
     * * change State.Stp and load better initial estimate of
     *   the step length.
     */
    if( !state->drep )
    {
        goto lbl_62;
    }
    minbleic_clearrequestfields(state, _state);
    state->lsstart = ae_true;
    state->lbfgssearch = ae_false;
    state->boundedstep = state->cidx>=0;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->g.ptr.p_double[0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fc;
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->lsstart = ae_false;
lbl_62:
    
    /*
     * Perform optimization of F along XC+alpha*D.
     */
    state->mcstage = 0;
    ae_v_move(&state->xn.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->gn.ptr.p_double[0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fn = state->fc;
    mcsrch(n, &state->xn, &state->fn, &state->gn, &state->d, &state->stp, state->curstpmax, minbleic_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
lbl_64:
    if( state->mcstage==0 )
    {
        goto lbl_65;
    }
    
    /*
     * Enforce constraints (correction) in XN.
     * Copy current point from XN to X.
     */
    sascorrection(&state->sas, &state->xn, &penalty, _state);
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->xn.ptr.p_double[i];
    }
    
    /*
     * Gradient, either user-provided or numerical differentiation
     */
    minbleic_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,0) )
    {
        goto lbl_66;
    }
    
    /*
     * Analytic gradient
     */
    state->needfg = ae_true;
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->needfg = ae_false;
    state->repnfev = state->repnfev+1;
    goto lbl_67;
lbl_66:
    
    /*
     * Numerical differentiation
     */
    state->needf = ae_true;
    state->rstate.stage = 16;
    goto lbl_rcomm;
lbl_16:
    state->fbase = state->f;
    i = 0;
lbl_68:
    if( i>n-1 )
    {
        goto lbl_70;
    }
    v = state->x.ptr.p_double[i];
    b = ae_false;
    if( state->hasbndl.ptr.p_bool[i] )
    {
        b = b||ae_fp_less(v-state->diffstep*state->s.ptr.p_double[i],state->bndl.ptr.p_double[i]);
    }
    if( state->hasbndu.ptr.p_bool[i] )
    {
        b = b||ae_fp_greater(v+state->diffstep*state->s.ptr.p_double[i],state->bndu.ptr.p_double[i]);
    }
    if( b )
    {
        goto lbl_71;
    }
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 17;
    goto lbl_rcomm;
lbl_17:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 18;
    goto lbl_rcomm;
lbl_18:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 19;
    goto lbl_rcomm;
lbl_19:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 20;
    goto lbl_rcomm;
lbl_20:
    state->fp2 = state->f;
    state->g.ptr.p_double[i] = (8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/(6*state->diffstep*state->s.ptr.p_double[i]);
    state->repnfev = state->repnfev+4;
    goto lbl_72;
lbl_71:
    state->xm1 = v-state->diffstep*state->s.ptr.p_double[i];
    state->xp1 = v+state->diffstep*state->s.ptr.p_double[i];
    if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(state->xm1,state->bndl.ptr.p_double[i]) )
    {
        state->xm1 = state->bndl.ptr.p_double[i];
    }
    if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(state->xp1,state->bndu.ptr.p_double[i]) )
    {
        state->xp1 = state->bndu.ptr.p_double[i];
    }
    state->x.ptr.p_double[i] = state->xm1;
    state->rstate.stage = 21;
    goto lbl_rcomm;
lbl_21:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = state->xp1;
    state->rstate.stage = 22;
    goto lbl_rcomm;
lbl_22:
    state->fp1 = state->f;
    if( ae_fp_neq(state->xm1,state->xp1) )
    {
        state->g.ptr.p_double[i] = (state->fp1-state->fm1)/(state->xp1-state->xm1);
    }
    else
    {
        state->g.ptr.p_double[i] = 0;
    }
    state->repnfev = state->repnfev+2;
lbl_72:
    state->x.ptr.p_double[i] = v;
    i = i+1;
    goto lbl_68;
lbl_70:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_67:
    
    /*
     * Back to MCSRCH
     *
     * NOTE: penalty term from correction is added to FN in order
     *       to penalize increase in infeasibility.
     */
    state->fn = state->f+minbleic_penaltyfactor*state->maxscaledgrad*penalty;
    ae_v_move(&state->gn.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    trimfunction(&state->fn, &state->gn, n, state->trimthreshold, _state);
    mcsrch(n, &state->xn, &state->fn, &state->gn, &state->d, &state->stp, state->curstpmax, minbleic_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
    goto lbl_64;
lbl_65:
    
    /*
     * Handle possible failure of the line search
     */
    if( mcinfo!=1&&mcinfo!=5 )
    {
        
        /*
         * We can not find step which decreases function value. We have
         * two possibilities:
         * (a) numerical properties of the function do not allow us to
         *     find good solution.
         * (b) we are close to activation of some constraint, and it is
         *     so close that step which activates it leads to change in
         *     target function which is smaller than numerical noise.
         *
         * Optimization algorithm must be able to handle case (b), because
         * inability to handle it will cause failure when algorithm
         * started very close to boundary of the feasible area.
         *
         * In order to correctly handle such cases we allow limited amount
         * of small steps which increase function value.
         */
        v = 0.0;
        for(i=0; i<=n-1; i++)
        {
            v = v+ae_sqr(state->d.ptr.p_double[i]*state->curstpmax/state->s.ptr.p_double[i], _state);
        }
        v = ae_sqrt(v, _state);
        if( (state->cidx>=0&&ae_fp_less_eq(v,minbleic_maxnonmonotoniclen))&&state->nonmonotoniccnt>0 )
        {
            
            /*
             * We enforce non-monotonic step:
             * * Stp    := CurStpMax
             * * MCINFO := 5
             * * XN     := XC+CurStpMax*D
             * * non-monotonic counter is decreased
             */
            state->stp = state->curstpmax;
            mcinfo = 5;
            v = state->curstpmax;
            ae_v_move(&state->xn.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
            ae_v_addd(&state->xn.ptr.p_double[0], 1, &state->d.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
            state->nonmonotoniccnt = state->nonmonotoniccnt-1;
        }
        else
        {
            
            /*
             * Numerical properties of the function does not allow us to solve problem
             */
            state->repterminationtype = 7;
            nextaction = 0;
            goto lbl_61;
        }
    }
    
    /*
     * Current point is updated.
     */
    ae_v_move(&state->xp.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->gp.ptr.p_double[0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fp = state->fc;
    actstatus = sasmoveto(&state->sas, &state->xn, state->cidx>=0&&ae_fp_greater_eq(state->stp,state->activationstep), state->cidx, state->cval, _state);
    ae_v_move(&state->gc.ptr.p_double[0], 1, &state->gn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fc = state->fn;
    state->repinneriterationscount = state->repinneriterationscount+1;
    if( !state->xrep )
    {
        goto lbl_73;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minbleic_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 23;
    goto lbl_rcomm;
lbl_23:
    state->xupdated = ae_false;
lbl_73:
    
    /*
     * Check for stopping.
     *
     * Step, gradient and function-based stopping criteria are tested only
     * for steps which satisfy Wolfe conditions.
     *
     * MaxIts-based stopping condition is checked for all steps
     */
    if( mcinfo==1 )
    {
        
        /*
         * Step is small enough
         */
        v = 0;
        vv = 0;
        for(i=0; i<=n-1; i++)
        {
            v = v+ae_sqr((state->sas.xc.ptr.p_double[i]-state->xp.ptr.p_double[i])/state->s.ptr.p_double[i], _state);
            vv = vv+ae_sqr(state->sas.xc.ptr.p_double[i]-state->xp.ptr.p_double[i], _state);
        }
        v = ae_sqrt(v, _state);
        vv = ae_sqrt(vv, _state);
        if( ae_fp_less_eq(v,state->epsx) )
        {
            state->repterminationtype = 2;
            nextaction = 0;
            goto lbl_61;
        }
        state->lastgoodstep = vv;
        minbleic_updateestimateofgoodstep(&state->lastscaledgoodstep, v, _state);
        
        /*
         * Function change is small enough
         */
        if( ae_fp_less_eq(ae_fabs(state->fp-state->fc, _state),state->epsf*ae_maxreal(ae_fabs(state->fc, _state), ae_maxreal(ae_fabs(state->fp, _state), 1.0, _state), _state)) )
        {
            
            /*
             * Function change is small enough
             */
            state->repterminationtype = 1;
            nextaction = 0;
            goto lbl_61;
        }
    }
    if( state->maxits>0&&state->repinneriterationscount>=state->maxits )
    {
        
        /*
         * Required number of iterations was performed
         */
        state->repterminationtype = 5;
        nextaction = 0;
        goto lbl_61;
    }
    
    /*
     * Decide where to move:
     * * in case only "candidate" constraints were activated, repeat stage 2
     * * in case no constraints was activated, move to stage 3
     * * otherwise, move to stage 1 (re-evaluation of the active set)
     */
    if( actstatus==0 )
    {
        goto lbl_60;
    }
    if( actstatus<0 )
    {
        nextaction = 1;
    }
    else
    {
        nextaction = -1;
    }
    goto lbl_61;
    goto lbl_60;
lbl_61:
    if( nextaction<0 )
    {
        goto lbl_51;
    }
    if( nextaction==0 )
    {
        goto lbl_52;
    }
    
    /*
     * Phase 3: L-BFGS step
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minbleic_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,0) )
    {
        goto lbl_75;
    }
    
    /*
     * Analytic gradient
     */
    state->needfg = ae_true;
    state->rstate.stage = 24;
    goto lbl_rcomm;
lbl_24:
    state->needfg = ae_false;
    state->repnfev = state->repnfev+1;
    goto lbl_76;
lbl_75:
    
    /*
     * Numerical differentiation
     */
    state->needf = ae_true;
    state->rstate.stage = 25;
    goto lbl_rcomm;
lbl_25:
    state->fbase = state->f;
    i = 0;
lbl_77:
    if( i>n-1 )
    {
        goto lbl_79;
    }
    v = state->x.ptr.p_double[i];
    b = ae_false;
    if( state->hasbndl.ptr.p_bool[i] )
    {
        b = b||ae_fp_less(v-state->diffstep*state->s.ptr.p_double[i],state->bndl.ptr.p_double[i]);
    }
    if( state->hasbndu.ptr.p_bool[i] )
    {
        b = b||ae_fp_greater(v+state->diffstep*state->s.ptr.p_double[i],state->bndu.ptr.p_double[i]);
    }
    if( b )
    {
        goto lbl_80;
    }
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 26;
    goto lbl_rcomm;
lbl_26:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 27;
    goto lbl_rcomm;
lbl_27:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 28;
    goto lbl_rcomm;
lbl_28:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 29;
    goto lbl_rcomm;
lbl_29:
    state->fp2 = state->f;
    state->g.ptr.p_double[i] = (8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/(6*state->diffstep*state->s.ptr.p_double[i]);
    state->repnfev = state->repnfev+4;
    goto lbl_81;
lbl_80:
    state->xm1 = v-state->diffstep*state->s.ptr.p_double[i];
    state->xp1 = v+state->diffstep*state->s.ptr.p_double[i];
    if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(state->xm1,state->bndl.ptr.p_double[i]) )
    {
        state->xm1 = state->bndl.ptr.p_double[i];
    }
    if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(state->xp1,state->bndu.ptr.p_double[i]) )
    {
        state->xp1 = state->bndu.ptr.p_double[i];
    }
    state->x.ptr.p_double[i] = state->xm1;
    state->rstate.stage = 30;
    goto lbl_rcomm;
lbl_30:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = state->xp1;
    state->rstate.stage = 31;
    goto lbl_rcomm;
lbl_31:
    state->fp1 = state->f;
    if( ae_fp_neq(state->xm1,state->xp1) )
    {
        state->g.ptr.p_double[i] = (state->fp1-state->fm1)/(state->xp1-state->xm1);
    }
    else
    {
        state->g.ptr.p_double[i] = 0;
    }
    state->repnfev = state->repnfev+2;
lbl_81:
    state->x.ptr.p_double[i] = v;
    i = i+1;
    goto lbl_77;
lbl_79:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_76:
    state->fc = state->f;
    trimprepare(state->fc, &state->trimthreshold, _state);
    ae_v_move(&state->gc.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_moveneg(&state->d.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    sasconstraineddirection(&state->sas, &state->gc, _state);
    sasconstraineddirectionprec(&state->sas, &state->d, _state);
    ginit = 0.0;
    for(i=0; i<=n-1; i++)
    {
        ginit = ginit+ae_sqr(state->gc.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    ginit = ae_sqrt(ginit, _state);
    state->k = 0;
lbl_82:
    if( state->k>n )
    {
        goto lbl_83;
    }
    
    /*
     * Main cycle: prepare to 1-D line search
     */
    state->p = state->k%m;
    state->q = ae_minint(state->k, m-1, _state);
    
    /*
     * Store X[k], G[k]
     */
    ae_v_moveneg(&state->sk.ptr.pp_double[state->p][0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_moveneg(&state->yk.ptr.pp_double[state->p][0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * Try to use previous scaled step length as initial estimate for new step.
     */
    v = 0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->d.ptr.p_double[i]/state->s.ptr.p_double[i], _state);
    }
    v = ae_sqrt(v, _state);
    if( ae_fp_greater(state->lastscaledgoodstep,0)&&ae_fp_greater(v,0) )
    {
        state->stp = state->lastscaledgoodstep/v;
    }
    else
    {
        state->stp = 1.0;
    }
    
    /*
     * Calculate bound on step length
     */
    sasexploredirection(&state->sas, &state->d, &state->curstpmax, &state->cidx, &state->cval, _state);
    state->activationstep = state->curstpmax;
    if( state->cidx>=0&&ae_fp_eq(state->activationstep,0) )
    {
        goto lbl_83;
    }
    if( ae_fp_greater(state->stpmax,0) )
    {
        v = ae_v_dotproduct(&state->d.ptr.p_double[0], 1, &state->d.ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = ae_sqrt(v, _state);
        if( ae_fp_greater(v,0) )
        {
            state->curstpmax = ae_minreal(state->curstpmax, state->stpmax/v, _state);
        }
    }
    
    /*
     * Report beginning of line search (if requested by caller).
     * See description of the MinBLEICState for more information
     * about fields accessible to caller.
     *
     * Caller may do following:
     * * change State.Stp and load better initial estimate of
     *   the step length.
     * Caller may not terminate algorithm.
     */
    if( !state->drep )
    {
        goto lbl_84;
    }
    minbleic_clearrequestfields(state, _state);
    state->lsstart = ae_true;
    state->lbfgssearch = ae_true;
    state->boundedstep = state->cidx>=0;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->rstate.stage = 32;
    goto lbl_rcomm;
lbl_32:
    state->lsstart = ae_false;
lbl_84:
    
    /*
     * Minimize F(x+alpha*d)
     */
    ae_v_move(&state->xn.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->gn.ptr.p_double[0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fn = state->fc;
    state->mcstage = 0;
    mcsrch(n, &state->xn, &state->fn, &state->gn, &state->d, &state->stp, state->curstpmax, minbleic_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
lbl_86:
    if( state->mcstage==0 )
    {
        goto lbl_87;
    }
    
    /*
     * Perform correction (constraints are enforced)
     * Copy XN to X
     */
    sascorrection(&state->sas, &state->xn, &penalty, _state);
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->xn.ptr.p_double[i];
    }
    
    /*
     * Gradient, either user-provided or numerical differentiation
     */
    minbleic_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,0) )
    {
        goto lbl_88;
    }
    
    /*
     * Analytic gradient
     */
    state->needfg = ae_true;
    state->rstate.stage = 33;
    goto lbl_rcomm;
lbl_33:
    state->needfg = ae_false;
    state->repnfev = state->repnfev+1;
    goto lbl_89;
lbl_88:
    
    /*
     * Numerical differentiation
     */
    state->needf = ae_true;
    state->rstate.stage = 34;
    goto lbl_rcomm;
lbl_34:
    state->fbase = state->f;
    i = 0;
lbl_90:
    if( i>n-1 )
    {
        goto lbl_92;
    }
    v = state->x.ptr.p_double[i];
    b = ae_false;
    if( state->hasbndl.ptr.p_bool[i] )
    {
        b = b||ae_fp_less(v-state->diffstep*state->s.ptr.p_double[i],state->bndl.ptr.p_double[i]);
    }
    if( state->hasbndu.ptr.p_bool[i] )
    {
        b = b||ae_fp_greater(v+state->diffstep*state->s.ptr.p_double[i],state->bndu.ptr.p_double[i]);
    }
    if( b )
    {
        goto lbl_93;
    }
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 35;
    goto lbl_rcomm;
lbl_35:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 36;
    goto lbl_rcomm;
lbl_36:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 37;
    goto lbl_rcomm;
lbl_37:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 38;
    goto lbl_rcomm;
lbl_38:
    state->fp2 = state->f;
    state->g.ptr.p_double[i] = (8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/(6*state->diffstep*state->s.ptr.p_double[i]);
    state->repnfev = state->repnfev+4;
    goto lbl_94;
lbl_93:
    state->xm1 = v-state->diffstep*state->s.ptr.p_double[i];
    state->xp1 = v+state->diffstep*state->s.ptr.p_double[i];
    if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(state->xm1,state->bndl.ptr.p_double[i]) )
    {
        state->xm1 = state->bndl.ptr.p_double[i];
    }
    if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(state->xp1,state->bndu.ptr.p_double[i]) )
    {
        state->xp1 = state->bndu.ptr.p_double[i];
    }
    state->x.ptr.p_double[i] = state->xm1;
    state->rstate.stage = 39;
    goto lbl_rcomm;
lbl_39:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = state->xp1;
    state->rstate.stage = 40;
    goto lbl_rcomm;
lbl_40:
    state->fp1 = state->f;
    if( ae_fp_neq(state->xm1,state->xp1) )
    {
        state->g.ptr.p_double[i] = (state->fp1-state->fm1)/(state->xp1-state->xm1);
    }
    else
    {
        state->g.ptr.p_double[i] = 0;
    }
    state->repnfev = state->repnfev+2;
lbl_94:
    state->x.ptr.p_double[i] = v;
    i = i+1;
    goto lbl_90;
lbl_92:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_89:
    
    /*
     * Back to MCSRCH
     *
     * NOTE: penalty term from correction is added to FN in order
     *       to penalize increase in infeasibility.
     */
    state->fn = state->f+minbleic_penaltyfactor*state->maxscaledgrad*penalty;
    ae_v_move(&state->gn.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    sasconstraineddirection(&state->sas, &state->gn, _state);
    trimfunction(&state->fn, &state->gn, n, state->trimthreshold, _state);
    mcsrch(n, &state->xn, &state->fn, &state->gn, &state->d, &state->stp, state->curstpmax, minbleic_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
    goto lbl_86;
lbl_87:
    ae_v_add(&state->sk.ptr.pp_double[state->p][0], 1, &state->xn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_add(&state->yk.ptr.pp_double[state->p][0], 1, &state->gn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * Handle possible failure of the line search
     */
    if( mcinfo!=1&&mcinfo!=5 )
    {
        goto lbl_83;
    }
    
    /*
     * Current point is updated.
     */
    ae_v_move(&state->xp.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->gp.ptr.p_double[0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fp = state->fc;
    actstatus = sasmoveto(&state->sas, &state->xn, state->cidx>=0&&ae_fp_greater_eq(state->stp,state->activationstep), state->cidx, state->cval, _state);
    ae_v_move(&state->gc.ptr.p_double[0], 1, &state->gn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fc = state->fn;
    if( !state->xrep )
    {
        goto lbl_95;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minbleic_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 41;
    goto lbl_rcomm;
lbl_41:
    state->xupdated = ae_false;
lbl_95:
    state->repinneriterationscount = state->repinneriterationscount+1;
    
    /*
     * Update length of the good step
     */
    if( mcinfo==1 )
    {
        v = 0;
        vv = 0;
        for(i=0; i<=n-1; i++)
        {
            v = v+ae_sqr((state->sas.xc.ptr.p_double[i]-state->xp.ptr.p_double[i])/state->s.ptr.p_double[i], _state);
            vv = vv+ae_sqr(state->sas.xc.ptr.p_double[i]-state->xp.ptr.p_double[i], _state);
        }
        state->lastgoodstep = ae_sqrt(vv, _state);
        minbleic_updateestimateofgoodstep(&state->lastscaledgoodstep, ae_sqrt(v, _state), _state);
    }
    
    /*
     * Termination of the L-BFGS algorithm:
     * a) line search was performed with activation of constraint
     * b) scaled gradient decreased below GDecay
     * c) iterations counter >= MaxIts
     */
    if( actstatus>=0 )
    {
        goto lbl_83;
    }
    v = 0.0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->gc.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    if( ae_fp_less(ae_sqrt(v, _state),gdecay*ginit) )
    {
        goto lbl_83;
    }
    if( state->maxits>0&&state->repinneriterationscount>=state->maxits )
    {
        goto lbl_83;
    }
    
    /*
     * Update L-BFGS model:
     * * calculate Rho[k]
     * * calculate d(k+1) = -H(k+1)*g(k+1)
     *   (use constrained preconditioner to perform multiplication)
     */
    v = ae_v_dotproduct(&state->yk.ptr.pp_double[state->p][0], 1, &state->sk.ptr.pp_double[state->p][0], 1, ae_v_len(0,n-1));
    vv = ae_v_dotproduct(&state->yk.ptr.pp_double[state->p][0], 1, &state->yk.ptr.pp_double[state->p][0], 1, ae_v_len(0,n-1));
    if( ae_fp_eq(v,0)||ae_fp_eq(vv,0) )
    {
        goto lbl_83;
    }
    state->rho.ptr.p_double[state->p] = 1/v;
    ae_v_move(&state->work.ptr.p_double[0], 1, &state->gn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    for(i=state->k; i>=state->k-state->q; i--)
    {
        ic = i%m;
        v = ae_v_dotproduct(&state->sk.ptr.pp_double[ic][0], 1, &state->work.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->theta.ptr.p_double[ic] = v;
        vv = v*state->rho.ptr.p_double[ic];
        ae_v_subd(&state->work.ptr.p_double[0], 1, &state->yk.ptr.pp_double[ic][0], 1, ae_v_len(0,n-1), vv);
    }
    sasconstraineddirectionprec(&state->sas, &state->work, _state);
    for(i=state->k-state->q; i<=state->k; i++)
    {
        ic = i%m;
        v = ae_v_dotproduct(&state->yk.ptr.pp_double[ic][0], 1, &state->work.ptr.p_double[0], 1, ae_v_len(0,n-1));
        vv = state->rho.ptr.p_double[ic]*(-v+state->theta.ptr.p_double[ic]);
        ae_v_addd(&state->work.ptr.p_double[0], 1, &state->sk.ptr.pp_double[ic][0], 1, ae_v_len(0,n-1), vv);
    }
    ae_v_moveneg(&state->d.ptr.p_double[0], 1, &state->work.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->k = state->k+1;
    goto lbl_82;
lbl_83:
    
    /*
     * Decrease decay coefficient. Subsequent L-BFGS stages will
     * have more stringent stopping criteria.
     */
    gdecay = ae_maxreal(gdecay*minbleic_decaycorrection, minbleic_mindecay, _state);
    goto lbl_51;
lbl_52:
    sasstopoptimization(&state->sas, _state);
    state->repouteriterationscount = 1;
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
    state->rstate.ia.ptr.p_int[3] = j;
    state->rstate.ia.ptr.p_int[4] = badbfgsits;
    state->rstate.ia.ptr.p_int[5] = nextaction;
    state->rstate.ia.ptr.p_int[6] = mcinfo;
    state->rstate.ia.ptr.p_int[7] = actstatus;
    state->rstate.ia.ptr.p_int[8] = ic;
    state->rstate.ba.ptr.p_bool[0] = b;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = vv;
    state->rstate.ra.ptr.p_double[2] = penalty;
    state->rstate.ra.ptr.p_double[3] = ginit;
    state->rstate.ra.ptr.p_double[4] = gdecay;
    return result;
}


/*************************************************************************
BLEIC results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -7   gradient verification failed.
                       See MinBLEICSetGradientCheck() for more information.
                * -3   inconsistent constraints. Feasible point is
                       either nonexistent or too hard to find. Try to
                       restart optimizer with better initial approximation
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken
                More information about fields of this  structure  can  be
                found in the comments on MinBLEICReport datatype.
   
  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresults(minbleicstate* state,
     /* Real    */ ae_vector* x,
     minbleicreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _minbleicreport_clear(rep);

    minbleicresultsbuf(state, x, rep, _state);
}


/*************************************************************************
BLEIC results

Buffered implementation of MinBLEICResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresultsbuf(minbleicstate* state,
     /* Real    */ ae_vector* x,
     minbleicreport* rep,
     ae_state *_state)
{
    ae_int_t i;


    if( x->cnt<state->nmain )
    {
        ae_vector_set_length(x, state->nmain, _state);
    }
    rep->iterationscount = state->repinneriterationscount;
    rep->inneriterationscount = state->repinneriterationscount;
    rep->outeriterationscount = state->repouteriterationscount;
    rep->nfev = state->repnfev;
    rep->varidx = state->repvaridx;
    rep->terminationtype = state->repterminationtype;
    if( state->repterminationtype>0 )
    {
        ae_v_move(&x->ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,state->nmain-1));
    }
    else
    {
        for(i=0; i<=state->nmain-1; i++)
        {
            x->ptr.p_double[i] = _state->v_nan;
        }
    }
    rep->debugeqerr = state->repdebugeqerr;
    rep->debugfs = state->repdebugfs;
    rep->debugff = state->repdebugff;
    rep->debugdx = state->repdebugdx;
    rep->debugfeasqpits = state->repdebugfeasqpits;
    rep->debugfeasgpaits = state->repdebugfeasgpaits;
}


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicrestartfrom(minbleicstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;


    n = state->nmain;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(x->cnt>=n, "MinBLEICRestartFrom: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinBLEICRestartFrom: X contains infinite or NaN values!", _state);
    
    /*
     * Set XC
     */
    ae_v_move(&state->xstart.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * prepare RComm facilities
     */
    ae_vector_set_length(&state->rstate.ia, 8+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 4+1, _state);
    state->rstate.stage = -1;
    minbleic_clearrequestfields(state, _state);
    sasstopoptimization(&state->sas, _state);
}


/*************************************************************************
This subroutine finalizes internal structures after emergency  termination
from State.LSStart report (see comments on MinBLEICState for more information).

INPUT PARAMETERS:
    State   -   structure after exit from LSStart report

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicemergencytermination(minbleicstate* state, ae_state *_state)
{


    sasstopoptimization(&state->sas, _state);
}


/*************************************************************************
This  subroutine  turns  on  verification  of  the  user-supplied analytic
gradient:
* user calls this subroutine before optimization begins
* MinBLEICOptimize() is called
* prior to  actual  optimization, for each component  of  parameters being
  optimized X[i] algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * if needed, steps are bounded with respect to constraints on X[]
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point
  * in case difference between prediction and actual value is higher  than
    some predetermined threshold, algorithm stops with completion code -7;
    Rep.VarIdx is set to index of the parameter with incorrect derivative.
* after verification is over, algorithm proceeds to the actual optimization.

NOTE 1: verification  needs  N (parameters count) gradient evaluations. It
        is very costly and you should use  it  only  for  low  dimensional
        problems,  when  you  want  to  be  sure  that  you've   correctly
        calculated  analytic  derivatives.  You  should  not use it in the
        production code (unless you want to check derivatives provided  by
        some third party).

NOTE 2: you  should  carefully  choose  TestStep. Value which is too large
        (so large that function behaviour is significantly non-cubic) will
        lead to false alarms. You may use  different  step  for  different
        parameters by means of setting scale with MinBLEICSetScale().

NOTE 3: this function may lead to false positives. In case it reports that
        I-th  derivative was calculated incorrectly, you may decrease test
        step  and  try  one  more  time  - maybe your function changes too
        sharply  and  your  step  is  too  large for such rapidly chanding
        function.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification

  -- ALGLIB --
     Copyright 15.06.2012 by Bochkanov Sergey
*************************************************************************/
void minbleicsetgradientcheck(minbleicstate* state,
     double teststep,
     ae_state *_state)
{


    ae_assert(ae_isfinite(teststep, _state), "MinBLEICSetGradientCheck: TestStep contains NaN or Infinite", _state);
    ae_assert(ae_fp_greater_eq(teststep,0), "MinBLEICSetGradientCheck: invalid argument TestStep(TestStep<0)", _state);
    state->teststep = teststep;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forget to clear something)
*************************************************************************/
static void minbleic_clearrequestfields(minbleicstate* state,
     ae_state *_state)
{


    state->needf = ae_false;
    state->needfg = ae_false;
    state->xupdated = ae_false;
    state->lsstart = ae_false;
}


/*************************************************************************
Internal initialization subroutine
*************************************************************************/
static void minbleic_minbleicinitinternal(ae_int_t n,
     /* Real    */ ae_vector* x,
     double diffstep,
     minbleicstate* state,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_matrix c;
    ae_vector ct;

    ae_frame_make(_state, &_frame_block);
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);

    
    /*
     * Initialize
     */
    state->teststep = 0;
    state->nmain = n;
    state->diffstep = diffstep;
    sasinit(n, &state->sas, _state);
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->hasbndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->hasbndu, n, _state);
    ae_vector_set_length(&state->xstart, n, _state);
    ae_vector_set_length(&state->gc, n, _state);
    ae_vector_set_length(&state->xn, n, _state);
    ae_vector_set_length(&state->gn, n, _state);
    ae_vector_set_length(&state->xp, n, _state);
    ae_vector_set_length(&state->gp, n, _state);
    ae_vector_set_length(&state->d, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->g, n, _state);
    ae_vector_set_length(&state->work, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->hasbndl.ptr.p_bool[i] = ae_false;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
        state->hasbndu.ptr.p_bool[i] = ae_false;
        state->s.ptr.p_double[i] = 1.0;
    }
    minbleicsetlc(state, &c, &ct, 0, _state);
    minbleicsetcond(state, 0.0, 0.0, 0.0, 0, _state);
    minbleicsetxrep(state, ae_false, _state);
    minbleicsetdrep(state, ae_false, _state);
    minbleicsetstpmax(state, 0.0, _state);
    minbleicsetprecdefault(state, _state);
    minbleicrestartfrom(state, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This subroutine updates estimate of the good step length given:
1) previous estimate
2) new length of the good step

It makes sure that estimate does not change too rapidly - ratio of new and
old estimates will be at least 0.01, at most 100.0

In case previous estimate of good step is zero (no estimate), new estimate
is used unconditionally.

  -- ALGLIB --
     Copyright 16.01.2013 by Bochkanov Sergey
*************************************************************************/
static void minbleic_updateestimateofgoodstep(double* estimate,
     double newstep,
     ae_state *_state)
{


    if( ae_fp_eq(*estimate,0) )
    {
        *estimate = newstep;
        return;
    }
    if( ae_fp_less(newstep,*estimate*0.01) )
    {
        *estimate = *estimate*0.01;
        return;
    }
    if( ae_fp_greater(newstep,*estimate*100) )
    {
        *estimate = *estimate*100;
        return;
    }
    *estimate = newstep;
}


ae_bool _minbleicstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minbleicstate *p = (minbleicstate*)_p;
    ae_touch_ptr((void*)p);
    if( !_sactiveset_init(&p->sas, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->diagh, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init(&p->rstate, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->gc, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xn, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->gn, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xp, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->gp, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->cleic, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xstart, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_snnlssolver_init(&p->solver, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmpprec, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->work, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_linminstate_init(&p->lstate, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rho, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->yk, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->sk, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->theta, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _minbleicstate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minbleicstate *dst = (minbleicstate*)_dst;
    minbleicstate *src = (minbleicstate*)_src;
    dst->nmain = src->nmain;
    dst->nslack = src->nslack;
    dst->epsg = src->epsg;
    dst->epsf = src->epsf;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->drep = src->drep;
    dst->stpmax = src->stpmax;
    dst->diffstep = src->diffstep;
    if( !_sactiveset_init_copy(&dst->sas, &src->sas, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic) )
        return ae_false;
    dst->prectype = src->prectype;
    if( !ae_vector_init_copy(&dst->diagh, &src->diagh, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic) )
        return ae_false;
    dst->f = src->f;
    if( !ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic) )
        return ae_false;
    dst->needf = src->needf;
    dst->needfg = src->needfg;
    dst->xupdated = src->xupdated;
    dst->lsstart = src->lsstart;
    dst->lbfgssearch = src->lbfgssearch;
    dst->boundedstep = src->boundedstep;
    dst->teststep = src->teststep;
    if( !_rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->gc, &src->gc, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->xn, &src->xn, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->gn, &src->gn, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->xp, &src->xp, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->gp, &src->gp, _state, make_automatic) )
        return ae_false;
    dst->fc = src->fc;
    dst->fn = src->fn;
    dst->fp = src->fp;
    if( !ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->cleic, &src->cleic, _state, make_automatic) )
        return ae_false;
    dst->nec = src->nec;
    dst->nic = src->nic;
    dst->lastgoodstep = src->lastgoodstep;
    dst->lastscaledgoodstep = src->lastscaledgoodstep;
    dst->maxscaledgrad = src->maxscaledgrad;
    if( !ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic) )
        return ae_false;
    dst->repinneriterationscount = src->repinneriterationscount;
    dst->repouteriterationscount = src->repouteriterationscount;
    dst->repnfev = src->repnfev;
    dst->repvaridx = src->repvaridx;
    dst->repterminationtype = src->repterminationtype;
    dst->repdebugeqerr = src->repdebugeqerr;
    dst->repdebugfs = src->repdebugfs;
    dst->repdebugff = src->repdebugff;
    dst->repdebugdx = src->repdebugdx;
    dst->repdebugfeasqpits = src->repdebugfeasqpits;
    dst->repdebugfeasgpaits = src->repdebugfeasgpaits;
    if( !ae_vector_init_copy(&dst->xstart, &src->xstart, _state, make_automatic) )
        return ae_false;
    if( !_snnlssolver_init_copy(&dst->solver, &src->solver, _state, make_automatic) )
        return ae_false;
    dst->fbase = src->fbase;
    dst->fm2 = src->fm2;
    dst->fm1 = src->fm1;
    dst->fp1 = src->fp1;
    dst->fp2 = src->fp2;
    dst->xm1 = src->xm1;
    dst->xp1 = src->xp1;
    dst->gm1 = src->gm1;
    dst->gp1 = src->gp1;
    dst->cidx = src->cidx;
    dst->cval = src->cval;
    if( !ae_vector_init_copy(&dst->tmpprec, &src->tmpprec, _state, make_automatic) )
        return ae_false;
    dst->nfev = src->nfev;
    dst->mcstage = src->mcstage;
    dst->stp = src->stp;
    dst->curstpmax = src->curstpmax;
    dst->activationstep = src->activationstep;
    if( !ae_vector_init_copy(&dst->work, &src->work, _state, make_automatic) )
        return ae_false;
    if( !_linminstate_init_copy(&dst->lstate, &src->lstate, _state, make_automatic) )
        return ae_false;
    dst->trimthreshold = src->trimthreshold;
    dst->nonmonotoniccnt = src->nonmonotoniccnt;
    dst->k = src->k;
    dst->q = src->q;
    dst->p = src->p;
    if( !ae_vector_init_copy(&dst->rho, &src->rho, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->yk, &src->yk, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->sk, &src->sk, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->theta, &src->theta, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _minbleicstate_clear(void* _p)
{
    minbleicstate *p = (minbleicstate*)_p;
    ae_touch_ptr((void*)p);
    _sactiveset_clear(&p->sas);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->diagh);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->g);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->gc);
    ae_vector_clear(&p->xn);
    ae_vector_clear(&p->gn);
    ae_vector_clear(&p->xp);
    ae_vector_clear(&p->gp);
    ae_vector_clear(&p->d);
    ae_matrix_clear(&p->cleic);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->xstart);
    _snnlssolver_clear(&p->solver);
    ae_vector_clear(&p->tmpprec);
    ae_vector_clear(&p->work);
    _linminstate_clear(&p->lstate);
    ae_vector_clear(&p->rho);
    ae_matrix_clear(&p->yk);
    ae_matrix_clear(&p->sk);
    ae_vector_clear(&p->theta);
}


void _minbleicstate_destroy(void* _p)
{
    minbleicstate *p = (minbleicstate*)_p;
    ae_touch_ptr((void*)p);
    _sactiveset_destroy(&p->sas);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->diagh);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->g);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->gc);
    ae_vector_destroy(&p->xn);
    ae_vector_destroy(&p->gn);
    ae_vector_destroy(&p->xp);
    ae_vector_destroy(&p->gp);
    ae_vector_destroy(&p->d);
    ae_matrix_destroy(&p->cleic);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->xstart);
    _snnlssolver_destroy(&p->solver);
    ae_vector_destroy(&p->tmpprec);
    ae_vector_destroy(&p->work);
    _linminstate_destroy(&p->lstate);
    ae_vector_destroy(&p->rho);
    ae_matrix_destroy(&p->yk);
    ae_matrix_destroy(&p->sk);
    ae_vector_destroy(&p->theta);
}


ae_bool _minbleicreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minbleicreport *p = (minbleicreport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _minbleicreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minbleicreport *dst = (minbleicreport*)_dst;
    minbleicreport *src = (minbleicreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfev = src->nfev;
    dst->varidx = src->varidx;
    dst->terminationtype = src->terminationtype;
    dst->debugeqerr = src->debugeqerr;
    dst->debugfs = src->debugfs;
    dst->debugff = src->debugff;
    dst->debugdx = src->debugdx;
    dst->debugfeasqpits = src->debugfeasqpits;
    dst->debugfeasgpaits = src->debugfeasgpaits;
    dst->inneriterationscount = src->inneriterationscount;
    dst->outeriterationscount = src->outeriterationscount;
    return ae_true;
}


void _minbleicreport_clear(void* _p)
{
    minbleicreport *p = (minbleicreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minbleicreport_destroy(void* _p)
{
    minbleicreport *p = (minbleicreport*)_p;
    ae_touch_ptr((void*)p);
}




/*************************************************************************
        LIMITED MEMORY BFGS METHOD FOR LARGE SCALE OPTIMIZATION

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by  using  a  quasi-
Newton method (LBFGS scheme) which is optimized to use  a  minimum  amount
of memory.
The subroutine generates the approximation of an inverse Hessian matrix by
using information about the last M steps of the algorithm  (instead of N).
It lessens a required amount of memory from a value  of  order  N^2  to  a
value of order 2*N*M.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinLBFGSCreate() call
2. User tunes solver parameters with MinLBFGSSetCond() MinLBFGSSetStpMax()
   and other functions
3. User calls MinLBFGSOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinLBFGSResults() to get solution
5. Optionally user may call MinLBFGSRestartFrom() to solve another problem
   with same N/M but another starting point and/or another function.
   MinLBFGSRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension. N>0
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   initial solution approximation, array[0..N-1].


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state
    

NOTES:
1. you may tune stopping conditions with MinLBFGSSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLBFGSSetStpMax() function to bound algorithm's  steps.  However,
   L-BFGS rarely needs such a tuning.


  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreate(ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x,
     minlbfgsstate* state,
     ae_state *_state)
{

    _minlbfgsstate_clear(state);

    ae_assert(n>=1, "MinLBFGSCreate: N<1!", _state);
    ae_assert(m>=1, "MinLBFGSCreate: M<1", _state);
    ae_assert(m<=n, "MinLBFGSCreate: M>N", _state);
    ae_assert(x->cnt>=n, "MinLBFGSCreate: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinLBFGSCreate: X contains infinite or NaN values!", _state);
    minlbfgscreatex(n, m, x, 0, 0.0, state, _state);
}


/*************************************************************************
The subroutine is finite difference variant of MinLBFGSCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinLBFGSCreate() in  order  to  get
more information about creation of LBFGS optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinLBFGSSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is   less  robust  and  precise.  LBFGS  needs  exact  gradient values.
   Imprecise gradient may slow  down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreatef(ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x,
     double diffstep,
     minlbfgsstate* state,
     ae_state *_state)
{

    _minlbfgsstate_clear(state);

    ae_assert(n>=1, "MinLBFGSCreateF: N too small!", _state);
    ae_assert(m>=1, "MinLBFGSCreateF: M<1", _state);
    ae_assert(m<=n, "MinLBFGSCreateF: M>N", _state);
    ae_assert(x->cnt>=n, "MinLBFGSCreateF: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinLBFGSCreateF: X contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(diffstep, _state), "MinLBFGSCreateF: DiffStep is infinite or NaN!", _state);
    ae_assert(ae_fp_greater(diffstep,0), "MinLBFGSCreateF: DiffStep is non-positive!", _state);
    minlbfgscreatex(n, m, x, 0, diffstep, state, _state);
}


/*************************************************************************
This function sets stopping conditions for L-BFGS optimization algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinLBFGSSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinLBFGSSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetcond(minlbfgsstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinLBFGSSetCond: EpsG is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsg,0), "MinLBFGSSetCond: negative EpsG!", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinLBFGSSetCond: EpsF is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsf,0), "MinLBFGSSetCond: negative EpsF!", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinLBFGSSetCond: EpsX is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsx,0), "MinLBFGSSetCond: negative EpsX!", _state);
    ae_assert(maxits>=0, "MinLBFGSSetCond: negative MaxIts!", _state);
    if( ((ae_fp_eq(epsg,0)&&ae_fp_eq(epsf,0))&&ae_fp_eq(epsx,0))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    state->epsg = epsg;
    state->epsf = epsf;
    state->epsx = epsx;
    state->maxits = maxits;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinLBFGSOptimize().


  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetxrep(minlbfgsstate* state,
     ae_bool needxrep,
     ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0 (default),  if
                you don't want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetstpmax(minlbfgsstate* state,
     double stpmax,
     ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "MinLBFGSSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,0), "MinLBFGSSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************
This function sets scaling coefficients for LBFGS optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  LBFGS  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinLBFGSSetPrec...()
functions.

There  is  special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetscale(minlbfgsstate* state,
     /* Real    */ ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinLBFGSSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinLBFGSSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],0), "MinLBFGSSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


/*************************************************************************
Extended subroutine for internal use only.

Accepts additional parameters:

    Flags - additional settings:
            * Flags = 0     means no additional settings
            * Flags = 1     "do not allocate memory". used when solving
                            a many subsequent tasks with  same N/M  values.
                            First  call MUST  be without this flag bit set,
                            subsequent  calls   of   MinLBFGS   with   same
                            MinLBFGSState structure can set Flags to 1.
    DiffStep - numerical differentiation step

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreatex(ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x,
     ae_int_t flags,
     double diffstep,
     minlbfgsstate* state,
     ae_state *_state)
{
    ae_bool allocatemem;
    ae_int_t i;


    ae_assert(n>=1, "MinLBFGS: N too small!", _state);
    ae_assert(m>=1, "MinLBFGS: M too small!", _state);
    ae_assert(m<=n, "MinLBFGS: M too large!", _state);
    
    /*
     * Initialize
     */
    state->teststep = 0;
    state->diffstep = diffstep;
    state->n = n;
    state->m = m;
    allocatemem = flags%2==0;
    flags = flags/2;
    if( allocatemem )
    {
        ae_vector_set_length(&state->rho, m, _state);
        ae_vector_set_length(&state->theta, m, _state);
        ae_matrix_set_length(&state->yk, m, n, _state);
        ae_matrix_set_length(&state->sk, m, n, _state);
        ae_vector_set_length(&state->d, n, _state);
        ae_vector_set_length(&state->x, n, _state);
        ae_vector_set_length(&state->s, n, _state);
        ae_vector_set_length(&state->g, n, _state);
        ae_vector_set_length(&state->work, n, _state);
    }
    minlbfgssetcond(state, 0, 0, 0, 0, _state);
    minlbfgssetxrep(state, ae_false, _state);
    minlbfgssetstpmax(state, 0, _state);
    minlbfgsrestartfrom(state, x, _state);
    for(i=0; i<=n-1; i++)
    {
        state->s.ptr.p_double[i] = 1.0;
    }
    state->prectype = 0;
}


/*************************************************************************
Modification  of  the  preconditioner:  default  preconditioner    (simple
scaling, same for all elements of X) is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecdefault(minlbfgsstate* state, ae_state *_state)
{


    state->prectype = 0;
}


/*************************************************************************
Modification of the preconditioner: Cholesky factorization of  approximate
Hessian is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    P       -   triangular preconditioner, Cholesky factorization of
                the approximate Hessian. array[0..N-1,0..N-1],
                (if larger, only leading N elements are used).
    IsUpper -   whether upper or lower triangle of P is given
                (other triangle is not referenced)

After call to this function preconditioner is changed to P  (P  is  copied
into the internal buffer).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2:  P  should  be nonsingular. Exception will be thrown otherwise.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetpreccholesky(minlbfgsstate* state,
     /* Real    */ ae_matrix* p,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t i;
    double mx;


    ae_assert(isfinitertrmatrix(p, state->n, isupper, _state), "MinLBFGSSetPrecCholesky: P contains infinite or NAN values!", _state);
    mx = 0;
    for(i=0; i<=state->n-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(p->ptr.pp_double[i][i], _state), _state);
    }
    ae_assert(ae_fp_greater(mx,0), "MinLBFGSSetPrecCholesky: P is strictly singular!", _state);
    if( state->denseh.rows<state->n||state->denseh.cols<state->n )
    {
        ae_matrix_set_length(&state->denseh, state->n, state->n, _state);
    }
    state->prectype = 1;
    if( isupper )
    {
        rmatrixcopy(state->n, state->n, p, 0, 0, &state->denseh, 0, 0, _state);
    }
    else
    {
        rmatrixtranspose(state->n, state->n, p, 0, 0, &state->denseh, 0, 0, _state);
    }
}


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2: D[i] should be positive. Exception will be thrown otherwise.

NOTE 3: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecdiag(minlbfgsstate* state,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(d->cnt>=state->n, "MinLBFGSSetPrecDiag: D is too short", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(d->ptr.p_double[i], _state), "MinLBFGSSetPrecDiag: D contains infinite or NAN elements", _state);
        ae_assert(ae_fp_greater(d->ptr.p_double[i],0), "MinLBFGSSetPrecDiag: D contains non-positive elements", _state);
    }
    rvectorsetlengthatleast(&state->diagh, state->n, _state);
    state->prectype = 2;
    for(i=0; i<=state->n-1; i++)
    {
        state->diagh.ptr.p_double[i] = d->ptr.p_double[i];
    }
}


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinLBFGSSetScale()
call  (before  or after MinLBFGSSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecscale(minlbfgsstate* state, ae_state *_state)
{


    state->prectype = 3;
}


/*************************************************************************
NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either MinLBFGSCreate() for analytical gradient  or  MinLBFGSCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinLBFGSOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinLBFGSOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinLBFGSOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinLBFGSOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinLBFGSCreateF() |     work                FAIL
   MinLBFGSCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinLBFGSOptimize()  version.   Attemps   to   use   such
   combination (for example, to create optimizer with MinLBFGSCreateF() and
   to pass gradient information to MinCGOptimize()) will lead to exception
   being thrown. Either  you  did  not pass gradient when it WAS needed or
   you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool minlbfgsiteration(minlbfgsstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    ae_int_t ic;
    ae_int_t mcinfo;
    double v;
    double vv;
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
        j = state->rstate.ia.ptr.p_int[3];
        ic = state->rstate.ia.ptr.p_int[4];
        mcinfo = state->rstate.ia.ptr.p_int[5];
        v = state->rstate.ra.ptr.p_double[0];
        vv = state->rstate.ra.ptr.p_double[1];
    }
    else
    {
        n = -983;
        m = -989;
        i = -834;
        j = 900;
        ic = -287;
        mcinfo = 364;
        v = 214;
        vv = -338;
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
    if( state->rstate.stage==8 )
    {
        goto lbl_8;
    }
    if( state->rstate.stage==9 )
    {
        goto lbl_9;
    }
    if( state->rstate.stage==10 )
    {
        goto lbl_10;
    }
    if( state->rstate.stage==11 )
    {
        goto lbl_11;
    }
    if( state->rstate.stage==12 )
    {
        goto lbl_12;
    }
    if( state->rstate.stage==13 )
    {
        goto lbl_13;
    }
    if( state->rstate.stage==14 )
    {
        goto lbl_14;
    }
    if( state->rstate.stage==15 )
    {
        goto lbl_15;
    }
    if( state->rstate.stage==16 )
    {
        goto lbl_16;
    }
    
    /*
     * Routine body
     */
    
    /*
     * Unload frequently used variables from State structure
     * (just for typing convinience)
     */
    n = state->n;
    m = state->m;
    state->repterminationtype = 0;
    state->repiterationscount = 0;
    state->repvaridx = -1;
    state->repnfev = 0;
    
    /*
     *  Check, that transferred derivative value is right
     */
    minlbfgs_clearrequestfields(state, _state);
    if( !(ae_fp_eq(state->diffstep,0)&&ae_fp_greater(state->teststep,0)) )
    {
        goto lbl_17;
    }
    state->needfg = ae_true;
    i = 0;
lbl_19:
    if( i>n-1 )
    {
        goto lbl_21;
    }
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->teststep*state->s.ptr.p_double[i];
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->fm1 = state->f;
    state->fp1 = state->g.ptr.p_double[i];
    state->x.ptr.p_double[i] = v+state->teststep*state->s.ptr.p_double[i];
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->fm2 = state->f;
    state->fp2 = state->g.ptr.p_double[i];
    state->x.ptr.p_double[i] = v;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    
    /*
     * 2*State.TestStep   -   scale parameter
     * width of segment [Xi-TestStep;Xi+TestStep]
     */
    if( !derivativecheck(state->fm1, state->fp1, state->fm2, state->fp2, state->f, state->g.ptr.p_double[i], 2*state->teststep, _state) )
    {
        state->repvaridx = i;
        state->repterminationtype = -7;
        result = ae_false;
        return result;
    }
    i = i+1;
    goto lbl_19;
lbl_21:
    state->needfg = ae_false;
lbl_17:
    
    /*
     * Calculate F/G at the initial point
     */
    minlbfgs_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,0) )
    {
        goto lbl_22;
    }
    state->needfg = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needfg = ae_false;
    goto lbl_23;
lbl_22:
    state->needf = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->fbase = state->f;
    i = 0;
lbl_24:
    if( i>n-1 )
    {
        goto lbl_26;
    }
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->fp2 = state->f;
    state->x.ptr.p_double[i] = v;
    state->g.ptr.p_double[i] = (8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/(6*state->diffstep*state->s.ptr.p_double[i]);
    i = i+1;
    goto lbl_24;
lbl_26:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_23:
    trimprepare(state->f, &state->trimthreshold, _state);
    if( !state->xrep )
    {
        goto lbl_27;
    }
    minlbfgs_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->xupdated = ae_false;
lbl_27:
    state->repnfev = 1;
    state->fold = state->f;
    v = 0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->g.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    if( ae_fp_less_eq(ae_sqrt(v, _state),state->epsg) )
    {
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    
    /*
     * Choose initial step and direction.
     * Apply preconditioner, if we have something other than default.
     */
    ae_v_moveneg(&state->d.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    if( state->prectype==0 )
    {
        
        /*
         * Default preconditioner is used, but we can't use it before iterations will start
         */
        v = ae_v_dotproduct(&state->g.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = ae_sqrt(v, _state);
        if( ae_fp_eq(state->stpmax,0) )
        {
            state->stp = ae_minreal(1.0/v, 1, _state);
        }
        else
        {
            state->stp = ae_minreal(1.0/v, state->stpmax, _state);
        }
    }
    if( state->prectype==1 )
    {
        
        /*
         * Cholesky preconditioner is used
         */
        fblscholeskysolve(&state->denseh, 1.0, n, ae_true, &state->d, &state->autobuf, _state);
        state->stp = 1;
    }
    if( state->prectype==2 )
    {
        
        /*
         * diagonal approximation is used
         */
        for(i=0; i<=n-1; i++)
        {
            state->d.ptr.p_double[i] = state->d.ptr.p_double[i]/state->diagh.ptr.p_double[i];
        }
        state->stp = 1;
    }
    if( state->prectype==3 )
    {
        
        /*
         * scale-based preconditioner is used
         */
        for(i=0; i<=n-1; i++)
        {
            state->d.ptr.p_double[i] = state->d.ptr.p_double[i]*state->s.ptr.p_double[i]*state->s.ptr.p_double[i];
        }
        state->stp = 1;
    }
    
    /*
     * Main cycle
     */
    state->k = 0;
lbl_29:
    if( ae_false )
    {
        goto lbl_30;
    }
    
    /*
     * Main cycle: prepare to 1-D line search
     */
    state->p = state->k%m;
    state->q = ae_minint(state->k, m-1, _state);
    
    /*
     * Store X[k], G[k]
     */
    ae_v_moveneg(&state->sk.ptr.pp_double[state->p][0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_moveneg(&state->yk.ptr.pp_double[state->p][0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * Minimize F(x+alpha*d)
     * Calculate S[k], Y[k]
     */
    state->mcstage = 0;
    if( state->k!=0 )
    {
        state->stp = 1.0;
    }
    linminnormalized(&state->d, &state->stp, n, _state);
    mcsrch(n, &state->x, &state->f, &state->g, &state->d, &state->stp, state->stpmax, minlbfgs_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
lbl_31:
    if( state->mcstage==0 )
    {
        goto lbl_32;
    }
    minlbfgs_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,0) )
    {
        goto lbl_33;
    }
    state->needfg = ae_true;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->needfg = ae_false;
    goto lbl_34;
lbl_33:
    state->needf = ae_true;
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->fbase = state->f;
    i = 0;
lbl_35:
    if( i>n-1 )
    {
        goto lbl_37;
    }
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->fp2 = state->f;
    state->x.ptr.p_double[i] = v;
    state->g.ptr.p_double[i] = (8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/(6*state->diffstep*state->s.ptr.p_double[i]);
    i = i+1;
    goto lbl_35;
lbl_37:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_34:
    trimfunction(&state->f, &state->g, n, state->trimthreshold, _state);
    mcsrch(n, &state->x, &state->f, &state->g, &state->d, &state->stp, state->stpmax, minlbfgs_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
    goto lbl_31;
lbl_32:
    if( !state->xrep )
    {
        goto lbl_38;
    }
    
    /*
     * report
     */
    minlbfgs_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 16;
    goto lbl_rcomm;
lbl_16:
    state->xupdated = ae_false;
lbl_38:
    state->repnfev = state->repnfev+state->nfev;
    state->repiterationscount = state->repiterationscount+1;
    ae_v_add(&state->sk.ptr.pp_double[state->p][0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_add(&state->yk.ptr.pp_double[state->p][0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * Stopping conditions
     */
    if( state->repiterationscount>=state->maxits&&state->maxits>0 )
    {
        
        /*
         * Too many iterations
         */
        state->repterminationtype = 5;
        result = ae_false;
        return result;
    }
    v = 0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->g.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    if( ae_fp_less_eq(ae_sqrt(v, _state),state->epsg) )
    {
        
        /*
         * Gradient is small enough
         */
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    if( ae_fp_less_eq(state->fold-state->f,state->epsf*ae_maxreal(ae_fabs(state->fold, _state), ae_maxreal(ae_fabs(state->f, _state), 1.0, _state), _state)) )
    {
        
        /*
         * F(k+1)-F(k) is small enough
         */
        state->repterminationtype = 1;
        result = ae_false;
        return result;
    }
    v = 0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->sk.ptr.pp_double[state->p][i]/state->s.ptr.p_double[i], _state);
    }
    if( ae_fp_less_eq(ae_sqrt(v, _state),state->epsx) )
    {
        
        /*
         * X(k+1)-X(k) is small enough
         */
        state->repterminationtype = 2;
        result = ae_false;
        return result;
    }
    
    /*
     * If Wolfe conditions are satisfied, we can update
     * limited memory model.
     *
     * However, if conditions are not satisfied (NFEV limit is met,
     * function is too wild, ...), we'll skip L-BFGS update
     */
    if( mcinfo!=1 )
    {
        
        /*
         * Skip update.
         *
         * In such cases we'll initialize search direction by
         * antigradient vector, because it  leads to more
         * transparent code with less number of special cases
         */
        state->fold = state->f;
        ae_v_moveneg(&state->d.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    }
    else
    {
        
        /*
         * Calculate Rho[k], GammaK
         */
        v = ae_v_dotproduct(&state->yk.ptr.pp_double[state->p][0], 1, &state->sk.ptr.pp_double[state->p][0], 1, ae_v_len(0,n-1));
        vv = ae_v_dotproduct(&state->yk.ptr.pp_double[state->p][0], 1, &state->yk.ptr.pp_double[state->p][0], 1, ae_v_len(0,n-1));
        if( ae_fp_eq(v,0)||ae_fp_eq(vv,0) )
        {
            
            /*
             * Rounding errors make further iterations impossible.
             */
            state->repterminationtype = -2;
            result = ae_false;
            return result;
        }
        state->rho.ptr.p_double[state->p] = 1/v;
        state->gammak = v/vv;
        
        /*
         *  Calculate d(k+1) = -H(k+1)*g(k+1)
         *
         *  for I:=K downto K-Q do
         *      V = s(i)^T * work(iteration:I)
         *      theta(i) = V
         *      work(iteration:I+1) = work(iteration:I) - V*Rho(i)*y(i)
         *  work(last iteration) = H0*work(last iteration) - preconditioner
         *  for I:=K-Q to K do
         *      V = y(i)^T*work(iteration:I)
         *      work(iteration:I+1) = work(iteration:I) +(-V+theta(i))*Rho(i)*s(i)
         *
         *  NOW WORK CONTAINS d(k+1)
         */
        ae_v_move(&state->work.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
        for(i=state->k; i>=state->k-state->q; i--)
        {
            ic = i%m;
            v = ae_v_dotproduct(&state->sk.ptr.pp_double[ic][0], 1, &state->work.ptr.p_double[0], 1, ae_v_len(0,n-1));
            state->theta.ptr.p_double[ic] = v;
            vv = v*state->rho.ptr.p_double[ic];
            ae_v_subd(&state->work.ptr.p_double[0], 1, &state->yk.ptr.pp_double[ic][0], 1, ae_v_len(0,n-1), vv);
        }
        if( state->prectype==0 )
        {
            
            /*
             * Simple preconditioner is used
             */
            v = state->gammak;
            ae_v_muld(&state->work.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
        }
        if( state->prectype==1 )
        {
            
            /*
             * Cholesky preconditioner is used
             */
            fblscholeskysolve(&state->denseh, 1, n, ae_true, &state->work, &state->autobuf, _state);
        }
        if( state->prectype==2 )
        {
            
            /*
             * diagonal approximation is used
             */
            for(i=0; i<=n-1; i++)
            {
                state->work.ptr.p_double[i] = state->work.ptr.p_double[i]/state->diagh.ptr.p_double[i];
            }
        }
        if( state->prectype==3 )
        {
            
            /*
             * scale-based preconditioner is used
             */
            for(i=0; i<=n-1; i++)
            {
                state->work.ptr.p_double[i] = state->work.ptr.p_double[i]*state->s.ptr.p_double[i]*state->s.ptr.p_double[i];
            }
        }
        for(i=state->k-state->q; i<=state->k; i++)
        {
            ic = i%m;
            v = ae_v_dotproduct(&state->yk.ptr.pp_double[ic][0], 1, &state->work.ptr.p_double[0], 1, ae_v_len(0,n-1));
            vv = state->rho.ptr.p_double[ic]*(-v+state->theta.ptr.p_double[ic]);
            ae_v_addd(&state->work.ptr.p_double[0], 1, &state->sk.ptr.pp_double[ic][0], 1, ae_v_len(0,n-1), vv);
        }
        ae_v_moveneg(&state->d.ptr.p_double[0], 1, &state->work.ptr.p_double[0], 1, ae_v_len(0,n-1));
        
        /*
         * Next step
         */
        state->fold = state->f;
        state->k = state->k+1;
    }
    goto lbl_29;
lbl_30:
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
    state->rstate.ia.ptr.p_int[3] = j;
    state->rstate.ia.ptr.p_int[4] = ic;
    state->rstate.ia.ptr.p_int[5] = mcinfo;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = vv;
    return result;
}


/*************************************************************************
L-BFGS algorithm results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -7    gradient verification failed.
                            See MinLBFGSSetGradientCheck() for more information.
                    * -2    rounding errors prevent further improvement.
                            X contains best point found.
                    * -1    incorrect parameters were specified
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresults(minlbfgsstate* state,
     /* Real    */ ae_vector* x,
     minlbfgsreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _minlbfgsreport_clear(rep);

    minlbfgsresultsbuf(state, x, rep, _state);
}


/*************************************************************************
L-BFGS algorithm results

Buffered implementation of MinLBFGSResults which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresultsbuf(minlbfgsstate* state,
     /* Real    */ ae_vector* x,
     minlbfgsreport* rep,
     ae_state *_state)
{


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nfev = state->repnfev;
    rep->varidx = state->repvaridx;
    rep->terminationtype = state->repterminationtype;
}


/*************************************************************************
This  subroutine restarts LBFGS algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsrestartfrom(minlbfgsstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{


    ae_assert(x->cnt>=state->n, "MinLBFGSRestartFrom: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, state->n, _state), "MinLBFGSRestartFrom: X contains infinite or NaN values!", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_vector_set_length(&state->rstate.ia, 5+1, _state);
    ae_vector_set_length(&state->rstate.ra, 1+1, _state);
    state->rstate.stage = -1;
    minlbfgs_clearrequestfields(state, _state);
}


/*************************************************************************
This  subroutine  turns  on  verification  of  the  user-supplied analytic
gradient:
* user calls this subroutine before optimization begins
* MinLBFGSOptimize() is called
* prior to  actual  optimization, for each component  of  parameters being
  optimized X[i] algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * if needed, steps are bounded with respect to constraints on X[]
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point
  * in case difference between prediction and actual value is higher  than
    some predetermined threshold, algorithm stops with completion code -7;
    Rep.VarIdx is set to index of the parameter with incorrect derivative.
* after verification is over, algorithm proceeds to the actual optimization.

NOTE 1: verification  needs  N (parameters count) gradient evaluations. It
        is very costly and you should use  it  only  for  low  dimensional
        problems,  when  you  want  to  be  sure  that  you've   correctly
        calculated  analytic  derivatives.  You  should  not use it in the
        production code (unless you want to check derivatives provided  by
        some third party).

NOTE 2: you  should  carefully  choose  TestStep. Value which is too large
        (so large that function behaviour is significantly non-cubic) will
        lead to false alarms. You may use  different  step  for  different
        parameters by means of setting scale with MinLBFGSSetScale().

NOTE 3: this function may lead to false positives. In case it reports that
        I-th  derivative was calculated incorrectly, you may decrease test
        step  and  try  one  more  time  - maybe your function changes too
        sharply  and  your  step  is  too  large for such rapidly chanding
        function.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification

  -- ALGLIB --
     Copyright 24.05.2012 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetgradientcheck(minlbfgsstate* state,
     double teststep,
     ae_state *_state)
{


    ae_assert(ae_isfinite(teststep, _state), "MinLBFGSSetGradientCheck: TestStep contains NaN or Infinite", _state);
    ae_assert(ae_fp_greater_eq(teststep,0), "MinLBFGSSetGradientCheck: invalid argument TestStep(TestStep<0)", _state);
    state->teststep = teststep;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void minlbfgs_clearrequestfields(minlbfgsstate* state,
     ae_state *_state)
{


    state->needf = ae_false;
    state->needfg = ae_false;
    state->xupdated = ae_false;
}


ae_bool _minlbfgsstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minlbfgsstate *p = (minlbfgsstate*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rho, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->yk, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->sk, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->theta, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->work, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->denseh, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->diagh, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->autobuf, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init(&p->rstate, _state, make_automatic) )
        return ae_false;
    if( !_linminstate_init(&p->lstate, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _minlbfgsstate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minlbfgsstate *dst = (minlbfgsstate*)_dst;
    minlbfgsstate *src = (minlbfgsstate*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->epsg = src->epsg;
    dst->epsf = src->epsf;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->stpmax = src->stpmax;
    if( !ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic) )
        return ae_false;
    dst->diffstep = src->diffstep;
    dst->nfev = src->nfev;
    dst->mcstage = src->mcstage;
    dst->k = src->k;
    dst->q = src->q;
    dst->p = src->p;
    if( !ae_vector_init_copy(&dst->rho, &src->rho, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->yk, &src->yk, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->sk, &src->sk, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->theta, &src->theta, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic) )
        return ae_false;
    dst->stp = src->stp;
    if( !ae_vector_init_copy(&dst->work, &src->work, _state, make_automatic) )
        return ae_false;
    dst->fold = src->fold;
    dst->trimthreshold = src->trimthreshold;
    dst->prectype = src->prectype;
    dst->gammak = src->gammak;
    if( !ae_matrix_init_copy(&dst->denseh, &src->denseh, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->diagh, &src->diagh, _state, make_automatic) )
        return ae_false;
    dst->fbase = src->fbase;
    dst->fm2 = src->fm2;
    dst->fm1 = src->fm1;
    dst->fp1 = src->fp1;
    dst->fp2 = src->fp2;
    if( !ae_vector_init_copy(&dst->autobuf, &src->autobuf, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic) )
        return ae_false;
    dst->f = src->f;
    if( !ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic) )
        return ae_false;
    dst->needf = src->needf;
    dst->needfg = src->needfg;
    dst->xupdated = src->xupdated;
    dst->teststep = src->teststep;
    if( !_rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic) )
        return ae_false;
    dst->repiterationscount = src->repiterationscount;
    dst->repnfev = src->repnfev;
    dst->repvaridx = src->repvaridx;
    dst->repterminationtype = src->repterminationtype;
    if( !_linminstate_init_copy(&dst->lstate, &src->lstate, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _minlbfgsstate_clear(void* _p)
{
    minlbfgsstate *p = (minlbfgsstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->rho);
    ae_matrix_clear(&p->yk);
    ae_matrix_clear(&p->sk);
    ae_vector_clear(&p->theta);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->work);
    ae_matrix_clear(&p->denseh);
    ae_vector_clear(&p->diagh);
    ae_vector_clear(&p->autobuf);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->g);
    _rcommstate_clear(&p->rstate);
    _linminstate_clear(&p->lstate);
}


void _minlbfgsstate_destroy(void* _p)
{
    minlbfgsstate *p = (minlbfgsstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->rho);
    ae_matrix_destroy(&p->yk);
    ae_matrix_destroy(&p->sk);
    ae_vector_destroy(&p->theta);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->work);
    ae_matrix_destroy(&p->denseh);
    ae_vector_destroy(&p->diagh);
    ae_vector_destroy(&p->autobuf);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->g);
    _rcommstate_destroy(&p->rstate);
    _linminstate_destroy(&p->lstate);
}


ae_bool _minlbfgsreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minlbfgsreport *p = (minlbfgsreport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _minlbfgsreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minlbfgsreport *dst = (minlbfgsreport*)_dst;
    minlbfgsreport *src = (minlbfgsreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfev = src->nfev;
    dst->varidx = src->varidx;
    dst->terminationtype = src->terminationtype;
    return ae_true;
}


void _minlbfgsreport_clear(void* _p)
{
    minlbfgsreport *p = (minlbfgsreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minlbfgsreport_destroy(void* _p)
{
    minlbfgsreport *p = (minlbfgsreport*)_p;
    ae_touch_ptr((void*)p);
}




/*************************************************************************
                    CONSTRAINED QUADRATIC PROGRAMMING

The subroutine creates QP optimizer. After initial creation,  it  contains
default optimization problem with zero quadratic and linear terms  and  no
constraints. You should set quadratic/linear terms with calls to functions
provided by MinQP subpackage.

INPUT PARAMETERS:
    N       -   problem size
    
OUTPUT PARAMETERS:
    State   -   optimizer with zero quadratic/linear terms
                and no constraints

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpcreate(ae_int_t n, minqpstate* state, ae_state *_state)
{
    ae_int_t i;

    _minqpstate_clear(state);

    ae_assert(n>=1, "MinQPCreate: N<1", _state);
    
    /*
     * initialize QP solver
     */
    state->n = n;
    state->nec = 0;
    state->nic = 0;
    state->repterminationtype = 0;
    state->anorm = 1;
    state->akind = 0;
    cqminit(n, &state->a, _state);
    sasinit(n, &state->sas, _state);
    ae_vector_set_length(&state->b, n, _state);
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->workbndl, n, _state);
    ae_vector_set_length(&state->workbndu, n, _state);
    ae_vector_set_length(&state->havebndl, n, _state);
    ae_vector_set_length(&state->havebndu, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->startx, n, _state);
    ae_vector_set_length(&state->xorigin, n, _state);
    ae_vector_set_length(&state->xs, n, _state);
    ae_vector_set_length(&state->xn, n, _state);
    ae_vector_set_length(&state->gc, n, _state);
    ae_vector_set_length(&state->pg, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
        state->havebndl.ptr.p_bool[i] = ae_false;
        state->havebndu.ptr.p_bool[i] = ae_false;
        state->b.ptr.p_double[i] = 0.0;
        state->startx.ptr.p_double[i] = 0.0;
        state->xorigin.ptr.p_double[i] = 0.0;
        state->s.ptr.p_double[i] = 1.0;
    }
    state->havex = ae_false;
    minqpsetalgocholesky(state, _state);
    normestimatorcreate(n, n, 5, 5, &state->estimator, _state);
    minbleiccreate(n, &state->startx, &state->solver, _state);
}


/*************************************************************************
This function sets linear term for QP solver.

By default, linear term is zero.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    B       -   linear term, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetlinearterm(minqpstate* state,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(b->cnt>=n, "MinQPSetLinearTerm: Length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "MinQPSetLinearTerm: B contains infinite or NaN elements", _state);
    minqpsetlineartermfast(state, b, _state);
}


/*************************************************************************
This  function  sets  dense  quadratic  term  for  QP solver. By  default,
quadratic term is zero.

SUPPORT BY ALGLIB QP ALGORITHMS:

Dense quadratic term can be handled by any of the QP algorithms  supported
by ALGLIB QP Solver.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   (optional) storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn’t used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn’t used
                * if not given, both lower and upper  triangles  must  be
                  filled.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadraticterm(minqpstate* state,
     /* Real    */ ae_matrix* a,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(a->rows>=n, "MinQPSetQuadraticTerm: Rows(A)<N", _state);
    ae_assert(a->cols>=n, "MinQPSetQuadraticTerm: Cols(A)<N", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "MinQPSetQuadraticTerm: A contains infinite or NaN elements", _state);
    minqpsetquadratictermfast(state, a, isupper, 0.0, _state);
}


/*************************************************************************
This  function  sets  sparse  quadratic  term  for  QP solver. By default,
quadratic term is zero.

SUPPORT BY ALGLIB QP ALGORITHMS:

Sparse quadratic term is supported only by BLEIC-based QP  algorithm  (one
which is activated by MinQPSetAlgoBLEIC function). Cholesky-based QP  algo
won't be able to deal  with  sparse  quadratic  term  and  will  terminate
abnormally.

IF YOU CALLED THIS FUNCTION, YOU MUST SWITCH TO BLEIC-BASED  QP  ALGORITHM
BEFORE CALLING MINQPOPTIMIZE() FUNCTION.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   (optional) storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn’t used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn’t used
                * if not given, both lower and upper  triangles  must  be
                  filled.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadratictermsparse(minqpstate* state,
     sparsematrix* a,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(sparsegetnrows(a, _state)>=n, "MinQPSetQuadraticTermSparse: Rows(A)<N", _state);
    ae_assert(sparsegetncols(a, _state)>=n, "MinQPSetQuadraticTermSparse: Cols(A)<N", _state);
    sparsecopytocrs(a, &state->sparsea, _state);
    state->sparseaupper = isupper;
    state->akind = 1;
}


/*************************************************************************
This function sets starting point for QP solver. It is useful to have
good initial approximation to the solution, because it will increase
speed of convergence and identification of active constraints.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    X       -   starting point, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetstartingpoint(minqpstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(x->cnt>=n, "MinQPSetStartingPoint: Length(B)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinQPSetStartingPoint: X contains infinite or NaN elements", _state);
    minqpsetstartingpointfast(state, x, _state);
}


/*************************************************************************
This  function sets origin for QP solver. By default, following QP program
is solved:

    min(0.5*x'*A*x+b'*x)
    
This function allows to solve different problem:

    min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))
    
INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    XOrigin -   origin, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetorigin(minqpstate* state,
     /* Real    */ ae_vector* xorigin,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(xorigin->cnt>=n, "MinQPSetOrigin: Length(B)<N", _state);
    ae_assert(isfinitevector(xorigin, n, _state), "MinQPSetOrigin: B contains infinite or NaN elements", _state);
    minqpsetoriginfast(state, xorigin, _state);
}


/*************************************************************************
This function sets scaling coefficients.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

BLEIC-based QP solver uses scale for two purposes:
* to evaluate stopping conditions
* for preconditioning of the underlying BLEIC solver

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetscale(minqpstate* state,
     /* Real    */ ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinQPSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinQPSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],0), "MinQPSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function tells solver to use Cholesky-based algorithm. This algorithm
is active by default.

DESCRIPTION:

Cholesky-based algorithm can be used only for problems which:
* have dense quadratic term, set  by  MinQPSetQuadraticTerm(),  sparse  or
  structured problems are not supported.
* are strictly convex, i.e. quadratic term is symmetric positive definite,
  indefinite or semidefinite problems are not supported by this algorithm.

If anything of what listed above is violated, you may use  BLEIC-based  QP
algorithm which can be activated by MinQPSetAlgoBLEIC().

BENEFITS AND DRAWBACKS:

This  algorithm  gives  best  precision amongst all QP solvers provided by
ALGLIB (Newton iterations  have  much  higher  precision  than  any  other
optimization algorithm). This solver also gracefully handles problems with
very large amount of constraints.

Performance of the algorithm is good because internally  it  uses  Level 3
Dense BLAS for its performance-critical parts.


From the other side, algorithm has  O(N^3)  complexity  for  unconstrained
problems and up to orders of  magnitude  slower  on  constrained  problems
(these additional iterations are needed to identify  active  constraints).
So, its running time depends on number of constraints active  at solution.

Furthermore, this algorithm can not solve problems with sparse matrices or
problems with semidefinite/indefinite matrices of any kind (dense/sparse).

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgocholesky(minqpstate* state, ae_state *_state)
{


    state->algokind = 1;
}


/*************************************************************************
This function tells solver to use BLEIC-based algorithm and sets  stopping
criteria for the algorithm.

DESCRIPTION:

BLEIC-based QP algorithm can be used for any kind of QP problems:
* problems with both dense and sparse quadratic terms
* problems with positive definite, semidefinite, indefinite terms

BLEIC-based algorithm can solve even indefinite problems - as long as they
are bounded from below on the feasible set. Of course, global  minimum  is
found only  for  positive  definite  and  semidefinite  problems.  As  for
indefinite ones - only local minimum is found.

BENEFITS AND DRAWBACKS:

This algorithm can be used to solve both convex and indefinite QP problems
and it can utilize sparsity of the quadratic  term  (algorithm  calculates
matrix-vector products, which can be  performed  efficiently  in  case  of
sparse matrix).

Algorithm has iteration cost, which (assuming fixed amount of non-boundary
linear constraints) linearly depends on problem size. Boundary constraints
does not significantly change iteration cost.

Thus, it outperforms Cholesky-based QP algorithm (CQP) on high-dimensional
sparse problems with moderate amount of constraints.


From the other side, unlike CQP solver, this algorithm does NOT  make  use
of Level 3 Dense BLAS. Thus, its performance on dense problems is inferior
to that of CQP solver.

Its precision is also inferior to that of CQP. CQP performs  Newton  steps
which are know to achieve very good  precision. In many cases Newton  step
leads us exactly to the solution. BLEIC-QP performs LBFGS steps, which are
good at detecting neighborhood of the solution, buy need  many  iterations
to find solution with 6 digits of precision.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled constrained gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinQPSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if exploratory steepest
                descent  step  on  k+1-th  iteration  satisfies   following
                condition:  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
    EpsX    -   >=0
                The  subroutine  finishes  its work if exploratory steepest
                descent  step  on  k+1-th  iteration  satisfies   following
                condition:  
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinQPSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection (presently it is  small    step
length, but it may change in the future versions of ALGLIB).

IT IS VERY IMPORTANT THAT YOU CALL MinQPSetScale() WHEN YOU USE THIS ALGO!

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgobleic(minqpstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinQPSetAlgoBLEIC: EpsG is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsg,0), "MinQPSetAlgoBLEIC: negative EpsG", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinQPSetAlgoBLEIC: EpsF is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsf,0), "MinQPSetAlgoBLEIC: negative EpsF", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinQPSetAlgoBLEIC: EpsX is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsx,0), "MinQPSetAlgoBLEIC: negative EpsX", _state);
    ae_assert(maxits>=0, "MinQPSetAlgoBLEIC: negative MaxIts!", _state);
    state->algokind = 2;
    if( ((ae_fp_eq(epsg,0)&&ae_fp_eq(epsf,0))&&ae_fp_eq(epsx,0))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    state->bleicepsg = epsg;
    state->bleicepsf = epsf;
    state->bleicepsx = epsx;
    state->bleicmaxits = maxits;
}


/*************************************************************************
This function sets boundary constraints for QP solver

Boundary constraints are inactive by default (after initial creation).
After  being  set,  they  are  preserved  until explicitly turned off with
another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).
                
NOTE: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbc(minqpstate* state,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(bndl->cnt>=n, "MinQPSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinQPSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinQPSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinQPSetBC: BndU contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->havebndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->havebndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function sets linear constraints for QP optimizer.

Linear constraints are inactive by default (after initial creation).

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately  -
        there always exists some minor violation (about 10^-10...10^-13)
        due to numerical errors.

  -- ALGLIB --
     Copyright 19.06.2012 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc(minqpstate* state,
     /* Real    */ ae_matrix* c,
     /* Integer */ ae_vector* ct,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    double v;


    n = state->n;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(k>=0, "MinQPSetLC: K<0", _state);
    ae_assert(c->cols>=n+1||k==0, "MinQPSetLC: Cols(C)<N+1", _state);
    ae_assert(c->rows>=k, "MinQPSetLC: Rows(C)<K", _state);
    ae_assert(ct->cnt>=k, "MinQPSetLC: Length(CT)<K", _state);
    ae_assert(apservisfinitematrix(c, k, n+1, _state), "MinQPSetLC: C contains infinite or NaN values!", _state);
    
    /*
     * Handle zero K
     */
    if( k==0 )
    {
        state->nec = 0;
        state->nic = 0;
        return;
    }
    
    /*
     * Equality constraints are stored first, in the upper
     * NEC rows of State.CLEIC matrix. Inequality constraints
     * are stored in the next NIC rows.
     *
     * NOTE: we convert inequality constraints to the form
     * A*x<=b before copying them.
     */
    rmatrixsetlengthatleast(&state->cleic, k, n+1, _state);
    state->nec = 0;
    state->nic = 0;
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]==0 )
        {
            ae_v_move(&state->cleic.ptr.pp_double[state->nec][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            state->nec = state->nec+1;
        }
    }
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]!=0 )
        {
            if( ct->ptr.p_int[i]>0 )
            {
                ae_v_moveneg(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            else
            {
                ae_v_move(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            state->nic = state->nic+1;
        }
    }
    
    /*
     * Normalize rows of State.CLEIC: each row must have unit norm.
     * Norm is calculated using first N elements (i.e. right part is
     * not counted when we calculate norm).
     */
    for(i=0; i<=k-1; i++)
    {
        v = 0;
        for(j=0; j<=n-1; j++)
        {
            v = v+ae_sqr(state->cleic.ptr.pp_double[i][j], _state);
        }
        if( ae_fp_eq(v,0) )
        {
            continue;
        }
        v = 1/ae_sqrt(v, _state);
        ae_v_muld(&state->cleic.ptr.pp_double[i][0], 1, ae_v_len(0,n), v);
    }
}


/*************************************************************************
This function solves quadratic programming problem.
You should call it after setting solver options with MinQPSet...() calls.

INPUT PARAMETERS:
    State   -   algorithm state

You should use MinQPResults() function to access results after calls
to this function.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey.
     Special thanks to Elvira Illarionova  for  important  suggestions  on
     the linearly constrained QP algorithm.
*************************************************************************/
void minqpoptimize(minqpstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t nbc;
    double v0;
    double v1;
    double v;
    double d2;
    double d1;
    double d0;
    double noisetolerance;
    double fprev;
    double fcand;
    double fcur;
    ae_int_t nextaction;
    ae_int_t actstatus;
    double noiselevel;
    ae_int_t badnewtonits;
    double maxscaledgrad;


    noisetolerance = 10;
    n = state->n;
    state->repterminationtype = -5;
    state->repinneriterationscount = 0;
    state->repouteriterationscount = 0;
    state->repncholesky = 0;
    state->repnmv = 0;
    state->debugphase1flops = 0;
    state->debugphase2flops = 0;
    state->debugphase3flops = 0;
    rvectorsetlengthatleast(&state->rctmpg, n, _state);
    
    /*
     * check correctness of constraints
     */
    for(i=0; i<=n-1; i++)
    {
        if( state->havebndl.ptr.p_bool[i]&&state->havebndu.ptr.p_bool[i] )
        {
            if( ae_fp_greater(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->repterminationtype = -3;
                return;
            }
        }
    }
    
    /*
     * count number of bound and linear constraints
     */
    nbc = 0;
    for(i=0; i<=n-1; i++)
    {
        if( state->havebndl.ptr.p_bool[i] )
        {
            nbc = nbc+1;
        }
        if( state->havebndu.ptr.p_bool[i] )
        {
            nbc = nbc+1;
        }
    }
    
    /*
     * Initial point:
     * * if we have starting point in StartX, we just have to bound it
     * * if we do not have StartX, deduce initial point from boundary constraints
     */
    if( state->havex )
    {
        for(i=0; i<=n-1; i++)
        {
            state->xs.ptr.p_double[i] = state->startx.ptr.p_double[i];
            if( state->havebndl.ptr.p_bool[i]&&ae_fp_less(state->xs.ptr.p_double[i],state->bndl.ptr.p_double[i]) )
            {
                state->xs.ptr.p_double[i] = state->bndl.ptr.p_double[i];
            }
            if( state->havebndu.ptr.p_bool[i]&&ae_fp_greater(state->xs.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->xs.ptr.p_double[i] = state->bndu.ptr.p_double[i];
            }
        }
    }
    else
    {
        for(i=0; i<=n-1; i++)
        {
            if( state->havebndl.ptr.p_bool[i]&&state->havebndu.ptr.p_bool[i] )
            {
                state->xs.ptr.p_double[i] = 0.5*(state->bndl.ptr.p_double[i]+state->bndu.ptr.p_double[i]);
                continue;
            }
            if( state->havebndl.ptr.p_bool[i] )
            {
                state->xs.ptr.p_double[i] = state->bndl.ptr.p_double[i];
                continue;
            }
            if( state->havebndu.ptr.p_bool[i] )
            {
                state->xs.ptr.p_double[i] = state->bndu.ptr.p_double[i];
                continue;
            }
            state->xs.ptr.p_double[i] = 0;
        }
    }
    
    /*
     * Cholesky solver.
     */
    if( state->algokind==1 )
    {
        
        /*
         * Check matrix type.
         * Cholesky solver supports only dense matrices.
         */
        if( state->akind!=0 )
        {
            state->repterminationtype = -5;
            return;
        }
        
        /*
         * Our formulation of quadratic problem includes origin point,
         * i.e. we have F(x-x_origin) which is minimized subject to
         * constraints on x, instead of having simply F(x).
         *
         * Here we make transition from non-zero origin to zero one.
         * In order to make such transition we have to:
         * 1. subtract x_origin from x_start
         * 2. modify constraints
         * 3. solve problem
         * 4. add x_origin to solution
         *
         * There is alternate solution - to modify quadratic function
         * by expansion of multipliers containing (x-x_origin), but
         * we prefer to modify constraints, because it is a) more precise
         * and b) easier to to.
         *
         * Parts (1)-(2) are done here. After this block is over,
         * we have:
         * * XS, which stores shifted XStart (if we don't have XStart,
         *   value of XS will be ignored later)
         * * WorkBndL, WorkBndU, which store modified boundary constraints.
         */
        for(i=0; i<=n-1; i++)
        {
            if( state->havebndl.ptr.p_bool[i] )
            {
                state->workbndl.ptr.p_double[i] = state->bndl.ptr.p_double[i]-state->xorigin.ptr.p_double[i];
            }
            else
            {
                state->workbndl.ptr.p_double[i] = _state->v_neginf;
            }
            if( state->havebndu.ptr.p_bool[i] )
            {
                state->workbndu.ptr.p_double[i] = state->bndu.ptr.p_double[i]-state->xorigin.ptr.p_double[i];
            }
            else
            {
                state->workbndu.ptr.p_double[i] = _state->v_posinf;
            }
        }
        rmatrixsetlengthatleast(&state->workcleic, state->nec+state->nic, n+1, _state);
        for(i=0; i<=state->nec+state->nic-1; i++)
        {
            v = ae_v_dotproduct(&state->cleic.ptr.pp_double[i][0], 1, &state->xorigin.ptr.p_double[0], 1, ae_v_len(0,n-1));
            ae_v_move(&state->workcleic.ptr.pp_double[i][0], 1, &state->cleic.ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
            state->workcleic.ptr.pp_double[i][n] = state->cleic.ptr.pp_double[i][n]-v;
        }
        
        /*
         * Starting point XS
         */
        if( state->havex )
        {
            
            /*
             * We have starting point in StartX, so we just have to shift and bound it
             */
            for(i=0; i<=n-1; i++)
            {
                state->xs.ptr.p_double[i] = state->startx.ptr.p_double[i]-state->xorigin.ptr.p_double[i];
                if( state->havebndl.ptr.p_bool[i] )
                {
                    if( ae_fp_less(state->xs.ptr.p_double[i],state->workbndl.ptr.p_double[i]) )
                    {
                        state->xs.ptr.p_double[i] = state->workbndl.ptr.p_double[i];
                    }
                }
                if( state->havebndu.ptr.p_bool[i] )
                {
                    if( ae_fp_greater(state->xs.ptr.p_double[i],state->workbndu.ptr.p_double[i]) )
                    {
                        state->xs.ptr.p_double[i] = state->workbndu.ptr.p_double[i];
                    }
                }
            }
        }
        else
        {
            
            /*
             * We don't have starting point, so we deduce it from
             * constraints (if they are present).
             *
             * NOTE: XS contains some meaningless values from previous block
             * which are ignored by code below.
             */
            for(i=0; i<=n-1; i++)
            {
                if( state->havebndl.ptr.p_bool[i]&&state->havebndu.ptr.p_bool[i] )
                {
                    state->xs.ptr.p_double[i] = 0.5*(state->workbndl.ptr.p_double[i]+state->workbndu.ptr.p_double[i]);
                    if( ae_fp_less(state->xs.ptr.p_double[i],state->workbndl.ptr.p_double[i]) )
                    {
                        state->xs.ptr.p_double[i] = state->workbndl.ptr.p_double[i];
                    }
                    if( ae_fp_greater(state->xs.ptr.p_double[i],state->workbndu.ptr.p_double[i]) )
                    {
                        state->xs.ptr.p_double[i] = state->workbndu.ptr.p_double[i];
                    }
                    continue;
                }
                if( state->havebndl.ptr.p_bool[i] )
                {
                    state->xs.ptr.p_double[i] = state->workbndl.ptr.p_double[i];
                    continue;
                }
                if( state->havebndu.ptr.p_bool[i] )
                {
                    state->xs.ptr.p_double[i] = state->workbndu.ptr.p_double[i];
                    continue;
                }
                state->xs.ptr.p_double[i] = 0;
            }
        }
        
        /*
         * Handle special case - no constraints
         */
        if( nbc==0&&state->nec+state->nic==0 )
        {
            
            /*
             * "Simple" unconstrained Cholesky
             */
            bvectorsetlengthatleast(&state->tmpb, n, _state);
            for(i=0; i<=n-1; i++)
            {
                state->tmpb.ptr.p_bool[i] = ae_false;
            }
            state->repncholesky = state->repncholesky+1;
            cqmsetb(&state->a, &state->b, _state);
            cqmsetactiveset(&state->a, &state->xs, &state->tmpb, _state);
            if( !cqmconstrainedoptimum(&state->a, &state->xn, _state) )
            {
                state->repterminationtype = -5;
                return;
            }
            ae_v_move(&state->xs.ptr.p_double[0], 1, &state->xn.ptr.p_double[0], 1, ae_v_len(0,n-1));
            ae_v_add(&state->xs.ptr.p_double[0], 1, &state->xorigin.ptr.p_double[0], 1, ae_v_len(0,n-1));
            state->repinneriterationscount = 1;
            state->repouteriterationscount = 1;
            state->repterminationtype = 4;
            return;
        }
        
        /*
         * Prepare "active set" structure
         */
        sassetbc(&state->sas, &state->workbndl, &state->workbndu, _state);
        sassetlcx(&state->sas, &state->workcleic, state->nec, state->nic, _state);
        sassetscale(&state->sas, &state->s, _state);
        if( !sasstartoptimization(&state->sas, &state->xs, _state) )
        {
            state->repterminationtype = -3;
            return;
        }
        
        /*
         * Main cycle of CQP algorithm
         */
        state->repterminationtype = 4;
        badnewtonits = 0;
        maxscaledgrad = 0.0;
        for(;;)
        {
            
            /*
             * Update iterations count
             */
            inc(&state->repouteriterationscount, _state);
            inc(&state->repinneriterationscount, _state);
            
            /*
             * Phase 1.
             *
             * Determine active set.
             * Update MaxScaledGrad.
             */
            cqmadx(&state->a, &state->sas.xc, &state->rctmpg, _state);
            ae_v_add(&state->rctmpg.ptr.p_double[0], 1, &state->b.ptr.p_double[0], 1, ae_v_len(0,n-1));
            sasreactivateconstraints(&state->sas, &state->rctmpg, _state);
            v = 0.0;
            for(i=0; i<=n-1; i++)
            {
                v = v+ae_sqr(state->rctmpg.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
            }
            maxscaledgrad = ae_maxreal(maxscaledgrad, ae_sqrt(v, _state), _state);
            
            /*
             * Phase 2: perform penalized steepest descent step.
             *
             * NextAction control variable is set on exit from this loop:
             * * NextAction>0 in case we have to proceed to Phase 3 (Newton step)
             * * NextAction<0 in case we have to proceed to Phase 1 (recalculate active set)
             * * NextAction=0 in case we found solution (step along projected gradient is small enough)
             */
            for(;;)
            {
                
                /*
                 * Calculate constrained descent direction, store to PG.
                 * Successful termination if PG is zero.
                 */
                cqmadx(&state->a, &state->sas.xc, &state->gc, _state);
                ae_v_add(&state->gc.ptr.p_double[0], 1, &state->b.ptr.p_double[0], 1, ae_v_len(0,n-1));
                sasconstraineddescent(&state->sas, &state->gc, &state->pg, _state);
                state->debugphase2flops = state->debugphase2flops+4*(state->nec+state->nic)*n;
                v0 = ae_v_dotproduct(&state->pg.ptr.p_double[0], 1, &state->pg.ptr.p_double[0], 1, ae_v_len(0,n-1));
                if( ae_fp_eq(v0,0) )
                {
                    
                    /*
                     * Constrained derivative is zero.
                     * Solution found.
                     */
                    nextaction = 0;
                    break;
                }
                
                /*
                 * Build quadratic model of F along descent direction:
                 *     F(xc+alpha*pg) = D2*alpha^2 + D1*alpha + D0
                 * Store noise level in the XC (noise level is used to classify
                 * step as singificant or insignificant).
                 *
                 * In case function curvature is negative or product of descent
                 * direction and gradient is non-negative, iterations are terminated.
                 *
                 * NOTE: D0 is not actually used, but we prefer to maintain it.
                 */
                fprev = minqp_minqpmodelvalue(&state->a, &state->b, &state->sas.xc, n, &state->tmp0, _state);
                fprev = fprev+minqp_penaltyfactor*maxscaledgrad*sasactivelcpenalty1(&state->sas, &state->sas.xc, _state);
                cqmevalx(&state->a, &state->sas.xc, &v, &noiselevel, _state);
                v0 = cqmxtadx2(&state->a, &state->pg, _state);
                state->debugphase2flops = state->debugphase2flops+3*2*n*n;
                d2 = v0;
                v1 = ae_v_dotproduct(&state->pg.ptr.p_double[0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
                d1 = v1;
                d0 = fprev;
                if( ae_fp_less_eq(d2,0) )
                {
                    
                    /*
                     * Second derivative is non-positive, function is non-convex.
                     */
                    state->repterminationtype = -5;
                    nextaction = 0;
                    break;
                }
                if( ae_fp_greater_eq(d1,0) )
                {
                    
                    /*
                     * Second derivative is positive, first derivative is non-negative.
                     * Solution found.
                     */
                    nextaction = 0;
                    break;
                }
                
                /*
                 * Modify quadratic model - add penalty for violation of the active
                 * constraints.
                 *
                 * Boundary constraints are always satisfied exactly, so we do not
                 * add penalty term for them. General equality constraint of the
                 * form a'*(xc+alpha*d)=b adds penalty term:
                 *     P(alpha) = (a'*(xc+alpha*d)-b)^2
                 *              = (alpha*(a'*d) + (a'*xc-b))^2
                 *              = alpha^2*(a'*d)^2 + alpha*2*(a'*d)*(a'*xc-b) + (a'*xc-b)^2
                 * Each penalty term is multiplied by 100*Anorm before adding it to
                 * the 1-dimensional quadratic model.
                 *
                 * Penalization of the quadratic model improves behavior of the
                 * algorithm in the presense of the multiple degenerate constraints.
                 * In particular, it prevents algorithm from making large steps in
                 * directions which violate equality constraints.
                 */
                for(i=0; i<=state->nec+state->nic-1; i++)
                {
                    if( state->sas.activeset.ptr.p_int[n+i]>0 )
                    {
                        v0 = ae_v_dotproduct(&state->workcleic.ptr.pp_double[i][0], 1, &state->pg.ptr.p_double[0], 1, ae_v_len(0,n-1));
                        v1 = ae_v_dotproduct(&state->workcleic.ptr.pp_double[i][0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
                        v1 = v1-state->workcleic.ptr.pp_double[i][n];
                        v = 100*state->anorm;
                        d2 = d2+v*ae_sqr(v0, _state);
                        d1 = d1+v*2*v0*v1;
                        d0 = d0+v*ae_sqr(v1, _state);
                    }
                }
                state->debugphase2flops = state->debugphase2flops+2*2*(state->nec+state->nic)*n;
                
                /*
                 * Try unbounded step.
                 * In case function change is dominated by noise or function actually increased
                 * instead of decreasing, we terminate iterations.
                 */
                v = -d1/(2*d2);
                ae_v_move(&state->xn.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
                ae_v_addd(&state->xn.ptr.p_double[0], 1, &state->pg.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
                fcand = minqp_minqpmodelvalue(&state->a, &state->b, &state->xn, n, &state->tmp0, _state);
                fcand = fcand+minqp_penaltyfactor*maxscaledgrad*sasactivelcpenalty1(&state->sas, &state->xn, _state);
                state->debugphase2flops = state->debugphase2flops+2*n*n;
                if( ae_fp_greater_eq(fcand,fprev-noiselevel*noisetolerance) )
                {
                    nextaction = 0;
                    break;
                }
                
                /*
                 * Save active set
                 * Perform bounded step with (possible) activation
                 */
                actstatus = minqp_minqpboundedstepandactivation(state, &state->xn, &state->tmp0, _state);
                fcur = minqp_minqpmodelvalue(&state->a, &state->b, &state->sas.xc, n, &state->tmp0, _state);
                state->debugphase2flops = state->debugphase2flops+2*n*n;
                
                /*
                 * Depending on results, decide what to do:
                 * 1. In case step was performed without activation of constraints,
                 *    we proceed to Newton method
                 * 2. In case there was activated at least one constraint with ActiveSet[I]<0,
                 *    we proceed to Phase 1 and re-evaluate active set.
                 * 3. Otherwise (activation of the constraints with ActiveSet[I]=0)
                 *    we try Phase 2 one more time.
                 */
                if( actstatus<0 )
                {
                    
                    /*
                     * Step without activation, proceed to Newton
                     */
                    nextaction = 1;
                    break;
                }
                if( actstatus==0 )
                {
                    
                    /*
                     * No new constraints added during last activation - only
                     * ones which were at the boundary (ActiveSet[I]=0), but
                     * inactive due to numerical noise.
                     *
                     * Now, these constraints are added to the active set, and
                     * we try to perform steepest descent (Phase 2) one more time.
                     */
                    continue;
                }
                else
                {
                    
                    /*
                     * Last step activated at least one significantly new
                     * constraint (ActiveSet[I]<0), we have to re-evaluate
                     * active set (Phase 1).
                     */
                    nextaction = -1;
                    break;
                }
            }
            if( nextaction<0 )
            {
                continue;
            }
            if( nextaction==0 )
            {
                break;
            }
            
            /*
             * Phase 3: fast equality-constrained solver
             *
             * NOTE: this solver uses Augmented Lagrangian algorithm to solve
             *       equality-constrained subproblems. This algorithm may
             *       perform steps which increase function values instead of
             *       decreasing it (in hard cases, like overconstrained problems).
             *
             *       Such non-monononic steps may create a loop, when Augmented
             *       Lagrangian algorithm performs uphill step, and steepest
             *       descent algorithm (Phase 2) performs downhill step in the
             *       opposite direction.
             *
             *       In order to prevent iterations to continue forever we
             *       count iterations when AL algorithm increased function
             *       value instead of decreasing it. When number of such "bad"
             *       iterations will increase beyong MaxBadNewtonIts, we will
             *       terminate algorithm.
             */
            fprev = minqp_minqpmodelvalue(&state->a, &state->b, &state->sas.xc, n, &state->tmp0, _state);
            for(;;)
            {
                
                /*
                 * Calculate optimum subject to presently active constraints
                 */
                state->repncholesky = state->repncholesky+1;
                state->debugphase3flops = state->debugphase3flops+ae_pow(n, 3, _state)/3;
                if( !minqp_minqpconstrainedoptimum(state, &state->a, state->anorm, &state->b, &state->xn, &state->tmp0, &state->tmpb, &state->tmp1, _state) )
                {
                    state->repterminationtype = -5;
                    sasstopoptimization(&state->sas, _state);
                    return;
                }
                
                /*
                 * Add constraints.
                 * If no constraints was added, accept candidate point XN and move to next phase.
                 */
                if( minqp_minqpboundedstepandactivation(state, &state->xn, &state->tmp0, _state)<0 )
                {
                    break;
                }
            }
            fcur = minqp_minqpmodelvalue(&state->a, &state->b, &state->sas.xc, n, &state->tmp0, _state);
            if( ae_fp_greater_eq(fcur,fprev) )
            {
                badnewtonits = badnewtonits+1;
            }
            if( badnewtonits>=minqp_maxbadnewtonits )
            {
                
                /*
                 * Algorithm found solution, but keeps iterating because Newton
                 * algorithm performs uphill steps (noise in the Augmented Lagrangian
                 * algorithm). We terminate algorithm; it is considered normal
                 * termination.
                 */
                break;
            }
        }
        sasstopoptimization(&state->sas, _state);
        
        /*
         * Post-process: add XOrigin to XC
         */
        for(i=0; i<=n-1; i++)
        {
            if( state->havebndl.ptr.p_bool[i]&&ae_fp_eq(state->sas.xc.ptr.p_double[i],state->workbndl.ptr.p_double[i]) )
            {
                state->xs.ptr.p_double[i] = state->bndl.ptr.p_double[i];
                continue;
            }
            if( state->havebndu.ptr.p_bool[i]&&ae_fp_eq(state->sas.xc.ptr.p_double[i],state->workbndu.ptr.p_double[i]) )
            {
                state->xs.ptr.p_double[i] = state->bndu.ptr.p_double[i];
                continue;
            }
            state->xs.ptr.p_double[i] = boundval(state->sas.xc.ptr.p_double[i]+state->xorigin.ptr.p_double[i], state->bndl.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
        }
        return;
    }
    
    /*
     * BLEIC solver
     */
    if( state->algokind==2 )
    {
        ae_assert(state->akind==0||state->akind==1, "MinQPOptimize: unexpected AKind", _state);
        ivectorsetlengthatleast(&state->tmpi, state->nec+state->nic, _state);
        rvectorsetlengthatleast(&state->tmp0, n, _state);
        rvectorsetlengthatleast(&state->tmp1, n, _state);
        for(i=0; i<=state->nec-1; i++)
        {
            state->tmpi.ptr.p_int[i] = 0;
        }
        for(i=0; i<=state->nic-1; i++)
        {
            state->tmpi.ptr.p_int[state->nec+i] = -1;
        }
        minbleicsetlc(&state->solver, &state->cleic, &state->tmpi, state->nec+state->nic, _state);
        minbleicsetbc(&state->solver, &state->bndl, &state->bndu, _state);
        minbleicsetdrep(&state->solver, ae_true, _state);
        minbleicsetcond(&state->solver, ae_minrealnumber, 0.0, 0.0, state->bleicmaxits, _state);
        minbleicsetscale(&state->solver, &state->s, _state);
        minbleicsetprecscale(&state->solver, _state);
        minbleicrestartfrom(&state->solver, &state->xs, _state);
        state->repterminationtype = 0;
        while(minbleiciteration(&state->solver, _state))
        {
            
            /*
             * Line search started
             */
            if( state->solver.lsstart )
            {
                
                /*
                 * Iteration counters:
                 * * inner iterations count is increased on every line search
                 * * outer iterations count is increased only at steepest descent line search
                 */
                inc(&state->repinneriterationscount, _state);
                if( !state->solver.lbfgssearch )
                {
                    inc(&state->repouteriterationscount, _state);
                }
                
                /*
                 * Build quadratic model of F along descent direction:
                 *     F(x+alpha*d) = D2*alpha^2 + D1*alpha + D0
                 */
                d0 = state->solver.f;
                d1 = ae_v_dotproduct(&state->solver.d.ptr.p_double[0], 1, &state->solver.g.ptr.p_double[0], 1, ae_v_len(0,n-1));
                d2 = 0;
                if( state->akind==0 )
                {
                    d2 = cqmxtadx2(&state->a, &state->solver.d, _state);
                }
                if( state->akind==1 )
                {
                    sparsesmv(&state->sparsea, state->sparseaupper, &state->solver.d, &state->tmp0, _state);
                    d2 = 0.0;
                    for(i=0; i<=n-1; i++)
                    {
                        d2 = d2+state->solver.d.ptr.p_double[i]*state->tmp0.ptr.p_double[i];
                    }
                    d2 = 0.5*d2;
                }
                
                /*
                 * Suggest new step
                 */
                if( ae_fp_less(d1,0)&&ae_fp_greater(d2,0) )
                {
                    state->solver.stp = safeminposrv(-d1, 2*d2, state->solver.curstpmax, _state);
                }
                
                /*
                 * This line search may be started from steepest descent
                 * stage (stage 2) or from L-BFGS stage (stage 3) of the
                 * BLEIC algorithm. Depending on stage type, different
                 * checks are performed.
                 *
                 * Say, L-BFGS stage is an equality-constrained refinement
                 * stage of BLEIC. This stage refines current iterate
                 * under "frozen" equality constraints. We can terminate
                 * iterations at this stage only when we encounter
                 * unconstrained direction of negative curvature. In all
                 * other cases (say, when constrained gradient is zero)
                 * we should not terminate algorithm because everything may
                 * change after de-activating presently active constraints.
                 *
                 * At steepest descent stage of BLEIC we can terminate algorithm
                 * because it found minimum (steepest descent step is zero
                 * or too short). We also perform check for direction of
                 * negative curvature.
                 */
                if( (ae_fp_less(d2,0)||(ae_fp_eq(d2,0)&&ae_fp_less(d1,0)))&&!state->solver.boundedstep )
                {
                    
                    /*
                     * Function is unbounded from below:
                     * * function will decrease along D, i.e. either:
                     *   * D2<0
                     *   * D2=0 and D1<0
                     * * step is unconstrained
                     *
                     * If these conditions are true, we abnormally terminate QP
                     * algorithm with return code -4 (we can do so at any stage
                     * of BLEIC - whether it is L-BFGS or steepest descent one).
                     */
                    state->repterminationtype = -4;
                    for(i=0; i<=n-1; i++)
                    {
                        state->xs.ptr.p_double[i] = state->solver.x.ptr.p_double[i];
                    }
                    break;
                }
                if( !state->solver.lbfgssearch&&ae_fp_greater_eq(d2,0) )
                {
                    
                    /*
                     * Tests for "normal" convergence.
                     *
                     * These tests are performed only at "steepest descent" stage
                     * of the BLEIC algorithm, and only when function is non-concave
                     * (D2>=0) along direction D.
                     *
                     * NOTE: we do not test iteration count (MaxIts) here, because
                     *       this stopping condition is tested by BLEIC itself.
                     */
                    if( ae_fp_greater_eq(d1,0) )
                    {
                        
                        /*
                         * "Emergency" stopping condition: D is non-descent direction.
                         * Sometimes it is possible because of numerical noise in the
                         * target function.
                         */
                        state->repterminationtype = 4;
                        for(i=0; i<=n-1; i++)
                        {
                            state->xs.ptr.p_double[i] = state->solver.x.ptr.p_double[i];
                        }
                        break;
                    }
                    if( ae_fp_greater(d2,0) )
                    {
                        
                        /*
                         * Stopping condition #4 - gradient norm is small:
                         *
                         * 1. rescale State.Solver.D and State.Solver.G according to
                         *    current scaling, store results to Tmp0 and Tmp1.
                         * 2. Normalize Tmp0 (scaled direction vector).
                         * 3. compute directional derivative (in scaled variables),
                         *    which is equal to DOTPRODUCT(Tmp0,Tmp1).
                         */
                        v = 0;
                        for(i=0; i<=n-1; i++)
                        {
                            state->tmp0.ptr.p_double[i] = state->solver.d.ptr.p_double[i]/state->s.ptr.p_double[i];
                            state->tmp1.ptr.p_double[i] = state->solver.g.ptr.p_double[i]*state->s.ptr.p_double[i];
                            v = v+ae_sqr(state->tmp0.ptr.p_double[i], _state);
                        }
                        ae_assert(ae_fp_greater(v,0), "MinQPOptimize: inernal errror (scaled direction is zero)", _state);
                        v = 1/ae_sqrt(v, _state);
                        ae_v_muld(&state->tmp0.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
                        v = ae_v_dotproduct(&state->tmp0.ptr.p_double[0], 1, &state->tmp1.ptr.p_double[0], 1, ae_v_len(0,n-1));
                        if( ae_fp_less_eq(ae_fabs(v, _state),state->bleicepsg) )
                        {
                            state->repterminationtype = 4;
                            for(i=0; i<=n-1; i++)
                            {
                                state->xs.ptr.p_double[i] = state->solver.x.ptr.p_double[i];
                            }
                            break;
                        }
                        
                        /*
                         * Stopping condition #1 - relative function improvement is small:
                         *
                         * 1. calculate steepest descent step:   V = -D1/(2*D2)
                         * 2. calculate function change:         V1= D2*V^2 + D1*V
                         * 3. stop if function change is small enough
                         */
                        v = -d1/(2*d2);
                        v1 = d2*v*v+d1*v;
                        if( ae_fp_less_eq(ae_fabs(v1, _state),state->bleicepsf*ae_maxreal(d0, 1.0, _state)) )
                        {
                            state->repterminationtype = 1;
                            for(i=0; i<=n-1; i++)
                            {
                                state->xs.ptr.p_double[i] = state->solver.x.ptr.p_double[i];
                            }
                            break;
                        }
                        
                        /*
                         * Stopping condition #2 - scaled step is small:
                         *
                         * 1. calculate step multiplier V0 (step itself is D*V0)
                         * 2. calculate scaled step length V
                         * 3. stop if step is small enough
                         */
                        v0 = -d1/(2*d2);
                        v = 0;
                        for(i=0; i<=n-1; i++)
                        {
                            v = v+ae_sqr(v0*state->solver.d.ptr.p_double[i]/state->s.ptr.p_double[i], _state);
                        }
                        if( ae_fp_less_eq(ae_sqrt(v, _state),state->bleicepsx) )
                        {
                            state->repterminationtype = 2;
                            for(i=0; i<=n-1; i++)
                            {
                                state->xs.ptr.p_double[i] = state->solver.x.ptr.p_double[i];
                            }
                            break;
                        }
                    }
                }
            }
            
            /*
             * Gradient evaluation
             */
            if( state->solver.needfg )
            {
                for(i=0; i<=n-1; i++)
                {
                    state->tmp0.ptr.p_double[i] = state->solver.x.ptr.p_double[i]-state->xorigin.ptr.p_double[i];
                }
                if( state->akind==0 )
                {
                    cqmadx(&state->a, &state->tmp0, &state->tmp1, _state);
                }
                if( state->akind==1 )
                {
                    sparsesmv(&state->sparsea, state->sparseaupper, &state->tmp0, &state->tmp1, _state);
                }
                v0 = ae_v_dotproduct(&state->tmp0.ptr.p_double[0], 1, &state->tmp1.ptr.p_double[0], 1, ae_v_len(0,n-1));
                v1 = ae_v_dotproduct(&state->tmp0.ptr.p_double[0], 1, &state->b.ptr.p_double[0], 1, ae_v_len(0,n-1));
                state->solver.f = 0.5*v0+v1;
                ae_v_move(&state->solver.g.ptr.p_double[0], 1, &state->tmp1.ptr.p_double[0], 1, ae_v_len(0,n-1));
                ae_v_add(&state->solver.g.ptr.p_double[0], 1, &state->b.ptr.p_double[0], 1, ae_v_len(0,n-1));
            }
        }
        if( state->repterminationtype==0 )
        {
            
            /*
             * BLEIC optimizer was terminated by one of its inner stopping
             * conditions. Usually it is iteration counter (if such
             * stopping condition was specified by user).
             */
            minbleicresults(&state->solver, &state->xs, &state->solverrep, _state);
            state->repterminationtype = state->solverrep.terminationtype;
        }
        else
        {
            
            /*
             * BLEIC optimizer was terminated in "emergency" mode by QP
             * solver.
             *
             * NOTE: such termination is "emergency" only when viewed from
             *       BLEIC's position. QP solver sees such termination as
             *       routine one, triggered by QP's stopping criteria.
             */
            minbleicemergencytermination(&state->solver, _state);
        }
        return;
    }
}


/*************************************************************************
QP solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution.
                This array is allocated and initialized only when
                Rep.TerminationType parameter is positive (success).
    Rep     -   optimization report. You should check Rep.TerminationType,
                which contains completion code, and you may check  another
                fields which contain another information  about  algorithm
                functioning.
                
                Failure codes returned by algorithm are:
                * -5    inappropriate solver was used:
                        * Cholesky solver for (semi)indefinite problems
                        * Cholesky solver for problems with sparse matrix
                * -4    BLEIC-QP algorithm found unconstrained direction
                        of negative curvature (function is unbounded from
                        below  even  under  constraints),  no  meaningful
                        minimum can be found.
                * -3    inconsistent constraints (or maybe  feasible point
                        is too  hard  to  find).  If  you  are  sure  that
                        constraints are feasible, try to restart optimizer
                        with better initial approximation.
                        
                Completion codes specific for Cholesky algorithm:
                *  4   successful completion
                
                Completion codes specific for BLEIC-based algorithm:
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresults(minqpstate* state,
     /* Real    */ ae_vector* x,
     minqpreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _minqpreport_clear(rep);

    minqpresultsbuf(state, x, rep, _state);
}


/*************************************************************************
QP results

Buffered implementation of MinQPResults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresultsbuf(minqpstate* state,
     /* Real    */ ae_vector* x,
     minqpreport* rep,
     ae_state *_state)
{


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->xs.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->inneriterationscount = state->repinneriterationscount;
    rep->outeriterationscount = state->repouteriterationscount;
    rep->nmv = state->repnmv;
    rep->ncholesky = state->repncholesky;
    rep->terminationtype = state->repterminationtype;
}


/*************************************************************************
Fast version of MinQPSetLinearTerm(), which doesn't check its arguments.
For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetlineartermfast(minqpstate* state,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{


    ae_v_move(&state->b.ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
}


/*************************************************************************
Fast version of MinQPSetQuadraticTerm(), which doesn't check its arguments.

It accepts additional parameter - shift S, which allows to "shift"  matrix
A by adding s*I to A. S must be positive (although it is not checked).

For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadratictermfast(minqpstate* state,
     /* Real    */ ae_matrix* a,
     ae_bool isupper,
     double s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;


    n = state->n;
    state->akind = 0;
    cqmseta(&state->a, a, isupper, 1.0, _state);
    if( ae_fp_greater(s,0) )
    {
        rvectorsetlengthatleast(&state->tmp0, n, _state);
        for(i=0; i<=n-1; i++)
        {
            state->tmp0.ptr.p_double[i] = a->ptr.pp_double[i][i]+s;
        }
        cqmrewritedensediagonal(&state->a, &state->tmp0, _state);
    }
    
    /*
     * Estimate norm of A
     * (it will be used later in the quadratic penalty function)
     */
    state->anorm = 0;
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            for(j=i; j<=n-1; j++)
            {
                state->anorm = ae_maxreal(state->anorm, ae_fabs(a->ptr.pp_double[i][j], _state), _state);
            }
        }
        else
        {
            for(j=0; j<=i; j++)
            {
                state->anorm = ae_maxreal(state->anorm, ae_fabs(a->ptr.pp_double[i][j], _state), _state);
            }
        }
    }
    state->anorm = state->anorm*n;
}


/*************************************************************************
Internal function which allows to rewrite diagonal of quadratic term.
For internal use only.

This function can be used only when you have dense A and already made
MinQPSetQuadraticTerm(Fast) call.

  -- ALGLIB --
     Copyright 16.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqprewritediagonal(minqpstate* state,
     /* Real    */ ae_vector* s,
     ae_state *_state)
{


    cqmrewritedensediagonal(&state->a, s, _state);
}


/*************************************************************************
Fast version of MinQPSetStartingPoint(), which doesn't check its arguments.
For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetstartingpointfast(minqpstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_v_move(&state->startx.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->havex = ae_true;
}


/*************************************************************************
Fast version of MinQPSetOrigin(), which doesn't check its arguments.
For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetoriginfast(minqpstate* state,
     /* Real    */ ae_vector* xorigin,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_v_move(&state->xorigin.ptr.p_double[0], 1, &xorigin->ptr.p_double[0], 1, ae_v_len(0,n-1));
}


/*************************************************************************
Having feasible current point XC and possibly infeasible candidate   point
XN,  this  function  performs  longest  step  from  XC to XN which retains
feasibility. In case XN is found to be infeasible, at least one constraint
is activated.

For example, if we have:
  XC=0.5
  XN=1.2
  x>=0, x<=1
then this function will move us to X=1.0 and activate constraint "x<=1".

INPUT PARAMETERS:
    State   -   MinQP state.
    XC      -   current point, must be feasible with respect to
                all constraints
    XN      -   candidate point, can be infeasible with respect to some
                constraints. Must be located in the subspace of current
                active set, i.e. it is feasible with respect to already
                active constraints.
    Buf     -   temporary buffer, automatically resized if needed

OUTPUT PARAMETERS:
    State   -   this function changes following fields of State:
                * State.ActiveSet
                * State.ActiveC     -   active linear constraints
    XC      -   new position

RESULT:
    >0, in case at least one inactive non-candidate constraint was activated
    =0, in case only "candidate" constraints were activated
    <0, in case no constraints were activated by the step


  -- ALGLIB --
     Copyright 29.02.2012 by Bochkanov Sergey
*************************************************************************/
static ae_int_t minqp_minqpboundedstepandactivation(minqpstate* state,
     /* Real    */ ae_vector* xn,
     /* Real    */ ae_vector* buf,
     ae_state *_state)
{
    ae_int_t n;
    double stpmax;
    ae_int_t cidx;
    double cval;
    ae_bool needact;
    double v;
    ae_int_t result;


    n = state->n;
    rvectorsetlengthatleast(buf, n, _state);
    ae_v_move(&buf->ptr.p_double[0], 1, &xn->ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_sub(&buf->ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    sasexploredirection(&state->sas, buf, &stpmax, &cidx, &cval, _state);
    needact = ae_fp_less_eq(stpmax,1);
    v = ae_minreal(stpmax, 1.0, _state);
    ae_v_muld(&buf->ptr.p_double[0], 1, ae_v_len(0,n-1), v);
    ae_v_add(&buf->ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    result = sasmoveto(&state->sas, buf, needact, cidx, cval, _state);
    return result;
}


/*************************************************************************
Model value: f = 0.5*x'*A*x + b'*x

INPUT PARAMETERS:
    A       -   convex quadratic model; only main quadratic term is used,
                other parts of the model (D/Q/linear term) are ignored.
                This function does not modify model state.
    B       -   right part
    XC      -   evaluation point
    Tmp     -   temporary buffer, automatically resized if needed

  -- ALGLIB --
     Copyright 20.06.2012 by Bochkanov Sergey
*************************************************************************/
static double minqp_minqpmodelvalue(convexquadraticmodel* a,
     /* Real    */ ae_vector* b,
     /* Real    */ ae_vector* xc,
     ae_int_t n,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    double v0;
    double v1;
    double result;


    rvectorsetlengthatleast(tmp, n, _state);
    cqmadx(a, xc, tmp, _state);
    v0 = ae_v_dotproduct(&xc->ptr.p_double[0], 1, &tmp->ptr.p_double[0], 1, ae_v_len(0,n-1));
    v1 = ae_v_dotproduct(&xc->ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    result = 0.5*v0+v1;
    return result;
}


/*************************************************************************
Optimum of A subject to:
a) active boundary constraints (given by ActiveSet[] and corresponding
   elements of XC)
b) active linear constraints (given by C, R, LagrangeC)

INPUT PARAMETERS:
    A       -   main quadratic term of the model;
                although structure may  store  linear  and  rank-K  terms,
                these terms are ignored and rewritten  by  this  function.
    ANorm   -   estimate of ||A|| (2-norm is used)
    B       -   array[N], linear term of the model
    XN      -   possibly preallocated buffer
    Tmp     -   temporary buffer (automatically resized)
    Tmp1    -   temporary buffer (automatically resized)

OUTPUT PARAMETERS:
    A       -   modified quadratic model (this function changes rank-K
                term and linear term of the model)
    LagrangeC-  current estimate of the Lagrange coefficients
    XN      -   solution

RESULT:
    True on success, False on failure (non-SPD model)

  -- ALGLIB --
     Copyright 20.06.2012 by Bochkanov Sergey
*************************************************************************/
static ae_bool minqp_minqpconstrainedoptimum(minqpstate* state,
     convexquadraticmodel* a,
     double anorm,
     /* Real    */ ae_vector* b,
     /* Real    */ ae_vector* xn,
     /* Real    */ ae_vector* tmp,
     /* Boolean */ ae_vector* tmpb,
     /* Real    */ ae_vector* lagrangec,
     ae_state *_state)
{
    ae_int_t itidx;
    ae_int_t i;
    double v;
    double feaserrold;
    double feaserrnew;
    double theta;
    ae_int_t n;
    ae_bool result;


    n = state->n;
    
    /*
     * Rebuild basis accroding to current active set.
     * We call SASRebuildBasis() to make sure that fields of SAS
     * store up to date values.
     */
    sasrebuildbasis(&state->sas, _state);
    
    /*
     * Allocate temporaries.
     */
    rvectorsetlengthatleast(tmp, ae_maxint(n, state->sas.basissize, _state), _state);
    bvectorsetlengthatleast(tmpb, n, _state);
    rvectorsetlengthatleast(lagrangec, state->sas.basissize, _state);
    
    /*
     * Prepare model
     */
    for(i=0; i<=state->sas.basissize-1; i++)
    {
        tmp->ptr.p_double[i] = state->sas.pbasis.ptr.pp_double[i][n];
    }
    theta = 100.0*anorm;
    for(i=0; i<=n-1; i++)
    {
        if( state->sas.activeset.ptr.p_int[i]>0 )
        {
            tmpb->ptr.p_bool[i] = ae_true;
        }
        else
        {
            tmpb->ptr.p_bool[i] = ae_false;
        }
    }
    cqmsetactiveset(a, &state->sas.xc, tmpb, _state);
    cqmsetq(a, &state->sas.pbasis, tmp, state->sas.basissize, theta, _state);
    
    /*
     * Iterate until optimal values of Lagrange multipliers are found
     */
    for(i=0; i<=state->sas.basissize-1; i++)
    {
        lagrangec->ptr.p_double[i] = 0;
    }
    feaserrnew = ae_maxrealnumber;
    result = ae_true;
    for(itidx=1; itidx<=minqp_maxlagrangeits; itidx++)
    {
        
        /*
         * Generate right part B using linear term and current
         * estimate of the Lagrange multipliers.
         */
        ae_v_move(&tmp->ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
        for(i=0; i<=state->sas.basissize-1; i++)
        {
            v = lagrangec->ptr.p_double[i];
            ae_v_subd(&tmp->ptr.p_double[0], 1, &state->sas.pbasis.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
        }
        cqmsetb(a, tmp, _state);
        
        /*
         * Solve
         */
        result = cqmconstrainedoptimum(a, xn, _state);
        if( !result )
        {
            return result;
        }
        
        /*
         * Compare feasibility errors.
         * Terminate if error decreased too slowly.
         */
        feaserrold = feaserrnew;
        feaserrnew = 0;
        for(i=0; i<=state->sas.basissize-1; i++)
        {
            v = ae_v_dotproduct(&state->sas.pbasis.ptr.pp_double[i][0], 1, &xn->ptr.p_double[0], 1, ae_v_len(0,n-1));
            feaserrnew = feaserrnew+ae_sqr(v-state->sas.pbasis.ptr.pp_double[i][n], _state);
        }
        feaserrnew = ae_sqrt(feaserrnew, _state);
        if( ae_fp_greater_eq(feaserrnew,0.2*feaserrold) )
        {
            break;
        }
        
        /*
         * Update Lagrange multipliers
         */
        for(i=0; i<=state->sas.basissize-1; i++)
        {
            v = ae_v_dotproduct(&state->sas.pbasis.ptr.pp_double[i][0], 1, &xn->ptr.p_double[0], 1, ae_v_len(0,n-1));
            lagrangec->ptr.p_double[i] = lagrangec->ptr.p_double[i]-theta*(v-state->sas.pbasis.ptr.pp_double[i][n]);
        }
    }
    return result;
}


ae_bool _minqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minqpstate *p = (minqpstate*)_p;
    ae_touch_ptr((void*)p);
    if( !_convexquadraticmodel_init(&p->a, _state, make_automatic) )
        return ae_false;
    if( !_sparsematrix_init(&p->sparsea, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->havebndl, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->havebndu, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xorigin, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->startx, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->cleic, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_sactiveset_init(&p->sas, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->gc, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xn, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->pg, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->workbndl, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->workbndu, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->workcleic, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xs, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmpb, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->rctmpg, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmpi, 0, DT_INT, _state, make_automatic) )
        return ae_false;
    if( !_normestimatorstate_init(&p->estimator, _state, make_automatic) )
        return ae_false;
    if( !_minbleicstate_init(&p->solver, _state, make_automatic) )
        return ae_false;
    if( !_minbleicreport_init(&p->solverrep, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _minqpstate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minqpstate *dst = (minqpstate*)_dst;
    minqpstate *src = (minqpstate*)_src;
    dst->n = src->n;
    dst->algokind = src->algokind;
    dst->akind = src->akind;
    if( !_convexquadraticmodel_init_copy(&dst->a, &src->a, _state, make_automatic) )
        return ae_false;
    if( !_sparsematrix_init_copy(&dst->sparsea, &src->sparsea, _state, make_automatic) )
        return ae_false;
    dst->sparseaupper = src->sparseaupper;
    dst->anorm = src->anorm;
    if( !ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->havebndl, &src->havebndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->havebndu, &src->havebndu, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->xorigin, &src->xorigin, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->startx, &src->startx, _state, make_automatic) )
        return ae_false;
    dst->havex = src->havex;
    if( !ae_matrix_init_copy(&dst->cleic, &src->cleic, _state, make_automatic) )
        return ae_false;
    dst->nec = src->nec;
    dst->nic = src->nic;
    dst->bleicepsg = src->bleicepsg;
    dst->bleicepsf = src->bleicepsf;
    dst->bleicepsx = src->bleicepsx;
    dst->bleicmaxits = src->bleicmaxits;
    if( !_sactiveset_init_copy(&dst->sas, &src->sas, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->gc, &src->gc, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->xn, &src->xn, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->pg, &src->pg, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->workbndl, &src->workbndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->workbndu, &src->workbndu, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->workcleic, &src->workcleic, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->xs, &src->xs, _state, make_automatic) )
        return ae_false;
    dst->repinneriterationscount = src->repinneriterationscount;
    dst->repouteriterationscount = src->repouteriterationscount;
    dst->repncholesky = src->repncholesky;
    dst->repnmv = src->repnmv;
    dst->repterminationtype = src->repterminationtype;
    dst->debugphase1flops = src->debugphase1flops;
    dst->debugphase2flops = src->debugphase2flops;
    dst->debugphase3flops = src->debugphase3flops;
    if( !ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmpb, &src->tmpb, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->rctmpg, &src->rctmpg, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmpi, &src->tmpi, _state, make_automatic) )
        return ae_false;
    if( !_normestimatorstate_init_copy(&dst->estimator, &src->estimator, _state, make_automatic) )
        return ae_false;
    if( !_minbleicstate_init_copy(&dst->solver, &src->solver, _state, make_automatic) )
        return ae_false;
    if( !_minbleicreport_init_copy(&dst->solverrep, &src->solverrep, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _minqpstate_clear(void* _p)
{
    minqpstate *p = (minqpstate*)_p;
    ae_touch_ptr((void*)p);
    _convexquadraticmodel_clear(&p->a);
    _sparsematrix_clear(&p->sparsea);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->havebndl);
    ae_vector_clear(&p->havebndu);
    ae_vector_clear(&p->xorigin);
    ae_vector_clear(&p->startx);
    ae_matrix_clear(&p->cleic);
    _sactiveset_clear(&p->sas);
    ae_vector_clear(&p->gc);
    ae_vector_clear(&p->xn);
    ae_vector_clear(&p->pg);
    ae_vector_clear(&p->workbndl);
    ae_vector_clear(&p->workbndu);
    ae_matrix_clear(&p->workcleic);
    ae_vector_clear(&p->xs);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmpb);
    ae_vector_clear(&p->rctmpg);
    ae_vector_clear(&p->tmpi);
    _normestimatorstate_clear(&p->estimator);
    _minbleicstate_clear(&p->solver);
    _minbleicreport_clear(&p->solverrep);
}


void _minqpstate_destroy(void* _p)
{
    minqpstate *p = (minqpstate*)_p;
    ae_touch_ptr((void*)p);
    _convexquadraticmodel_destroy(&p->a);
    _sparsematrix_destroy(&p->sparsea);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->havebndl);
    ae_vector_destroy(&p->havebndu);
    ae_vector_destroy(&p->xorigin);
    ae_vector_destroy(&p->startx);
    ae_matrix_destroy(&p->cleic);
    _sactiveset_destroy(&p->sas);
    ae_vector_destroy(&p->gc);
    ae_vector_destroy(&p->xn);
    ae_vector_destroy(&p->pg);
    ae_vector_destroy(&p->workbndl);
    ae_vector_destroy(&p->workbndu);
    ae_matrix_destroy(&p->workcleic);
    ae_vector_destroy(&p->xs);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmpb);
    ae_vector_destroy(&p->rctmpg);
    ae_vector_destroy(&p->tmpi);
    _normestimatorstate_destroy(&p->estimator);
    _minbleicstate_destroy(&p->solver);
    _minbleicreport_destroy(&p->solverrep);
}


ae_bool _minqpreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minqpreport *p = (minqpreport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _minqpreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minqpreport *dst = (minqpreport*)_dst;
    minqpreport *src = (minqpreport*)_src;
    dst->inneriterationscount = src->inneriterationscount;
    dst->outeriterationscount = src->outeriterationscount;
    dst->nmv = src->nmv;
    dst->ncholesky = src->ncholesky;
    dst->terminationtype = src->terminationtype;
    return ae_true;
}


void _minqpreport_clear(void* _p)
{
    minqpreport *p = (minqpreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minqpreport_destroy(void* _p)
{
    minqpreport *p = (minqpreport*)_p;
    ae_touch_ptr((void*)p);
}




/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] and Jacobian of f[].


REQUIREMENTS:
This algorithm will request following information during its operation:

* function vector f[] at given point X
* function vector f[] and Jacobian of f[] (simultaneously) at given point

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec()  and jac() callbacks.
First  one  is used to calculate f[] at given point, second one calculates
f[] and Jacobian df[i]/dx[j].

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works  with  general  form function and does not provide Jacobian), but it
will  lead  to  exception  being  thrown  after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateVJ() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevj(ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x,
     minlmstate* state,
     ae_state *_state)
{

    _minlmstate_clear(state);

    ae_assert(n>=1, "MinLMCreateVJ: N<1!", _state);
    ae_assert(m>=1, "MinLMCreateVJ: M<1!", _state);
    ae_assert(x->cnt>=n, "MinLMCreateVJ: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinLMCreateVJ: X contains infinite or NaN values!", _state);
    
    /*
     * initialize, check parameters
     */
    state->teststep = 0;
    state->n = n;
    state->m = m;
    state->algomode = 1;
    state->hasf = ae_false;
    state->hasfi = ae_true;
    state->hasg = ae_false;
    
    /*
     * second stage of initialization
     */
    minlm_lmprepare(n, m, ae_false, state, _state);
    minlmsetacctype(state, 0, _state);
    minlmsetcond(state, 0, 0, 0, 0, _state);
    minlmsetxrep(state, ae_false, _state);
    minlmsetstpmax(state, 0, _state);
    minlmrestartfrom(state, x, _state);
}


/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] only. Finite differences  are  used  to
calculate Jacobian.


REQUIREMENTS:
This algorithm will request following information during its operation:
* function vector f[] at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec() callback.

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works with general form function and does not accept function vector), but
it will  lead  to  exception being thrown after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateV() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

See also MinLMIteration, MinLMResults.

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatev(ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x,
     double diffstep,
     minlmstate* state,
     ae_state *_state)
{

    _minlmstate_clear(state);

    ae_assert(ae_isfinite(diffstep, _state), "MinLMCreateV: DiffStep is not finite!", _state);
    ae_assert(ae_fp_greater(diffstep,0), "MinLMCreateV: DiffStep<=0!", _state);
    ae_assert(n>=1, "MinLMCreateV: N<1!", _state);
    ae_assert(m>=1, "MinLMCreateV: M<1!", _state);
    ae_assert(x->cnt>=n, "MinLMCreateV: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinLMCreateV: X contains infinite or NaN values!", _state);
    
    /*
     * Initialize
     */
    state->teststep = 0;
    state->n = n;
    state->m = m;
    state->algomode = 0;
    state->hasf = ae_false;
    state->hasfi = ae_true;
    state->hasg = ae_false;
    state->diffstep = diffstep;
    
    /*
     * Second stage of initialization
     */
    minlm_lmprepare(n, m, ae_false, state, _state);
    minlmsetacctype(state, 1, _state);
    minlmsetcond(state, 0, 0, 0, 0, _state);
    minlmsetxrep(state, ae_false, _state);
    minlmsetstpmax(state, 0, _state);
    minlmrestartfrom(state, x, _state);
}


/*************************************************************************
    LEVENBERG-MARQUARDT-LIKE METHOD FOR NON-LINEAR OPTIMIZATION

DESCRIPTION:
This  function  is  used  to  find  minimum  of general form (not "sum-of-
-squares") function
    F = F(x[0], ..., x[n-1])
using  its  gradient  and  Hessian.  Levenberg-Marquardt modification with
L-BFGS pre-optimization and internal pre-conditioned  L-BFGS  optimization
after each Levenberg-Marquardt step is used.


REQUIREMENTS:
This algorithm will request following information during its operation:

* function value F at given point X
* F and gradient G (simultaneously) at given point X
* F, G and Hessian H (simultaneously) at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts func(),  grad()  and  hess()
function pointers. First pointer is used to calculate F  at  given  point,
second  one  calculates  F(x)  and  grad F(x),  third one calculates F(x),
grad F(x), hess F(x).

You can try to initialize MinLMState structure with FGH-function and  then
use incorrect version of MinLMOptimize() (for example, version which  does
not provide Hessian matrix), but it will lead to  exception  being  thrown
after first attempt to calculate Hessian.


USAGE:
1. User initializes algorithm state with MinLMCreateFGH() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   pointers (delegates, etc.) to callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefgh(ae_int_t n,
     /* Real    */ ae_vector* x,
     minlmstate* state,
     ae_state *_state)
{

    _minlmstate_clear(state);

    ae_assert(n>=1, "MinLMCreateFGH: N<1!", _state);
    ae_assert(x->cnt>=n, "MinLMCreateFGH: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinLMCreateFGH: X contains infinite or NaN values!", _state);
    
    /*
     * initialize
     */
    state->teststep = 0;
    state->n = n;
    state->m = 0;
    state->algomode = 2;
    state->hasf = ae_true;
    state->hasfi = ae_false;
    state->hasg = ae_true;
    
    /*
     * init2
     */
    minlm_lmprepare(n, 0, ae_true, state, _state);
    minlmsetacctype(state, 2, _state);
    minlmsetcond(state, 0, 0, 0, 0, _state);
    minlmsetxrep(state, ae_false, _state);
    minlmsetstpmax(state, 0, _state);
    minlmrestartfrom(state, x, _state);
}


/*************************************************************************
This function sets stopping conditions for Levenberg-Marquardt optimization
algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinLMSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinLMSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations   is    unlimited.   Only   Levenberg-Marquardt
                iterations  are  counted  (L-BFGS/CG  iterations  are  NOT
                counted because their cost is very low compared to that of
                LM).

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetcond(minlmstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinLMSetCond: EpsG is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsg,0), "MinLMSetCond: negative EpsG!", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinLMSetCond: EpsF is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsf,0), "MinLMSetCond: negative EpsF!", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinLMSetCond: EpsX is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsx,0), "MinLMSetCond: negative EpsX!", _state);
    ae_assert(maxits>=0, "MinLMSetCond: negative MaxIts!", _state);
    if( ((ae_fp_eq(epsg,0)&&ae_fp_eq(epsf,0))&&ae_fp_eq(epsx,0))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    state->epsg = epsg;
    state->epsf = epsf;
    state->epsx = epsx;
    state->maxits = maxits;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinLMOptimize(). Both Levenberg-Marquardt and internal  L-BFGS
iterations are reported.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetxrep(minlmstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

NOTE: non-zero StpMax leads to moderate  performance  degradation  because
intermediate  step  of  preconditioned L-BFGS optimization is incompatible
with limits on step size.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetstpmax(minlmstate* state, double stpmax, ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "MinLMSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,0), "MinLMSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************
This function sets scaling coefficients for LM optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Generally, scale is NOT considered to be a form of preconditioner.  But LM
optimizer is unique in that it uses scaling matrix both  in  the  stopping
condition tests and as Marquardt damping factor.

Proper scaling is very important for the algorithm performance. It is less
important for the quality of results, but still has some influence (it  is
easier  to  converge  when  variables  are  properly  scaled, so premature
stopping is possible when very badly scalled variables are  combined  with
relaxed stopping conditions).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetscale(minlmstate* state,
     /* Real    */ ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinLMSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinLMSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],0), "MinLMSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function sets boundary constraints for LM optimizer

Boundary constraints are inactive by default (after initial creation).
They are preserved until explicitly turned off with another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by bound constraints
  or at its boundary

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetbc(minlmstate* state,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(bndl->cnt>=n, "MinLMSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinLMSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinLMSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinLMSetBC: BndU contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->havebndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->havebndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function is used to change acceleration settings

You can choose between three acceleration strategies:
* AccType=0, no acceleration.
* AccType=1, secant updates are used to update quadratic model after  each
  iteration. After fixed number of iterations (or after  model  breakdown)
  we  recalculate  quadratic  model  using  analytic  Jacobian  or  finite
  differences. Number of secant-based iterations depends  on  optimization
  settings: about 3 iterations - when we have analytic Jacobian, up to 2*N
  iterations - when we use finite differences to calculate Jacobian.

AccType=1 is recommended when Jacobian  calculation  cost  is  prohibitive
high (several Mx1 function vector calculations  followed  by  several  NxN
Cholesky factorizations are faster than calculation of one M*N  Jacobian).
It should also be used when we have no Jacobian, because finite difference
approximation takes too much time to compute.

Table below list  optimization  protocols  (XYZ  protocol  corresponds  to
MinLMCreateXYZ) and acceleration types they support (and use by  default).

ACCELERATION TYPES SUPPORTED BY OPTIMIZATION PROTOCOLS:

protocol    0   1   comment
V           +   +
VJ          +   +
FGH         +

DAFAULT VALUES:

protocol    0   1   comment
V               x   without acceleration it is so slooooooooow
VJ          x
FGH         x

NOTE: this  function should be called before optimization. Attempt to call
it during algorithm iterations may result in unexpected behavior.

NOTE: attempt to call this function with unsupported protocol/acceleration
combination will result in exception being thrown.

  -- ALGLIB --
     Copyright 14.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetacctype(minlmstate* state,
     ae_int_t acctype,
     ae_state *_state)
{


    ae_assert((acctype==0||acctype==1)||acctype==2, "MinLMSetAccType: incorrect AccType!", _state);
    if( acctype==2 )
    {
        acctype = 0;
    }
    if( acctype==0 )
    {
        state->maxmodelage = 0;
        state->makeadditers = ae_false;
        return;
    }
    if( acctype==1 )
    {
        ae_assert(state->hasfi, "MinLMSetAccType: AccType=1 is incompatible with current protocol!", _state);
        if( state->algomode==0 )
        {
            state->maxmodelage = 2*state->n;
        }
        else
        {
            state->maxmodelage = minlm_smallmodelage;
        }
        state->makeadditers = ae_false;
        return;
    }
}


/*************************************************************************
NOTES:

1. Depending on function used to create state  structure,  this  algorithm
   may accept Jacobian and/or Hessian and/or gradient.  According  to  the
   said above, there ase several versions of this function,  which  accept
   different sets of callbacks.

   This flexibility opens way to subtle errors - you may create state with
   MinLMCreateFGH() (optimization using Hessian), but call function  which
   does not accept Hessian. So when algorithm will request Hessian,  there
   will be no callback to call. In this case exception will be thrown.

   Be careful to avoid such errors because there is no way to find them at
   compile time - you can see them at runtime only.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool minlmiteration(minlmstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_bool bflag;
    ae_int_t iflag;
    double v;
    double s;
    double t;
    ae_int_t i;
    ae_int_t k;
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
        iflag = state->rstate.ia.ptr.p_int[2];
        i = state->rstate.ia.ptr.p_int[3];
        k = state->rstate.ia.ptr.p_int[4];
        bflag = state->rstate.ba.ptr.p_bool[0];
        v = state->rstate.ra.ptr.p_double[0];
        s = state->rstate.ra.ptr.p_double[1];
        t = state->rstate.ra.ptr.p_double[2];
    }
    else
    {
        n = -983;
        m = -989;
        iflag = -834;
        i = 900;
        k = -287;
        bflag = ae_false;
        v = 214;
        s = -338;
        t = -686;
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
    if( state->rstate.stage==8 )
    {
        goto lbl_8;
    }
    if( state->rstate.stage==9 )
    {
        goto lbl_9;
    }
    if( state->rstate.stage==10 )
    {
        goto lbl_10;
    }
    if( state->rstate.stage==11 )
    {
        goto lbl_11;
    }
    if( state->rstate.stage==12 )
    {
        goto lbl_12;
    }
    if( state->rstate.stage==13 )
    {
        goto lbl_13;
    }
    if( state->rstate.stage==14 )
    {
        goto lbl_14;
    }
    if( state->rstate.stage==15 )
    {
        goto lbl_15;
    }
    if( state->rstate.stage==16 )
    {
        goto lbl_16;
    }
    if( state->rstate.stage==17 )
    {
        goto lbl_17;
    }
    if( state->rstate.stage==18 )
    {
        goto lbl_18;
    }
    
    /*
     * Routine body
     */
    
    /*
     * prepare
     */
    n = state->n;
    m = state->m;
    state->repiterationscount = 0;
    state->repterminationtype = 0;
    state->repfuncidx = -1;
    state->repvaridx = -1;
    state->repnfunc = 0;
    state->repnjac = 0;
    state->repngrad = 0;
    state->repnhess = 0;
    state->repncholesky = 0;
    
    /*
     * check consistency of constraints,
     * enforce feasibility of the solution
     * set constraints
     */
    if( !enforceboundaryconstraints(&state->xbase, &state->bndl, &state->havebndl, &state->bndu, &state->havebndu, n, 0, _state) )
    {
        state->repterminationtype = -3;
        result = ae_false;
        return result;
    }
    minqpsetbc(&state->qpstate, &state->bndl, &state->bndu, _state);
    
    /*
     *  Check, that transferred derivative value is right
     */
    minlm_clearrequestfields(state, _state);
    if( !(state->algomode==1&&ae_fp_greater(state->teststep,0)) )
    {
        goto lbl_19;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->needfij = ae_true;
    i = 0;
lbl_21:
    if( i>n-1 )
    {
        goto lbl_23;
    }
    ae_assert((state->havebndl.ptr.p_bool[i]&&ae_fp_less_eq(state->bndl.ptr.p_double[i],state->x.ptr.p_double[i]))||!state->havebndl.ptr.p_bool[i], "MinLM: internal error(State.X is out of bounds)", _state);
    ae_assert((state->havebndu.ptr.p_bool[i]&&ae_fp_less_eq(state->x.ptr.p_double[i],state->bndu.ptr.p_double[i]))||!state->havebndu.ptr.p_bool[i], "MinLMIteration: internal error(State.X is out of bounds)", _state);
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->teststep*state->s.ptr.p_double[i];
    if( state->havebndl.ptr.p_bool[i] )
    {
        state->x.ptr.p_double[i] = ae_maxreal(state->x.ptr.p_double[i], state->bndl.ptr.p_double[i], _state);
    }
    state->xm1 = state->x.ptr.p_double[i];
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    ae_v_move(&state->fm1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    ae_v_move(&state->gm1.ptr.p_double[0], 1, &state->j.ptr.pp_double[0][i], state->j.stride, ae_v_len(0,m-1));
    state->x.ptr.p_double[i] = v+state->teststep*state->s.ptr.p_double[i];
    if( state->havebndu.ptr.p_bool[i] )
    {
        state->x.ptr.p_double[i] = ae_minreal(state->x.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
    }
    state->xp1 = state->x.ptr.p_double[i];
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    ae_v_move(&state->fp1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    ae_v_move(&state->gp1.ptr.p_double[0], 1, &state->j.ptr.pp_double[0][i], state->j.stride, ae_v_len(0,m-1));
    state->x.ptr.p_double[i] = (state->xm1+state->xp1)/2;
    if( state->havebndl.ptr.p_bool[i] )
    {
        state->x.ptr.p_double[i] = ae_maxreal(state->x.ptr.p_double[i], state->bndl.ptr.p_double[i], _state);
    }
    if( state->havebndu.ptr.p_bool[i] )
    {
        state->x.ptr.p_double[i] = ae_minreal(state->x.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
    }
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    ae_v_move(&state->fc1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    ae_v_move(&state->gc1.ptr.p_double[0], 1, &state->j.ptr.pp_double[0][i], state->j.stride, ae_v_len(0,m-1));
    state->x.ptr.p_double[i] = v;
    for(k=0; k<=m-1; k++)
    {
        if( !derivativecheck(state->fm1.ptr.p_double[k], state->gm1.ptr.p_double[k], state->fp1.ptr.p_double[k], state->gp1.ptr.p_double[k], state->fc1.ptr.p_double[k], state->gc1.ptr.p_double[k], state->xp1-state->xm1, _state) )
        {
            state->repfuncidx = k;
            state->repvaridx = i;
            state->repterminationtype = -7;
            result = ae_false;
            return result;
        }
    }
    i = i+1;
    goto lbl_21;
lbl_23:
    state->needfij = ae_false;
lbl_19:
    
    /*
     * Initial report of current point
     *
     * Note 1: we rewrite State.X twice because
     * user may accidentally change it after first call.
     *
     * Note 2: we set NeedF or NeedFI depending on what
     * information about function we have.
     */
    if( !state->xrep )
    {
        goto lbl_24;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    if( !state->hasf )
    {
        goto lbl_26;
    }
    state->needf = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needf = ae_false;
    goto lbl_27;
lbl_26:
    ae_assert(state->hasfi, "MinLM: internal error 2!", _state);
    state->needfi = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needfi = ae_false;
    v = ae_v_dotproduct(&state->fi.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    state->f = v;
lbl_27:
    state->repnfunc = state->repnfunc+1;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->xupdated = ae_false;
lbl_24:
    
    /*
     * Prepare control variables
     */
    state->nu = 1;
    state->lambdav = -ae_maxrealnumber;
    state->modelage = state->maxmodelage+1;
    state->deltaxready = ae_false;
    state->deltafready = ae_false;
    
    /*
     * Main cycle.
     *
     * We move through it until either:
     * * one of the stopping conditions is met
     * * we decide that stopping conditions are too stringent
     *   and break from cycle
     *
     */
lbl_28:
    if( ae_false )
    {
        goto lbl_29;
    }
    
    /*
     * First, we have to prepare quadratic model for our function.
     * We use BFlag to ensure that model is prepared;
     * if it is false at the end of this block, something went wrong.
     *
     * We may either calculate brand new model or update old one.
     *
     * Before this block we have:
     * * State.XBase            - current position.
     * * State.DeltaX           - if DeltaXReady is True
     * * State.DeltaF           - if DeltaFReady is True
     *
     * After this block is over, we will have:
     * * State.XBase            - base point (unchanged)
     * * State.FBase            - F(XBase)
     * * State.GBase            - linear term
     * * State.QuadraticModel   - quadratic term
     * * State.LambdaV          - current estimate for lambda
     *
     * We also clear DeltaXReady/DeltaFReady flags
     * after initialization is done.
     */
    bflag = ae_false;
    if( !(state->algomode==0||state->algomode==1) )
    {
        goto lbl_30;
    }
    
    /*
     * Calculate f[] and Jacobian
     */
    if( !(state->modelage>state->maxmodelage||!(state->deltaxready&&state->deltafready)) )
    {
        goto lbl_32;
    }
    
    /*
     * Refresh model (using either finite differences or analytic Jacobian)
     */
    if( state->algomode!=0 )
    {
        goto lbl_34;
    }
    
    /*
     * Optimization using F values only.
     * Use finite differences to estimate Jacobian.
     */
    ae_assert(state->hasfi, "MinLMIteration: internal error when estimating Jacobian (no f[])", _state);
    k = 0;
lbl_36:
    if( k>n-1 )
    {
        goto lbl_38;
    }
    
    /*
     * We guard X[k] from leaving [BndL,BndU].
     * In case BndL=BndU, we assume that derivative in this direction is zero.
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    if( state->havebndl.ptr.p_bool[k] )
    {
        state->x.ptr.p_double[k] = ae_maxreal(state->x.ptr.p_double[k], state->bndl.ptr.p_double[k], _state);
    }
    if( state->havebndu.ptr.p_bool[k] )
    {
        state->x.ptr.p_double[k] = ae_minreal(state->x.ptr.p_double[k], state->bndu.ptr.p_double[k], _state);
    }
    state->xm1 = state->x.ptr.p_double[k];
    minlm_clearrequestfields(state, _state);
    state->needfi = ae_true;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->repnfunc = state->repnfunc+1;
    ae_v_move(&state->fm1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    if( state->havebndl.ptr.p_bool[k] )
    {
        state->x.ptr.p_double[k] = ae_maxreal(state->x.ptr.p_double[k], state->bndl.ptr.p_double[k], _state);
    }
    if( state->havebndu.ptr.p_bool[k] )
    {
        state->x.ptr.p_double[k] = ae_minreal(state->x.ptr.p_double[k], state->bndu.ptr.p_double[k], _state);
    }
    state->xp1 = state->x.ptr.p_double[k];
    minlm_clearrequestfields(state, _state);
    state->needfi = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->repnfunc = state->repnfunc+1;
    ae_v_move(&state->fp1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    v = state->xp1-state->xm1;
    if( ae_fp_neq(v,0) )
    {
        v = 1/v;
        ae_v_moved(&state->j.ptr.pp_double[0][k], state->j.stride, &state->fp1.ptr.p_double[0], 1, ae_v_len(0,m-1), v);
        ae_v_subd(&state->j.ptr.pp_double[0][k], state->j.stride, &state->fm1.ptr.p_double[0], 1, ae_v_len(0,m-1), v);
    }
    else
    {
        for(i=0; i<=m-1; i++)
        {
            state->j.ptr.pp_double[i][k] = 0;
        }
    }
    k = k+1;
    goto lbl_36;
lbl_38:
    
    /*
     * Calculate F(XBase)
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->needfi = ae_true;
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->needfi = ae_false;
    state->repnfunc = state->repnfunc+1;
    state->repnjac = state->repnjac+1;
    
    /*
     * New model
     */
    state->modelage = 0;
    goto lbl_35;
lbl_34:
    
    /*
     * Obtain f[] and Jacobian
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->needfij = ae_true;
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->needfij = ae_false;
    state->repnfunc = state->repnfunc+1;
    state->repnjac = state->repnjac+1;
    
    /*
     * New model
     */
    state->modelage = 0;
lbl_35:
    goto lbl_33;
lbl_32:
    
    /*
     * State.J contains Jacobian or its current approximation;
     * refresh it using secant updates:
     *
     * f(x0+dx) = f(x0) + J*dx,
     * J_new = J_old + u*h'
     * h = x_new-x_old
     * u = (f_new - f_old - J_old*h)/(h'h)
     *
     * We can explicitly generate h and u, but it is
     * preferential to do in-place calculations. Only
     * I-th row of J_old is needed to calculate u[I],
     * so we can update J row by row in one pass.
     *
     * NOTE: we expect that State.XBase contains new point,
     * State.FBase contains old point, State.DeltaX and
     * State.DeltaY contain updates from last step.
     */
    ae_assert(state->deltaxready&&state->deltafready, "MinLMIteration: uninitialized DeltaX/DeltaF", _state);
    t = ae_v_dotproduct(&state->deltax.ptr.p_double[0], 1, &state->deltax.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_assert(ae_fp_neq(t,0), "MinLM: internal error (T=0)", _state);
    for(i=0; i<=m-1; i++)
    {
        v = ae_v_dotproduct(&state->j.ptr.pp_double[i][0], 1, &state->deltax.ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = (state->deltaf.ptr.p_double[i]-v)/t;
        ae_v_addd(&state->j.ptr.pp_double[i][0], 1, &state->deltax.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
    }
    ae_v_move(&state->fi.ptr.p_double[0], 1, &state->fibase.ptr.p_double[0], 1, ae_v_len(0,m-1));
    ae_v_add(&state->fi.ptr.p_double[0], 1, &state->deltaf.ptr.p_double[0], 1, ae_v_len(0,m-1));
    
    /*
     * Increase model age
     */
    state->modelage = state->modelage+1;
lbl_33:
    
    /*
     * Generate quadratic model:
     *     f(xbase+dx) =
     *       = (f0 + J*dx)'(f0 + J*dx)
     *       = f0^2 + dx'J'f0 + f0*J*dx + dx'J'J*dx
     *       = f0^2 + 2*f0*J*dx + dx'J'J*dx
     *
     * Note that we calculate 2*(J'J) instead of J'J because
     * our quadratic model is based on Tailor decomposition,
     * i.e. it has 0.5 before quadratic term.
     */
    rmatrixgemm(n, n, m, 2.0, &state->j, 0, 0, 1, &state->j, 0, 0, 0, 0.0, &state->quadraticmodel, 0, 0, _state);
    rmatrixmv(n, m, &state->j, 0, 0, 1, &state->fi, 0, &state->gbase, 0, _state);
    ae_v_muld(&state->gbase.ptr.p_double[0], 1, ae_v_len(0,n-1), 2);
    v = ae_v_dotproduct(&state->fi.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    state->fbase = v;
    ae_v_move(&state->fibase.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    
    /*
     * set control variables
     */
    bflag = ae_true;
lbl_30:
    if( state->algomode!=2 )
    {
        goto lbl_39;
    }
    ae_assert(!state->hasfi, "MinLMIteration: internal error (HasFI is True in Hessian-based mode)", _state);
    
    /*
     * Obtain F, G, H
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->needfgh = ae_true;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->needfgh = ae_false;
    state->repnfunc = state->repnfunc+1;
    state->repngrad = state->repngrad+1;
    state->repnhess = state->repnhess+1;
    rmatrixcopy(n, n, &state->h, 0, 0, &state->quadraticmodel, 0, 0, _state);
    ae_v_move(&state->gbase.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fbase = state->f;
    
    /*
     * set control variables
     */
    bflag = ae_true;
    state->modelage = 0;
lbl_39:
    ae_assert(bflag, "MinLM: internal integrity check failed!", _state);
    state->deltaxready = ae_false;
    state->deltafready = ae_false;
    
    /*
     * If Lambda is not initialized, initialize it using quadratic model
     */
    if( ae_fp_less(state->lambdav,0) )
    {
        state->lambdav = 0;
        for(i=0; i<=n-1; i++)
        {
            state->lambdav = ae_maxreal(state->lambdav, ae_fabs(state->quadraticmodel.ptr.pp_double[i][i], _state)*ae_sqr(state->s.ptr.p_double[i], _state), _state);
        }
        state->lambdav = 0.001*state->lambdav;
        if( ae_fp_eq(state->lambdav,0) )
        {
            state->lambdav = 1;
        }
    }
    
    /*
     * Test stopping conditions for function gradient
     */
    if( ae_fp_greater(minlm_boundedscaledantigradnorm(state, &state->xbase, &state->gbase, _state),state->epsg) )
    {
        goto lbl_41;
    }
    if( state->modelage!=0 )
    {
        goto lbl_43;
    }
    
    /*
     * Model is fresh, we can rely on it and terminate algorithm
     */
    state->repterminationtype = 4;
    if( !state->xrep )
    {
        goto lbl_45;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fbase;
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->xupdated = ae_false;
lbl_45:
    result = ae_false;
    return result;
    goto lbl_44;
lbl_43:
    
    /*
     * Model is not fresh, we should refresh it and test
     * conditions once more
     */
    state->modelage = state->maxmodelage+1;
    goto lbl_28;
lbl_44:
lbl_41:
    
    /*
     * Find value of Levenberg-Marquardt damping parameter which:
     * * leads to positive definite damped model
     * * within bounds specified by StpMax
     * * generates step which decreases function value
     *
     * After this block IFlag is set to:
     * * -3, if constraints are infeasible
     * * -2, if model update is needed (either Lambda growth is too large
     *       or step is too short, but we can't rely on model and stop iterations)
     * * -1, if model is fresh, Lambda have grown too large, termination is needed
     * *  0, if everything is OK, continue iterations
     *
     * State.Nu can have any value on enter, but after exit it is set to 1.0
     */
    iflag = -99;
lbl_47:
    if( ae_false )
    {
        goto lbl_48;
    }
    
    /*
     * Do we need model update?
     */
    if( state->modelage>0&&ae_fp_greater_eq(state->nu,minlm_suspiciousnu) )
    {
        iflag = -2;
        goto lbl_48;
    }
    
    /*
     * Setup quadratic solver and solve quadratic programming problem.
     * After problem is solved we'll try to bound step by StpMax
     * (Lambda will be increased if step size is too large).
     *
     * We use BFlag variable to indicate that we have to increase Lambda.
     * If it is False, we will try to increase Lambda and move to new iteration.
     */
    bflag = ae_true;
    minqpsetstartingpointfast(&state->qpstate, &state->xbase, _state);
    minqpsetoriginfast(&state->qpstate, &state->xbase, _state);
    minqpsetlineartermfast(&state->qpstate, &state->gbase, _state);
    minqpsetquadratictermfast(&state->qpstate, &state->quadraticmodel, ae_true, 0.0, _state);
    for(i=0; i<=n-1; i++)
    {
        state->tmp0.ptr.p_double[i] = state->quadraticmodel.ptr.pp_double[i][i]+state->lambdav/ae_sqr(state->s.ptr.p_double[i], _state);
    }
    minqprewritediagonal(&state->qpstate, &state->tmp0, _state);
    minqpoptimize(&state->qpstate, _state);
    minqpresultsbuf(&state->qpstate, &state->xdir, &state->qprep, _state);
    if( state->qprep.terminationtype>0 )
    {
        
        /*
         * successful solution of QP problem
         */
        ae_v_sub(&state->xdir.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = ae_v_dotproduct(&state->xdir.ptr.p_double[0], 1, &state->xdir.ptr.p_double[0], 1, ae_v_len(0,n-1));
        if( ae_isfinite(v, _state) )
        {
            v = ae_sqrt(v, _state);
            if( ae_fp_greater(state->stpmax,0)&&ae_fp_greater(v,state->stpmax) )
            {
                bflag = ae_false;
            }
        }
        else
        {
            bflag = ae_false;
        }
    }
    else
    {
        
        /*
         * Either problem is non-convex (increase LambdaV) or constraints are inconsistent
         */
        ae_assert(state->qprep.terminationtype==-3||state->qprep.terminationtype==-5, "MinLM: unexpected completion code from QP solver", _state);
        if( state->qprep.terminationtype==-3 )
        {
            iflag = -3;
            goto lbl_48;
        }
        bflag = ae_false;
    }
    if( !bflag )
    {
        
        /*
         * Solution failed:
         * try to increase lambda to make matrix positive definite and continue.
         */
        if( !minlm_increaselambda(&state->lambdav, &state->nu, _state) )
        {
            iflag = -1;
            goto lbl_48;
        }
        goto lbl_47;
    }
    
    /*
     * Step in State.XDir and it is bounded by StpMax.
     *
     * We should check stopping conditions on step size here.
     * DeltaX, which is used for secant updates, is initialized here.
     *
     * This code is a bit tricky because sometimes XDir<>0, but
     * it is so small that XDir+XBase==XBase (in finite precision
     * arithmetics). So we set DeltaX to XBase, then
     * add XDir, and then subtract XBase to get exact value of
     * DeltaX.
     *
     * Step length is estimated using DeltaX.
     *
     * NOTE: stopping conditions are tested
     * for fresh models only (ModelAge=0)
     */
    ae_v_move(&state->deltax.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_add(&state->deltax.ptr.p_double[0], 1, &state->xdir.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_sub(&state->deltax.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->deltaxready = ae_true;
    v = 0.0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->deltax.ptr.p_double[i]/state->s.ptr.p_double[i], _state);
    }
    v = ae_sqrt(v, _state);
    if( ae_fp_greater(v,state->epsx) )
    {
        goto lbl_49;
    }
    if( state->modelage!=0 )
    {
        goto lbl_51;
    }
    
    /*
     * Step is too short, model is fresh and we can rely on it.
     * Terminating.
     */
    state->repterminationtype = 2;
    if( !state->xrep )
    {
        goto lbl_53;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fbase;
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    state->xupdated = ae_false;
lbl_53:
    result = ae_false;
    return result;
    goto lbl_52;
lbl_51:
    
    /*
     * Step is suspiciously short, but model is not fresh
     * and we can't rely on it.
     */
    iflag = -2;
    goto lbl_48;
lbl_52:
lbl_49:
    
    /*
     * Let's evaluate new step:
     * a) if we have Fi vector, we evaluate it using rcomm, and
     *    then we manually calculate State.F as sum of squares of Fi[]
     * b) if we have F value, we just evaluate it through rcomm interface
     *
     * We prefer (a) because we may need Fi vector for additional
     * iterations
     */
    ae_assert(state->hasfi||state->hasf, "MinLM: internal error 2!", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_add(&state->x.ptr.p_double[0], 1, &state->xdir.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    if( !state->hasfi )
    {
        goto lbl_55;
    }
    state->needfi = ae_true;
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->needfi = ae_false;
    v = ae_v_dotproduct(&state->fi.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    state->f = v;
    ae_v_move(&state->deltaf.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    ae_v_sub(&state->deltaf.ptr.p_double[0], 1, &state->fibase.ptr.p_double[0], 1, ae_v_len(0,m-1));
    state->deltafready = ae_true;
    goto lbl_56;
lbl_55:
    state->needf = ae_true;
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->needf = ae_false;
lbl_56:
    state->repnfunc = state->repnfunc+1;
    if( ae_fp_greater_eq(state->f,state->fbase) )
    {
        
        /*
         * Increase lambda and continue
         */
        if( !minlm_increaselambda(&state->lambdav, &state->nu, _state) )
        {
            iflag = -1;
            goto lbl_48;
        }
        goto lbl_47;
    }
    
    /*
     * We've found our step!
     */
    iflag = 0;
    goto lbl_48;
    goto lbl_47;
lbl_48:
    state->nu = 1;
    ae_assert(iflag>=-3&&iflag<=0, "MinLM: internal integrity check failed!", _state);
    if( iflag==-3 )
    {
        state->repterminationtype = -3;
        result = ae_false;
        return result;
    }
    if( iflag==-2 )
    {
        state->modelage = state->maxmodelage+1;
        goto lbl_28;
    }
    if( iflag==-1 )
    {
        goto lbl_29;
    }
    
    /*
     * Levenberg-Marquardt step is ready.
     * Compare predicted vs. actual decrease and decide what to do with lambda.
     *
     * NOTE: we expect that State.DeltaX contains direction of step,
     * State.F contains function value at new point.
     */
    ae_assert(state->deltaxready, "MinLM: deltaX is not ready", _state);
    t = 0;
    for(i=0; i<=n-1; i++)
    {
        v = ae_v_dotproduct(&state->quadraticmodel.ptr.pp_double[i][0], 1, &state->deltax.ptr.p_double[0], 1, ae_v_len(0,n-1));
        t = t+state->deltax.ptr.p_double[i]*state->gbase.ptr.p_double[i]+0.5*state->deltax.ptr.p_double[i]*v;
    }
    state->predicteddecrease = -t;
    state->actualdecrease = -(state->f-state->fbase);
    if( ae_fp_less_eq(state->predicteddecrease,0) )
    {
        goto lbl_29;
    }
    v = state->actualdecrease/state->predicteddecrease;
    if( ae_fp_greater_eq(v,0.1) )
    {
        goto lbl_57;
    }
    if( minlm_increaselambda(&state->lambdav, &state->nu, _state) )
    {
        goto lbl_59;
    }
    
    /*
     * Lambda is too large, we have to break iterations.
     */
    state->repterminationtype = 7;
    if( !state->xrep )
    {
        goto lbl_61;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fbase;
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->xupdated = ae_false;
lbl_61:
    result = ae_false;
    return result;
lbl_59:
lbl_57:
    if( ae_fp_greater(v,0.5) )
    {
        minlm_decreaselambda(&state->lambdav, &state->nu, _state);
    }
    
    /*
     * Accept step, report it and
     * test stopping conditions on iterations count and function decrease.
     *
     * NOTE: we expect that State.DeltaX contains direction of step,
     * State.F contains function value at new point.
     *
     * NOTE2: we should update XBase ONLY. In the beginning of the next
     * iteration we expect that State.FIBase is NOT updated and
     * contains old value of a function vector.
     */
    ae_v_add(&state->xbase.ptr.p_double[0], 1, &state->deltax.ptr.p_double[0], 1, ae_v_len(0,n-1));
    if( !state->xrep )
    {
        goto lbl_63;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 16;
    goto lbl_rcomm;
lbl_16:
    state->xupdated = ae_false;
lbl_63:
    state->repiterationscount = state->repiterationscount+1;
    if( state->repiterationscount>=state->maxits&&state->maxits>0 )
    {
        state->repterminationtype = 5;
    }
    if( state->modelage==0 )
    {
        if( ae_fp_less_eq(ae_fabs(state->f-state->fbase, _state),state->epsf*ae_maxreal(1, ae_maxreal(ae_fabs(state->f, _state), ae_fabs(state->fbase, _state), _state), _state)) )
        {
            state->repterminationtype = 1;
        }
    }
    if( state->repterminationtype<=0 )
    {
        goto lbl_65;
    }
    if( !state->xrep )
    {
        goto lbl_67;
    }
    
    /*
     * Report: XBase contains new point, F contains function value at new point
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 17;
    goto lbl_rcomm;
lbl_17:
    state->xupdated = ae_false;
lbl_67:
    result = ae_false;
    return result;
lbl_65:
    state->modelage = state->modelage+1;
    goto lbl_28;
lbl_29:
    
    /*
     * Lambda is too large, we have to break iterations.
     */
    state->repterminationtype = 7;
    if( !state->xrep )
    {
        goto lbl_69;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fbase;
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 18;
    goto lbl_rcomm;
lbl_18:
    state->xupdated = ae_false;
lbl_69:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = m;
    state->rstate.ia.ptr.p_int[2] = iflag;
    state->rstate.ia.ptr.p_int[3] = i;
    state->rstate.ia.ptr.p_int[4] = k;
    state->rstate.ba.ptr.p_bool[0] = bflag;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = s;
    state->rstate.ra.ptr.p_double[2] = t;
    return result;
}


/*************************************************************************
Levenberg-Marquardt algorithm results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report;
                see comments for this structure for more info.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresults(minlmstate* state,
     /* Real    */ ae_vector* x,
     minlmreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _minlmreport_clear(rep);

    minlmresultsbuf(state, x, rep, _state);
}


/*************************************************************************
Levenberg-Marquardt algorithm results

Buffered implementation of MinLMResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresultsbuf(minlmstate* state,
     /* Real    */ ae_vector* x,
     minlmreport* rep,
     ae_state *_state)
{


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->terminationtype = state->repterminationtype;
    rep->funcidx = state->repfuncidx;
    rep->varidx = state->repvaridx;
    rep->nfunc = state->repnfunc;
    rep->njac = state->repnjac;
    rep->ngrad = state->repngrad;
    rep->nhess = state->repnhess;
    rep->ncholesky = state->repncholesky;
}


/*************************************************************************
This  subroutine  restarts  LM  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used for reverse communication previously
                allocated with MinLMCreateXXX call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minlmrestartfrom(minlmstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{


    ae_assert(x->cnt>=state->n, "MinLMRestartFrom: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, state->n, _state), "MinLMRestartFrom: X contains infinite or NaN values!", _state);
    ae_v_move(&state->xbase.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_vector_set_length(&state->rstate.ia, 4+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
    minlm_clearrequestfields(state, _state);
}


/*************************************************************************
This is obsolete function.

Since ALGLIB 3.3 it is equivalent to MinLMCreateVJ().

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevgj(ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x,
     minlmstate* state,
     ae_state *_state)
{

    _minlmstate_clear(state);

    minlmcreatevj(n, m, x, state, _state);
}


/*************************************************************************
This is obsolete function.

Since ALGLIB 3.3 it is equivalent to MinLMCreateFJ().

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefgj(ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x,
     minlmstate* state,
     ae_state *_state)
{

    _minlmstate_clear(state);

    minlmcreatefj(n, m, x, state, _state);
}


/*************************************************************************
This function is considered obsolete since ALGLIB 3.1.0 and is present for
backward  compatibility  only.  We  recommend  to use MinLMCreateVJ, which
provides similar, but more consistent and feature-rich interface.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefj(ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x,
     minlmstate* state,
     ae_state *_state)
{

    _minlmstate_clear(state);

    ae_assert(n>=1, "MinLMCreateFJ: N<1!", _state);
    ae_assert(m>=1, "MinLMCreateFJ: M<1!", _state);
    ae_assert(x->cnt>=n, "MinLMCreateFJ: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinLMCreateFJ: X contains infinite or NaN values!", _state);
    
    /*
     * initialize
     */
    state->teststep = 0;
    state->n = n;
    state->m = m;
    state->algomode = 1;
    state->hasf = ae_true;
    state->hasfi = ae_false;
    state->hasg = ae_false;
    
    /*
     * init 2
     */
    minlm_lmprepare(n, m, ae_true, state, _state);
    minlmsetacctype(state, 0, _state);
    minlmsetcond(state, 0, 0, 0, 0, _state);
    minlmsetxrep(state, ae_false, _state);
    minlmsetstpmax(state, 0, _state);
    minlmrestartfrom(state, x, _state);
}


/*************************************************************************
This  subroutine  turns  on  verification  of  the  user-supplied analytic
gradient:
* user calls this subroutine before optimization begins
* MinLMOptimize() is called
* prior to actual optimization, for  each  function Fi and each  component
  of parameters  being  optimized X[j] algorithm performs following steps:
  * two trial steps are made to X[j]-TestStep*S[j] and X[j]+TestStep*S[j],
    where X[j] is j-th parameter and S[j] is a scale of j-th parameter
  * if needed, steps are bounded with respect to constraints on X[]
  * Fi(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point
  * in case difference between prediction and actual value is higher  than
    some predetermined threshold, algorithm stops with completion code -7;
    Rep.VarIdx is set to index of the parameter with incorrect derivative,
    Rep.FuncIdx is set to index of the function.
* after verification is over, algorithm proceeds to the actual optimization.

NOTE 1: verification  needs  N (parameters count) Jacobian evaluations. It
        is  very  costly  and  you  should use it only for low dimensional
        problems,  when  you  want  to  be  sure  that  you've   correctly
        calculated  analytic  derivatives.  You should not  use  it in the
        production code  (unless  you  want  to check derivatives provided
        by some third party).

NOTE 2: you  should  carefully  choose  TestStep. Value which is too large
        (so large that function behaviour is significantly non-cubic) will
        lead to false alarms. You may use  different  step  for  different
        parameters by means of setting scale with MinLMSetScale().

NOTE 3: this function may lead to false positives. In case it reports that
        I-th  derivative was calculated incorrectly, you may decrease test
        step  and  try  one  more  time  - maybe your function changes too
        sharply  and  your  step  is  too  large for such rapidly chanding
        function.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification

  -- ALGLIB --
     Copyright 15.06.2012 by Bochkanov Sergey
*************************************************************************/
void minlmsetgradientcheck(minlmstate* state,
     double teststep,
     ae_state *_state)
{


    ae_assert(ae_isfinite(teststep, _state), "MinLMSetGradientCheck: TestStep contains NaN or Infinite", _state);
    ae_assert(ae_fp_greater_eq(teststep,0), "MinLMSetGradientCheck: invalid argument TestStep(TestStep<0)", _state);
    state->teststep = teststep;
}


/*************************************************************************
Prepare internal structures (except for RComm).

Note: M must be zero for FGH mode, non-zero for V/VJ/FJ/FGJ mode.
*************************************************************************/
static void minlm_lmprepare(ae_int_t n,
     ae_int_t m,
     ae_bool havegrad,
     minlmstate* state,
     ae_state *_state)
{
    ae_int_t i;


    if( n<=0||m<0 )
    {
        return;
    }
    if( havegrad )
    {
        ae_vector_set_length(&state->g, n, _state);
    }
    if( m!=0 )
    {
        ae_matrix_set_length(&state->j, m, n, _state);
        ae_vector_set_length(&state->fi, m, _state);
        ae_vector_set_length(&state->fibase, m, _state);
        ae_vector_set_length(&state->deltaf, m, _state);
        ae_vector_set_length(&state->fm1, m, _state);
        ae_vector_set_length(&state->fp1, m, _state);
        ae_vector_set_length(&state->fc1, m, _state);
        ae_vector_set_length(&state->gm1, m, _state);
        ae_vector_set_length(&state->gp1, m, _state);
        ae_vector_set_length(&state->gc1, m, _state);
    }
    else
    {
        ae_matrix_set_length(&state->h, n, n, _state);
    }
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->deltax, n, _state);
    ae_matrix_set_length(&state->quadraticmodel, n, n, _state);
    ae_vector_set_length(&state->xbase, n, _state);
    ae_vector_set_length(&state->gbase, n, _state);
    ae_vector_set_length(&state->xdir, n, _state);
    ae_vector_set_length(&state->tmp0, n, _state);
    
    /*
     * prepare internal L-BFGS
     */
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = 0;
    }
    minlbfgscreate(n, ae_minint(minlm_additers, n, _state), &state->x, &state->internalstate, _state);
    minlbfgssetcond(&state->internalstate, 0.0, 0.0, 0.0, ae_minint(minlm_additers, n, _state), _state);
    
    /*
     * Prepare internal QP solver
     */
    minqpcreate(n, &state->qpstate, _state);
    minqpsetalgocholesky(&state->qpstate, _state);
    
    /*
     * Prepare boundary constraints
     */
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->havebndl, n, _state);
    ae_vector_set_length(&state->havebndu, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->havebndl.ptr.p_bool[i] = ae_false;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
        state->havebndu.ptr.p_bool[i] = ae_false;
    }
    
    /*
     * Prepare scaling matrix
     */
    ae_vector_set_length(&state->s, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->s.ptr.p_double[i] = 1.0;
    }
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void minlm_clearrequestfields(minlmstate* state, ae_state *_state)
{


    state->needf = ae_false;
    state->needfg = ae_false;
    state->needfgh = ae_false;
    state->needfij = ae_false;
    state->needfi = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Increases lambda, returns False when there is a danger of overflow
*************************************************************************/
static ae_bool minlm_increaselambda(double* lambdav,
     double* nu,
     ae_state *_state)
{
    double lnlambda;
    double lnnu;
    double lnlambdaup;
    double lnmax;
    ae_bool result;


    result = ae_false;
    lnlambda = ae_log(*lambdav, _state);
    lnlambdaup = ae_log(minlm_lambdaup, _state);
    lnnu = ae_log(*nu, _state);
    lnmax = ae_log(ae_maxrealnumber, _state);
    if( ae_fp_greater(lnlambda+lnlambdaup+lnnu,0.25*lnmax) )
    {
        return result;
    }
    if( ae_fp_greater(lnnu+ae_log(2, _state),lnmax) )
    {
        return result;
    }
    *lambdav = *lambdav*minlm_lambdaup*(*nu);
    *nu = *nu*2;
    result = ae_true;
    return result;
}


/*************************************************************************
Decreases lambda, but leaves it unchanged when there is danger of underflow.
*************************************************************************/
static void minlm_decreaselambda(double* lambdav,
     double* nu,
     ae_state *_state)
{


    *nu = 1;
    if( ae_fp_less(ae_log(*lambdav, _state)+ae_log(minlm_lambdadown, _state),ae_log(ae_minrealnumber, _state)) )
    {
        *lambdav = ae_minrealnumber;
    }
    else
    {
        *lambdav = *lambdav*minlm_lambdadown;
    }
}


/*************************************************************************
Returns norm of bounded scaled anti-gradient.

Bounded antigradient is a vector obtained from  anti-gradient  by  zeroing
components which point outwards:
    result = norm(v)
    v[i]=0     if ((-g[i]<0)and(x[i]=bndl[i])) or
                  ((-g[i]>0)and(x[i]=bndu[i]))
    v[i]=-g[i]*s[i] otherwise, where s[i] is a scale for I-th variable

This function may be used to check a stopping criterion.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
static double minlm_boundedscaledantigradnorm(minlmstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* g,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double v;
    double result;


    result = 0;
    n = state->n;
    for(i=0; i<=n-1; i++)
    {
        v = -g->ptr.p_double[i]*state->s.ptr.p_double[i];
        if( state->havebndl.ptr.p_bool[i] )
        {
            if( ae_fp_less_eq(x->ptr.p_double[i],state->bndl.ptr.p_double[i])&&ae_fp_less(-g->ptr.p_double[i],0) )
            {
                v = 0;
            }
        }
        if( state->havebndu.ptr.p_bool[i] )
        {
            if( ae_fp_greater_eq(x->ptr.p_double[i],state->bndu.ptr.p_double[i])&&ae_fp_greater(-g->ptr.p_double[i],0) )
            {
                v = 0;
            }
        }
        result = result+ae_sqr(v, _state);
    }
    result = ae_sqrt(result, _state);
    return result;
}


ae_bool _minlmstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minlmstate *p = (minlmstate*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->h, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xbase, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->fibase, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->gbase, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init(&p->quadraticmodel, 0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->havebndl, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->havebndu, 0, DT_BOOL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xdir, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->deltax, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->deltaf, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init(&p->rstate, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->choleskybuf, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->fm1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->fp1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->fc1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->gm1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->gp1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->gc1, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_minlbfgsstate_init(&p->internalstate, _state, make_automatic) )
        return ae_false;
    if( !_minlbfgsreport_init(&p->internalrep, _state, make_automatic) )
        return ae_false;
    if( !_minqpstate_init(&p->qpstate, _state, make_automatic) )
        return ae_false;
    if( !_minqpreport_init(&p->qprep, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _minlmstate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minlmstate *dst = (minlmstate*)_dst;
    minlmstate *src = (minlmstate*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->diffstep = src->diffstep;
    dst->epsg = src->epsg;
    dst->epsf = src->epsf;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->stpmax = src->stpmax;
    dst->maxmodelage = src->maxmodelage;
    dst->makeadditers = src->makeadditers;
    if( !ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic) )
        return ae_false;
    dst->f = src->f;
    if( !ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->h, &src->h, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic) )
        return ae_false;
    dst->needf = src->needf;
    dst->needfg = src->needfg;
    dst->needfgh = src->needfgh;
    dst->needfij = src->needfij;
    dst->needfi = src->needfi;
    dst->xupdated = src->xupdated;
    dst->algomode = src->algomode;
    dst->hasf = src->hasf;
    dst->hasfi = src->hasfi;
    dst->hasg = src->hasg;
    if( !ae_vector_init_copy(&dst->xbase, &src->xbase, _state, make_automatic) )
        return ae_false;
    dst->fbase = src->fbase;
    if( !ae_vector_init_copy(&dst->fibase, &src->fibase, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->gbase, &src->gbase, _state, make_automatic) )
        return ae_false;
    if( !ae_matrix_init_copy(&dst->quadraticmodel, &src->quadraticmodel, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->havebndl, &src->havebndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->havebndu, &src->havebndu, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic) )
        return ae_false;
    dst->lambdav = src->lambdav;
    dst->nu = src->nu;
    dst->modelage = src->modelage;
    if( !ae_vector_init_copy(&dst->xdir, &src->xdir, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->deltax, &src->deltax, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->deltaf, &src->deltaf, _state, make_automatic) )
        return ae_false;
    dst->deltaxready = src->deltaxready;
    dst->deltafready = src->deltafready;
    dst->teststep = src->teststep;
    dst->repiterationscount = src->repiterationscount;
    dst->repterminationtype = src->repterminationtype;
    dst->repfuncidx = src->repfuncidx;
    dst->repvaridx = src->repvaridx;
    dst->repnfunc = src->repnfunc;
    dst->repnjac = src->repnjac;
    dst->repngrad = src->repngrad;
    dst->repnhess = src->repnhess;
    dst->repncholesky = src->repncholesky;
    if( !_rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->choleskybuf, &src->choleskybuf, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic) )
        return ae_false;
    dst->actualdecrease = src->actualdecrease;
    dst->predicteddecrease = src->predicteddecrease;
    dst->xm1 = src->xm1;
    dst->xp1 = src->xp1;
    if( !ae_vector_init_copy(&dst->fm1, &src->fm1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->fp1, &src->fp1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->fc1, &src->fc1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->gm1, &src->gm1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->gp1, &src->gp1, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->gc1, &src->gc1, _state, make_automatic) )
        return ae_false;
    if( !_minlbfgsstate_init_copy(&dst->internalstate, &src->internalstate, _state, make_automatic) )
        return ae_false;
    if( !_minlbfgsreport_init_copy(&dst->internalrep, &src->internalrep, _state, make_automatic) )
        return ae_false;
    if( !_minqpstate_init_copy(&dst->qpstate, &src->qpstate, _state, make_automatic) )
        return ae_false;
    if( !_minqpreport_init_copy(&dst->qprep, &src->qprep, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _minlmstate_clear(void* _p)
{
    minlmstate *p = (minlmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    ae_matrix_clear(&p->h);
    ae_vector_clear(&p->g);
    ae_vector_clear(&p->xbase);
    ae_vector_clear(&p->fibase);
    ae_vector_clear(&p->gbase);
    ae_matrix_clear(&p->quadraticmodel);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->havebndl);
    ae_vector_clear(&p->havebndu);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->xdir);
    ae_vector_clear(&p->deltax);
    ae_vector_clear(&p->deltaf);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->choleskybuf);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->fm1);
    ae_vector_clear(&p->fp1);
    ae_vector_clear(&p->fc1);
    ae_vector_clear(&p->gm1);
    ae_vector_clear(&p->gp1);
    ae_vector_clear(&p->gc1);
    _minlbfgsstate_clear(&p->internalstate);
    _minlbfgsreport_clear(&p->internalrep);
    _minqpstate_clear(&p->qpstate);
    _minqpreport_clear(&p->qprep);
}


void _minlmstate_destroy(void* _p)
{
    minlmstate *p = (minlmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    ae_matrix_destroy(&p->h);
    ae_vector_destroy(&p->g);
    ae_vector_destroy(&p->xbase);
    ae_vector_destroy(&p->fibase);
    ae_vector_destroy(&p->gbase);
    ae_matrix_destroy(&p->quadraticmodel);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->havebndl);
    ae_vector_destroy(&p->havebndu);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->xdir);
    ae_vector_destroy(&p->deltax);
    ae_vector_destroy(&p->deltaf);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->choleskybuf);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->fm1);
    ae_vector_destroy(&p->fp1);
    ae_vector_destroy(&p->fc1);
    ae_vector_destroy(&p->gm1);
    ae_vector_destroy(&p->gp1);
    ae_vector_destroy(&p->gc1);
    _minlbfgsstate_destroy(&p->internalstate);
    _minlbfgsreport_destroy(&p->internalrep);
    _minqpstate_destroy(&p->qpstate);
    _minqpreport_destroy(&p->qprep);
}


ae_bool _minlmreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minlmreport *p = (minlmreport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _minlmreport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minlmreport *dst = (minlmreport*)_dst;
    minlmreport *src = (minlmreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->terminationtype = src->terminationtype;
    dst->funcidx = src->funcidx;
    dst->varidx = src->varidx;
    dst->nfunc = src->nfunc;
    dst->njac = src->njac;
    dst->ngrad = src->ngrad;
    dst->nhess = src->nhess;
    dst->ncholesky = src->ncholesky;
    return ae_true;
}


void _minlmreport_clear(void* _p)
{
    minlmreport *p = (minlmreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minlmreport_destroy(void* _p)
{
    minlmreport *p = (minlmreport*)_p;
    ae_touch_ptr((void*)p);
}




/*************************************************************************
Obsolete function, use MinLBFGSSetPrecDefault() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetdefaultpreconditioner(minlbfgsstate* state,
     ae_state *_state)
{


    minlbfgssetprecdefault(state, _state);
}


/*************************************************************************
Obsolete function, use MinLBFGSSetCholeskyPreconditioner() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetcholeskypreconditioner(minlbfgsstate* state,
     /* Real    */ ae_matrix* p,
     ae_bool isupper,
     ae_state *_state)
{


    minlbfgssetpreccholesky(state, p, isupper, _state);
}


/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierwidth(minbleicstate* state,
     double mu,
     ae_state *_state)
{


}


/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierdecay(minbleicstate* state,
     double mudecay,
     ae_state *_state)
{


}


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void minasacreate(ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     minasastate* state,
     ae_state *_state)
{
    ae_int_t i;

    _minasastate_clear(state);

    ae_assert(n>=1, "MinASA: N too small!", _state);
    ae_assert(x->cnt>=n, "MinCGCreate: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinCGCreate: X contains infinite or NaN values!", _state);
    ae_assert(bndl->cnt>=n, "MinCGCreate: Length(BndL)<N!", _state);
    ae_assert(isfinitevector(bndl, n, _state), "MinCGCreate: BndL contains infinite or NaN values!", _state);
    ae_assert(bndu->cnt>=n, "MinCGCreate: Length(BndU)<N!", _state);
    ae_assert(isfinitevector(bndu, n, _state), "MinCGCreate: BndU contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_fp_less_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]), "MinASA: inconsistent bounds!", _state);
        ae_assert(ae_fp_less_eq(bndl->ptr.p_double[i],x->ptr.p_double[i]), "MinASA: infeasible X!", _state);
        ae_assert(ae_fp_less_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]), "MinASA: infeasible X!", _state);
    }
    
    /*
     * Initialize
     */
    state->n = n;
    minasasetcond(state, 0, 0, 0, 0, _state);
    minasasetxrep(state, ae_false, _state);
    minasasetstpmax(state, 0, _state);
    minasasetalgorithm(state, -1, _state);
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->ak, n, _state);
    ae_vector_set_length(&state->xk, n, _state);
    ae_vector_set_length(&state->dk, n, _state);
    ae_vector_set_length(&state->an, n, _state);
    ae_vector_set_length(&state->xn, n, _state);
    ae_vector_set_length(&state->dn, n, _state);
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->d, n, _state);
    ae_vector_set_length(&state->g, n, _state);
    ae_vector_set_length(&state->gc, n, _state);
    ae_vector_set_length(&state->work, n, _state);
    ae_vector_set_length(&state->yk, n, _state);
    minasarestartfrom(state, x, bndl, bndu, _state);
}


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetcond(minasastate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinASASetCond: EpsG is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsg,0), "MinASASetCond: negative EpsG!", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinASASetCond: EpsF is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsf,0), "MinASASetCond: negative EpsF!", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinASASetCond: EpsX is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsx,0), "MinASASetCond: negative EpsX!", _state);
    ae_assert(maxits>=0, "MinASASetCond: negative MaxIts!", _state);
    if( ((ae_fp_eq(epsg,0)&&ae_fp_eq(epsf,0))&&ae_fp_eq(epsx,0))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    state->epsg = epsg;
    state->epsf = epsf;
    state->epsx = epsx;
    state->maxits = maxits;
}


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetxrep(minasastate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetalgorithm(minasastate* state,
     ae_int_t algotype,
     ae_state *_state)
{


    ae_assert(algotype>=-1&&algotype<=1, "MinASASetAlgorithm: incorrect AlgoType!", _state);
    if( algotype==-1 )
    {
        algotype = 1;
    }
    state->cgtype = algotype;
}


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetstpmax(minasastate* state, double stpmax, ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "MinASASetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,0), "MinASASetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool minasaiteration(minasastate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double betak;
    double v;
    double vv;
    ae_int_t mcinfo;
    ae_bool b;
    ae_bool stepfound;
    ae_int_t diffcnt;
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
        i = state->rstate.ia.ptr.p_int[1];
        mcinfo = state->rstate.ia.ptr.p_int[2];
        diffcnt = state->rstate.ia.ptr.p_int[3];
        b = state->rstate.ba.ptr.p_bool[0];
        stepfound = state->rstate.ba.ptr.p_bool[1];
        betak = state->rstate.ra.ptr.p_double[0];
        v = state->rstate.ra.ptr.p_double[1];
        vv = state->rstate.ra.ptr.p_double[2];
    }
    else
    {
        n = -983;
        i = -989;
        mcinfo = -834;
        diffcnt = 900;
        b = ae_true;
        stepfound = ae_false;
        betak = 214;
        v = -338;
        vv = -686;
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
    if( state->rstate.stage==8 )
    {
        goto lbl_8;
    }
    if( state->rstate.stage==9 )
    {
        goto lbl_9;
    }
    if( state->rstate.stage==10 )
    {
        goto lbl_10;
    }
    if( state->rstate.stage==11 )
    {
        goto lbl_11;
    }
    if( state->rstate.stage==12 )
    {
        goto lbl_12;
    }
    if( state->rstate.stage==13 )
    {
        goto lbl_13;
    }
    if( state->rstate.stage==14 )
    {
        goto lbl_14;
    }
    
    /*
     * Routine body
     */
    
    /*
     * Prepare
     */
    n = state->n;
    state->repterminationtype = 0;
    state->repiterationscount = 0;
    state->repnfev = 0;
    state->debugrestartscount = 0;
    state->cgtype = 1;
    ae_v_move(&state->xk.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(state->xk.ptr.p_double[i],state->bndl.ptr.p_double[i])||ae_fp_eq(state->xk.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            state->ak.ptr.p_double[i] = 0;
        }
        else
        {
            state->ak.ptr.p_double[i] = 1;
        }
    }
    state->mu = 0.1;
    state->curalgo = 0;
    
    /*
     * Calculate F/G, initialize algorithm
     */
    mincomp_clearrequestfields(state, _state);
    state->needfg = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfg = ae_false;
    if( !state->xrep )
    {
        goto lbl_15;
    }
    
    /*
     * progress report
     */
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
lbl_15:
    if( ae_fp_less_eq(mincomp_asaboundedantigradnorm(state, _state),state->epsg) )
    {
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    state->repnfev = state->repnfev+1;
    
    /*
     * Main cycle
     *
     * At the beginning of new iteration:
     * * CurAlgo stores current algorithm selector
     * * State.XK, State.F and State.G store current X/F/G
     * * State.AK stores current set of active constraints
     */
lbl_17:
    if( ae_false )
    {
        goto lbl_18;
    }
    
    /*
     * GPA algorithm
     */
    if( state->curalgo!=0 )
    {
        goto lbl_19;
    }
    state->k = 0;
    state->acount = 0;
lbl_21:
    if( ae_false )
    {
        goto lbl_22;
    }
    
    /*
     * Determine Dk = proj(xk - gk)-xk
     */
    for(i=0; i<=n-1; i++)
    {
        state->d.ptr.p_double[i] = boundval(state->xk.ptr.p_double[i]-state->g.ptr.p_double[i], state->bndl.ptr.p_double[i], state->bndu.ptr.p_double[i], _state)-state->xk.ptr.p_double[i];
    }
    
    /*
     * Armijo line search.
     * * exact search with alpha=1 is tried first,
     *   'exact' means that we evaluate f() EXACTLY at
     *   bound(x-g,bndl,bndu), without intermediate floating
     *   point operations.
     * * alpha<1 are tried if explicit search wasn't successful
     * Result is placed into XN.
     *
     * Two types of search are needed because we can't
     * just use second type with alpha=1 because in finite
     * precision arithmetics (x1-x0)+x0 may differ from x1.
     * So while x1 is correctly bounded (it lie EXACTLY on
     * boundary, if it is active), (x1-x0)+x0 may be
     * not bounded.
     */
    v = ae_v_dotproduct(&state->d.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->dginit = v;
    state->finit = state->f;
    if( !(ae_fp_less_eq(mincomp_asad1norm(state, _state),state->stpmax)||ae_fp_eq(state->stpmax,0)) )
    {
        goto lbl_23;
    }
    
    /*
     * Try alpha=1 step first
     */
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = boundval(state->xk.ptr.p_double[i]-state->g.ptr.p_double[i], state->bndl.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
    }
    mincomp_clearrequestfields(state, _state);
    state->needfg = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfg = ae_false;
    state->repnfev = state->repnfev+1;
    stepfound = ae_fp_less_eq(state->f,state->finit+mincomp_gpaftol*state->dginit);
    goto lbl_24;
lbl_23:
    stepfound = ae_false;
lbl_24:
    if( !stepfound )
    {
        goto lbl_25;
    }
    
    /*
     * we are at the boundary(ies)
     */
    ae_v_move(&state->xn.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->stp = 1;
    goto lbl_26;
lbl_25:
    
    /*
     * alpha=1 is too large, try smaller values
     */
    state->stp = 1;
    linminnormalized(&state->d, &state->stp, n, _state);
    state->dginit = state->dginit/state->stp;
    state->stp = mincomp_gpadecay*state->stp;
    if( ae_fp_greater(state->stpmax,0) )
    {
        state->stp = ae_minreal(state->stp, state->stpmax, _state);
    }
lbl_27:
    if( ae_false )
    {
        goto lbl_28;
    }
    v = state->stp;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xk.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_addd(&state->x.ptr.p_double[0], 1, &state->d.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
    mincomp_clearrequestfields(state, _state);
    state->needfg = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needfg = ae_false;
    state->repnfev = state->repnfev+1;
    if( ae_fp_less_eq(state->stp,mincomp_stpmin) )
    {
        goto lbl_28;
    }
    if( ae_fp_less_eq(state->f,state->finit+state->stp*mincomp_gpaftol*state->dginit) )
    {
        goto lbl_28;
    }
    state->stp = state->stp*mincomp_gpadecay;
    goto lbl_27;
lbl_28:
    ae_v_move(&state->xn.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
lbl_26:
    state->repiterationscount = state->repiterationscount+1;
    if( !state->xrep )
    {
        goto lbl_29;
    }
    
    /*
     * progress report
     */
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->xupdated = ae_false;
lbl_29:
    
    /*
     * Calculate new set of active constraints.
     * Reset counter if active set was changed.
     * Prepare for the new iteration
     */
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(state->xn.ptr.p_double[i],state->bndl.ptr.p_double[i])||ae_fp_eq(state->xn.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            state->an.ptr.p_double[i] = 0;
        }
        else
        {
            state->an.ptr.p_double[i] = 1;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(state->ak.ptr.p_double[i],state->an.ptr.p_double[i]) )
        {
            state->acount = -1;
            break;
        }
    }
    state->acount = state->acount+1;
    ae_v_move(&state->xk.ptr.p_double[0], 1, &state->xn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->ak.ptr.p_double[0], 1, &state->an.ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * Stopping conditions
     */
    if( !(state->repiterationscount>=state->maxits&&state->maxits>0) )
    {
        goto lbl_31;
    }
    
    /*
     * Too many iterations
     */
    state->repterminationtype = 5;
    if( !state->xrep )
    {
        goto lbl_33;
    }
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->xupdated = ae_false;
lbl_33:
    result = ae_false;
    return result;
lbl_31:
    if( ae_fp_greater(mincomp_asaboundedantigradnorm(state, _state),state->epsg) )
    {
        goto lbl_35;
    }
    
    /*
     * Gradient is small enough
     */
    state->repterminationtype = 4;
    if( !state->xrep )
    {
        goto lbl_37;
    }
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->xupdated = ae_false;
lbl_37:
    result = ae_false;
    return result;
lbl_35:
    v = ae_v_dotproduct(&state->d.ptr.p_double[0], 1, &state->d.ptr.p_double[0], 1, ae_v_len(0,n-1));
    if( ae_fp_greater(ae_sqrt(v, _state)*state->stp,state->epsx) )
    {
        goto lbl_39;
    }
    
    /*
     * Step size is too small, no further improvement is
     * possible
     */
    state->repterminationtype = 2;
    if( !state->xrep )
    {
        goto lbl_41;
    }
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->xupdated = ae_false;
lbl_41:
    result = ae_false;
    return result;
lbl_39:
    if( ae_fp_greater(state->finit-state->f,state->epsf*ae_maxreal(ae_fabs(state->finit, _state), ae_maxreal(ae_fabs(state->f, _state), 1.0, _state), _state)) )
    {
        goto lbl_43;
    }
    
    /*
     * F(k+1)-F(k) is small enough
     */
    state->repterminationtype = 1;
    if( !state->xrep )
    {
        goto lbl_45;
    }
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->xupdated = ae_false;
lbl_45:
    result = ae_false;
    return result;
lbl_43:
    
    /*
     * Decide - should we switch algorithm or not
     */
    if( mincomp_asauisempty(state, _state) )
    {
        if( ae_fp_greater_eq(mincomp_asaginorm(state, _state),state->mu*mincomp_asad1norm(state, _state)) )
        {
            state->curalgo = 1;
            goto lbl_22;
        }
        else
        {
            state->mu = state->mu*mincomp_asarho;
        }
    }
    else
    {
        if( state->acount==mincomp_n1 )
        {
            if( ae_fp_greater_eq(mincomp_asaginorm(state, _state),state->mu*mincomp_asad1norm(state, _state)) )
            {
                state->curalgo = 1;
                goto lbl_22;
            }
        }
    }
    
    /*
     * Next iteration
     */
    state->k = state->k+1;
    goto lbl_21;
lbl_22:
lbl_19:
    
    /*
     * CG algorithm
     */
    if( state->curalgo!=1 )
    {
        goto lbl_47;
    }
    
    /*
     * first, check that there are non-active constraints.
     * move to GPA algorithm, if all constraints are active
     */
    b = ae_true;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(state->ak.ptr.p_double[i],0) )
        {
            b = ae_false;
            break;
        }
    }
    if( b )
    {
        state->curalgo = 0;
        goto lbl_17;
    }
    
    /*
     * CG iterations
     */
    state->fold = state->f;
    ae_v_move(&state->xk.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    for(i=0; i<=n-1; i++)
    {
        state->dk.ptr.p_double[i] = -state->g.ptr.p_double[i]*state->ak.ptr.p_double[i];
        state->gc.ptr.p_double[i] = state->g.ptr.p_double[i]*state->ak.ptr.p_double[i];
    }
lbl_49:
    if( ae_false )
    {
        goto lbl_50;
    }
    
    /*
     * Store G[k] for later calculation of Y[k]
     */
    for(i=0; i<=n-1; i++)
    {
        state->yk.ptr.p_double[i] = -state->gc.ptr.p_double[i];
    }
    
    /*
     * Make a CG step in direction given by DK[]:
     * * calculate step. Step projection into feasible set
     *   is used. It has several benefits: a) step may be
     *   found with usual line search, b) multiple constraints
     *   may be activated with one step, c) activated constraints
     *   are detected in a natural way - just compare x[i] with
     *   bounds
     * * update active set, set B to True, if there
     *   were changes in the set.
     */
    ae_v_move(&state->d.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->xn.ptr.p_double[0], 1, &state->xk.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->mcstage = 0;
    state->stp = 1;
    linminnormalized(&state->d, &state->stp, n, _state);
    if( ae_fp_neq(state->laststep,0) )
    {
        state->stp = state->laststep;
    }
    mcsrch(n, &state->xn, &state->f, &state->gc, &state->d, &state->stp, state->stpmax, mincomp_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
lbl_51:
    if( state->mcstage==0 )
    {
        goto lbl_52;
    }
    
    /*
     * preprocess data: bound State.XN so it belongs to the
     * feasible set and store it in the State.X
     */
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = boundval(state->xn.ptr.p_double[i], state->bndl.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
    }
    
    /*
     * RComm
     */
    mincomp_clearrequestfields(state, _state);
    state->needfg = ae_true;
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->needfg = ae_false;
    
    /*
     * postprocess data: zero components of G corresponding to
     * the active constraints
     */
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(state->x.ptr.p_double[i],state->bndl.ptr.p_double[i])||ae_fp_eq(state->x.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            state->gc.ptr.p_double[i] = 0;
        }
        else
        {
            state->gc.ptr.p_double[i] = state->g.ptr.p_double[i];
        }
    }
    mcsrch(n, &state->xn, &state->f, &state->gc, &state->d, &state->stp, state->stpmax, mincomp_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
    goto lbl_51;
lbl_52:
    diffcnt = 0;
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * XN contains unprojected result, project it,
         * save copy to X (will be used for progress reporting)
         */
        state->xn.ptr.p_double[i] = boundval(state->xn.ptr.p_double[i], state->bndl.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
        
        /*
         * update active set
         */
        if( ae_fp_eq(state->xn.ptr.p_double[i],state->bndl.ptr.p_double[i])||ae_fp_eq(state->xn.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            state->an.ptr.p_double[i] = 0;
        }
        else
        {
            state->an.ptr.p_double[i] = 1;
        }
        if( ae_fp_neq(state->an.ptr.p_double[i],state->ak.ptr.p_double[i]) )
        {
            diffcnt = diffcnt+1;
        }
        state->ak.ptr.p_double[i] = state->an.ptr.p_double[i];
    }
    ae_v_move(&state->xk.ptr.p_double[0], 1, &state->xn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->repnfev = state->repnfev+state->nfev;
    state->repiterationscount = state->repiterationscount+1;
    if( !state->xrep )
    {
        goto lbl_53;
    }
    
    /*
     * progress report
     */
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->xupdated = ae_false;
lbl_53:
    
    /*
     * Update info about step length
     */
    v = ae_v_dotproduct(&state->d.ptr.p_double[0], 1, &state->d.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->laststep = ae_sqrt(v, _state)*state->stp;
    
    /*
     * Check stopping conditions.
     */
    if( ae_fp_greater(mincomp_asaboundedantigradnorm(state, _state),state->epsg) )
    {
        goto lbl_55;
    }
    
    /*
     * Gradient is small enough
     */
    state->repterminationtype = 4;
    if( !state->xrep )
    {
        goto lbl_57;
    }
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->xupdated = ae_false;
lbl_57:
    result = ae_false;
    return result;
lbl_55:
    if( !(state->repiterationscount>=state->maxits&&state->maxits>0) )
    {
        goto lbl_59;
    }
    
    /*
     * Too many iterations
     */
    state->repterminationtype = 5;
    if( !state->xrep )
    {
        goto lbl_61;
    }
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    state->xupdated = ae_false;
lbl_61:
    result = ae_false;
    return result;
lbl_59:
    if( !(ae_fp_greater_eq(mincomp_asaginorm(state, _state),state->mu*mincomp_asad1norm(state, _state))&&diffcnt==0) )
    {
        goto lbl_63;
    }
    
    /*
     * These conditions (EpsF/EpsX) are explicitly or implicitly
     * related to the current step size and influenced
     * by changes in the active constraints.
     *
     * For these reasons they are checked only when we don't
     * want to 'unstick' at the end of the iteration and there
     * were no changes in the active set.
     *
     * NOTE: consition |G|>=Mu*|D1| must be exactly opposite
     * to the condition used to switch back to GPA. At least
     * one inequality must be strict, otherwise infinite cycle
     * may occur when |G|=Mu*|D1| (we DON'T test stopping
     * conditions and we DON'T switch to GPA, so we cycle
     * indefinitely).
     */
    if( ae_fp_greater(state->fold-state->f,state->epsf*ae_maxreal(ae_fabs(state->fold, _state), ae_maxreal(ae_fabs(state->f, _state), 1.0, _state), _state)) )
    {
        goto lbl_65;
    }
    
    /*
     * F(k+1)-F(k) is small enough
     */
    state->repterminationtype = 1;
    if( !state->xrep )
    {
        goto lbl_67;
    }
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->xupdated = ae_false;
lbl_67:
    result = ae_false;
    return result;
lbl_65:
    if( ae_fp_greater(state->laststep,state->epsx) )
    {
        goto lbl_69;
    }
    
    /*
     * X(k+1)-X(k) is small enough
     */
    state->repterminationtype = 2;
    if( !state->xrep )
    {
        goto lbl_71;
    }
    mincomp_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->xupdated = ae_false;
lbl_71:
    result = ae_false;
    return result;
lbl_69:
lbl_63:
    
    /*
     * Check conditions for switching
     */
    if( ae_fp_less(mincomp_asaginorm(state, _state),state->mu*mincomp_asad1norm(state, _state)) )
    {
        state->curalgo = 0;
        goto lbl_50;
    }
    if( diffcnt>0 )
    {
        if( mincomp_asauisempty(state, _state)||diffcnt>=mincomp_n2 )
        {
            state->curalgo = 1;
        }
        else
        {
            state->curalgo = 0;
        }
        goto lbl_50;
    }
    
    /*
     * Calculate D(k+1)
     *
     * Line search may result in:
     * * maximum feasible step being taken (already processed)
     * * point satisfying Wolfe conditions
     * * some kind of error (CG is restarted by assigning 0.0 to Beta)
     */
    if( mcinfo==1 )
    {
        
        /*
         * Standard Wolfe conditions are satisfied:
         * * calculate Y[K] and BetaK
         */
        ae_v_add(&state->yk.ptr.p_double[0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
        vv = ae_v_dotproduct(&state->yk.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = ae_v_dotproduct(&state->gc.ptr.p_double[0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->betady = v/vv;
        v = ae_v_dotproduct(&state->gc.ptr.p_double[0], 1, &state->yk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->betahs = v/vv;
        if( state->cgtype==0 )
        {
            betak = state->betady;
        }
        if( state->cgtype==1 )
        {
            betak = ae_maxreal(0, ae_minreal(state->betady, state->betahs, _state), _state);
        }
    }
    else
    {
        
        /*
         * Something is wrong (may be function is too wild or too flat).
         *
         * We'll set BetaK=0, which will restart CG algorithm.
         * We can stop later (during normal checks) if stopping conditions are met.
         */
        betak = 0;
        state->debugrestartscount = state->debugrestartscount+1;
    }
    ae_v_moveneg(&state->dn.ptr.p_double[0], 1, &state->gc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_addd(&state->dn.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1), betak);
    ae_v_move(&state->dk.ptr.p_double[0], 1, &state->dn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * update other information
     */
    state->fold = state->f;
    state->k = state->k+1;
    goto lbl_49;
lbl_50:
lbl_47:
    goto lbl_17;
lbl_18:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = i;
    state->rstate.ia.ptr.p_int[2] = mcinfo;
    state->rstate.ia.ptr.p_int[3] = diffcnt;
    state->rstate.ba.ptr.p_bool[0] = b;
    state->rstate.ba.ptr.p_bool[1] = stepfound;
    state->rstate.ra.ptr.p_double[0] = betak;
    state->rstate.ra.ptr.p_double[1] = v;
    state->rstate.ra.ptr.p_double[2] = vv;
    return result;
}


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresults(minasastate* state,
     /* Real    */ ae_vector* x,
     minasareport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _minasareport_clear(rep);

    minasaresultsbuf(state, x, rep, _state);
}


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresultsbuf(minasastate* state,
     /* Real    */ ae_vector* x,
     minasareport* rep,
     ae_state *_state)
{
    ae_int_t i;


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nfev = state->repnfev;
    rep->terminationtype = state->repterminationtype;
    rep->activeconstraints = 0;
    for(i=0; i<=state->n-1; i++)
    {
        if( ae_fp_eq(state->ak.ptr.p_double[i],0) )
        {
            rep->activeconstraints = rep->activeconstraints+1;
        }
    }
}


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minasarestartfrom(minasastate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     ae_state *_state)
{


    ae_assert(x->cnt>=state->n, "MinASARestartFrom: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, state->n, _state), "MinASARestartFrom: X contains infinite or NaN values!", _state);
    ae_assert(bndl->cnt>=state->n, "MinASARestartFrom: Length(BndL)<N!", _state);
    ae_assert(isfinitevector(bndl, state->n, _state), "MinASARestartFrom: BndL contains infinite or NaN values!", _state);
    ae_assert(bndu->cnt>=state->n, "MinASARestartFrom: Length(BndU)<N!", _state);
    ae_assert(isfinitevector(bndu, state->n, _state), "MinASARestartFrom: BndU contains infinite or NaN values!", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_v_move(&state->bndl.ptr.p_double[0], 1, &bndl->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_v_move(&state->bndu.ptr.p_double[0], 1, &bndu->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->laststep = 0;
    ae_vector_set_length(&state->rstate.ia, 3+1, _state);
    ae_vector_set_length(&state->rstate.ba, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
    mincomp_clearrequestfields(state, _state);
}


/*************************************************************************
Returns norm of bounded anti-gradient.

Bounded antigradient is a vector obtained from  anti-gradient  by  zeroing
components which point outwards:
    result = norm(v)
    v[i]=0     if ((-g[i]<0)and(x[i]=bndl[i])) or
                  ((-g[i]>0)and(x[i]=bndu[i]))
    v[i]=-g[i] otherwise

This function may be used to check a stopping criterion.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
static double mincomp_asaboundedantigradnorm(minasastate* state,
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;


    result = 0;
    for(i=0; i<=state->n-1; i++)
    {
        v = -state->g.ptr.p_double[i];
        if( ae_fp_eq(state->x.ptr.p_double[i],state->bndl.ptr.p_double[i])&&ae_fp_less(-state->g.ptr.p_double[i],0) )
        {
            v = 0;
        }
        if( ae_fp_eq(state->x.ptr.p_double[i],state->bndu.ptr.p_double[i])&&ae_fp_greater(-state->g.ptr.p_double[i],0) )
        {
            v = 0;
        }
        result = result+ae_sqr(v, _state);
    }
    result = ae_sqrt(result, _state);
    return result;
}


/*************************************************************************
Returns norm of GI(x).

GI(x) is  a  gradient  vector  whose  components  associated  with  active
constraints are zeroed. It  differs  from  bounded  anti-gradient  because
components  of   GI(x)   are   zeroed  independently  of  sign(g[i]),  and
anti-gradient's components are zeroed with respect to both constraint  and
sign.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
static double mincomp_asaginorm(minasastate* state, ae_state *_state)
{
    ae_int_t i;
    double result;


    result = 0;
    for(i=0; i<=state->n-1; i++)
    {
        if( ae_fp_neq(state->x.ptr.p_double[i],state->bndl.ptr.p_double[i])&&ae_fp_neq(state->x.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            result = result+ae_sqr(state->g.ptr.p_double[i], _state);
        }
    }
    result = ae_sqrt(result, _state);
    return result;
}


/*************************************************************************
Returns norm(D1(State.X))

For a meaning of D1 see 'NEW ACTIVE SET ALGORITHM FOR BOX CONSTRAINED
OPTIMIZATION' by WILLIAM W. HAGER AND HONGCHAO ZHANG.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
static double mincomp_asad1norm(minasastate* state, ae_state *_state)
{
    ae_int_t i;
    double result;


    result = 0;
    for(i=0; i<=state->n-1; i++)
    {
        result = result+ae_sqr(boundval(state->x.ptr.p_double[i]-state->g.ptr.p_double[i], state->bndl.ptr.p_double[i], state->bndu.ptr.p_double[i], _state)-state->x.ptr.p_double[i], _state);
    }
    result = ae_sqrt(result, _state);
    return result;
}


/*************************************************************************
Returns True, if U set is empty.

* State.X is used as point,
* State.G - as gradient,
* D is calculated within function (because State.D may have different
  meaning depending on current optimization algorithm)

For a meaning of U see 'NEW ACTIVE SET ALGORITHM FOR BOX CONSTRAINED
OPTIMIZATION' by WILLIAM W. HAGER AND HONGCHAO ZHANG.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
static ae_bool mincomp_asauisempty(minasastate* state, ae_state *_state)
{
    ae_int_t i;
    double d;
    double d2;
    double d32;
    ae_bool result;


    d = mincomp_asad1norm(state, _state);
    d2 = ae_sqrt(d, _state);
    d32 = d*d2;
    result = ae_true;
    for(i=0; i<=state->n-1; i++)
    {
        if( ae_fp_greater_eq(ae_fabs(state->g.ptr.p_double[i], _state),d2)&&ae_fp_greater_eq(ae_minreal(state->x.ptr.p_double[i]-state->bndl.ptr.p_double[i], state->bndu.ptr.p_double[i]-state->x.ptr.p_double[i], _state),d32) )
        {
            result = ae_false;
            return result;
        }
    }
    return result;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void mincomp_clearrequestfields(minasastate* state,
     ae_state *_state)
{


    state->needfg = ae_false;
    state->xupdated = ae_false;
}


ae_bool _minasastate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minasastate *p = (minasastate*)_p;
    ae_touch_ptr((void*)p);
    if( !ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->ak, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xk, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->dk, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->an, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->xn, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->dn, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->work, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->yk, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->gc, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic) )
        return ae_false;
    if( !_rcommstate_init(&p->rstate, _state, make_automatic) )
        return ae_false;
    if( !_linminstate_init(&p->lstate, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _minasastate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minasastate *dst = (minasastate*)_dst;
    minasastate *src = (minasastate*)_src;
    dst->n = src->n;
    dst->epsg = src->epsg;
    dst->epsf = src->epsf;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->stpmax = src->stpmax;
    dst->cgtype = src->cgtype;
    dst->k = src->k;
    dst->nfev = src->nfev;
    dst->mcstage = src->mcstage;
    if( !ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic) )
        return ae_false;
    dst->curalgo = src->curalgo;
    dst->acount = src->acount;
    dst->mu = src->mu;
    dst->finit = src->finit;
    dst->dginit = src->dginit;
    if( !ae_vector_init_copy(&dst->ak, &src->ak, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->xk, &src->xk, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->dk, &src->dk, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->an, &src->an, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->xn, &src->xn, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->dn, &src->dn, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic) )
        return ae_false;
    dst->fold = src->fold;
    dst->stp = src->stp;
    if( !ae_vector_init_copy(&dst->work, &src->work, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->yk, &src->yk, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->gc, &src->gc, _state, make_automatic) )
        return ae_false;
    dst->laststep = src->laststep;
    if( !ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic) )
        return ae_false;
    dst->f = src->f;
    if( !ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic) )
        return ae_false;
    dst->needfg = src->needfg;
    dst->xupdated = src->xupdated;
    if( !_rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic) )
        return ae_false;
    dst->repiterationscount = src->repiterationscount;
    dst->repnfev = src->repnfev;
    dst->repterminationtype = src->repterminationtype;
    dst->debugrestartscount = src->debugrestartscount;
    if( !_linminstate_init_copy(&dst->lstate, &src->lstate, _state, make_automatic) )
        return ae_false;
    dst->betahs = src->betahs;
    dst->betady = src->betady;
    return ae_true;
}


void _minasastate_clear(void* _p)
{
    minasastate *p = (minasastate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->ak);
    ae_vector_clear(&p->xk);
    ae_vector_clear(&p->dk);
    ae_vector_clear(&p->an);
    ae_vector_clear(&p->xn);
    ae_vector_clear(&p->dn);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->work);
    ae_vector_clear(&p->yk);
    ae_vector_clear(&p->gc);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->g);
    _rcommstate_clear(&p->rstate);
    _linminstate_clear(&p->lstate);
}


void _minasastate_destroy(void* _p)
{
    minasastate *p = (minasastate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->ak);
    ae_vector_destroy(&p->xk);
    ae_vector_destroy(&p->dk);
    ae_vector_destroy(&p->an);
    ae_vector_destroy(&p->xn);
    ae_vector_destroy(&p->dn);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->work);
    ae_vector_destroy(&p->yk);
    ae_vector_destroy(&p->gc);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->g);
    _rcommstate_destroy(&p->rstate);
    _linminstate_destroy(&p->lstate);
}


ae_bool _minasareport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minasareport *p = (minasareport*)_p;
    ae_touch_ptr((void*)p);
    return ae_true;
}


ae_bool _minasareport_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic)
{
    minasareport *dst = (minasareport*)_dst;
    minasareport *src = (minasareport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfev = src->nfev;
    dst->terminationtype = src->terminationtype;
    dst->activeconstraints = src->activeconstraints;
    return ae_true;
}


void _minasareport_clear(void* _p)
{
    minasareport *p = (minasareport*)_p;
    ae_touch_ptr((void*)p);
}


void _minasareport_destroy(void* _p)
{
    minasareport *p = (minasareport*)_p;
    ae_touch_ptr((void*)p);
}



}

