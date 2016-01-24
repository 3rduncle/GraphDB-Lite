#ifndef MINI_PYTHON_BUILDIN_STRING_H
#define MINI_PYTHON_BUILDIN_STRING_H

#include "type_object.h"

namespace egdb {
namespace detail {

struct StringType : public TypeObject {
    StringType();
};

}
}
#endif
