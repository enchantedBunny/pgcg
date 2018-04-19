from eigency.core cimport *

include "variable.pyx"
include "variable_old.pyx"

def differeniate_c(var vara,var varb,np.ndarray npist):
    global it
    it += 1
    cdef Variable *l = vara.thisptr
    cdef Variable *r = varb.thisptr
    #same as differeniate but in c with eigen
    out =  ndarray(differentiate(l,r, Map[MatrixXd](npist), it));
    it +=100
    return out


#var storing current print setting, quiet mode doesn't print anything except errors, executive mode doesn't even show errors
mode = "loud"

#debug print fuction for strings
cdef public void cPrint(string s):
    if mode =="quiet" or mode == "executive":
        return
    print "         ",s.decode('utf8')
#debug print fuction for matrices
cdef public void cPrintM(MatrixXd i):
    if mode =="quiet" or mode == "executive":
        return
    print ndarray(i)
#raise error function
cdef public void cError(string s):
    if mode =="executive":
        return
    raise ValueError(s.decode('utf8')) 
#sets mode
def setMode(n):
    global mode
    mode = n 
#returns mode
def getMode():
    global mode
    return mode