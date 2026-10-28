// Lifetime emission only: preserves the original native owner and cleanup order.
#include "original_subtype3_record_owner50la.h"

namespace original_subtype3_record {
Owner::~Owner() {if(!close())std::terminate();}
} // namespace original_subtype3_record
