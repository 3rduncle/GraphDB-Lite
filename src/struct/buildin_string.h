#ifndef MINI_PYTHON_BUILDIN_STRING_H
#define MINI_PYTHON_BUILDIN_STRING_H

#include "type_object.h"

namespace ipython {

struct StringType : public TypeObject {
    StringType();
};

}
#endif
