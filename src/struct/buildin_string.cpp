#include "struct.h"

namespace ipython {

typedef GeneralObject<std::string> StringObject;
typedef std::shared_ptr<StringObject> StringPtr;

StringType::StringType() {
    tp_print = PrintHelper<std::string>();
    tp_string = ToStringHelper<std::string>();
	tp_hash = HashHelper<std::string>();

    tp_as_number.nb_add = AddHelper<std::string>();

    tp_logic.equal = EqualHelper<std::string>();
    tp_logic.less = LessHelper<std::string>();
    tp_logic.great = GreatHelper<std::string>();
}

}
