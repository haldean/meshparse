#include "mesh.h"

#include <iostream>
#include <fstream>

#include "objparse.h"
#include "stlparse.h"

namespace meshparse {

using std::cout;
using std::endl;
using std::ifstream;
using std::string;

bool load_mesh(const string &filename, ifstream &input, mesh &mesh) {
    string ext = filename.substr(filename.find_last_of('.') + 1);
    if (ext == "stl") {
        bool is_ascii = is_ascii_stl(input);
        input.seekg(0);
        if (is_ascii) {
            return load_ascii_stl(input, mesh);
        } else {
            return load_binary_stl(input, mesh);
        }
    } else if (ext == "obj") {
        return load_obj(input, mesh);
    }
    cout << "Unknown file type " << ext << endl;
    return false;
}

}
