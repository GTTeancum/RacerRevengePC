#pragma once
#include "../../src/assets/track_height_descriptor50jb.h"
#include "rc_cold_sources50la.h"
#include "tb_cold_sources50la.h"
#include "gb_cold_sources50la.h"
#include <type_traits>
#include "water_resources50jc_fixture.h"
#include "../../src/game/undertrack_reset50ew.h"
#include <string>
#include <utility>

namespace track_height_cold50JB {
// Cold, immutable source ownership shared by every vehicle210770. Present
// records the required original provider branch, not a constructed native
// height implementation. The latter remains explicitly unavailable.
struct Owner {
    rr::assets::TrackHeightDescriptor50JB descriptor{};
    rr::game::UndertrackPhysicsDomain50EW sourceDomain{};
    std::string sceneMember,waterMember;
    std::uint32_t waterLogicalOffset=0;
    std::vector<std::uint8_t> waterSource;
    std::shared_ptr<water_resources50JC::Owner> nativeWaterResources;
    // Resource storage alone never sets waterNativeQueryBound.
    // RC typed path pins the SAME immutable source; legacy paths leave empty.
    std::shared_ptr<const rc_cold_sources50LA::Owner> rcSource;
    std::shared_ptr<const tb_cold_sources50LA::Owner> tbSource;
    std::shared_ptr<const gb_cold_sources50LA::Owner> gbSource;
    bool sourceBound=false;
    bool waterNativeQueryBound=false;
};
inline bool sameAscii(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i){
        const auto lower=[](char c){return c>='A'&&c<='Z'?char(c+'a'-'A'):c;};
        if(lower(a[i])!=lower(b[i]))return false;
    }
    return true;
}
// Actual fresh1A9EE0 game-track path only. Its initializer270550 clears3FC
// and400; the complete path invokes water+130 but never terrain+12C. This
// contract does not apply to reused tracks or calls to274460 from elsewhere.
// scene/document/archive must originate from the same authenticated mount.
// A catalog hit is usable without pretending this archive is the entire
// resource manager. A miss remains unknown, never synthetic Absent.
namespace detail {
using RcSource=std::shared_ptr<const rc_cold_sources50LA::Owner>;
using TbSource=std::shared_ptr<const tb_cold_sources50LA::Owner>;
using GbSource=std::shared_ptr<const gb_cold_sources50LA::Owner>;
inline const auto& entries(const rr::assets::ResArchive48C& archive)noexcept{return archive.entries;}
inline const auto& entries(const RcSource& archive)noexcept{return archive->archive().entries();}
inline const auto& entries(const TbSource& archive)noexcept{return archive->archive().entries();}
inline const auto& entries(const GbSource& archive)noexcept{return archive->archive().entries();}
inline bool extract(const rr::assets::ResArchive48C& archive,const std::string& name,
    std::vector<std::uint8_t>& out,rr::assets::DecodeError48C& d){return rr::assets::extractResMember48C(archive,name,out,d);}
inline bool extract(const RcSource& archive,const std::string& name,
    std::vector<std::uint8_t>& out,rr::assets::DecodeError48C& d){return archive->extractMember(name,out,d,8u*1024u*1024u);}
inline bool extract(const TbSource& archive,const std::string& name,
    std::vector<std::uint8_t>& out,rr::assets::DecodeError48C& d){return archive->extractMember(name,out,d,8u*1024u*1024u);}
inline bool extract(const GbSource& archive,const std::string& name,
    std::vector<std::uint8_t>& out,rr::assets::DecodeError48C& d){return archive->extractMember(name,out,d,8u*1024u*1024u);}
template<class Archive> requires (std::is_same_v<Archive,rr::assets::ResArchive48C>||std::is_same_v<Archive,RcSource>||std::is_same_v<Archive,TbSource>||std::is_same_v<Archive,GbSource>)
inline bool prepareFreshShared(const rr::assets::World48F& world,
    const Archive& archive,const std::string& sceneMember,
    Owner& out,std::string& error) {
    using namespace rr::assets;using rr::game::UndertrackHeightOwner50EW;
    if constexpr(std::is_same_v<Archive,RcSource>||std::is_same_v<Archive,TbSource>||std::is_same_v<Archive,GbSource>){
        if(!archive||!archive->ready()||&world!=&archive->world()){error="RC height requires same ready retained source";return false;}
    }
    Owner staged{};
    if constexpr(std::is_same_v<Archive,RcSource>)staged.rcSource=archive;
    if constexpr(std::is_same_v<Archive,TbSource>)staged.tbSource=archive;
    if constexpr(std::is_same_v<Archive,GbSource>)staged.gbSource=archive;
    if(readTrackHeightDescriptor50JB(world.parameters,staged.descriptor)!=
        TrackHeightReadStatus50JB::Complete){error="original Environment height strings unavailable";return false;}
    //1B38C4 splitpath and1B38DC drive+directory, retained relative/no-drive
    // source-member domain. Do not normalize authored filenames or strip dots.
    if(sceneMember.empty()||sceneMember.find(':')!=std::string::npos||
       sceneMember.front()=='/'||sceneMember.front()=='\\'||
       sceneMember.find("..")!=std::string::npos){error="track source path outside retained relative domain";return false;}
    const auto slash=sceneMember.find_last_of("/\\");
    if(slash==std::string::npos){error="track source directory unavailable";return false;}
    staged.sceneMember=sceneMember;
    staged.sourceDomain.terrain330=UndertrackHeightOwner50EW::Absent;
    const std::string water=staged.descriptor.waterFile110.value.data();
    if(water=="none"){
        //2745B0 exact, case-sensitive comparison bypasses water construction.
        staged.sourceDomain.water334=UndertrackHeightOwner50EW::Absent;
    }else{
        staged.waterMember=sceneMember.substr(0,slash+1)+water;
        if(staged.waterMember.size()>=260u){error="water source path exceeds original cold buffer";return false;}
        const ResourceEntry48C* match=nullptr;
        for(const auto& entry:entries(archive))if(sameAscii(entry.name,staged.waterMember)){
            if(match){error="water resource identity is ambiguous";return false;}match=&entry;
        }
        if(!match){error="water resource lookup needs the complete original mounted catalog";return false;}
        DecodeError48C decodeError;
        if(!extract(archive,staged.waterMember,staged.waterSource,decodeError)){
            error="actual water source extraction failed: "+decodeError.code;return false;
        }
        staged.waterLogicalOffset=match->logical_offset;
        //27473C constructs2B8370 and274744 stores track400 before setup;
        //210800 copies this required original provider into vehicle334.
        staged.sourceDomain.water334=UndertrackHeightOwner50EW::Present;
    }
    staged.sourceBound=true;out=std::move(staged);error.clear();return true;
}
} // detail
inline bool prepareFreshGameTrack(const rr::assets::World48F& world,
    const rr::assets::ResArchive48C& archive,const std::string& sceneMember,
    Owner& out,std::string& error){return detail::prepareFreshShared(world,archive,sceneMember,out,error);}
inline bool prepareFreshGameTrack(std::shared_ptr<const rc_cold_sources50LA::Owner> source,
    const std::string& sceneMember,Owner& out,std::string& error){
    if(!source||!source->ready()){error="RC height requires ready retained source";return false;}
    return detail::prepareFreshShared(source->world(),source,sceneMember,out,error);
}
inline bool prepareFreshGameTrack(std::shared_ptr<const tb_cold_sources50LA::Owner> source,
    const std::string& sceneMember,Owner& out,std::string& error){
    if(!source||!source->ready()){error="TB height requires ready retained source";return false;}
    return detail::prepareFreshShared(source->world(),source,sceneMember,out,error);
}

inline bool prepareFreshGameTrack(std::shared_ptr<const gb_cold_sources50LA::Owner> source,
    const std::string& sceneMember,Owner& out,std::string& error){
    if(!source||!source->ready()){error="GB height requires ready retained source";return false;}
    return detail::prepareFreshShared(source->world(),source,sceneMember,out,error);
}

} // namespace track_height_cold50JB
