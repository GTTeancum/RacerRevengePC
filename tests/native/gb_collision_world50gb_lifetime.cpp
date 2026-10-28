// Exact existing non-template bodies, outlined without owner or layout changes.
#include "gb_collision_world50gb.h"

namespace gb_collision_world50GB {
Owner::~Owner(){if(!thread()||!retire())std::terminate();}
} // namespace gb_collision_world50GB
