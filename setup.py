from setuptools import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = [
    Extension("intersection", sources=[
        "new_main.pyx",
        "intersection.cpp",
        ],  extra_compile_args=["-O0"], language="c++")
]

setup(
    name="intersection",
    ext_modules = cythonize(extensions),
)
