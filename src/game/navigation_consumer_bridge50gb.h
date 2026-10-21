#pragma once
#include "track_navigation_view50gb.h"
#include <type_traits>
#include "ai_cold_owners50iu.h"

namespace rr::game::navigation_consumer_detail50MB {
// Only immutable issued views enter these local facades. No public span/count
// factory, storage resizing, node truncation or replacement controller exists.
class GraphRead50GB {
public:
    const UndertrackNode50ET* const nodes;
    const std::uint32_t count;
    const std::size_t extent;
private:
    UndertrackTrackView50GB owner_;
    explicit GraphRead50GB(const UndertrackTrackView50GB& v) noexcept
        :nodes(v.nodes()),count(v.count()),extent(v.capacity()),owner_(v){}
    friend GraphRead50GB read(const UndertrackTrackView50GB&) noexcept;
    friend class NavigationRead50GB;
public:
    bool runtimeValid() const noexcept {return owner_.runtimeValid();}
};
class NavigationRead50GB {
public:
    const GraphRead50GB track;
    const TrackNavigationNode50EU* const nodes;
    const bool gatesPrepared;
    const std::size_t extent;
private:
    TrackNavigationView50GB owner_;
    explicit NavigationRead50GB(const TrackNavigationView50GB& v) noexcept
        :track(v.track()),nodes(v.nodes()),gatesPrepared(v.gatesPrepared()),extent(v.capacity()),owner_(v){}
    friend NavigationRead50GB read(const TrackNavigationView50GB&) noexcept;
public:
    const TrackNavigationView50GB& source() const noexcept {return owner_;}
};
class MetadataRead50GB {
public:
    const AiTrackNodeMetadata50IU* const nodes;
    const float trackLength3C;
    const std::uint32_t count;
    const bool bound;
    const std::size_t extent;
private:
    AiTrackMetadataView50GB owner_;
    explicit MetadataRead50GB(const AiTrackMetadataView50GB& v) noexcept
        :nodes(v.nodes()),trackLength3C(v.trackLength3C()),count(v.count()),bound(v.bound()),extent(v.capacity()),owner_(v){}
    friend MetadataRead50GB read(const AiTrackMetadataView50GB&) noexcept;
public:
    const AiTrackMetadataView50GB& source() const noexcept {return owner_;}
};
inline const NavigationRead50GB& read(const NavigationRead50GB& v) noexcept {return v;}
inline const GraphRead50GB& read(const GraphRead50GB& v) noexcept {return v;}
inline const MetadataRead50GB& read(const MetadataRead50GB& v) noexcept {return v;}
inline GraphRead50GB read(const UndertrackTrackView50GB& v) noexcept {
    return GraphRead50GB(v);
}
inline NavigationRead50GB read(const TrackNavigationView50GB& v) noexcept {
    return NavigationRead50GB(v);
}
inline MetadataRead50GB read(const AiTrackMetadataView50GB& v) noexcept {
    return MetadataRead50GB(v);
}
inline std::size_t capacity(const NavigationRead50GB& v) noexcept {return v.extent;}
inline std::size_t capacity(const GraphRead50GB& v) noexcept {return v.extent;}
inline std::size_t capacity(const MetadataRead50GB& v) noexcept {return v.extent;}
inline const TrackNavigationView50GB* pointer(const TrackNavigationView50GB& v) noexcept {return &v;}
inline const AiTrackMetadataView50GB* pointer(const AiTrackMetadataView50GB& v) noexcept {return &v;}
inline bool aiReady(const TrackNavigationView50GB& ai) noexcept {
    return ai.runtimeValid()&&ai.role()==NavigationRole50GB::Ai;
}
inline bool mapReady(const TrackNavigationView50GB& map) noexcept {
    return map.runtimeValid()&&map.role()==NavigationRole50GB::Map;
}
inline bool aiJoined(const TrackNavigationView50GB& ai,const AiTrackMetadataView50GB& metadata) noexcept {
    return aiReady(ai)&&metadata.matches(ai);
}
inline bool pairReady(const TrackNavigationView50GB& map,const TrackNavigationView50GB& ai) noexcept {
    return mapReady(map)&&aiReady(ai)&&map.sameOwner(ai);
}

// Only genuine GB final views can satisfy the following typed admission joins.
inline bool metadataReady(const AiTrackMetadataView50GB& metadata) noexcept {
    return metadata.runtimeValid()&&metadata.role()==NavigationRole50GB::Ai;
}
inline bool aiReady(const NavigationRead50GB& v) noexcept {return aiReady(v.source());}
inline bool mapReady(const NavigationRead50GB& v) noexcept {return mapReady(v.source());}
inline bool aiJoined(const NavigationRead50GB& n,const AiTrackMetadataView50GB& m) noexcept {return aiJoined(n.source(),m);}
inline bool aiJoined(const NavigationRead50GB& n,const MetadataRead50GB& m) noexcept {return aiJoined(n.source(),m.source());}
inline bool aiJoined(const TrackNavigationView50GB& n,const MetadataRead50GB& m) noexcept {return aiJoined(n,m.source());}
inline bool metadataReady(const MetadataRead50GB& m) noexcept {return metadataReady(m.source());}
inline bool pairReady(const NavigationRead50GB& m,const NavigationRead50GB& a) noexcept {return pairReady(m.source(),a.source());}
} // namespace rr::game::navigation_consumer_detail50MB
