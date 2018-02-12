cimport cython
from eigency.core cimport *
from libcpp.string cimport string
from libcpp.map cimport map
import numpy as np
import inspect
cdef extern from "Variable.h" namespace "calc":
    cdef cppclass Variable:
        Variable(Map[MatrixXd]);
        Variable(int r, int c);
        Variable(Variable *a, Variable *b, string *op);
        Variable(Variable *a, string *op, string *op);
        void setID(int bId);
        MatrixXd getValue();
        void setValue(int, Map[MatrixXd] &);
        void setOpperand(int p)
        void show()
        void resetErr()
        int getErr()
cdef public void cPrint(string s):
    if mode =="quiet":
        return
    print "         ",s.decode('utf8')

cdef public void cError(string s):
    #raise ValueError(s.decode('utf8'))
    return
    
cdef public void cPrintM(MatrixXd i):
    if mode =="quiet":
        return
    print ndarray(i)
def setMode(self,n):
    self.mode = n 
mode = "quiet"
cdef int b = 0
def function(a, b, inop=None):
    g = var("function")
    if type(a) == var and type(b) ==var and inop!=None:
        g.f(a, b, inop)
        return g
    elif type(a) == var and type(b)==str and inop == None:
        g.sf(a, b)
        return g
    elif type(a) == var and type(b)==str and type(inop) == int:
        g.sf2(a, b, inop)
        return g
    #add a thing for pow and such 
    print("I don't get it")

def constant(arr):
    g = var("constant", arr)
    return g

def stochastic_constant(arr, rows):
    g = var("constant", arr)
    g.stochastic(rows)
    return g


def variable(rows, cols):
    g = var("independent", rows, cols)
    return g
    
cdef class var:
    cdef:
        Variable *thisptr
        t
        t2
        int ID
        var va
        var vb
        inop
    def __cinit__(self,type, array=None, cols = None):
        self.t = type
        if type == "constant":
            self.thisptr = new Variable(Map[MatrixXd](array))
        elif type == "independent":
            self.thisptr = new Variable(array, cols)
        else:
            return
        global b
        b +=1
        self.thisptr.setID(b)
        self.ID = b
    def f(self, var va, var vb, inop):
        self.va = va
        self.vb = vb
        self.inop = inop
        self.t2 = "two"
        cdef string s
        s = str.encode(inop)
        cdef Variable *l = va.thisptr
        cdef Variable *r = vb.thisptr
        self.thisptr = new Variable(l, r, &s)
        global b
        b +=1
        self.thisptr.setID(b)
        self.ID = b
    def sf(self, var va, inop):
        self.va = va
        self.inop = inop
        self.t2 = "one"
        cdef string s
        s = str.encode(inop)
        cdef Variable *l = va.thisptr
        self.thisptr = new Variable(l, &s, &s)
        global b
        b +=1
        self.thisptr.setID(b)
        self.ID = b
    def sf2(self, var va, inop, g):
        self.va = va
        self.inop = inop
        self.t2 = "one_plus"
        cdef string s
        s = str.encode(inop)
        cdef Variable *l = va.thisptr
        self.thisptr = new Variable(l, &s, &s)
        self.thisptr.setOpperand(g);
        global b
        b +=1
        self.thisptr.setID(b)
        self.ID = b
    def stochastic(self,r):
        self.thisptr.setOpperand(r);
    def getID(self):
        return self.ID
    def value(self, dIn=None, egg = True):
        self.thisptr.resetErr()
        if dIn != None:
            for key in dIn:
                self.thisptr.setValue(key.getID(), Map[MatrixXd](dIn[key]))
        if self.thisptr.getErr()>0:
            egg = False;
        if egg:
            return ndarray(self.thisptr.getValue())
    def T(self):
        g = var("function")
        g.sf(self, "transpose")
        return g
    def deriv(self, g):
        g = var("function")
        if self.t2 == "one":
            g.sf(self, self.inop + "_r")
        return g
    def preview(self):
        self.thisptr.show()
        
