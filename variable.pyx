cimport cython
from eigency.core cimport *
from libcpp.string cimport string
import numpy as np
import inspect
cdef extern from "Variable.h" namespace "calc":
    cdef cppclass Variable:
        Variable(Map[MatrixXd]);
        Variable(int r, int c);
        Variable(Variable *a, Variable *b, string *op);
        Variable(Variable *a, string *op, string *blop);
        void setID(int bId);
        MatrixXd getValue(Map[MatrixXd]);

cdef public void cPrint(string s):
    print(s.decode('utf8'))
    return
cdef public void cPrintM(MatrixXd i):
    print ndarray(i)
    return
cdef int b = 0
#
#Learn how to pass string bitch
#
#
cdef class var:
    cdef:
        Variable *thisptr
        t
    def __cinit__(self,type, np.ndarray array=None):
        self.t = type
        if type == "constant":
            self.thisptr = new Variable(Map[MatrixXd](array))
        elif type == "independent":
            self.thisptr = new Variable(array[0],array[1])
        else:
            return
        global b
        b +=1
        self.thisptr.setID(b)
    def f(self, var va, var vb, inop):
        cdef string s
        s = str.encode(inop)
        cdef Variable *l = va.thisptr
        cdef Variable *r = vb.thisptr
        self.thisptr = new Variable(l, r, &s)
        global b
        b +=1
        self.thisptr.setID(b)
    def sf(self, var va, inop):
        cdef string s
        s = str.encode(inop)
        cdef Variable *l = va.thisptr
        self.thisptr = new Variable(l, &s, &s)
        global b
        b +=1
        self.thisptr.setID(b)
    def value(self, array):
        return ndarray(self.thisptr.getValue(Map[MatrixXd](array)))