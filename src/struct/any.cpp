#include <algorithm>
#include <boost/foreach.hpp>
#include "struct.h"

namespace ipython {

RunTimeOperatorManeger* RunTimeOperatorManeger::_s_instance =
    RunTimeOperatorManeger::get_instance();

/* any成员函数区 */
any::any() {
}
any::~any() {
}

any::any(const any& rhs) {
    if (!rhs.get_content()) {
        _content.reset();
    }
    _content = rhs.get_content()->clone();
}

any::any(const char* rhs) {
    _content = std::make_shared<GeneralObject<std::string> >(rhs);
    //new(this) any(std::string(rhs));
}

any& any::operator=(const any& rhs) {
    //return swap(rhs); // move语义
    /* copy语义
    if (!rhs.get_content()) {
        _content.reset();
        return *this;
    }
    _content = rhs.get_content()->clone();
    */
    _content = rhs.get_content(); //引用语义
    return *this;

}

any& any::operator=(const char* rhs) {
    this->set_value(std::string(rhs));
    return *this;
}

bool any::operator!() const {
    return !_content;
}

const std::type_info& any::type() const {
    return _content ? _content->type() : typeid(void);
}

any& any::swap(any& rhs) {
    std::swap(_content, rhs._content);
    return *this;
}

ObjectPtr any::get_content() const {
    return _content;
}

void any::set_value(const char* val) {
    set_value(std::string(val));
}

bool operator<(const any& lhs, const any& rhs) {
	if (lhs.get_content()->_ob_type->tp_logic.less) {
		auto res = lhs.get_content()->_ob_type->tp_logic.less(lhs, rhs); 
		return any_cast<bool>(res);
	}
	return false;
}

bool operator==(const any& lhs, const any& rhs) {
	if (lhs.type() != rhs.type()) {
		return false;
	}
	if (lhs.get_content()->_ob_type->tp_logic.equal) {
		auto res = lhs.get_content()->_ob_type->tp_logic.less(lhs, rhs);
		return any_cast<bool>(res);
	}
	return false;
}

any operator+(const any& lhs, const any& rhs) {
    if (lhs.get_content()->_ob_type->tp_as_number.nb_add) {
        return lhs.get_content()->_ob_type->tp_as_number.nb_add(lhs, rhs);
    } else {
        return any();
    }
}

any operator*(const any& lhs, const any& rhs) {
    if (lhs.get_content()->_ob_type->tp_as_number.nb_mul) {
        return lhs.get_content()->_ob_type->tp_as_number.nb_mul(lhs, rhs);
    } else {
        return any();
    }
}

any operator-(const any& lhs, const any& rhs) {
    if (lhs.get_content()->_ob_type->tp_as_number.nb_sub) {
        return lhs.get_content()->_ob_type->tp_as_number.nb_sub(lhs, rhs);
    } else {
        return any();
    }
}

any operator/(const any& lhs, const any& rhs) {
    if (lhs.get_content()->_ob_type->tp_as_number.nb_div) {
        return lhs.get_content()->_ob_type->tp_as_number.nb_div(lhs, rhs);
    } else {
        return any();
    }
}

any& any::operator+=(const any& rhs) {
    if (_content->_ob_type->tp_as_number.nb_add) {
        *this = _content->_ob_type->tp_as_number.nb_add(*this, rhs);
    } else {
        ;
    }
    return *this;
}

any& any::operator*=(const any& rhs) {
    if (_content->_ob_type->tp_as_number.nb_mul) {
        *this = _content->_ob_type->tp_as_number.nb_mul(*this, rhs);
    } else {
        ;
    }
    return *this;
}

any& any::operator-=(const any& rhs) {
    if (_content->_ob_type->tp_as_number.nb_sub) {
        *this = _content->_ob_type->tp_as_number.nb_sub(*this, rhs);
    } else {
        ;
    }
    return *this;
}

any& any::operator/=(const any& rhs) {
    if (_content->_ob_type->tp_as_number.nb_div) {
        *this = _content->_ob_type->tp_as_number.nb_div(*this, rhs);
    } else {
        ;
    }
    return *this;
}

bool is_numeric_datanode(const any& dn) {
    return dn.type() == typeid(int) || dn.type() == typeid(double);
}

template<>
bool any::get_value<int>(int& val) const {
    if (type() == typeid(int)) {
        val = (dynamic_cast<GeneralObject<int>* >(_content.get()))->_core;
        return true;
    }
    if (type() == typeid(double)) {
        val = static_cast<int>((dynamic_cast<GeneralObject<double>* >(_content.get()))->_core);
        return true;
    }
    return false;
}

template<>
bool any::get_value<double>(double& val) const {
    if (type() == typeid(double)) {
        val = (dynamic_cast<GeneralObject<double>* >(_content.get()))->_core;
        return true;
    }
    if (type() == typeid(int)) {
        val = static_cast<double>((dynamic_cast<GeneralObject<int>* >(_content.get()))->_core);
        return true;
    }
    return false;
}

}//namespace kg_compute
