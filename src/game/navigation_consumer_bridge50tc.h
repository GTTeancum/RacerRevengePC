#pragma once
#include "track_navigation_view50tc.h"
#include <type_traits>
#include "ai_cold_owners50iu.h"

namespace rr::game::navigation_consumer_detail50MB {
// Only immutable issued views enter these local facades. No public span/count
// factory, storage resizing, node truncation or replacement controller exists.
class GraphRead50TC {
public:
    const UndertrackNode50ET* const nodes;
    const std::uint32_t count;
    const std::size_t extent;
private:
    UndertrackTrackView50TC owner_;
    explicit GraphRead50TC(const UndertrackTrackView50TC& v) noexcept
        :nodes(v.nodes()),count(v.count()),extent(v.capacity()),owner_(v){}
    friend GraphRead50TC read(const UndertrackTrackView50TC&) noexcept;
    friend class NavigationRead50TC;
public:
    bool runtimeValid() const noexcept {return owner_.runtimeValid();}
};
class NavigationRead50TC {
public:
    const GraphRead50TC track;
    const TrackNavigationNode50EU* const nodes;
    const bool gatesPrepared;
    const std::size_t extent;
private:
    TrackNavigationView50TC owner_;
    explicit NavigationRead50TC(const TrackNavigationView50TC& v) noexcept
        :track(v.track()),nodes(v.nodes()),gatesPrepared(v.gatesPrepared()),extent(v.capacity()),owner_(v){}
    friend NavigationRead50TC read(const TrackNavigationView50TC&) noexcept;
public:
    const TrackNavigationView50TC& source() const noexcept {return owner_;}
};
class MetadataRead50TC {
public:
    const AiTrackNodeMetadata50IU* const nodes;
    const float trackLength3C;
    const std::uint32_t count;
    const bool bound;
    const std::size_t extent;
private:
    AiTrackMetadataView50TC owner_;
    explicit MetadataRead50TC(const AiTrackMetadataView50TC& v) noexcept
        :nodes(v.nodes()),trackLength3C(v.trackLength3C()),count(v.count()),bound(v.bound()),extent(v.capacity()),owner_(v){}
    friend MetadataRead50TC read(const AiTrackMetadataView50TC&) noexcept;
public:
    const AiTrackMetadataView50TC& source() const noexcept {return owner_;}
};
inline const NavigationRead50TC& read(const NavigationRead50TC& v) noexcept {return v;}
inline const GraphRead50TC& read(const GraphRead50TC& v) noexcept {return v;}
inline const MetadataRead50TC& read(const MetadataRead50TC& v) noexcept {return v;}
inline GraphRead50TC read(const UndertrackTrackView50TC& v) noexcept {
    return GraphRead50TC(v);
}
inline NavigationRead50TC read(const TrackNavigationView50TC& v) noexcept {
    return NavigationRead50TC(v);
}
inline MetadataRead50TC read(const AiTrackMetadataView50TC& v) noexcept {
    return MetadataRead50TC(v);
}
inline std::size_t capacity(const NavigationRead50TC& v) noexcept {return v.extent;}
inline std::size_t capacity(const GraphRead50TC& v) noexcept {return v.extent;}
inline std::size_t capacity(const MetadataRead50TC& v) noexcept {return v.extent;}
inline const TrackNavigationView50TC* pointer(const TrackNavigationView50TC& v) noexcept {return &v;}
inline const AiTrackMetadataView50TC* pointer(const AiTrackMetadataView50TC& v) noexcept {return &v;}
inline bool aiReady(const TrackNavigationView50TC& ai) noexcept {
    return ai.runtimeValid()&&ai.role()==NavigationRole50TC::Ai;
}
inline bool mapReady(const TrackNavigationView50TC& map) noexcept {
    return map.runtimeValid()&&map.role()==NavigationRole50TC::Map;
}
inline bool aiJoined(const TrackNavigationView50TC& ai,const AiTrackMetadataView50TC& metadata) noexcept {
    return aiReady(ai)&&metadata.matches(ai);
}
inline bool pairReady(const TrackNavigationView50TC& map,const TrackNavigationView50TC& ai) noexcept {
    return mapReady(map)&&aiReady(ai)&&map.sameOwner(ai);
}

// Only genuine TC final views can satisfy the following typed admission joins.
inline bool metadataReady(const AiTrackMetadataView50TC& metadata) noexcept {
    return metadata.runtimeValid()&&metadata.role()==NavigationRole50TC::Ai;
}
inline bool aiReady(const NavigationRead50TC& v) noexcept {return aiReady(v.source());}
inline bool mapReady(const NavigationRead50TC& v) noexcept {return mapReady(v.source());}
inline bool aiJoined(const NavigationRead50TC& n,const AiTrackMetadataView50TC& m) noexcept {return aiJoined(n.source(),m);}
inline bool aiJoined(const NavigationRead50TC& n,const MetadataRead50TC& m) noexcept {return aiJoined(n.source(),m.source());}
inline bool aiJoined(const TrackNavigationView50TC& n,const MetadataRead50TC& m) noexcept {return aiJoined(n,m.source());}
inline bool metadataReady(const MetadataRead50TC& m) noexcept {return metadataReady(m.source());}
inline bool pairReady(const NavigationRead50TC& m,const NavigationRead50TC& a) noexcept {return pairReady(m.source(),a.source());}
} // namespace rr::game::navigation_consumer_detail50MB
