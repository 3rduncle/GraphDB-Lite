#ifndef MINI_PYTHON_BUILDIN_NUMBER_H
#define MINI_PYTHON_BUILDIN_NUMBER_H

#include "type_object.h"
#include "operator_helper.h"

namespace ipython {

const double EPS = 1e-8;
/**********************/
struct NumberAdd : public MultiTypeBinaryFunc {
    NumberAdd();
};

struct NumberSub : public MultiTypeBinaryFunc {
    NumberSub();
};

struct NumberMul : public MultiTypeBinaryFunc {
    NumberMul();
};

struct NumberDiv : public MultiTypeBinaryFunc {
    NumberDiv();
};

struct NumberEqual : public MultiTypeBinaryFunc {
    NumberEqual();
};

struct NumberLess : public MultiTypeBinaryFunc {
    NumberLess();
};

struct NumberGreat : public MultiTypeBinaryFunc {
    NumberGreat();
};

struct IntType : public TypeObject {
    IntType();
};

struct DoubleType : public TypeObject {
    DoubleType();
};

}
#endif
