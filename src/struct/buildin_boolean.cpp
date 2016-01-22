#include "struct.h"

namespace ipython {

BooleanType::BooleanType() {
    tp_print = PrintHelper<bool>();
    tp_string = ToStringHelper<bool>();
    tp_hash = HashHelper<bool>();
}

}
