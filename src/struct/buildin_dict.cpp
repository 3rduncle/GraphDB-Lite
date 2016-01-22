#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "struct.h"

namespace ipython {

typedef std::unordered_map<any, any> Dict;
typedef std::pair<any, any> AnyPair;
typedef GeneralObject<Dict> DictObject;
typedef std::shared_ptr<DictObject> DictPtr;

struct DictPrint {
    void operator()(const any& l) {
        Dict& dl = l.get_refer<Dict>();
        std::vector<std::string> vec;
        std::transform(
            dl.begin(), dl.end(), vec.begin(),
			[](const AnyPair& p) {
				auto& k = p.first;
				auto& v = p.second;
				return k.get_content()->_ob_type->tp_repr(k)
					   + ": "
					   + k.get_content()->_ob_type->tp_repr(v);
			}
        );
        std::cout << "{" << boost::join(vec, ", ") << "}" << std::endl;
    }
};

DictType::DictType() {
    tp_print = DictPrint();
}

}
