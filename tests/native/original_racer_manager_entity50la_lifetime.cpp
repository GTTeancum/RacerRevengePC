// Lifetime emission only: preserves the original native owner and cleanup order.
#include "original_racer_manager_entity50la.h"

namespace original_racer_manager_entity {
Owner::~Owner() {if(!close())std::terminate();}
} // namespace original_racer_manager_entity
