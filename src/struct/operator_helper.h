#ifndef MINI_PYTHON_OPERATOR_HELPER_H
#define MINI_PYTHON_OPERATOR_HELPER_H

#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>
#include "type_object.h"
#include "any.h"

namespace ipython {

class MultiTypeBinaryFunc {
public:
    any operator()(const any& l, const any& r) {
        auto it =  _funcs.find(std::string(l.type().name()) + "," + r.type().name());
        if (it == _funcs.end()) {
            return any();
        }
        return it->second(l, r);
    }
    template<class Left, class Right>
    void operator_register(const BinaryFunc& func) {
        _funcs.insert(
            std::make_pair(
                std::string(typeid(Left).name()) + "," + typeid(Right).name(),
                func
            )
        );
    }
    template<class Left>
    void operator_register(const BinaryFunc& func) {
        operator_register<Left, Left>(func);
    }
private:
    std::unordered_map<std::string, BinaryFunc> _funcs;
};

template<class Left, class Right = Left>
struct AddHelper {
    any operator()(const any& l, const any& r) {
        if (l.type() != typeid(Left) || r.type() != typeid(Right)) {
            return any();
        }
        try {
            return l.get_refer<Left>() + r.get_refer<Right>();
        } catch (...) {
            return any();
        }
    }
};

template<class Left, class Right = Left>
struct SubHelper {
    any operator()(const any& l, const any& r) {
        if (l.type() != typeid(Left) || r.type() != typeid(Right)) {
            return any();
        }
        try {
            return l.get_refer<Left>() - r.get_refer<Right>();
        } catch (...) {
            return any();
        }
    }
};

template<class Left, class Right = Left>
struct MulHelper {
    any operator()(const any& l, const any& r) {
        if (l.type() != typeid(Left) || r.type() != typeid(Right)) {
            return any();
        }
        try {
            return l.get_refer<Left>() * r.get_refer<Right>();
        } catch (...) {
            return any();
        }
    }
};

template<class Left, class Right = Left>
struct DivHelper {
    any operator()(const any& l, const any& r) {
        if (l.type() != typeid(Left) || r.type() != typeid(Right)) {
            return any();
        }
        auto& inquiry = l.get_content()->_ob_type->tp_as_number.nb_nonzero;
        if (inquiry && !inquiry(l)) {
            return any();
        }
        try {
            return l.get_refer<Left>() / r.get_refer<Right>();
        } catch (...) {
            return any();
        }
    }
};

template<class Left, class Right = Left>
struct LessHelper {
    any operator()(const any& l, const any& r) {
        if (l.type() != typeid(Left) || r.type() != typeid(Right)) {
            return any();
        }
        try {
            return l.get_refer<Left>() < r.get_refer<Right>();
        } catch (...) {
            return any();
        }
    }
};

template<class Left, class Right = Left>
struct EqualHelper {
    any operator()(const any& l, const any& r) {
        if (l.type() != typeid(Left) || r.type() != typeid(Right)) {
            return false;
        }
        try {
            return l.get_refer<Left>() == r.get_refer<Right>();
        } catch (...) {
            return any();
        }
    }
};

template<class Left, class Right = Left>
struct GreatHelper {
    any operator()(const any& l, const any& r) {
        if (l.type() != typeid(Left) || r.type() != typeid(Right)) {
            return any();
        }
        try {
            return l.get_refer<Left>() > r.get_refer<Right>();
        } catch (...) {
            return any();
        }
    }
};

template<class T>
struct PrintHelper {
    void operator()(const any& l) {
        if (l.type() != typeid(T)) {
            return;
        }
        std::cout << l.get_refer<T>() << std::endl;
    }
};

template<class T>
struct ToStringHelper {
    std::string operator()(const any& l) {
        if (l.type() != typeid(T)) {
            return "";
        }
        std::stringstream ss;
        ss << l.get_refer<T>();
        return ss.str();
    }
};

template<class T>
struct HashHelper {
    size_t operator()(const any& l) {
        if (l.type() != typeid(T)) {
            return 0;
        }
        return std::hash<T>()(l.get_refer<T>());
    }
};

}
#endif
