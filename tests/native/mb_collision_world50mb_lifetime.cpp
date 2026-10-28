// Exact existing non-template bodies, outlined without owner or layout changes.
#include "mb_collision_world50mb.h"

namespace mb_collision_world50MB {
Owner::~Owner(){if(actionOwner_||std::this_thread::get_id()!=ownerThread_)std::terminate();}
} // namespace mb_collision_world50MB
