# distutils: language = c++
# distutils: sources = Variable.cpp
from libc.stdlib cimport malloc, free
from libcpp.string cimport string
from itertools import zip_longest
import math
cimport cython
cdef extern from "Variable_old.h" namespace "calc_old":
    cpdef enum vType:
            independent_old, constant_old, function_old, matrix_old, special_old
    cdef cppclass Variable_old:
        int id;
        string op;
        float value;
        vType type;
        string build();
        string preview();
        string previewDeriv();
        Variable_old();
        void setID(int bId);
        float getValue(float *v);
        float getValue(float **v, int rows)
        float* feed(float **v, int rows)
        void c(float value);
        void m(Variable_old *a);
        void i();
        string f(Variable_old *a, Variable_old *b, string *op);
        string f(Variable_old *a, string* op);
        string f(Variable_old *a, string *o, int p)
        float* fValues;
        float getDerivValue(int g,float *v);
cdef int bbbb = 0



def reduce_mean(pyfloats):
    return sum(pyfloats[0:len(pyfloats)])/len(pyfloats)

class presets:
    def sigmoid(ag= None):
        out, a, one, zero, negone,c, c1, c2 = var_old(),var_old('i'), var_old('c', 1), var_old('c', 0),var_old('c', -1), var_old(),var_old(),var_old()
        c2.f(zero, a, '-')
        c1.exp(c2) #exp(-x)
        c.f(c1,one,'+') #1+exp(-x)
        out.f(one, c, '/')
        out, a, one, zero, negone,c, c1, c2 = var_old(),var_old('i'), var_old('c', 1), var_old('c', 0),var_old('c', -1), var_old(),var_old(),var_old()
        c2.f(zero, a, '-')
        c1.exp(c2) #exp(-x)
        c.f(c1,one,'+') #1+exp(-x)
        out.f(one, c, '/')
        return out


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
    def previewDeriv(self):
        print(self.thisptr.previewDeriv().decode('utf8'))
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
        if self.thisptr.type == matrix_old:
            cfloatses = <float **> malloc(len(pyfloats)*cython.sizeof(float))
            for grop in range(len(pyfloats)):
                cfloats = <float *> malloc(len(pyfloats[grop])*cython.sizeof(float))
                if cfloats is NULL:
                    raise MemoryError()
                for i in range(len(pyfloats[grop])):
                    cfloats[i] = pyfloats[grop][i]
                cfloatses[grop] = cfloats
            return self.thisptr.getValue(cfloatses,len(pyfloats))
        cfloats = <float *> malloc(len(pyfloats)*cython.sizeof(float))
        if cfloats is NULL:
            raise MemoryError()
        for i in range(len(pyfloats)):
            cfloats[i] = pyfloats[i]
        return(self.thisptr.getValue(cfloats))
    def feed(self, floatses):
        cfloatses = <float **> malloc(len(floatses)*cython.sizeof(float))
        for gr in range(len(floatses)):
            cfloats = <float *> malloc(len(floatses[gr])*cython.sizeof(float))
            if cfloats is NULL:
                raise MemoryError()
            for i in range(len(floatses[gr])):
                cfloats[i] = floatses[gr][i]
            cfloatses[gr] = cfloats
        cdef float* glob = self.thisptr.feed(cfloatses, len(floatses))
        out = []
        for i in range(len(floatses)):
            out.append(glob[i])
        return out
    def type(self):
        if self.thisptr.type == constant_old:
            print('constant')
        if self.thisptr.type == independent_old:
            print('independent')
        if self.thisptr.type == function_old:
            print('function')
        if self.thisptr.type == matrix_old:
            print('matrix')
        if self.thisptr.type == special_old:
            print('special')
        print(self.thisptr.type)
    def c(self, float x):
        self.thisptr.c(x)
    def i(self):
        self.thisptr.i()
    def f(self, var_old a, var_old b, inop):
        cdef string s
        s = str.encode(inop)
        cdef Variable_old *l = &a.thisptr
        cdef Variable_old *r = &b.thisptr
        print(self.thisptr.f(l, r, &s).decode('utf8'))
    def exp(self, var_old a):
        inop = "exp"
        cdef string s
        s = str.encode(inop)
        cdef Variable_old *l = &a.thisptr
        print(self.thisptr.f(l, &s).decode('utf8'))
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
    def m(self, var_old a):
        cdef Variable_old *l = &a.thisptr
        self.thisptr.m(l)
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
            
            
        

