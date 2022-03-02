from setuptools import Extension
from Cython.Build import cythonize

extensions = [
    Extension("intersection.src", sources=[
        "src/cython/get_intersection.pyx",
        "src/intersection.cpp",
        "src/io.cpp",
        "src/orientation.cpp",
        ],  extra_compile_args=["-O0"])
]

def build(setup_kwargs):
    setup_kwargs.update({
        'ext_modules': cythonize(extensions, language_level=3)
    })