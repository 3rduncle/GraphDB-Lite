#ifndef MINI_PYTHON_BUILDIN_DICT_H
#define MINI_PYTHON_BUILDIN_DICT_H

#include "type_object.h"

namespace egdb {
namespace detail {

struct DictType : public TypeObject {
    DictType();
};

}
}
#endif
