#ifndef __STLPARSE_H__
#define __STLPARSE_H__

#include <istream>

#include "mesh.h"

namespace meshparse {
    bool load_stl(std::istream &input, mesh &mesh);
}

#endif
