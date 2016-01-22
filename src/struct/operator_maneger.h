#ifndef  MINI_PYTHON_OPERTOR_MANEGER_H
#define  MINI_PYTHON_OPERTOR_MANEGER_H
#include <unordered_map>
#include "type_object.h"

namespace ipython {

//--------------类型操作跟类型值绑定------------------------
class RunTimeOperatorManeger {
public:
    static RunTimeOperatorManeger* get_instance();
    TypeObject* get_operator(const std::string& obj_type);
    void register_operator(const std::string&, TypeObject*);
private:
    RunTimeOperatorManeger();
    ~RunTimeOperatorManeger() {}
    static RunTimeOperatorManeger* _s_instance;
    std::unordered_map<std::string, TypeObject*> _operators;
};

}
#endif
