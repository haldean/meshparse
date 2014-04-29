#include "stlparse.h"

#include <fstream>
#include <vector>
#include <map>
#include <iostream>

#include <Eigen/Dense>

#include "parse_common.h"

#pragma GCC visibility push(hidden)

namespace meshparse {

using namespace Eigen;

using std::cout;
using std::endl;
using std::ifstream;
using std::ios_base;
using std::istream;
using std::pair;

inline uint32_t read_int(istream &file) {
    char buf32[4];
    file.read(buf32, 4);
    return *((uint32_t*) buf32);
}

inline float read_float(istream &file) {
    char buf32[4];
    file.read(buf32, 4);
    return *((float*) buf32);
}

void try_read_face(istream &file, stlmesh &stlm) {
    stlface face;
    face.normal[0] = read_float(file);
    face.normal[1] = read_float(file);
    face.normal[2] = read_float(file);

    for (int i = 0; i < 3; i++) {
        float vx = read_float(file);
        float vy = read_float(file);
        float vz = read_float(file);
        Vector3f vec(vx, vy, vz);

        uint32_t id;
        auto idp = stlm.vert_to_id.find(vec);
        if (idp == stlm.vert_to_id.end()) {
            id = stlm.next_vertid++;
            stlm.vert_to_id.insert(pair<Vector3f, uint32_t>(vec, id));
        } else {
            id = idp->second;
        }
        face.verts[i] = id;
    }

    file.seekg(2, ios_base::cur);
    stlm.faces.push_back(face);
}

bool load_binary_stl(ifstream &file, mesh &mesh) {
    file.seekg(80);
    const uint32_t tris = read_int(file);

    stlmesh stlm;
    for (int i = 0; i < tris; i++) {
        try_read_face(file, stlm);
    }

    stl_to_mesh(stlm, mesh);
    merge_half_edges(mesh);

    cout << "Loaded mesh: " << endl
        << "  " << mesh.verteces.size() << " verteces." << endl
        << "  " << mesh.edges.size() << " edges." << endl;
    return true;
}

} // namespace meshparse

#pragma GCC visibility pop
