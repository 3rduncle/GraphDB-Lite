#include "struct.h"

namespace ipython {

typedef GeneralObject<int> IntObject;
typedef std::shared_ptr<IntObject> IntPtr;

typedef GeneralObject<double> DoubleObject;
typedef std::shared_ptr<DoubleObject> DoublePtr;

typedef GeneralObject<std::string> StringObject;
typedef std::shared_ptr<StringObject> StringPtr;

typedef std::vector<any> List;
typedef GeneralObject<List > ListObject;
typedef std::shared_ptr<ListObject> ListPtr;

typedef std::unordered_map<any, any> Dict;
typedef GeneralObject<Dict> DictObject;
typedef std::shared_ptr<DictObject> DictPtr;

RunTimeOperatorManeger::RunTimeOperatorManeger() {
    _operators["NULL"] = new TypeObject();
    _operators[typeid(int).name()] = new IntType();
    _operators[typeid(double).name()] = new DoubleType();
    _operators[typeid(bool).name()] = new BooleanType();
    _operators[typeid(std::string).name()] = new StringType();
    _operators[typeid(List).name()] = new ListType();
    _operators[typeid(Dict).name()] = new DictType();
}

RunTimeOperatorManeger* RunTimeOperatorManeger::get_instance() {
    if (_s_instance == NULL) {
        _s_instance = new RunTimeOperatorManeger();
    }
    return _s_instance;
}

TypeObject* RunTimeOperatorManeger::get_operator(const std::string& obj_name) {
    auto it = _operators.find(obj_name);
    if (it == _operators.end()) {
        return _operators["NULL"];
    } else {
        return it->second;
    }
}

void RunTimeOperatorManeger::register_operator(const std::string& key, TypeObject* op) {
    _operators[key] = op;
}

}
