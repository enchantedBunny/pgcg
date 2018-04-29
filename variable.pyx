cimport cython

from libcpp.string cimport string
from libcpp.map cimport map
from libcpp cimport bool
import numpy as np
import inspect
cdef extern from "Variable.h" namespace "calc":
    cdef cppclass Variable:
        Variable(Map[MatrixXd] &);
        Variable(int r, int c);
        Variable(Variable *a, Variable *b, string *op);
        Variable(Variable *a, string *op, string *op);
        void setID(int bId);
        MatrixXd getValue(int it);
        void setValue(int, Map[MatrixXd] &);
        void setOpperand(int p)
        void show()
        void resetErr()
        int getErr()

cdef int b = 0
cdef int it = 0

#returns derivative of vara with respect to varb. npist should be the point at which. All other 'variables' in tree should have value
def differentiate(vara,varb,npist):
    h = 0.00000001
    npistF = np.copy(npist)
    for (x,y), value in np.ndenumerate(npist):
        npist2 = np.copy(npist)
        npist2[x][y] += h
        npistF[x][y]= (sum(vara.value(dict([(varb,npist2)]))) - sum(vara.value(dict([(varb,npist)]))))/h
    return npistF

#one call for all function variable inits
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
    print("I don't get it")
    
#call for all constant inits
def constant(arr):
    g = var("constant", arr)
    return g
#one call for all variable inits
def variable(rows, cols):
    g = var("independent", rows, cols)
    return g

cdef class var:
    cdef:
        Variable *thisptr
        t #stores type
        t2 #stores function type (if it's a function that is)
        int ID
        var va #left variable in the tree (functions only)
        var vb #right variable in the tree (functions only)
        inop #operation (functions only)
    #general init function, initialises constants and independents directly
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
    # init for regular functions with two inputs and an operation between them
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
    #init for functions with only one input and an operation
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
    #inito for functions with only one input, one opperand and an operation
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
    #getter for ID
    def getID(self):
        return self.ID
    #getter for value
    def value(self, dIn=None,):
        self.thisptr.resetErr()
        if dIn != None:
            for key in dIn:
                self.thisptr.setValue(key.getID(), Map[MatrixXd](dIn[key]))
        if self.thisptr.getErr()<0:
            global it
            it += 1
            out = ndarray(self.thisptr.getValue(it))
            return out
    #returns a new variable that is a transposed version of this one
    def T(self):
        g = var("function")
        g.sf(self, "transpose")
        return g