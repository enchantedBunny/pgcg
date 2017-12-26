import numpy as np
import pyCalc as p
import random
l0 = p.var("function")
l1 = p.var("function")
l1_error = p.var("function")
sig = p.var("function")
l1_delta = p.var("function")
hlp = p.var("function")
tmp = p.var("function")
xs = p.var("constant",np.array([[1,1,1,1],
                               [0,1,0,1],
                               [0,0,1,1]]                           
                               , dtype= float))
ys = p.var("constant", np.array([0,0,1,1], dtype=float))
weights =np.array([[-0.43812108],
 [ 0.91094098],
 [0.83655703]], dtype = float)	
ws = p.var("independent", np.array([4,4]))
l0.f(ws, xs, "*")
l1.sf(l0, "sigmoid")
l1_error.f(ys, l1, "-")
sig.sf(l1, "sigmoid_r")
l1_delta.f(sig, l1_error, "*")
hlp.f(xs,l1_delta, "rowwise")
tmp.sf(hlp, "rowsum")
print("xs")
print(xs.value())
print("ws")
print(ws.value(dict([(ws,weights)])))
print("l0")
print(l0.value(dict([(ws,weights)])))
print("l1")
print(l1.value(dict([(ws,weights)])))
print("l1_error")
print(l1_error.value(dict([(ws,weights)])))
print("sig")
print(sig.value(dict([(ws,weights)])))
print("l1_delta")
print(l1_delta.value(dict([(ws,weights)])))
print("hlp")
print(hlp.value(dict([(ws,weights)])))
print("tmp")
print(tmp.value(dict([(ws,weights)])))

