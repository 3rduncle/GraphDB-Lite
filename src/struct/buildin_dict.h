#ifndef MINI_PYTHON_BUILDIN_DICT_H
#define MINI_PYTHON_BUILDIN_DICT_H

#include "type_object.h"

namespace ipython {

struct DictType : public TypeObject {
    DictType();
};

}
#endif
