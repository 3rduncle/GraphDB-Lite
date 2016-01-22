#include <vector>
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "struct.h"

namespace ipython {

typedef std::vector<any> List;
typedef GeneralObject<List> ListObject;
typedef std::shared_ptr<ListObject> ListPtr;

struct ListAdd {
    any operator()(const any& l, const any& r) {
        if (l.type() != r.type() || l.type() != typeid(List)) {
            return any();
        }
        any res = l;
        List& ll = res.get_refer<List>();
        List& lr = r.get_refer<List>();
        ll.insert(ll.end(), lr.begin(), lr.end());
        return res;
    }
};

struct ListPrint {
    void operator()(const any& l) {
        assert(l.type() == typeid(List));
        List& self = l.get_refer<List>();
        std::vector<std::string> vec;
        std::transform(
            self.begin(), self.end(), vec.begin(),
        [](any& x) {
            return x.get_content()->_ob_type->tp_repr(x);
        }
        );
        std::cout <<  "[" <<  boost::join(vec, ",") << "]" << std::endl;;
    }
};

ListType::ListType() {
    tp_print = ListPrint();
    tp_as_number.nb_add = ListAdd();
}

}
