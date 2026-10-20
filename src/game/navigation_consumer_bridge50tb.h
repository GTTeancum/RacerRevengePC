#pragma once
#include "track_navigation_view50tb.h"
#include <type_traits>
#include "ai_cold_owners50iu.h"

namespace rr::game::navigation_consumer_detail50MB {
// Only immutable issued views enter these local facades. No public span/count
// factory, storage resizing, node truncation or replacement controller exists.
class GraphRead50TB {
public:
    const UndertrackNode50ET* const nodes;
    const std::uint32_t count;
    const std::size_t extent;
private:
    UndertrackTrackView50TB owner_;
    explicit GraphRead50TB(const UndertrackTrackView50TB& v) noexcept
        :nodes(v.nodes()),count(v.count()),extent(v.capacity()),owner_(v){}
    friend GraphRead50TB read(const UndertrackTrackView50TB&) noexcept;
    friend class NavigationRead50TB;
public:
    bool runtimeValid() const noexcept {return owner_.runtimeValid();}
};
class NavigationRead50TB {
public:
    const GraphRead50TB track;
    const TrackNavigationNode50EU* const nodes;
    const bool gatesPrepared;
    const std::size_t extent;
private:
    TrackNavigationView50TB owner_;
    explicit NavigationRead50TB(const TrackNavigationView50TB& v) noexcept
        :track(v.track()),nodes(v.nodes()),gatesPrepared(v.gatesPrepared()),extent(v.capacity()),owner_(v){}
    friend NavigationRead50TB read(const TrackNavigationView50TB&) noexcept;
public:
    const TrackNavigationView50TB& source() const noexcept {return owner_;}
};
class MetadataRead50TB {
public:
    const AiTrackNodeMetadata50IU* const nodes;
    const float trackLength3C;
    const std::uint32_t count;
    const bool bound;
    const std::size_t extent;
private:
    AiTrackMetadataView50TB owner_;
    explicit MetadataRead50TB(const AiTrackMetadataView50TB& v) noexcept
        :nodes(v.nodes()),trackLength3C(v.trackLength3C()),count(v.count()),bound(v.bound()),extent(v.capacity()),owner_(v){}
    friend MetadataRead50TB read(const AiTrackMetadataView50TB&) noexcept;
public:
    const AiTrackMetadataView50TB& source() const noexcept {return owner_;}
};
inline const NavigationRead50TB& read(const NavigationRead50TB& v) noexcept {return v;}
inline const GraphRead50TB& read(const GraphRead50TB& v) noexcept {return v;}
inline const MetadataRead50TB& read(const MetadataRead50TB& v) noexcept {return v;}
inline GraphRead50TB read(const UndertrackTrackView50TB& v) noexcept {
    return GraphRead50TB(v);
}
inline NavigationRead50TB read(const TrackNavigationView50TB& v) noexcept {
    return NavigationRead50TB(v);
}
inline MetadataRead50TB read(const AiTrackMetadataView50TB& v) noexcept {
    return MetadataRead50TB(v);
}
inline std::size_t capacity(const NavigationRead50TB& v) noexcept {return v.extent;}
inline std::size_t capacity(const GraphRead50TB& v) noexcept {return v.extent;}
inline std::size_t capacity(const MetadataRead50TB& v) noexcept {return v.extent;}
inline const TrackNavigationView50TB* pointer(const TrackNavigationView50TB& v) noexcept {return &v;}
inline const AiTrackMetadataView50TB* pointer(const AiTrackMetadataView50TB& v) noexcept {return &v;}
inline bool aiReady(const TrackNavigationView50TB& ai) noexcept {
    return ai.runtimeValid()&&ai.role()==NavigationRole50TB::Ai;
}
inline bool mapReady(const TrackNavigationView50TB& map) noexcept {
    return map.runtimeValid()&&map.role()==NavigationRole50TB::Map;
}
inline bool aiJoined(const TrackNavigationView50TB& ai,const AiTrackMetadataView50TB& metadata) noexcept {
    return aiReady(ai)&&metadata.matches(ai);
}
inline bool pairReady(const TrackNavigationView50TB& map,const TrackNavigationView50TB& ai) noexcept {
    return mapReady(map)&&aiReady(ai)&&map.sameOwner(ai);
}

// Only genuine TB final views can satisfy the following typed admission joins.
inline bool metadataReady(const AiTrackMetadataView50TB& metadata) noexcept {
    return metadata.runtimeValid()&&metadata.role()==NavigationRole50TB::Ai;
}
inline bool aiReady(const NavigationRead50TB& v) noexcept {return aiReady(v.source());}
inline bool mapReady(const NavigationRead50TB& v) noexcept {return mapReady(v.source());}
inline bool aiJoined(const NavigationRead50TB& n,const AiTrackMetadataView50TB& m) noexcept {return aiJoined(n.source(),m);}
inline bool aiJoined(const NavigationRead50TB& n,const MetadataRead50TB& m) noexcept {return aiJoined(n.source(),m.source());}
inline bool aiJoined(const TrackNavigationView50TB& n,const MetadataRead50TB& m) noexcept {return aiJoined(n,m.source());}
inline bool metadataReady(const MetadataRead50TB& m) noexcept {return metadataReady(m.source());}
inline bool pairReady(const NavigationRead50TB& m,const NavigationRead50TB& a) noexcept {return pairReady(m.source(),a.source());}
} // namespace rr::game::navigation_consumer_detail50MB
