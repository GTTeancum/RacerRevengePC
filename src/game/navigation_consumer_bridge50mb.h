#pragma once
#include "track_navigation_view50mb.h"
#include "ai_cold_owners50iu.h"

namespace rr::game::navigation_consumer_detail50MB {
// Only immutable issued views enter these local facades. No public span/count
// factory, storage resizing, node truncation or replacement controller exists.
class GraphRead {
public:
    const UndertrackNode50ET* const nodes;
    const std::uint32_t count;
    const std::size_t extent;
private:
    UndertrackTrackView50ET owner_;
    explicit GraphRead(const UndertrackTrackView50ET& v) noexcept
        :nodes(v.nodes()),count(v.count()),extent(v.capacity()),owner_(v){}
    friend GraphRead read(const UndertrackTrackView50ET&) noexcept;
    friend class NavigationRead;
public:
    bool runtimeValid() const noexcept {return owner_.runtimeValid();}
};
class NavigationRead {
public:
    const GraphRead track;
    const TrackNavigationNode50EU* const nodes;
    const bool gatesPrepared;
    const std::size_t extent;
private:
    TrackNavigationView50EU owner_;
    explicit NavigationRead(const TrackNavigationView50EU& v) noexcept
        :track(v.track()),nodes(v.nodes()),gatesPrepared(v.gatesPrepared()),extent(v.capacity()),owner_(v){}
    friend NavigationRead read(const TrackNavigationView50EU&) noexcept;
public:
    const TrackNavigationView50EU& source() const noexcept {return owner_;}
};
class MetadataRead {
public:
    const AiTrackNodeMetadata50IU* const nodes;
    const float trackLength3C;
    const std::uint32_t count;
    const bool bound;
    const std::size_t extent;
private:
    AiTrackMetadataView50IU owner_;
    explicit MetadataRead(const AiTrackMetadataView50IU& v) noexcept
        :nodes(v.nodes()),trackLength3C(v.trackLength3C()),count(v.count()),bound(v.bound()),extent(v.capacity()),owner_(v){}
    friend MetadataRead read(const AiTrackMetadataView50IU&) noexcept;
public:
    const AiTrackMetadataView50IU& source() const noexcept {return owner_;}
};
inline const TrackNavigation50EU& read(const TrackNavigation50EU& v) noexcept {return v;}
inline const UndertrackTrack50ET& read(const UndertrackTrack50ET& v) noexcept {return v;}
inline const AiTrackMetadata50IU& read(const AiTrackMetadata50IU& v) noexcept {return v;}
inline const NavigationRead& read(const NavigationRead& v) noexcept {return v;}
inline const GraphRead& read(const GraphRead& v) noexcept {return v;}
inline const MetadataRead& read(const MetadataRead& v) noexcept {return v;}
inline GraphRead read(const UndertrackTrackView50ET& v) noexcept {
    return GraphRead(v);
}
inline NavigationRead read(const TrackNavigationView50EU& v) noexcept {
    return NavigationRead(v);
}
inline MetadataRead read(const AiTrackMetadataView50IU& v) noexcept {
    return MetadataRead(v);
}
inline constexpr std::size_t capacity(const TrackNavigation50EU&) noexcept {return maxUndertrackNodes50ET;}
inline constexpr std::size_t capacity(const UndertrackTrack50ET&) noexcept {return maxUndertrackNodes50ET;}
inline constexpr std::size_t capacity(const AiTrackMetadata50IU&) noexcept {return maxUndertrackNodes50ET;}
inline std::size_t capacity(const NavigationRead& v) noexcept {return v.extent;}
inline std::size_t capacity(const GraphRead& v) noexcept {return v.extent;}
inline std::size_t capacity(const MetadataRead& v) noexcept {return v.extent;}
template<class T> inline const T* pointer(const T* p) noexcept {return p;}
inline const TrackNavigationView50EU* pointer(const TrackNavigationView50EU& v) noexcept {return &v;}
inline const AiTrackMetadataView50IU* pointer(const AiTrackMetadataView50IU& v) noexcept {return &v;}
inline bool aiReady(const TrackNavigationView50EU& ai) noexcept {
    return ai.runtimeValid()&&ai.role()==NavigationRole50MB::Ai;
}
inline bool mapReady(const TrackNavigationView50EU& map) noexcept {
    return map.runtimeValid()&&map.role()==NavigationRole50MB::Map;
}
inline bool aiJoined(const TrackNavigationView50EU& ai,const AiTrackMetadataView50IU& metadata) noexcept {
    return aiReady(ai)&&metadata.matches(ai);
}
inline bool pairReady(const TrackNavigationView50EU& map,const TrackNavigationView50EU& ai) noexcept {
    return mapReady(map)&&aiReady(ai)&&map.sameOwner(ai);
}

// Constant-true legacy overloads preserve every original admission boundary.
inline bool aiReady(const TrackNavigation50EU&) noexcept {return true;}
inline bool mapReady(const TrackNavigation50EU&) noexcept {return true;}
inline bool pairReady(const TrackNavigation50EU&,const TrackNavigation50EU&) noexcept {return true;}
inline bool aiJoined(const TrackNavigation50EU&,const AiTrackMetadata50IU&) noexcept {return true;}
inline bool metadataReady(const AiTrackMetadata50IU&) noexcept {return true;}
inline bool metadataReady(const AiTrackMetadataView50IU& metadata) noexcept {
    return metadata.runtimeValid()&&metadata.role()==NavigationRole50MB::Ai;
}
inline bool aiReady(const NavigationRead& v) noexcept {return aiReady(v.source());}
inline bool mapReady(const NavigationRead& v) noexcept {return mapReady(v.source());}
inline bool aiJoined(const NavigationRead& n,const AiTrackMetadataView50IU& m) noexcept {return aiJoined(n.source(),m);}
inline bool aiJoined(const NavigationRead& n,const MetadataRead& m) noexcept {return aiJoined(n.source(),m.source());}
inline bool aiJoined(const TrackNavigationView50EU& n,const MetadataRead& m) noexcept {return aiJoined(n,m.source());}
inline bool metadataReady(const MetadataRead& m) noexcept {return metadataReady(m.source());}
inline bool pairReady(const NavigationRead& m,const NavigationRead& a) noexcept {return pairReady(m.source(),a.source());}
} // namespace rr::game::navigation_consumer_detail50MB

#include "navigation_consumer_bridge50tc.h"
#include "navigation_consumer_bridge50tb.h"

#include "navigation_consumer_bridge50gb.h"

#include "navigation_consumer_bridge50mc.h"
