from eigency.core cimport *

include "variable.pyx"
include "variable_old.pyx"

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