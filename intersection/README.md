## Execute the file "example.py" to obtain the intersection between the fascicles with the cortical meshing. 
## Inside the file there is the path of the fascicles and the mesh in this layer, which can be modified to try with another subject. 
## The "example.py" calls the Makefile to perform the intersection calculation in C++ in a parallel way.

# Then the folder "/intersection/" is generated, which contains the intersection data of each issue with each hemisphere stored in binary files, which are:

- Index of the triangle that intersects with the initial end of the fiber.
- Index of the triangle that intersects with the end of the fiber.
- Exact point of intersection with the initial end of the fiber.
- Exact point of intersection with the endpoint of the fiber.
- Index of the fibre intersecting with the triangles.
