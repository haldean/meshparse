#include "stlparse.h"

#include "strings.h"

namespace meshparse {

using std::istream;

bool is_ascii_stl(istream &input) {
    char buffer[6];
    input.read(buffer, 5);
    buffer[5] = '\0';
    return strncmp(buffer, "solid", 5) == 0;
}

}
