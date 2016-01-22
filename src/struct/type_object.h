#ifndef MINI_PYTHON_TYPE_OBJECT_H
#define MINI_PYTHON_TYPE_OBJECT_H

#include <string>
#include <functional>
#include <memory>

namespace ipython {

class Object;
class any;
//--------------类型操作------------------------
typedef std::function<void (const any&)> VoidOfUnaryFunc;
typedef std::function<bool (const any&)> Inquiry;
typedef std::function<int (const any&)> LenFunc;
typedef std::function<std::string (const any&)> StrOfUnaryFunc;
typedef std::function<size_t (const any&)> HashOfUnaryFunc;
typedef std::function<any (const any&)> UnaryFunc;
typedef std::function<any (const any&, const any&)> BinaryFunc;
typedef std::shared_ptr<Object> ObjectPtr;

struct LogicMethods {
    BinaryFunc equal;
    BinaryFunc less;
    BinaryFunc great;
};

struct NumberMethods {
    Inquiry nb_nonzero;
    BinaryFunc nb_add;
    BinaryFunc nb_sub;
    BinaryFunc nb_mul;
    BinaryFunc nb_div;
};

struct SequenceMethods {
    LenFunc sq_length;
};

struct MappingMethods {
    LenFunc mp_length;
    BinaryFunc mp_subscript;
};

struct TypeObject {
    virtual ~TypeObject() {}
    std::string tp_name;
    VoidOfUnaryFunc tp_print;
    StrOfUnaryFunc tp_repr;
    StrOfUnaryFunc tp_string;     //to raw string
    HashOfUnaryFunc tp_hash;
    LogicMethods tp_logic;
    NumberMethods tp_as_number;
    SequenceMethods tp_as_sequence;
    MappingMethods tp_as_mapping;
};

}
#endif
