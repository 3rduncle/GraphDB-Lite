#ifndef MINI_PYTHON_STRUCT_ANY_H
#define MINI_PYTHON_STRUCT_ANY_H

#include <typeinfo>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory> // make_shared
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "type_object.h"
#include <iostream>

namespace ipython {

class any {
public:
    any();
    ~any();
    void clear() {
        _content.reset();
    };

    any(const char*);
    template<typename ValueType>
    any(const ValueType&);
    any(const any&);
    any& operator=(const any& rhs);
    template<typename ValueType>
    any& operator=(const ValueType&);
    any& operator=(const char* rhs);

    void set_value(const char*);
    template<typename ValueType>
    void set_value(const ValueType&);

    template<typename ValueType>
    bool get_value(ValueType&) const;
    template<typename ValueType>
    ValueType* get_value() const;
    template<typename ValueType>
    ValueType& get_refer() const;

    ObjectPtr get_content() const;

    template<typename ValueType>
    bool operator==(const ValueType& rhs);

    bool operator!() const;
    const std::type_info& type() const;
    any& swap(any& rhs);

    friend bool operator < (const any&, const any&);
    friend bool operator > (const any&, const any&);

	friend bool operator == (const any&, const any&);

    friend bool operator >= (const any&, const any&);
    friend bool operator <= (const any&, const any&);

    //bool operator < (const any& other) const;
    //bool operator > (const any& other) const;
    //bool operator >= (const any& other) const;
    //bool operator <= (const any& other) const;

    friend any operator+(const any&, const any&);
    friend any operator-(const any&, const any&);
    friend any operator*(const any&, const any&);
    friend any operator/(const any&, const any&);

    any& operator+=(const any&);
    any& operator-=(const any&);
    any& operator*=(const any&);
    any& operator/=(const any&);
private:
    ObjectPtr _content;
};

template<typename ValueType>
ValueType* any_cast(any* operand) {
    return operand && (operand->type() == typeid(ValueType))?
           &std::dynamic_pointer_cast<GeneralObject<ValueType> >(operand->get_content())->_core:
           0;
}

template<typename ValueType>
inline const ValueType* any_cast(const any* operand) {
    return any_cast<ValueType>(const_cast<any*>(operand));
}

template<typename ValueType>
ValueType any_cast(any& operand) {
    ValueType* result = any_cast<ValueType>(&operand);
    if (!result) {
        throw std::invalid_argument("Any Cast Failed!");
    }
    return *result;
}

template<typename ValueType>
inline ValueType any_cast(const any& operand) {
    return any_cast<ValueType>(const_cast<any&>(operand));
}

template<typename ValueType>
any::any(const ValueType& val) {
    _content = std::make_shared<GeneralObject<ValueType> >(val);
}

template<typename ValueType>
void any::set_value(const ValueType& val) {
    _content = std::make_shared<GeneralObject<ValueType> >(val);
}

template<typename ValueType>
any& any::operator=(const ValueType& val) {
    _content = std::make_shared<GeneralObject<ValueType> >(val);
    return *this;
}

template<typename ValueType>
ValueType* any::get_value() const {
    if (type() == typeid(ValueType)) {
        return &((dynamic_cast<GeneralObject<ValueType>* >(_content.get()))->_core);
    }
    else {
        return NULL;
    }
}

template<typename ValueType>
ValueType& any::get_refer() const {
    if (!(*this)) {
        throw std::logic_error("Fail Get Refer from Empty any!");
    }
    return (dynamic_cast<GeneralObject<ValueType>* >(_content.get()))->_core;
}

template<typename ValueType>
bool any::get_value(ValueType& val) const {
    if (type() == typeid(ValueType)) {
        val = (dynamic_cast<GeneralObject<ValueType>* >(_content.get()))->_core;
        return true;
    }
    return false;
}

template<>
bool any::get_value<int>(int& val) const;

template<>
bool any::get_value<double>(double& val) const;

/*
template<class ValueType>
inline bool any::operator==(const ValueType& rhs) {
    if (type() != typeid(ValueType)) {
        return false;
    }
    return std::dynamic_pointer_cast<ValueType>(_content)->_core == rhs;
}
*/

bool is_numeric_datanode(const any& dn);

} //namespace ipython

namespace std {

template<>
class hash<ipython::any> {
public:
    size_t operator()(const ipython::any& l) const {
        return l.get_content()->_ob_type->tp_hash(l);
    }
};

} //namespace std
#endif
