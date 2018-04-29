# distutils: language = c++
# distutils: sources = Variable.cpp
from libc.stdlib cimport malloc, free
from libcpp.string cimport string
from itertools import zip_longest
import math
cimport cython
cdef extern from "Variable_old.h" namespace "calc_old":
    cdef cppclass Variable_old:
        int id;
        string op;
        float value;
        Variable_old();
        void setID(int bId);
        float getValue(float *v);
        void c(float value);
        void i();
        void f(Variable_old *a, Variable_old *b, string *op);
        void f(Variable_old *a, string* op);
        void f(Variable_old *a, string *o, int p)
        float getDerivValue(int g,float *v);
cdef int bbbb = 0

#one call for all function variable inits
def function_old(a, b, inop=None):
    g = var_old('f')
    if type(a) == var_old and type(b) == var_old and type(inop)==str:
        g.f(a, b, inop)
    elif type(a) == var_old and type(b)==str and inop == None:
        if b == "exp":
            g.exp(a)
        elif b =="sigmoid":
            g.sigmoid(a)
    elif type(a) == var_old and b=="pow" and (type(inop) == int or type(inop) == float):
        g.pow(a, inop)
    else:
        print("I don't get it")
    return g
    
#call for all constant inits
def constant_old(c):
    g = var_old('c', c)
    return g
#one call for all variable inits
def variable_old():
    g = var_old('i')
    return g

cdef class var_old:
    cdef:
        int cint
        float * cfloats
        float ** cfloatses
        int i
        Variable_old thisptr      # hold a C++ instance which we're wrapping
        var_old l
        var_old r
    def __cinit__(self, type=None, val=None):
        global bbbb
        bbbb +=1
        self.thisptr = Variable_old()
        self.thisptr.setID(bbbb);
        if type == 'i':
            self.i()
        if type == 'c':
            self.c(val)
    def value(self, pyfloats):
        cfloats = <float *> malloc(len(pyfloats)*cython.sizeof(float))
        if cfloats is NULL:
            raise MemoryError()
        for i in range(len(pyfloats)):
            cfloats[i] = pyfloats[i]
        return(self.thisptr.getValue(cfloats))
    def c(self, float x):
        self.thisptr.c(x)
    def i(self):
        self.thisptr.i()
    def f(self, var_old a, var_old b, inop):
        cdef string s
        s = str.encode(inop)
        cdef Variable_old *l = &a.thisptr
        cdef Variable_old *r = &b.thisptr
        self.thisptr.f(l, r, &s)
    def exp(self, var_old a):
        inop = "exp"
        cdef string s
        s = str.encode(inop)
        cdef Variable_old *l = &a.thisptr
        self.thisptr.f(l, &s)
    def pow(self, var_old a, int g):
        inop = "pow"
        cdef string s
        s = str.encode(inop)
        cdef Variable_old *l = &a.thisptr
        self.thisptr.f(l, &s, g)
    def sigmoid(self, var_old a):
        inop = "sig"
        cdef string s
        s = str.encode(inop)
        cdef Variable_old *l = &a.thisptr
        self.thisptr.f(l, &s)
    def  getDeriv(self,g,pyfloats):
        cfloats = <float *> malloc(len(pyfloats)*cython.sizeof(float))
        if cfloats is NULL:
            raise MemoryError()
        for i in range(len(pyfloats)):
            cfloats[i] = pyfloats[i]
        out  = self.thisptr.getDerivValue(g,cfloats)
        if math.isnan(out):
            print("error - nan" + g + pyfloats)
            return -42
        return out