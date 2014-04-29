#include "mesh.h"

using std::cout;
using std::endl;
using std::vector;

vertex::vertex() {
    loc.setZero();
    normal.setZero();
}

/* This is here to allow for easy breakpoints to find places where copying takes
 * place. */
inline void vertex_copy() {
    cout << "vertex copied" << endl;
}

inline void mesh_copy() {
    cout << "mesh copied" << endl;
}

vertex::vertex(const vertex &other) {
#ifdef COPY_CHECKS
    vertex_copy();
#endif
    loc = other.loc;
    normal = other.normal;
    e = other.e;
    id = other.id;
}

bool vertex::operator==(const vertex &other) {
    return loc == other.loc;
}

bool vertex::onboundary() const {
    edge *eloop = e;
    do eloop = eloop->next->pair; while (eloop != e && eloop != NULL);
    return eloop == NULL;
}

edge* edge::previous() const {
    edge *prev = next;
    while (prev->next != this) prev = prev->next;
    return prev;
}

/* If a vertex is non-manifold (meaning it isn't surrounded by faces) find the
 * first edge such that repeatedly calling edge->next->pair will get you all
 * edges associated with the vertex. */
edge* edge::rewind() const {
    if (pair == NULL) return (edge*) this;
    edge *e = pair->previous();
    while (e != this && e->pair != NULL) e = e->pair->previous();
    return e;
}

Vector3f edge::as_vector() const {
    return vert->loc - previous()->vert->loc;
}

Vector3f edge::midpoint() const {
    return vert->loc - 0.5 * as_vector();
}

float edge::angle_between(const edge& other) const {
    Vector3f e1 = as_vector(), e2 = other.as_vector();
    return e1.cross(e2).norm();
}

face::face() {
    normal.setZero();
    e = NULL;
    id = 0;
}

face::face(const face &other) {
    normal = other.normal;
    id = other.id;
    e = other.e;
}

int face::sides() const {
    int sides = 0;
    edge *e0 = e;
    do {
        sides++;
        e0 = e0->next;
    } while (e0 != e);
    return sides;
}

float face::area() const {
    float area = 0;
    edge *e0 = e;
    Vector3f v0, v1;
    do {
        v0 = e0->vert->loc;
        v1 = e0->next->vert->loc;
        area += v0.cross(v1).norm();

        e0 = e0->next;
    } while (e0 != e);
    return area / 2;
}

Vector3f face::centroid() const {
    Vector3f centroid = Vector3f::Zero();

    edge *e0 = e;
    int n = 0;
    do {
        centroid += e0->vert->loc;
        n++;
        e0 = e0->next;
    } while (e0 != e);
    centroid /= n;

    return centroid;
}

void face::calculate_normal() {
    normal = e->as_vector().cross(e->next->as_vector());
    normal.normalize();
}

mesh::mesh() {
}

/* Deep copies with pointers are NO FUN. */
mesh::mesh(const mesh &other) {
#ifdef COPY_CHECKS
    mesh_copy();
#endif
    faces = vector<face*>(other.faces.size(), NULL);
    edges = vector<edge*>(other.edges.size(), NULL);
    verteces = vector<vertex*>(other.verteces.size(), NULL);

    for (auto vit = other.verteces.begin(); vit != other.verteces.end(); vit++) {
        vertex *v = new vertex();
        v->id = (*vit)->id;
        v->loc = (*vit)->loc;
        v->normal = (*vit)->normal;
        v->e = NULL;
        verteces[(*vit)->id - 1] = v;
    }

    for (auto fit = other.faces.begin(); fit != other.faces.end(); fit++) {
        face *newf = new face();
        newf->id = (*fit)->id;
        newf->normal = (*fit)->normal;
        newf->e = NULL;
        faces[newf->id - 1] = newf;
    }

    for (auto eit = other.edges.begin(); eit != other.edges.end(); eit++) {
        edge *newe = new edge();
        newe->id = (*eit)->id;

        newe->vert = verteces[(*eit)->vert->id - 1];
        if (newe->vert->e == NULL) newe->vert->e = newe;

        newe->f = faces[(*eit)->f->id - 1];
        edges[newe->id - 1] = newe;
    }

    for (auto eit = other.edges.begin(); eit != other.edges.end(); eit++) {
        edge *e = *eit;
        edges[e->id - 1]->next = edges[e->next->id - 1];
        if (e->pair != NULL) {
            edges[e->id - 1]->pair = edges[e->pair->id - 1];
        }
    }

    for (auto fit = other.faces.begin(); fit != other.faces.end(); fit++) {
        faces[(*fit)->id - 1]->e = edges[(*fit)->e->id - 1];
        faces[(*fit)->id - 1]->calculate_normal();
    }
}

void mesh::calculate_normals(normal_mode mode) {
    if (mode == NO_NORMALS) {
        for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
            (*vit)->normal.setZero();
        }
    } else {
        for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
            (*vit)->normal.setZero();
        }

        for (auto fit = faces.begin(); fit != faces.end(); fit++) {
            (*fit)->calculate_normal();
            edge *e = (*fit)->e;
            do {
                e->vert->normal += (*fit)->normal;
                e = e->next;
            } while (e != (*fit)->e);
        }

        for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
            (*vit)->normal.normalize();
        }
    }
}

bounds mesh::get_bounds() const {
    bounds b;
    b.min_x = INFINITY;
    b.min_y = INFINITY;
    b.min_z = INFINITY;
    b.max_x = -INFINITY;
    b.max_y = -INFINITY;
    b.max_z = -INFINITY;

    for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
        Vector3f loc = (*vit)->loc;
        if (loc[0] < b.min_x) b.min_x = loc[0];
        if (loc[0] > b.max_x) b.max_x = loc[0];
        if (loc[1] < b.min_y) b.min_y = loc[1];
        if (loc[1] > b.max_y) b.max_y = loc[1];
        if (loc[2] < b.min_z) b.min_z = loc[2];
        if (loc[2] > b.max_z) b.max_z = loc[2];
    }

    return b;
}

void mesh::scale_to_unit_cube() {
    bounds b = get_bounds();
    Vector3f translate(
            b.min_x - (b.min_x - b.max_x) / 2,
            b.min_y - (b.min_y - b.max_y) / 2,
            b.min_z - (b.min_z - b.max_z) / 2);
    float scale = 1. / fmax(
            b.max_x - b.min_x, fmax(b.max_y - b.min_y, b.max_z - b.min_z));

    for (auto vit = verteces.begin(); vit != verteces.end(); vit++) {
        (*vit)->loc -= translate;
        (*vit)->loc *= scale;
    }
}

bool vector_comparitor::operator()(const Vector3f &v1, const Vector3f &v2) const {
    float x_diff = v1[0] - v2[0];
    if (x_diff < -CMP_EPSILON) {
        return true;
    } else if (x_diff > CMP_EPSILON) {
        return false;
    }

    float y_diff = v1[1] - v2[1];
    if (y_diff < -CMP_EPSILON) {
        return true;
    } else if (y_diff > CMP_EPSILON) {
        return false;
    }

    float z_diff = v1[2] - v2[2];
    if (z_diff < -CMP_EPSILON) {
        return true;
    } else if (z_diff > CMP_EPSILON) {
        return false;
    }

    return false;
}
