#include "parse_common.h"

#include <map>

#include "objparse.h"
#include "stlparse.h"

namespace meshparse {

using std::istream;
using std::map;
using std::pair;
using std::string;
using std::vector;

bool load_mesh(const string &filename, istream &input, mesh &mesh) {
    string ext = filename.substr(filename.length() - 4);
    if (ext == "stl") {
        return load_stl(input, mesh);
    } else if (ext == "obj") {
        return load_obj(input, mesh);
    }
    return false;
}

void merge_half_edges(mesh &mesh) {
    map<pair<unsigned int, unsigned int>, edge*> halfedges;
    for (vector<edge*>::const_iterator edge_iter = mesh.edges.begin();
            edge_iter != mesh.edges.end(); edge_iter++) {
        edge *e = *edge_iter;

        int vid1 = e->vert->id;
        edge *previous = e;
        while (previous->next != e) previous = previous->next;
        int vid2 = previous->vert->id;

        pair<unsigned int, unsigned int> vids =
            pair<unsigned int, unsigned int>(
                    vid1 < vid2 ? vid1 : vid2, vid1 < vid2 ? vid2 : vid1);

        map<pair<unsigned int, unsigned int>, edge*>::iterator
            mapval = halfedges.find(vids);

        if (mapval == halfedges.end()) {
            halfedges.insert(
                    pair<pair<unsigned int, unsigned int>, edge*>(vids, e));
        } else {
            edge *opposite = mapval->second;
            e->pair = opposite;
            opposite->pair = e;
        }
    }
}

} // namespace meshparse
