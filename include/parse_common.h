// Functions that are shared across various format parsers.

#ifndef __TOOLPATH_PARSE_COMMON_H__
#define __TOOLPATH_PARSE_COMMON_H__

#include <istream>

#include "mesh.h"

// loads a mesh, guessing the appropriate loader from the file name.
bool load_mesh(const std::string &filename, std::istream &input, mesh &mesh);

// set the pair pointer on edges by finding their buddy.
void merge_half_edges(mesh &mesh);

#endif
