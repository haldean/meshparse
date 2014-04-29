// Functions that are shared across various format parsers.

#ifndef __TOOLPATH_PARSE_COMMON_H__
#define __TOOLPATH_PARSE_COMMON_H__

#include <istream>

#include "mesh.h"

namespace meshparse {
    // set the pair pointer on edges by finding their buddy.
    void merge_half_edges(mesh &mesh);
}

#endif
