// Exact existing non-template bodies, outlined without owner or layout changes.
#include "tc_collision_world50tc.h"

namespace tc_collision_world50TC {
Owner::~Owner(){if(!thread()||!retire())std::terminate();}
} // namespace tc_collision_world50TC
