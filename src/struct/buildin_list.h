#ifndef MINI_PYTHON_BUILDIN_LIST_H
#define MINI_PYTHON_BUILDIN_LIST_H

#include "type_object.h"

namespace ipython {

struct ListType : public TypeObject {
    ListType();
};

}
#endif
