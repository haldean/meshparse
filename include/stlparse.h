#ifndef __STLPARSE_H__
#define __STLPARSE_H__

#include <istream>

#include "mesh.h"

bool load_stl(istream& input, mesh& mesh);

#endif
