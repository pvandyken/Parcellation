from setuptools import Extension
from pybind11.setup_helpers import Pybind11Extension

extensions = [
    Pybind11Extension(
        "intersection.intersection",
        sources=[
            "src/intersection.cpp",
            "src/bundleIntersection.cpp",
            "src/io.cpp",
            "src/orientation.cpp",
            "src/bundles.cpp",
            "src/mesh.cpp",
            "bindings/entry.cpp"
        ],
        include_dirs = [
            "extern/pybind11/include"
        ],
        extra_compile_args=["-O0"],
    )
]

def build(setup_kwargs):
    setup_kwargs.update({
        'ext_modules': extensions
    })