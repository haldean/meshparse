#include "mesh.h"

#include <iostream>
#include <istream>

#include "objparse.h"
#include "stlparse.h"

namespace meshparse {

using std::cout;
using std::endl;
using std::istream;
using std::string;

bool load_mesh(const string &filename, istream &input, mesh &mesh) {
    string ext = filename.substr(filename.find_last_of('.') + 1);
    if (ext == "stl") {
        return load_stl(input, mesh);
    } else if (ext == "obj") {
        return load_obj(input, mesh);
    }
    cout << "Unknown file type " << ext << endl;
    return false;
}

}
