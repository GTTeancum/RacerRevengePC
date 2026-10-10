#include "collision_tree48i.h"
#include <limits>
#include <new>
#include <stdexcept>

namespace rr::game {
CollisionQueryStatus48I queryOrderedCollision48I(const LocalSegment48I& query,
    const CollisionCandidate48I* candidates,std::size_t count,CollisionResult48I& output,
    std::vector<CollisionVisit48I>* trace) noexcept {
    if((count&&!candidates)||count>std::numeric_limits<std::size_t>::max()/sizeof(*candidates))return CollisionQueryStatus48I::InvalidInput;
    try {
        // The compatibility interface owns its temporary allocations. Its bound
        // follows input record counts, not a guessed tree-depth constant. One
        // extra slot also lets the bounded core diagnose malformed cycles using
        // its visit limit rather than exhausting a valid-tree-only bound.
        std::size_t stackCapacity=0,traceCapacity=0;
        for(std::size_t i=0;i<count;++i){
            const auto* mesh=candidates[i].mesh;
            if(!mesh||mesh->nodes.size()>=std::numeric_limits<std::size_t>::max()-mesh->faces.size())return CollisionQueryStatus48I::InvalidInput;
            const auto records=mesh->nodes.size()+mesh->faces.size();
            if(records+1>stackCapacity)stackCapacity=records+1;
            if(trace){
                if(records>std::numeric_limits<std::size_t>::max()-traceCapacity)return CollisionQueryStatus48I::InvalidInput;
                traceCapacity+=records;
            }
        }
        std::vector<std::uint32_t> stack(stackCapacity);
        // Keep the caller's vector descriptor outside bounded scratch/output.
        // This also preserves a useful diagnostic prefix on non-Hit statuses.
        std::vector<CollisionVisit48I> visits;
        if(trace)visits.resize(traceCapacity);
        CollisionVisit48I emptyTrace{};
        const CollisionWorkspace48M workspace{stack.data(),stack.size(),
            trace?(visits.empty()?&emptyTrace:visits.data()):nullptr,visits.size()};
        std::size_t used=0;
        const auto status=queryOrderedCollisionBounded48M(query,candidates,count,workspace,output,used);
        if(trace){visits.resize(used);*trace=std::move(visits);}
        return status==CollisionQueryStatus48I::InsufficientCapacity?CollisionQueryStatus48I::InvalidInput:status;
    }catch(const std::bad_alloc&){return CollisionQueryStatus48I::InvalidInput;}
     catch(const std::length_error&){return CollisionQueryStatus48I::InvalidInput;}
}
}
