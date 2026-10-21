#pragma once
#include "track_navigation_view50mc.h"
#include <type_traits>
#include "ai_cold_owners50iu.h"

namespace rr::game::navigation_consumer_detail50MB {
// Only immutable issued views enter these local facades. No public span/count
// factory, storage resizing, node truncation or replacement controller exists.
class GraphRead50MC {
public:
    const UndertrackNode50ET* const nodes;
    const std::uint32_t count;
    const std::size_t extent;
private:
    UndertrackTrackView50MC owner_;
    explicit GraphRead50MC(const UndertrackTrackView50MC& v) noexcept
        :nodes(v.nodes()),count(v.count()),extent(v.capacity()),owner_(v){}
    friend GraphRead50MC read(const UndertrackTrackView50MC&) noexcept;
    friend class NavigationRead50MC;
public:
    bool runtimeValid() const noexcept {return owner_.runtimeValid();}
};
class NavigationRead50MC {
public:
    const GraphRead50MC track;
    const TrackNavigationNode50EU* const nodes;
    const bool gatesPrepared;
    const std::size_t extent;
private:
    TrackNavigationView50MC owner_;
    explicit NavigationRead50MC(const TrackNavigationView50MC& v) noexcept
        :track(v.track()),nodes(v.nodes()),gatesPrepared(v.gatesPrepared()),extent(v.capacity()),owner_(v){}
    friend NavigationRead50MC read(const TrackNavigationView50MC&) noexcept;
public:
    const TrackNavigationView50MC& source() const noexcept {return owner_;}
};
class MetadataRead50MC {
public:
    const AiTrackNodeMetadata50IU* const nodes;
    const float trackLength3C;
    const std::uint32_t count;
    const bool bound;
    const std::size_t extent;
private:
    AiTrackMetadataView50MC owner_;
    explicit MetadataRead50MC(const AiTrackMetadataView50MC& v) noexcept
        :nodes(v.nodes()),trackLength3C(v.trackLength3C()),count(v.count()),bound(v.bound()),extent(v.capacity()),owner_(v){}
    friend MetadataRead50MC read(const AiTrackMetadataView50MC&) noexcept;
public:
    const AiTrackMetadataView50MC& source() const noexcept {return owner_;}
};
inline const NavigationRead50MC& read(const NavigationRead50MC& v) noexcept {return v;}
inline const GraphRead50MC& read(const GraphRead50MC& v) noexcept {return v;}
inline const MetadataRead50MC& read(const MetadataRead50MC& v) noexcept {return v;}
inline GraphRead50MC read(const UndertrackTrackView50MC& v) noexcept {
    return GraphRead50MC(v);
}
inline NavigationRead50MC read(const TrackNavigationView50MC& v) noexcept {
    return NavigationRead50MC(v);
}
inline MetadataRead50MC read(const AiTrackMetadataView50MC& v) noexcept {
    return MetadataRead50MC(v);
}
inline std::size_t capacity(const NavigationRead50MC& v) noexcept {return v.extent;}
inline std::size_t capacity(const GraphRead50MC& v) noexcept {return v.extent;}
inline std::size_t capacity(const MetadataRead50MC& v) noexcept {return v.extent;}
inline const TrackNavigationView50MC* pointer(const TrackNavigationView50MC& v) noexcept {return &v;}
inline const AiTrackMetadataView50MC* pointer(const AiTrackMetadataView50MC& v) noexcept {return &v;}
inline bool aiReady(const TrackNavigationView50MC& ai) noexcept {
    return ai.runtimeValid()&&ai.role()==NavigationRole50MC::Ai;
}
inline bool mapReady(const TrackNavigationView50MC& map) noexcept {
    return map.runtimeValid()&&map.role()==NavigationRole50MC::Map;
}
inline bool aiJoined(const TrackNavigationView50MC& ai,const AiTrackMetadataView50MC& metadata) noexcept {
    return aiReady(ai)&&metadata.matches(ai);
}
inline bool pairReady(const TrackNavigationView50MC& map,const TrackNavigationView50MC& ai) noexcept {
    return mapReady(map)&&aiReady(ai)&&map.sameOwner(ai);
}

// Only genuine MC final views can satisfy the following typed admission joins.
inline bool metadataReady(const AiTrackMetadataView50MC& metadata) noexcept {
    return metadata.runtimeValid()&&metadata.role()==NavigationRole50MC::Ai;
}
inline bool aiReady(const NavigationRead50MC& v) noexcept {return aiReady(v.source());}
inline bool mapReady(const NavigationRead50MC& v) noexcept {return mapReady(v.source());}
inline bool aiJoined(const NavigationRead50MC& n,const AiTrackMetadataView50MC& m) noexcept {return aiJoined(n.source(),m);}
inline bool aiJoined(const NavigationRead50MC& n,const MetadataRead50MC& m) noexcept {return aiJoined(n.source(),m.source());}
inline bool aiJoined(const TrackNavigationView50MC& n,const MetadataRead50MC& m) noexcept {return aiJoined(n,m.source());}
inline bool metadataReady(const MetadataRead50MC& m) noexcept {return metadataReady(m.source());}
inline bool pairReady(const NavigationRead50MC& m,const NavigationRead50MC& a) noexcept {return pairReady(m.source(),a.source());}
} // namespace rr::game::navigation_consumer_detail50MB
