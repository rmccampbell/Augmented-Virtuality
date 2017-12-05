import os, time, ctypes
from ctypes import *

os.chdir(r'C:\Users\Ryan\Documents\3D\proj3\Augmented-Virtuality\OpenCVTest2\x64\Release')

dll = cdll.LoadLibrary('OpenCVTest2.dll')

class Circle(Structure):
    _fields_ = [('X', c_int), ('Y', c_int), ('Radius', c_int)]

dll.Init.argtypes = [POINTER(c_int), POINTER(c_int)]
dll.Detect.argtypes = [POINTER(Circle), c_int, POINTER(c_int)]

w, h = c_int(), c_int()
print(dll.Init(byref(w), byref(h)))
print(w.value, h.value)

try:
    while 1:
        #time.sleep(.01)
        circles = (Circle * 5)()
        n = c_int()
        print(dll.Detect(circles, 5, byref(n)))
finally:
    dll.Close()
