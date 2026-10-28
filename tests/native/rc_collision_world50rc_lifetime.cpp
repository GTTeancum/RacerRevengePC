// Exact existing non-template bodies, outlined without owner or layout changes.
#include "rc_collision_world50rc.h"

namespace rc_collision_world50RC {
Owner::~Owner(){if(!thread()||!retire())std::terminate();}
} // namespace rc_collision_world50RC
