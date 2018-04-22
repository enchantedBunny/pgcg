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
        string preview();
        Variable_old();
        void setID(int bId);
        float getValue(float *v);
        void c(float value);
        void i();
        string f(Variable_old *a, Variable_old *b, string *op);
        string f(Variable_old *a, string* op);
        string f(Variable_old *a, string *o, int p)
        float getDerivValue(int g,float *v);
cdef int bbbb = 0


cdef class var_old:
    cdef:
        int cint
        float * cfloats
        float ** cfloatses
        int i
        Variable_old thisptr      # hold a C++ instance which we're wrapping
        var_old l
        var_old r
    def preview(self):
        print(self.thisptr.preview().decode('utf8'))
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
        self.thisptr.f(l, r, &s).decode('utf8')
    def exp(self, var_old a):
        inop = "exp"
        cdef string s
        s = str.encode(inop)
        cdef Variable_old *l = &a.thisptr
        self.thisptr.f(l, &s).decode('utf8')
    def pow(self, var_old a, int g):
        inop = "pow"
        cdef string s
        s = str.encode(inop)
        cdef Variable_old *l = &a.thisptr
        print(self.thisptr.f(l, &s, g).decode('utf8'))
    def sigmoid(self, var_old a):
        inop = "sig"
        cdef string s
        s = str.encode(inop)
        cdef Variable_old *l = &a.thisptr
        print(self.thisptr.f(l, &s).decode('utf8'))
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
    def feedDeriv(self,g, floatses):
        out = []
        for f in range(len(floatses[0])):
            ror = [gr[f]  for gr in floatses]
            print(ror)
            out.append(self.getDeriv(g,ror))
        return out
    def __add__(self,other):
        h = var_old()
        h.f(self, other, "+")
        return h
    def __sub__(self,other):
        h = var_old()
        h.f(self, other, "-")
        return h
    def __mul__(self,other):
        h = var_old()
        h.f(self, other, "*")
        return h
    def __truediv__(self,other):
        h = var_old()
        h.f(self, other, "/")
        return h
    def __pow__(self,other, em):
        h = var_old()
        h.pow(self, other)
        return h
            
