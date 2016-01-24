#ifndef MINI_PYTHON_BUILDIN_BOOLEAN_H
#define MINI_PYTHON_BUILDIN_BOOLEAN_H

#include "type_object.h"

namespace egdb {
namespace detail {

struct BooleanType : public TypeObject {
    BooleanType();
};

}
}
#endif
