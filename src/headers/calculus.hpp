//author: John Manning

#ifndef CALCULUS
#define CALCULUS
#include "common.h"
#include "Cell.hpp"

typedef std::vector<std::vector<Cell>> CellField;
// calculus method definitions. 


//these will not be void 4 long

extern glm::vec2 gradient(const CellField cellField, int i, int j, int dx, int dy);
extern void divergence();
extern void laplacian();


#endif
