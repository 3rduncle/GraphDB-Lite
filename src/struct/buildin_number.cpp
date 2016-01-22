#include "struct.h"

namespace ipython {

typedef GeneralObject<int> IntObject;
typedef std::shared_ptr<IntObject> IntPtr;
typedef GeneralObject<double> DoubleObject;
typedef std::shared_ptr<DoubleObject> DoublePtr;

template<>
struct EqualHelper<double> {
    any operator()(const any& l, const any& r) {
        double& dp1 = l.get_refer<double>();
        double& dp2 = r.get_refer<double>();
        if (fabs(dp1 - dp2) < EPS) {
            return true;
        }
        return false;
    }
};

template<>
struct LessHelper<double> {
    any operator()(const any& l, const any& r) {
        double& dp1 = l.get_refer<double>();
        double& dp2 = r.get_refer<double>();
        if (dp1 - dp2 < -EPS) {
            return true;
        }
        return false;
    }
};

template<>
struct GreatHelper<double> {
    any operator()(const any& l, const any& r) {
        double& dp1 = l.get_refer<double>();
        double& dp2 = r.get_refer<double>();
        if (dp1 - dp2 > EPS) {
            return true;
        }
        return false;
    }
};


NumberAdd::NumberAdd() {
    operator_register<double, int>(AddHelper<double, int>());
    operator_register<double, double>(AddHelper<double>());
    operator_register<int, double>(AddHelper<int, double>());
    operator_register<int, int>(AddHelper<int>());
}

NumberSub::NumberSub() {
    operator_register<double, int>(SubHelper<double, int>());
    operator_register<double, double>(SubHelper<double>());
    operator_register<int, double>(SubHelper<int, double>());
    operator_register<int, int>(SubHelper<int>());
}

NumberMul::NumberMul() {
    operator_register<double, int>(MulHelper<double, int>());
    operator_register<double, double>(MulHelper<double>());
    operator_register<int, double>(MulHelper<int, double>());
    operator_register<int, int>(MulHelper<int>());
}

NumberDiv::NumberDiv() {
    operator_register<double, int>(DivHelper<double, int>());
    operator_register<double, double>(DivHelper<double>());
    operator_register<int, double>(DivHelper<int, double>());
    operator_register<int, int>(DivHelper<int>());
}

NumberEqual::NumberEqual() {
    operator_register<double, int>(EqualHelper<double, int>());
    operator_register<double, double>(EqualHelper<double>());
    operator_register<int, double>(EqualHelper<int, double>());
    operator_register<int, int>(EqualHelper<int>());
}

NumberLess::NumberLess() {
    operator_register<double, int>(LessHelper<double, int>());
    operator_register<double, double>(LessHelper<double>());
    operator_register<int, double>(LessHelper<int, double>());
    operator_register<int, int>(LessHelper<int>());
}

NumberGreat::NumberGreat() {
    operator_register<double, int>(GreatHelper<double, int>());
    operator_register<double, double>(GreatHelper<double>());
    operator_register<int, double>(GreatHelper<int, double>());
    operator_register<int, int>(GreatHelper<int>());
}

IntType::IntType() {
    tp_print = PrintHelper<int>();
    tp_string = ToStringHelper<int>();
    tp_hash = HashHelper<int>();

    tp_logic.equal = NumberEqual();
    tp_logic.less = NumberLess();
    tp_logic.great = NumberGreat();

    tp_as_number.nb_nonzero = [](const any& x) {
        return x.get_refer<int>() == 0;
    };
    tp_as_number.nb_add = NumberAdd();
    tp_as_number.nb_sub = NumberSub();
    tp_as_number.nb_mul = NumberMul();
    tp_as_number.nb_div = NumberDiv();
}

DoubleType::DoubleType() {
    tp_print = PrintHelper<double>();
    tp_string = ToStringHelper<double>();
    tp_hash = HashHelper<double>();

    tp_logic.equal = NumberEqual();
    tp_logic.less = NumberLess();
    tp_logic.great = NumberGreat();

    tp_as_number.nb_nonzero = [](const any& x) {
        return fabs(x.get_refer<double>()) < EPS;
    };
    tp_as_number.nb_add = NumberAdd();
    tp_as_number.nb_sub = NumberSub();
    tp_as_number.nb_mul = NumberMul();
    tp_as_number.nb_div = NumberDiv();
}

}
