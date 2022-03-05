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
            "extern/pybind11/include",
            "/usr/local/include"
        ],
        extra_compile_args=["-O3", "-fPIC"],
        # extra_link_args=["-fopenmp"]
    )
]



def build(setup_kwargs):
    setup_kwargs.update({
        # 'ext_modules': [CMakeExtension("intersection.test", Path(__file__).parent.absolute())],
        'ext_modules': extensions,
    })