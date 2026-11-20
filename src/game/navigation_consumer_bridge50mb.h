#pragma once
// Common rewrite (R4, B-C12): the ONE read facade the navigation consumers (AI
// 16FFE0 family, combat 2A2DC0, attack 1711E0/16EDD0, radar 1924C0, lap
// 2A1980/1A0F18, wrong-way 1A0B20) use over the one common view. Replaces the
// 50MB facade and its TC/TB/GB/MC copies (navigation_consumer_bridge50{tc,tb,gb,mc}.h).
#include "navigation_view50cr.h"
#include "ai_cold_owners50iu.h"

namespace rr::game::navigation_consumer_detail50MB {
// Only issued immutable views enter these local facades. No public span/count
// factory, storage resizing, node truncation or replacement controller exists.
class GraphRead {
public:
    const UndertrackNode50ET* const nodes;
    const std::uint32_t count;
    const std::size_t extent;
private:
    NavigationView50CR owner_;
    explicit GraphRead(const NavigationView50CR& v) noexcept
        :nodes(v.nodes()),count(v.count()),extent(v.capacity()),owner_(v){}
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
    NavigationView50CR owner_;
    explicit NavigationRead(const NavigationView50CR& v) noexcept
        :track(v),nodes(v.gates()),gatesPrepared(v.gatesPrepared()),extent(v.capacity()),owner_(v){}
    friend NavigationRead read(const NavigationView50CR&) noexcept;
public:
    const NavigationView50CR& source() const noexcept {return owner_;}
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
inline NavigationRead read(const NavigationView50CR& v) noexcept {
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
inline const NavigationView50CR* pointer(const NavigationView50CR& v) noexcept {return &v;}
inline const AiTrackMetadataView50IU* pointer(const AiTrackMetadataView50IU& v) noexcept {return &v;}
inline bool aiReady(const NavigationView50CR& ai) noexcept {
    return ai.runtimeValid()&&ai.role()==NavigationRole50CR::ai;
}
inline bool mapReady(const NavigationView50CR& map) noexcept {
    return map.runtimeValid()&&map.role()==NavigationRole50CR::map;
}
inline bool aiJoined(const NavigationView50CR& ai,const AiTrackMetadataView50IU& metadata) noexcept {
    return aiReady(ai)&&metadata.matches(ai);
}
inline bool pairReady(const NavigationView50CR& map,const NavigationView50CR& ai) noexcept {
    return mapReady(map)&&aiReady(ai)&&map.sameOwner(ai);
}

// Constant-true legacy overloads preserve every original admission boundary.
inline bool aiReady(const TrackNavigation50EU&) noexcept {return true;}
inline bool mapReady(const TrackNavigation50EU&) noexcept {return true;}
inline bool pairReady(const TrackNavigation50EU&,const TrackNavigation50EU&) noexcept {return true;}
inline bool aiJoined(const TrackNavigation50EU&,const AiTrackMetadata50IU&) noexcept {return true;}
inline bool metadataReady(const AiTrackMetadata50IU&) noexcept {return true;}
inline bool metadataReady(const AiTrackMetadataView50IU& metadata) noexcept {
    return metadata.runtimeValid()&&metadata.role()==NavigationRole50CR::ai;
}
inline bool aiReady(const NavigationRead& v) noexcept {return aiReady(v.source());}
inline bool mapReady(const NavigationRead& v) noexcept {return mapReady(v.source());}
inline bool aiJoined(const NavigationRead& n,const AiTrackMetadataView50IU& m) noexcept {return aiJoined(n.source(),m);}
inline bool aiJoined(const NavigationRead& n,const MetadataRead& m) noexcept {return aiJoined(n.source(),m.source());}
inline bool aiJoined(const NavigationView50CR& n,const MetadataRead& m) noexcept {return aiJoined(n,m.source());}
inline bool metadataReady(const MetadataRead& m) noexcept {return metadataReady(m.source());}
inline bool pairReady(const NavigationRead& m,const NavigationRead& a) noexcept {return pairReady(m.source(),a.source());}
} // namespace rr::game::navigation_consumer_detail50MB
