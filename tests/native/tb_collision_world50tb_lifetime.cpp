// Exact existing non-template bodies, outlined without owner or layout changes.
#include "tb_collision_world50tb.h"

namespace tb_collision_world50TB {
Owner::~Owner(){if(!thread()||!retire())std::terminate();}
} // namespace tb_collision_world50TB
