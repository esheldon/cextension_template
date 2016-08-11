import distutils
from distutils.core import setup, Extension, Command
import os
import numpy

ext=Extension("cextension_template._example",
              ["cextension_template/_example.c"])

# you need to add this if using numpy
#include_dirs=[numpy.get_include()]
include_dirs=[]

setup(name="cextension_template",
      packages=['cextension_template'],
      version="0.9.0",
      ext_modules=[ext],
      include_dirs=include_dirs)




