#ifndef MINI_PYTHON_BUILDIN_BOOLEAN_H
#define MINI_PYTHON_BUILDIN_BOOLEAN_H

#include "type_object.h"

namespace ipython {

struct BooleanType : public TypeObject {
    BooleanType();
};

}
#endif
