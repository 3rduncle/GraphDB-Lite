#ifndef MINI_PYTHON_OBJECT_H
#define MINI_PYTHON_OBJECT_H

#include <typeinfo>
#include <memory>
#include "type_object.h"
#include "operator_maneger.h"

namespace ipython {

//--------------操作跟值的管理-----------------------
class Object {
public:
    TypeObject* _ob_type;
    virtual ~Object() {}
    virtual const std::type_info& type() const = 0;
    virtual ObjectPtr clone() const = 0;
};

template<class ValueType>
class GeneralObject : public Object {
public:
    GeneralObject() {
        _ob_type = RunTimeOperatorManeger::get_instance()->get_operator("NULL");
    }
    GeneralObject(const ValueType& value) : _core(value) {
        _ob_type = RunTimeOperatorManeger::get_instance()->
                   get_operator(typeid(ValueType).name());
    }
    virtual const std::type_info& type() const {
        return typeid(ValueType);
    }
    virtual ObjectPtr clone() const {
        return std::make_shared<GeneralObject<ValueType> >(_core);
    }
    ValueType _core;
};

}
#endif
