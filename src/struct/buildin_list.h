#ifndef MINI_PYTHON_BUILDIN_LIST_H
#define MINI_PYTHON_BUILDIN_LIST_H

#include "type_object.h"

namespace egdb {
namespace detail {

struct ListType : public TypeObject {
    ListType();
};

}
}
#endif
