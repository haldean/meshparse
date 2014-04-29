meshparse
=========

Parse various mesh file formats into a half-edge data structure. To load a mesh,
create a mesh object and pass it to `load_mesh`, along with the name of the
input file and an istream for the file. The filename is used to determine which
mesh parser to use; if you know the format of your input, you can also pass in
the file extension of that format with a prepended dot, like ".obj". Currently
supported formats are:

- Wavefront OBJ (.obj)
- ASCII STL (.stl)
- Binary STL (.stl)

What's a half-edge data structure?
--

Half-edge structures are used to encode polygonal meshes in a way that is
reasonably memory efficient without losing any sort of connectivity information.
The three primary mesh primitives are verteces, edges and faces.

Verteces contain a location, a normal, and a pointer to an edge that "points" at
them. Edges have a vertex that they point at, a face, an edge that's next on the
polygon and a "pair" edge. Faces just have pointers to one of their edges.

Each edge belongs uniquely to one face. For edges that are conceptually shared
by two faces, two half-edges are created with their "pair" pointers initialized
to each other.

More information can be found in [this blog post][source1] or [these lecture
notes][source2].

[source1]: http://fgiesen.wordpress.com/2012/02/21/half-edge-based-mesh-representations-theory/
[source2]: http://mrl.nyu.edu/~dzorin/ig04/lecture24/meshes.pdf
