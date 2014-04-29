#include "objparse.h"

#include <cstdio>
#include <iterator>
#include <vector>
#include <map>
#include <iostream>

#include <Eigen/Dense>
#include <re2/re2.h>

#include "parse_common.h"

#define DONE 0
#define BAD_FILE 1
#define END_SOLID 2

#pragma GCC visibility push(hidden)

namespace meshparse {

using namespace Eigen;
using namespace re2;

using std::cout;
using std::endl;
using std::istream;
using std::map;
using std::pair;
using std::stod;
using std::string;
using std::vector;

// may god have mercy on my soul.
static RE2 empty_re("^[\\s]*$");
static RE2 endface_re("^[\\s]*endfacet[\\s]*$");
static RE2 endloop_re("^[\\s]*endloop[\\s]*$");
static RE2 endsolid_re("^[\\s]*endsolid[\\s]*([\\w]*)[\\s]*$");
static RE2 facet_re("^[\\s]*facet[\\s]+normal[\\s]+([\\d.e-]+)[\\s]+([\\d.e-]+)[\\s]+([\\d.e-]+)[\\s]*$");
static RE2 oloop_re("^[\\s]*outer loop[\\s]*$");
static RE2 solid_re("^[\\s]*solid[\\s]*([\\w]*)[\\s]*$");
static RE2 vertex_re("^[\\s]*vertex[\\s]+([\\d.e-]+)[\\s]+([\\d.e-]+)[\\s]+([\\d.e-]+)[\\s]*$");

typedef struct {
    uint32_t verts[3];
    Vector3f normal;
} stlface;

typedef struct {
    uint32_t next_vertid;
    map<Vector3f, uint32_t, vector_comparitor> vert_to_id;
    vector<stlface> faces;

} stlmesh;

bool check_pattern_compilation() {
    bool ok = true;
    ok &= empty_re.ok();
    ok &= endface_re.ok();
    ok &= endloop_re.ok();
    ok &= endsolid_re.ok();
    ok &= facet_re.ok();
    ok &= oloop_re.ok();
    ok &= solid_re.ok();
    ok &= vertex_re.ok();
    return ok;
}

string next_nonempty_line(istream &file) {
    string line;
    while (file.good() && RE2::FullMatch(line, empty_re)) {
        getline(file, line);
    }
    return line;
}

int try_parse_face(istream& file, stlmesh &stlm) {
    string line = next_nonempty_line(file);
    if (RE2::FullMatch(line, empty_re)) {
        cout << "File ended in middle of solid." << endl;
        return BAD_FILE;
    }
    if (RE2::FullMatch(line, endsolid_re)) {
        return END_SOLID;
    }

    stlface face;
    string nxs, nys, nzs;
    if (RE2::FullMatch(line, facet_re, &nxs, &nys, &nzs)) {
        face.normal = Vector3f(stod(nxs), stod(nys), stod(nzs));
    } else {
        cout << "Expecting face, got " << line << endl;
        return BAD_FILE;
    }

    line = next_nonempty_line(file);
    if (RE2::FullMatch(line, empty_re)) {
        cout << "File ended in middle of face." << endl;
        return BAD_FILE;
    }
    if (!RE2::FullMatch(line, oloop_re)) {
        cout << "Expecting outer loop, got " << line << endl;
        return BAD_FILE;
    }

    for (int i = 0; i < 3; i++) {
        line = next_nonempty_line(file);
        if (RE2::FullMatch(line, empty_re)) {
            cout << "File ended in middle of face." << endl;
            return BAD_FILE;
        }
        string vx, vy, vz;
        if (RE2::FullMatch(line, vertex_re, &vx, &vy, &vz)) {
            Vector3f vec(stod(vx), stod(vy), stod(vz));
            uint32_t id;
            auto idp = stlm.vert_to_id.find(vec);
            if (idp == stlm.vert_to_id.end()) {
                id = stlm.next_vertid++;
                stlm.vert_to_id.insert(pair<Vector3f, uint32_t>(vec, id));
            } else {
                id = idp->second;
            }
            face.verts[i] = id;
        } else {
            cout << "Expected vertex, got " << line << endl;
            return BAD_FILE;
        }
    }
    stlm.faces.push_back(face);

    line = next_nonempty_line(file);
    if (RE2::FullMatch(line, empty_re)) {
        cout << "File ended in middle of face." << endl;
        return BAD_FILE;
    }
    if (!RE2::FullMatch(line, endloop_re)) {
        cout << "Expecting outer loop end, got " << line << endl;
        return BAD_FILE;
    }

    line = next_nonempty_line(file);
    if (RE2::FullMatch(line, empty_re)) {
        cout << "File ended in middle of face." << endl;
        return BAD_FILE;
    }
    if (!RE2::FullMatch(line, endface_re)) {
        cout << "Expecting facet end, got " << line << endl;
        return BAD_FILE;
    }

    return DONE;
}

void stl_to_mesh(stlmesh &stlm, mesh &mesh) {
    map<uint32_t, vertex*> verts;
    for (auto stlv = stlm.vert_to_id.begin(); stlv != stlm.vert_to_id.end(); stlv++) {
        Vector3f loc = stlv->first;
        uint32_t id = stlv->second;
        vertex *v = new vertex();
        v->id = id;
        v->loc = loc;
        v->normal.setZero();
        verts.insert(pair<uint32_t, vertex*>(id, v));
        mesh.verteces.push_back(v);
    }

    int next_fid = 0;
    int next_eid = 0;
    for (auto stlf = stlm.faces.begin(); stlf != stlm.faces.end(); stlf++) {
        face *f = new face();
        f->id = next_fid++;
        f->normal = stlf->normal;

        edge *e1 = new edge(),
             *e2 = new edge(),
             *e3 = new edge();

        e1->id = next_eid++;
        e2->id = next_eid++;
        e3->id = next_eid++;

        e1->next = e2;
        e2->next = e3;
        e3->next = e1;

        e1->f = f;
        e2->f = f;
        e3->f = f;

        vertex *v1 = verts.find(stlf->verts[0])->second,
               *v2 = verts.find(stlf->verts[1])->second,
               *v3 = verts.find(stlf->verts[2])->second;

        e1->vert = v1;
        v1->e = e1;
        e2->vert = v2;
        v2->e = e2;
        e3->vert = v3;
        v3->e = e3;

        f->e = e1;

        mesh.edges.push_back(e1);
        mesh.edges.push_back(e2);
        mesh.edges.push_back(e3);
        mesh.faces.push_back(f);
    }
}

bool load_stl(istream& file, mesh &mesh) {
    if (!check_pattern_compilation()) {
        cout << "Regular expressions didn't compile, abort." << endl;
        return false;
    }

    if (!file.good()) {
        cout << "Could not read file." << endl;
        return false;
    }

    // find the start of the solid
    string line;
    string name;
    while (file.good()) {
        getline(file, line);
        if (RE2::FullMatch(line, solid_re, &name)) {
            cout << "Found solid " << name << endl;
            break;
        }
    }

    stlmesh stlm;
    stlm.next_vertid = 0;

    // parse the solid itself
    while (file.good()) {
        int res = try_parse_face(file, stlm);
        if (res == DONE) {
            continue;
        } else if (res == BAD_FILE) {
            cout << "Could not load mesh." << endl;
            return false;
        } else if (res == END_SOLID) {
            break;
        }
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
