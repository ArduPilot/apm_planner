#include "stdafx.h"
#include <math.h>
#include "alglibmisc.h"
#include "alglibinternal.h"
#include "linalg.h"
#include "statistics.h"
#include "dataanalysis.h"
#include "specialfunctions.h"
#include "solvers.h"
#include "optimization.h"
#include "diffequations.h"
#include "fasttransforms.h"
#include "integration.h"
#include "interpolation.h"

using namespace alglib;

const char *fmt_str = "%-25s %s\n";

//
// Functions and flag variable for test "issue_505"
//
bool issue505_passed = true;
bool issue478_passed = true;
bool issue528_passed = true;

//
// Service datatypes
//
typedef struct
{
    alglib_impl::ae_complex cval;
    double rval;
    alglib_impl::ae_int_t ival;
    ae_bool bval;
    alglib_impl::ae_vector i1val;
} innerrec;


typedef struct
{
    ae_bool bval;
    innerrec recval;
    alglib_impl::ae_shared_pool pool;
} seedrec;

ae_bool _innerrec_init(void* _p, alglib_impl::ae_state *_state, ae_bool make_automatic)
{
    innerrec *p = (innerrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    if( !alglib_impl::ae_vector_init(&p->i1val, 0, alglib_impl::DT_INT, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _innerrec_init_copy(void* _dst, void* _src, alglib_impl::ae_state *_state, ae_bool make_automatic)
{
    innerrec *dst = (innerrec*)_dst;
    innerrec *src = (innerrec*)_src;
    dst->cval = src->cval;
    dst->rval = src->rval;
    dst->ival = src->ival;
    dst->bval = src->bval;
    if( !alglib_impl::ae_vector_init_copy(&dst->i1val, &src->i1val, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _innerrec_clear(void* _p)
{
    innerrec *p = (innerrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    alglib_impl::ae_vector_clear(&p->i1val);
}


void _innerrec_destroy(void* _p)
{
    innerrec *p = (innerrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    alglib_impl::ae_vector_destroy(&p->i1val);
}


ae_bool _seedrec_init(void* _p, alglib_impl::ae_state *_state, ae_bool make_automatic)
{
    seedrec *p = (seedrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    if( !_innerrec_init(&p->recval, _state, make_automatic) )
        return ae_false;
    if( !alglib_impl::ae_shared_pool_init(&p->pool, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


ae_bool _seedrec_init_copy(void* _dst, void* _src, alglib_impl::ae_state *_state, ae_bool make_automatic)
{
    seedrec *dst = (seedrec*)_dst;
    seedrec *src = (seedrec*)_src;
    dst->bval = src->bval;
    if( !_innerrec_init_copy(&dst->recval, &src->recval, _state, make_automatic) )
        return ae_false;
    if( !alglib_impl::ae_shared_pool_init_copy(&dst->pool, &src->pool, _state, make_automatic) )
        return ae_false;
    return ae_true;
}


void _seedrec_clear(void* _p)
{
    seedrec *p = (seedrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    _innerrec_clear(&p->recval);
    alglib_impl::ae_shared_pool_clear(&p->pool);
}


void _seedrec_destroy(void* _p)
{
    seedrec *p = (seedrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    _innerrec_destroy(&p->recval);
    alglib_impl::ae_shared_pool_destroy(&p->pool);
}

void func505_grad(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    double x0 = *((double*)ptr);
    
    //
    // This block assigns zero vector to gradient. Because gradient is a proxy vector
    // (vector which uses another object as storage), sizes of gradient and vector being
    // assigned must be equal. In this case data are copied in the memory linked with
    // proxy.
    //
    // Early versions of ALGLIB failed to handle such assignment (it discrupted link
    // between proxy vector and actual gradient stored in the internals of ALGLIB).
    //
    real_1d_array z = "[0]";
    grad = "[0]";
    grad = z;
    
    //
    // This block tries to perform operations which are forbidden for proxy vector:
    // * assign vector of non-matching size
    // * change length of the vector
    // Correct implementation must throw an exception without breaking a link between
    // proxy object and actual vector.
    //
    z = "[0,1]";
    try
    {
        grad = "[0,1]";
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        grad = z;
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        grad.setlength(10);
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        grad.setlength(1);
        issue505_passed = false;
    }
    catch(...) {}
    
    //
    // This block actually calculates function/gradient
    //
    func = pow(x[0]-x0,4);
    grad[0] = 4*pow(x[0]-x0,3);
}

void  func505_vec(const real_1d_array &x, real_1d_array &fi, void *ptr)
{
    double x0 = *((double*)ptr);
    fi[0] = x[0]-x0;
    fi[1] = pow(x[0]-x0,2);
}

void  func505_jac(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr)
{
    double x0 = *((double*)ptr);
    
    //
    // This block assigns zero matrix to Jacobian. Because Jacobian is a proxy matrix
    // (matrix which uses another object as storage), sizes of Jacobian and matrix being
    // assigned must be equal. In this case data are copied in the memory linked with
    // proxy.
    //
    // Early versions of ALGLIB failed to handle such assignment (it discrupted link
    // between proxy and actual matrix stored in the internals of ALGLIB).
    //
    real_2d_array z = "[[0],[0]]";
    jac = "[[0],[0]]";
    jac = z;
    
    //
    // This block tries to perform operations which are forbidden for proxy vector:
    // * assign vector of non-matching size
    // * change length of the vector
    // Correct implementation must throw an exception without breaking a link between
    // proxy object and actual vector.
    //
    try
    {
        jac = "[[0]]";
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        jac = "[[0,0],[1,1]]";
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        z = "[[0,1]]";
        jac = z;
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        jac.setlength(10,6);
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        jac.setlength(2,1);
        issue505_passed = false;
    }
    catch(...) {}
    
    //
    // This block actually calculates function/gradient
    //
    fi[0] = x[0]-x0;
    fi[1] = pow(x[0]-x0,2);
    jac[0][0] = 1.0;
    jac[1][0] = 2*(x[0]-x0);
}

int main()
{
    //
    // Testing basic functionality
    //
    printf("Basic functions:\n");
    {
        //
        // Testing 1D array functionality
        //
        bool passed = true;
        try
        {
            //
            // 1D boolean
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            // * test setcontent/getcontent
            // * test getcontent(), operator() and operator[] on constant arrays
            //   (in this case distinct implementation is used which must be tested separately)
            //
            alglib::boolean_1d_array arr_0, arr_1("[]"), arr_2("[true,false,true]"), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = "[true,true,false]";
            passed = passed && (arr_0.length()==0);
            passed = passed && (arr_1.length()==0);
            passed = passed && (arr_2.length()==3);
            passed = passed && (arr_3.length()==3);
            passed = passed && (arr_2[0]==arr_2(0)) && (arr_2[1]==arr_2(1)) && (arr_2[2]==arr_2(2));
            passed = passed &&  arr_2[0] && !arr_2[1] &&  arr_2[2];
            passed = passed &&  arr_3[0] && !arr_3[1] &&  arr_3[2];
            passed = passed &&  arr_4[0] && !arr_4[1] &&  arr_4[2];
            passed = passed &&  arr_5[0] &&  arr_5[1] && !arr_5[2];
            passed = passed && (arr_2.tostring()=="[true,false,true]");
            passed = passed && (arr_3.tostring()=="[true,false,true]");
            passed = passed && (arr_4.tostring()=="[true,false,true]");
            passed = passed && (arr_5.tostring()=="[true,true,false]");
            arr_2[0] = false;
            passed = passed && !arr_2[0] && arr_3[0] && arr_4[0];
            arr_5.setlength(99);
            passed = passed && (arr_5.length()==99);
            
            // setcontent/getcontent
            bool a0[] = {true, false, true, false, false};
            bool a0_mod  = false;
            bool a0_orig = true;
            bool *p6;
            alglib::boolean_1d_array arr_6;
            arr_6.setcontent(5, a0);
            passed = passed && (arr_6[0]==a0[0]) && (arr_6[1]==a0[1]) && (arr_6[2]==a0[2]) && (arr_6[3]==a0[3]) && (arr_6[4]==a0[4]);
            p6 = arr_6.getcontent();
            passed = passed && (p6!=a0);
            passed = passed && (p6[0]==a0[0]) && (p6[1]==a0[1]) && (p6[2]==a0[2]) && (p6[3]==a0[3]) && (p6[4]==a0[4]);
            a0[0] = a0_mod;
            passed = passed && (arr_6[0]!=a0[0]);
            a0[0] = a0_orig;
            
            // operations on constant arrays
            {
                const alglib::boolean_1d_array &ac = arr_6;
                passed = passed && (ac[0]==a0[0]) && (ac[1]==a0[1]) && (ac[2]==a0[2]) && (ac[3]==a0[3]) && (ac[4]==a0[4]);
                passed = passed && (ac(0)==a0[0]) && (ac(1)==a0[1]) && (ac(2)==a0[2]) && (ac(3)==a0[3]) && (ac(4)==a0[4]);
                const bool *p = ac.getcontent();
                passed = passed && (p[0]==a0[0]) && (p[1]==a0[1]) && (p[2]==a0[2]) && (p[3]==a0[3]) && (p[4]==a0[4]);
            }
            
            //
            // Operations with proxy arrays:
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            alglib::boolean_1d_array proxy, targt, acopy;
            targt = "[true,false,false,true]";
            proxy.attach_to(targt.c_ptr());
            acopy = proxy;
            passed = passed &&  targt[0] && !targt[1] && !targt[2] &&  targt[3];
            passed = passed &&  proxy[0] && !proxy[1] && !proxy[2] &&  proxy[3];
            passed = passed &&  acopy[0] && !acopy[1] && !acopy[2] &&  acopy[3];
            
            targt[0] = false;
            passed = passed && !targt[0] && !proxy[0] && acopy[0];
            proxy[0] = true;
            passed = passed &&  targt[0] &&  proxy[0] && acopy[0];
            
            acopy = "[false,true,true,true]";
            proxy = acopy;
            passed = passed && !targt[0] &&  targt[1] &&  targt[2] &&  targt[3];
            passed = passed && !proxy[0] &&  proxy[1] &&  proxy[2] &&  proxy[3];
            proxy = "[true,false,true,true]";
            passed = passed &&  targt[0] && !targt[1] &&  targt[2] &&  targt[3];
            passed = passed &&  proxy[0] && !proxy[1] &&  proxy[2] &&  proxy[3];
            
            try
            {
                acopy = "[false,true,true]";
                proxy = acopy;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy = "[true,true,true]";
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(100);
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(proxy.length());
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
        }
        catch(...)
        { passed = false; }
        try
        {
            //
            // 1D integer
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            //
            const char *s1 = "[2,3,-1]";
            const char *s2 = "[5,4,3]";
            const char *s3 = "[6,7,3,-4]";
            const char *s4 = "[9,5,-12,-0]";
            const char *s5 = "[1,7,2,1]";
            const char *s6 = "[7,7,7]";
            int v10 =  2, v11 =  3, v12 = -1, v10_mod = 9;
            int v20 =  5, v21 =  4, v22 =  3;
            int v30 =  6, v31 =  7, v32 =  3, v33 = -4, v30_mod = -6;
            int v40 =  9, v41 =  5, v42 =-12, v43 = 0;
            int v50 =  1, v51 =  7, v52 =  2, v53 = 1;
            
            alglib::integer_1d_array arr_0, arr_1("[]"), arr_2(s1), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = s2;
            passed = passed && (arr_0.length()==0);
            passed = passed && (arr_1.length()==0);
            passed = passed && (arr_2.length()==3);
            passed = passed && (arr_3.length()==3);
            passed = passed && (arr_2[0]==arr_2(0)) && (arr_2[1]==arr_2(1)) && (arr_2[2]==arr_2(2));
            passed = passed && (arr_2[0]==v10) && (arr_2[1]==v11) && (arr_2[2]==v12);
            passed = passed && (arr_3[0]==v10) && (arr_3[1]==v11) && (arr_3[2]==v12);
            passed = passed && (arr_4[0]==v10) && (arr_4[1]==v11) && (arr_4[2]==v12);
            passed = passed && (arr_5[0]==v20) && (arr_5[1]==v21) && (arr_5[2]==v22);
            passed = passed && (arr_2.tostring()==s1);
            passed = passed && (arr_3.tostring()==s1);
            passed = passed && (arr_4.tostring()==s1);
            passed = passed && (arr_5.tostring()==s2);
            arr_2[0] = v10_mod;
            passed = passed && (arr_2[0]==v10_mod) && (arr_3[0]==v10) && (arr_4[0]==v10);
            arr_5.setlength(99);
            passed = passed && (arr_5.length()==99);
            
            // setcontent/getcontent
            alglib::ae_int_t a0[] =   {2, 3, 1, 9, 2};
            alglib::ae_int_t a0_mod  = 7;
            alglib::ae_int_t a0_orig = 2;
            alglib::ae_int_t *p6;
            alglib::integer_1d_array arr_6;
            arr_6.setcontent(5, a0);
            passed = passed && (arr_6[0]==a0[0]) && (arr_6[1]==a0[1]) && (arr_6[2]==a0[2]) && (arr_6[3]==a0[3]) && (arr_6[4]==a0[4]);
            p6 = arr_6.getcontent();
            passed = passed && (p6!=a0);
            passed = passed && (p6[0]==a0[0]) && (p6[1]==a0[1]) && (p6[2]==a0[2]) && (p6[3]==a0[3]) && (p6[4]==a0[4]);
            a0[0] = a0_mod;
            passed = passed && (arr_6[0]!=a0[0]);
            a0[0] = a0_orig;
            
            // operations on constant arrays
            {
                const alglib::integer_1d_array &ac = arr_6;
                passed = passed && (ac[0]==a0[0]) && (ac[1]==a0[1]) && (ac[2]==a0[2]) && (ac[3]==a0[3]) && (ac[4]==a0[4]);
                passed = passed && (ac(0)==a0[0]) && (ac(1)==a0[1]) && (ac(2)==a0[2]) && (ac(3)==a0[3]) && (ac(4)==a0[4]);
                const alglib::ae_int_t *p = ac.getcontent();
                passed = passed && (p[0]==a0[0]) && (p[1]==a0[1]) && (p[2]==a0[2]) && (p[3]==a0[3]) && (p[4]==a0[4]);
            }
            
            //
            // Operations with proxy arrays:
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            alglib::integer_1d_array proxy, targt, acopy;
            targt = s3;
            proxy.attach_to(targt.c_ptr());
            acopy = proxy;
            passed = passed && (targt[0]==v30) && (targt[1]==v31) && (targt[2]==v32) && (targt[3]==v33);
            passed = passed && (proxy[0]==v30) && (proxy[1]==v31) && (proxy[2]==v32) && (proxy[3]==v33);
            passed = passed && (acopy[0]==v30) && (acopy[1]==v31) && (acopy[2]==v32) && (acopy[3]==v33);
            
            targt[0] = v30_mod;
            passed = passed && (targt[0]==v30_mod) && (proxy[0]==v30_mod) && (acopy[0]==v30);
            proxy[0] = v30;
            passed = passed && (targt[0]==v30)     && (proxy[0]==v30)     && (acopy[0]==v30);
            
            acopy = s4;
            proxy = acopy;
            passed = passed && (targt[0]==v40) && (targt[1]==v41) && (targt[2]==v42) && (targt[3]==v43);
            passed = passed && (proxy[0]==v40) && (proxy[1]==v41) && (proxy[2]==v42) && (proxy[3]==v43);
            proxy = s5;
            passed = passed && (targt[0]==v50) && (targt[1]==v51) && (targt[2]==v52) && (targt[3]==v53);
            passed = passed && (proxy[0]==v50) && (proxy[1]==v51) && (proxy[2]==v52) && (proxy[3]==v53);
            
            try
            {
                acopy = s6;
                proxy = acopy;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy = s6;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(100);
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(proxy.length());
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
        }
        catch(...)
        { passed = false; }
        try
        {
            //
            // 1D real
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            //
            const char *s1     = "[2,3.5,-2.5E-1]";
            const char *s1_fmt = "[2.00,3.50,-0.25]";
            const char *s2     = "[5,4,3.125]";
            const char *s2_fmt = "[5.00,4.00,3.13]";
            const char *s3 = "[6,7,3,-4E2]";
            const char *s4 = "[9,5,-12,-0.01]";
            const char *s5 = "[1,7,2,1]";
            const char *s6 = "[7,7,7]";
            const int dps = 2;
            double v10 =  2, v11 =  3.5, v12 = -0.25, v10_mod = 9;
            double v20 =  5, v21 =  4, v22 =  3.125;
            double v30 =  6, v31 =  7, v32 =  3, v33 = -400, v30_mod = -6;
            double v40 =  9, v41 =  5, v42 =-12, v43 = -0.01;
            double v50 =  1, v51 =  7, v52 =  2, v53 = 1;
            
            alglib::real_1d_array arr_0, arr_1("[]"), arr_2(s1), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = s2;
            passed = passed && (arr_0.length()==0);
            passed = passed && (arr_1.length()==0);
            passed = passed && (arr_2.length()==3);
            passed = passed && (arr_3.length()==3);
            passed = passed && (arr_2[0]==arr_2(0)) && (arr_2[1]==arr_2(1)) && (arr_2[2]==arr_2(2));
            passed = passed && (arr_2[0]==v10) && (arr_2[1]==v11) && (arr_2[2]==v12);
            passed = passed && (arr_3[0]==v10) && (arr_3[1]==v11) && (arr_3[2]==v12);
            passed = passed && (arr_4[0]==v10) && (arr_4[1]==v11) && (arr_4[2]==v12);
            passed = passed && (arr_5[0]==v20) && (arr_5[1]==v21) && (arr_5[2]==v22);
            passed = passed && (arr_2.tostring(dps)==s1_fmt);
            passed = passed && (arr_3.tostring(dps)==s1_fmt);
            passed = passed && (arr_4.tostring(dps)==s1_fmt);
            passed = passed && (arr_5.tostring(dps)==s2_fmt);
            arr_2[0] = v10_mod;
            passed = passed && (arr_2[0]==v10_mod) && (arr_3[0]==v10) && (arr_4[0]==v10);
            arr_5.setlength(99);
            passed = passed && (arr_5.length()==99);
            
            // setcontent/getcontent
            double a0[] =   {2, 3.5, 1, 9.125, 2};
            double a0_mod  = 7;
            double a0_orig = 2;
            double *p6;
            alglib::real_1d_array arr_6;
            arr_6.setcontent(5, a0);
            passed = passed && (arr_6[0]==a0[0]) && (arr_6[1]==a0[1]) && (arr_6[2]==a0[2]) && (arr_6[3]==a0[3]) && (arr_6[4]==a0[4]);
            p6 = arr_6.getcontent();
            passed = passed && (p6!=a0);
            passed = passed && (p6[0]==a0[0]) && (p6[1]==a0[1]) && (p6[2]==a0[2]) && (p6[3]==a0[3]) && (p6[4]==a0[4]);
            a0[0] = a0_mod;
            passed = passed && (arr_6[0]!=a0[0]);
            a0[0] = a0_orig;
            
            // operations on constant arrays
            {
                const alglib::real_1d_array &ac = arr_6;
                passed = passed && (ac[0]==a0[0]) && (ac[1]==a0[1]) && (ac[2]==a0[2]) && (ac[3]==a0[3]) && (ac[4]==a0[4]);
                passed = passed && (ac(0)==a0[0]) && (ac(1)==a0[1]) && (ac(2)==a0[2]) && (ac(3)==a0[3]) && (ac(4)==a0[4]);
                const double *p = ac.getcontent();
                passed = passed && (p[0]==a0[0]) && (p[1]==a0[1]) && (p[2]==a0[2]) && (p[3]==a0[3]) && (p[4]==a0[4]);
            }
            
            //
            // Operations with proxy arrays:
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            alglib::real_1d_array proxy, targt, acopy;
            targt = s3;
            proxy.attach_to(targt.c_ptr());
            acopy = proxy;
            passed = passed && (targt[0]==v30) && (targt[1]==v31) && (targt[2]==v32) && (targt[3]==v33);
            passed = passed && (proxy[0]==v30) && (proxy[1]==v31) && (proxy[2]==v32) && (proxy[3]==v33);
            passed = passed && (acopy[0]==v30) && (acopy[1]==v31) && (acopy[2]==v32) && (acopy[3]==v33);
            
            targt[0] = v30_mod;
            passed = passed && (targt[0]==v30_mod) && (proxy[0]==v30_mod) && (acopy[0]==v30);
            proxy[0] = v30;
            passed = passed && (targt[0]==v30)     && (proxy[0]==v30)     && (acopy[0]==v30);
            
            acopy = s4;
            proxy = acopy;
            passed = passed && (targt[0]==v40) && (targt[1]==v41) && (targt[2]==v42) && (targt[3]==v43);
            passed = passed && (proxy[0]==v40) && (proxy[1]==v41) && (proxy[2]==v42) && (proxy[3]==v43);
            proxy = s5;
            passed = passed && (targt[0]==v50) && (targt[1]==v51) && (targt[2]==v52) && (targt[3]==v53);
            passed = passed && (proxy[0]==v50) && (proxy[1]==v51) && (proxy[2]==v52) && (proxy[3]==v53);
            
            try
            {
                acopy = s6;
                proxy = acopy;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy = s6;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(100);
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(proxy.length());
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
        }
        catch(...)
        { passed = false; }
        try
        {
            //
            // 1D complex
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            //
            const char *s1     = "[2,3.5i,1-2.5E-1i]";
            const char *s1_fmt = "[2.00,3.50i,1.00-0.25i]";
            const char *s2     = "[5,-4+1i,3.125]";
            const char *s2_fmt = "[5.00,-4.00+1.00i,3.13]";
            const char *s3 = "[6,7,3,-4E2]";
            const char *s4 = "[9,5,-12,-0.01]";
            const char *s5 = "[1,7,2,1]";
            const char *s6 = "[7,7,7]";
            const int dps = 2;
            alglib::complex v10 =  2, v11 = alglib::complex(0,3.5), v12 = alglib::complex(1,-0.25), v10_mod = 9;
            alglib::complex v20 =  5, v21 = alglib::complex(-4,1),  v22 =  3.125;
            alglib::complex v30 =  6, v31 =  7, v32 =  3, v33 = -400, v30_mod = -6;
            alglib::complex v40 =  9, v41 =  5, v42 =-12, v43 = -0.01;
            alglib::complex v50 =  1, v51 =  7, v52 =  2, v53 = 1;
            
            alglib::complex_1d_array arr_0, arr_1("[]"), arr_2(s1), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = s2;
            passed = passed && (arr_0.length()==0);
            passed = passed && (arr_1.length()==0);
            passed = passed && (arr_2.length()==3);
            passed = passed && (arr_3.length()==3);
            passed = passed && (arr_2[0]==arr_2(0)) && (arr_2[1]==arr_2(1)) && (arr_2[2]==arr_2(2));
            passed = passed && (arr_2[0]==v10) && (arr_2[1]==v11) && (arr_2[2]==v12);
            passed = passed && (arr_3[0]==v10) && (arr_3[1]==v11) && (arr_3[2]==v12);
            passed = passed && (arr_4[0]==v10) && (arr_4[1]==v11) && (arr_4[2]==v12);
            passed = passed && (arr_5[0]==v20) && (arr_5[1]==v21) && (arr_5[2]==v22);
            passed = passed && (arr_2.tostring(dps)==s1_fmt);
            passed = passed && (arr_3.tostring(dps)==s1_fmt);
            passed = passed && (arr_4.tostring(dps)==s1_fmt);
            passed = passed && (arr_5.tostring(dps)==s2_fmt);
            arr_2[0] = v10_mod;
            passed = passed && (arr_2[0]==v10_mod) && (arr_3[0]==v10) && (arr_4[0]==v10);
            arr_5.setlength(99);
            passed = passed && (arr_5.length()==99);
            
            // setcontent/getcontent
            alglib::complex a0[] =   {2, 3.5, 1, 9.125, 2};
            alglib::complex a0_mod  = 7;
            alglib::complex a0_orig = 2;
            alglib::complex *p6;
            alglib::complex_1d_array arr_6;
            arr_6.setcontent(5, a0);
            passed = passed && (arr_6[0]==a0[0]) && (arr_6[1]==a0[1]) && (arr_6[2]==a0[2]) && (arr_6[3]==a0[3]) && (arr_6[4]==a0[4]);
            p6 = arr_6.getcontent();
            passed = passed && (p6!=a0);
            passed = passed && (p6[0]==a0[0]) && (p6[1]==a0[1]) && (p6[2]==a0[2]) && (p6[3]==a0[3]) && (p6[4]==a0[4]);
            a0[0] = a0_mod;
            passed = passed && (arr_6[0]!=a0[0]);
            a0[0] = a0_orig;
            
            // operations on constant arrays
            {
                const alglib::complex_1d_array &ac = arr_6;
                passed = passed && (ac[0]==a0[0]) && (ac[1]==a0[1]) && (ac[2]==a0[2]) && (ac[3]==a0[3]) && (ac[4]==a0[4]);
                passed = passed && (ac(0)==a0[0]) && (ac(1)==a0[1]) && (ac(2)==a0[2]) && (ac(3)==a0[3]) && (ac(4)==a0[4]);
                const alglib::complex *p = ac.getcontent();
                passed = passed && (p[0]==a0[0]) && (p[1]==a0[1]) && (p[2]==a0[2]) && (p[3]==a0[3]) && (p[4]==a0[4]);
            }
            
            //
            // Operations with proxy arrays:
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            alglib::complex_1d_array proxy, targt, acopy;
            targt = s3;
            proxy.attach_to(targt.c_ptr());
            acopy = proxy;
            passed = passed && (targt[0]==v30) && (targt[1]==v31) && (targt[2]==v32) && (targt[3]==v33);
            passed = passed && (proxy[0]==v30) && (proxy[1]==v31) && (proxy[2]==v32) && (proxy[3]==v33);
            passed = passed && (acopy[0]==v30) && (acopy[1]==v31) && (acopy[2]==v32) && (acopy[3]==v33);
            
            targt[0] = v30_mod;
            passed = passed && (targt[0]==v30_mod) && (proxy[0]==v30_mod) && (acopy[0]==v30);
            proxy[0] = v30;
            passed = passed && (targt[0]==v30)     && (proxy[0]==v30)     && (acopy[0]==v30);
            
            acopy = s4;
            proxy = acopy;
            passed = passed && (targt[0]==v40) && (targt[1]==v41) && (targt[2]==v42) && (targt[3]==v43);
            passed = passed && (proxy[0]==v40) && (proxy[1]==v41) && (proxy[2]==v42) && (proxy[3]==v43);
            proxy = s5;
            passed = passed && (targt[0]==v50) && (targt[1]==v51) && (targt[2]==v52) && (targt[3]==v53);
            passed = passed && (proxy[0]==v50) && (proxy[1]==v51) && (proxy[2]==v52) && (proxy[3]==v53);
            
            try
            {
                acopy = s6;
                proxy = acopy;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy = s6;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(100);
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(proxy.length());
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
        }
        catch(...)
        { passed = false; }
        
        //
        // Report
        //
        printf(fmt_str, "* 1D arrays", passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !passed )
            return 1;
    }
    
    {
        //
        // Testing 2D array functionality
        //
        bool passed = true;
        try
        {
            //
            // 2D real
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            //
            const char *s1     = "[[2,3.5,-2.5E-1],[1,2,3]]";
            const char *s1_fmt = "[[2.00,3.50,-0.25],[1.00,2.00,3.00]]";
            const char *s2     = "[[5],[4],[3.125]]";
            const char *s2_fmt = "[[5.00],[4.00],[3.13]]";
            const char *s3 = "[[6,7],[3,-4E2],[-3,-1]]";
            const char *s4 = "[[9,5],[-12,-0.01],[-1,-2]]";
            const char *s5 = "[[1,7],[2,1],[0,4]]";
            const char *s60 = "[[7,7],[7,7]]";
            const char *s61 = "[[7],[7],[7]]";
            const int dps = 2;
            double v10 =  2, v11 =  3.5, v12 = -0.25, v13=1, v14 = 2, v15 = 3, v10_mod = 9;
            double v20 =  5, v21 =  4, v22 =  3.125;
            /*double v30 =  6, v31 =  7, v32 =  3, v33 = -400, v30_mod = -6;
            double v40 =  9, v41 =  5, v42 =-12, v43 = -0.01;
            double v50 =  1, v51 =  7, v52 =  2, v53 = 1;*/
            double r;
            
            alglib::real_2d_array arr_0, arr_1("[[]]"), arr_2(s1), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = s2;
            passed = passed && (arr_0.rows()==0) && (arr_0.cols()==0) && (arr_0.getstride()==0);
            passed = passed && (arr_1.rows()==0) && (arr_1.cols()==0) && (arr_1.getstride()==0);
            passed = passed && (arr_2.rows()==2) && (arr_2.cols()==3) && (arr_2.getstride()>=arr_2.cols());
            passed = passed && (arr_3.rows()==2) && (arr_3.cols()==3) && (arr_3.getstride()>=arr_3.cols());
            passed = passed && (arr_4.rows()==2) && (arr_4.cols()==3) && (arr_4.getstride()>=arr_4.cols());
            passed = passed && (arr_5.rows()==3) && (arr_5.cols()==1) && (arr_5.getstride()>=arr_5.cols());
            passed = passed && (arr_2[0][0]==arr_2(0,0)) && (arr_2[0][1]==arr_2(0,1)) && (arr_2[0][2]==arr_2(0,2));
            passed = passed && (arr_2[1][0]==arr_2(1,0)) && (arr_2[1][1]==arr_2(1,1)) && (arr_2[1][2]==arr_2(1,2));
            passed = passed && (arr_2[0][0]==v10) && (arr_2[0][1]==v11) && (arr_2[0][2]==v12);
            passed = passed && (arr_2[1][0]==v13) && (arr_2[1][1]==v14) && (arr_2[1][2]==v15);
            passed = passed && (arr_3[0][0]==v10) && (arr_3[0][1]==v11) && (arr_3[0][2]==v12);
            passed = passed && (arr_3[1][0]==v13) && (arr_3[1][1]==v14) && (arr_3[1][2]==v15);
            passed = passed && (arr_4[0][0]==v10) && (arr_4[0][1]==v11) && (arr_4[0][2]==v12);
            passed = passed && (arr_4[1][0]==v13) && (arr_4[1][1]==v14) && (arr_4[1][2]==v15);
            passed = passed && (arr_5[0][0]==v20) && (arr_5[1][0]==v21) && (arr_5[2][0]==v22);
            passed = passed && (arr_2.tostring(dps)==s1_fmt);
            passed = passed && (arr_3.tostring(dps)==s1_fmt);
            passed = passed && (arr_4.tostring(dps)==s1_fmt);
            passed = passed && (arr_5.tostring(dps)==s2_fmt);
            arr_2[0][0] = v10_mod;
            passed = passed && (arr_2[0][0]==v10_mod) && (arr_3[0][0]==v10) && (arr_4[0][0]==v10);
            arr_5.setlength(99,97);
            passed = passed && (arr_5.rows()==99) && (arr_5.cols()==97);
            
            //
            // setcontent/elementwise access/constant arrays
            //
            ae_int_t n, m, i, j;
            for(n=1; n<=10; n++)
                for(m=1; m<=10; m++)
                {
                    alglib::real_2d_array arr_6;
                    double a0[100];
                    
                    // fill array by random values, test setcontent(0
                    for(i=0; i<m*n; i++)
                        a0[i] = alglib::randomreal();
                    arr_6.setcontent(m, n, a0);
                    for(i=0; i<m; i++)
                        for(j=0; j<n; j++)
                        {
                            passed = passed && (arr_6[i][j]==a0[i*n+j]);
                            passed = passed && (arr_6(i,j)==a0[i*n+j]);
                        }
                    
                    // test that setcontent() actually copies data instead of creating just reference
                    r = a0[0];
                    a0[0] = a0[0]+1;
                    passed = passed && (arr_6[0][0]!=a0[0]);
                    a0[0] = r;
            
                    // operations on constant arrays
                    {
                        const alglib::real_2d_array &ac = arr_6;
                        for(i=0; i<m; i++)
                            for(j=0; j<n; j++)
                            {
                                passed = passed && (ac[i][j]==a0[i*n+j]);
                                passed = passed && (ac(i,j)==a0[i*n+j]);
                            }
                    }
                }
                
            
            //
            // Operations with proxy arrays:
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            
            // subtest 0
            alglib::real_2d_array proxy, targt, acopy, acopy2;
            targt = s3;
            proxy.attach_to(targt.c_ptr());
            acopy = proxy;
            for(i=0; i<targt.rows(); i++)
                for(j=0; j<targt.cols(); j++)
                {
                    passed = passed && (proxy[i][j]==targt[i][j]);
                    passed = passed && (acopy[i][j]==targt[i][j]);
                }
            r = targt[0][0];
            targt[0][0] = r+1;
            passed = passed && (targt[0][0]!=r) && (proxy[0][0]!=r) && (acopy[0][0]==r);
            proxy[0][0] = r;
            passed = passed && (targt[0][0]==r) && (proxy[0][0]==r) && (acopy[0][0]==r);
            
            // subtest 1
            acopy = s4;
            proxy = acopy;
            for(i=0; i<acopy.rows(); i++)
                for(j=0; j<acopy.cols(); j++)
                {
                    passed = passed && (proxy[i][j]==acopy[i][j]);
                    passed = passed && (targt[i][j]==acopy[i][j]);
                }
            r = targt[0][0];
            targt[0][0] = r+1;
            passed = passed && (targt[0][0]!=r) && (proxy[0][0]!=r) && (acopy[0][0]==r);
            proxy[0][0] = r;
            passed = passed && (targt[0][0]==r) && (proxy[0][0]==r) && (acopy[0][0]==r);
            
            // subtest 2
            acopy2 = s5;
            proxy = s5;
            for(i=0; i<acopy.rows(); i++)
                for(j=0; j<acopy.cols(); j++)
                {
                    passed = passed && (proxy[i][j]==acopy2[i][j]);
                    passed = passed && (targt[i][j]==acopy2[i][j]);
                }
            
            // error handling test 0
            try
            {
                acopy = s60;
                proxy = acopy;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            // error handling test 1
            try
            {
                acopy = s61;
                proxy = acopy;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            // error handling test 2
            try
            {
                proxy = s60;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            // error handling test 3
            try
            {
                proxy = s61;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            // error handling test 4
            try
            {
                proxy.setlength(100,99);
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            // error handling test 5
            try
            {
                proxy.setlength(proxy.rows(),proxy.cols());
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
        }
        catch(...)
        { passed = false; }
        
        //
        // Report
        //
        printf(fmt_str, "* 2D arrays", passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !passed )
            return 1;
    }
        
    //
    // Testing issues which must be fixed
    //
    printf("Issues:\n");
    {
        //
        // Testing issue #505 (http://bugs.alglib.net/view.php?id=505) in optimizers.
        // This issue was present in ALL optimizers, but we test it only on two: CG and LM.
        //
        try
        {
            //
            // Test CG
            // Stopping criteria - after |grad|<epsG
            //
            mincgstate state;
            mincgreport rep;
            real_1d_array x = "[0.0]";
            double x0 = 20*alglib::randomreal()-10;
            double epsg = 1.0E-9;
            mincgcreate(1, x, state);
            mincgsetcond(state, epsg, 0.0, 0.0, 0);
            mincgoptimize(state, func505_grad, NULL, &x0);
            mincgresults(state, x, rep);
            issue505_passed = issue505_passed && (fabs(4*pow(x[0]-x0,3))<epsg);
        }
        catch(...)
        { issue505_passed = false; }
        try
        {
            //
            // Test LM
            // Stopping criteria - after |grad|<epsG
            //
            minlmstate state;
            minlmreport rep;
            real_1d_array x = "[0.0]";
            double x0 = 20*alglib::randomreal()-10;
            double epsg = 1.0E-9;
            minlmcreatevj(1, 2, x, state);
            minlmsetcond(state, epsg, 0.0, 0.0, 0);
            minlmoptimize(state, func505_vec, func505_jac, NULL, &x0);
            minlmresults(state, x, rep);
            issue505_passed = issue505_passed && (fabs(x[0]-x0)<1.0E-3);
        }
        catch(...)
        { issue505_passed = false; }
        printf(fmt_str, "* issue 505", issue505_passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !issue505_passed )
            return 1;
            
        //
        // Testing issue #478 (http://bugs.alglib.net/view.php?id=478)
        // in high-quality RNG. It have to correctly handle random numbers
        // larger than 2^31.
        //
        // This test is performed only in 64-bit mode.
        //
        if( sizeof(alglib::ae_int_t)>4 )
        {
            //
            // 64-bit mode, perform test:
            // * use large NMax>2^31
            // * generate 1.000.000 random numbers
            // * use two bins - one for numbers less then NMax/2,
            //   another one for the rest of them
            // * bin sizes are equal to n0, n1
            // * both bins should be approximately equal, we use
            //   ad hoc threshold 0.45 < n0,n1 < 0.55.
            //
            try
            {
                alglib::hqrndstate rs;
                alglib::ae_int_t nmax[3];
                alglib::ae_int_t ncnt = 3, nidx;
                double n0, n1;
                alglib::hqrndrandomize(rs);
                
                //
                // nmax:
                // * first nmax is just large value to test basic uniformity of generator
                //
                nmax[0]  = 1000000;
                nmax[0]  = nmax[0]*nmax[0];
                nmax[1]  = 2147483562;
                nmax[1] *= 1.5;
                nmax[2]  = 2147483562;
                nmax[2] *= 3;
                
                for(nidx=0; nidx<ncnt; nidx++)
                {
                    n0 = 0;
                    n1 = 0;
                    for(int i=0; i<1000000; i++)
                    {
                        alglib::ae_int_t v = alglib::hqrnduniformi(rs, nmax[nidx]);
                        if( v<nmax[nidx]/2 )
                            n0++;
                        else
                            n1++;
                        issue478_passed = issue478_passed && (v>=0) && (v<nmax[nidx]);
                    }
                    issue478_passed = issue478_passed && (n0/(n0+n1)>0.45);
                    issue478_passed = issue478_passed && (n0/(n0+n1)<0.55);
                    issue478_passed = issue478_passed && (n1/(n0+n1)>0.45);
                    issue478_passed = issue478_passed && (n1/(n0+n1)<0.55);
                }
            }
            catch(...)
            { issue478_passed = false; }
            printf(fmt_str, "* issue 478", issue478_passed ? "OK" : "FAILED");
            fflush(stdout);
            if( !issue478_passed )
                return 1;
        }
        else
        {
            //
            // 32-bit mode, skip test
            //
            printf(fmt_str, "* issue 478", "OK (skipped in 32-bit mode)");
            fflush(stdout);
        }
            
        //
        // Testing issue #528 (http://bugs.alglib.net/view.php?id=528)
        // in shared pool and smart pointer which leak memory.
        //
        // In order to test it we create pool, seed it with specially
        // created structure, perform several operations, then clear it.
        // We test allocation counter before and after this operation.
        //
#ifndef AE_USE_ALLOC_COUNTER
#error AE_USE_ALLOC_COUNTER must be defined
#endif
        try
        {
            int alloc_cnt;
            alglib_impl::ae_state _alglib_env_state;
            alglib_impl::ae_frame _frame_block;
            alglib_impl::ae_shared_pool pool;
            alglib_impl::ae_smart_ptr ptr0, ptr1;
            void *p0, *p1;
            seedrec seed;
            
            // case #0: just seeding the pool
            alloc_cnt = alglib_impl::_alloc_counter;
            alglib_impl::ae_state_init(&_alglib_env_state);
            alglib_impl::ae_frame_make(&_alglib_env_state, &_frame_block);
            alglib_impl::ae_shared_pool_init(&pool, &_alglib_env_state, ae_true);
            _seedrec_init(&seed, &_alglib_env_state, ae_true);
            alglib_impl::ae_shared_pool_set_seed(&pool, &seed, sizeof(seed), _seedrec_init, _seedrec_init_copy, _seedrec_destroy, &_alglib_env_state);
            alglib_impl::ae_state_clear(&_alglib_env_state);
            issue528_passed = issue528_passed && (alloc_cnt==alglib_impl::_alloc_counter);
            
            // case #1: seeding and retrieving, not recycling
            alloc_cnt = alglib_impl::_alloc_counter;
            alglib_impl::ae_state_init(&_alglib_env_state);
            alglib_impl::ae_frame_make(&_alglib_env_state, &_frame_block);
            alglib_impl::ae_smart_ptr_init(&ptr0, (void**)&p0, &_alglib_env_state, ae_true);
            alglib_impl::ae_shared_pool_init(&pool, &_alglib_env_state, ae_true);
            _seedrec_init(&seed, &_alglib_env_state, ae_true);
            alglib_impl::ae_shared_pool_set_seed(&pool, &seed, sizeof(seed), _seedrec_init, _seedrec_init_copy, _seedrec_destroy, &_alglib_env_state);
            alglib_impl::ae_shared_pool_retrieve(&pool, &ptr0, &_alglib_env_state);
            alglib_impl::ae_state_clear(&_alglib_env_state);
            issue528_passed = issue528_passed && (alloc_cnt==alglib_impl::_alloc_counter);
            
            // case #2: seeding and retrieving twice, recycling both
            alloc_cnt = alglib_impl::_alloc_counter;
            alglib_impl::ae_state_init(&_alglib_env_state);
            alglib_impl::ae_frame_make(&_alglib_env_state, &_frame_block);
            alglib_impl::ae_smart_ptr_init(&ptr0, (void**)&p0, &_alglib_env_state, ae_true);
            alglib_impl::ae_smart_ptr_init(&ptr1, (void**)&p1, &_alglib_env_state, ae_true);
            alglib_impl::ae_shared_pool_init(&pool, &_alglib_env_state, ae_true);
            _seedrec_init(&seed, &_alglib_env_state, ae_true);
            alglib_impl::ae_shared_pool_set_seed(&pool, &seed, sizeof(seed), _seedrec_init, _seedrec_init_copy, _seedrec_destroy, &_alglib_env_state);
            alglib_impl::ae_shared_pool_retrieve(&pool, &ptr0, &_alglib_env_state);
            alglib_impl::ae_shared_pool_retrieve(&pool, &ptr1, &_alglib_env_state);
            alglib_impl::ae_shared_pool_recycle(&pool,  &ptr0, &_alglib_env_state);
            alglib_impl::ae_shared_pool_recycle(&pool,  &ptr1, &_alglib_env_state);
            alglib_impl::ae_state_clear(&_alglib_env_state);
            issue528_passed = issue528_passed && (alloc_cnt==alglib_impl::_alloc_counter);
        }
        catch(...)
        { issue528_passed = false; }
        printf(fmt_str, "* issue 528", issue528_passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !issue528_passed )
            return 1;
    }
    
    //
    // Return
    //
    return 0;
}