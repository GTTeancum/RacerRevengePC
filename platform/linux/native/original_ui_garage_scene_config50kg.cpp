#include "original_ui_garage_scene_config50kg.h"
#include <algorithm>
#include <cstring>
#include <exception>
namespace rr::platform {
bool OriginalUiGarageSceneConfig50KG::NativeOpenMember::current()const noexcept {
    if(!opened||!provider||!archive||!archive->loaded()||!member.member||!member.data||
       !authenticated.identity||!authenticated.data||cursor>member.size)return false;
    Provider::MemberView now;
    return provider->lookup132820(requested,now)==Provider::Lookup::present&&
        now.member==member.member&&now.data==member.data&&now.size==member.size&&
        now.mount==member.mount&&now.ordinal==member.ordinal&&
        authenticated.size==member.size;
}
bool OriginalUiGarageSceneConfig50KG::NativeOpenMember::read(void* out,std::size_t count,
    std::size_t& readCount)noexcept {
    readCount=0;if(!current()||(!out&&count))return false;
    readCount=std::min(count,member.size-cursor);
    if(readCount)std::memcpy(out,member.data+cursor,readCount);
    cursor+=readCount;return true;
}
bool OriginalUiGarageSceneConfig50KG::NativeOpenMember::seek(std::size_t position)noexcept {
    if(!current()||position>member.size)return false;
    cursor=position;return true;
}
void OriginalUiGarageSceneConfig50KG::NativeOpenMember::close131040()noexcept {
    opened=false;cursor=0;member={};authenticated={};requested.clear();archive.reset();provider.reset();
}
OriginalUiGarageSceneConfig50KG::OpenResult OriginalUiGarageSceneConfig50KG::open1577D0(
    const std::shared_ptr<Provider>& provider,
    const std::shared_ptr<const assets::OriginalUiArchive50KG>& archive,
    const std::string& name,assets::OriginalUiPageDiagnostic50KG& d)noexcept {
    d={};if(closed_)return OpenResult::stale;
    //1577D0 closes an old file and discards old indices before another open.
    file00_.close131040();records_={};readBuffer_.clear();parsed_=false;
    if(!provider||!archive||!archive->loaded())return OpenResult::stale;
    Provider::MemberView found;
    const auto lookup=provider->lookup132820(name,found);
    if(lookup==Provider::Lookup::absent)return OpenResult::absent;
    if(lookup!=Provider::Lookup::present)return OpenResult::stale;
    const auto authenticated=archive->find(name);
    // A present replacement/binary in the ACTUAL namespace cannot be hidden
    // by absence in the selected authenticated Garage archive.
    if(!authenticated.identity||!authenticated.data||authenticated.size!=found.size||
       !found.data||std::memcmp(authenticated.data,found.data,found.size))return OpenResult::unsupported;
    try {
        file00_.provider=provider;file00_.archive=archive;file00_.member=found;
        file00_.authenticated=authenticated;file00_.requested=name;file00_.opened=true;
        std::uint8_t signature[7]{};std::size_t count=0;
        if(!file00_.read(signature,sizeof(signature),count))return OpenResult::stale;
        // Binary BPB input is a real reached unsupported branch, never text
        // fallback. Only an actual failed OPEN may select .scn/.toy.
        if(count>=3&&signature[0]=='B'&&signature[1]=='P'&&signature[2]=='B')return OpenResult::unsupported;
        if(!file00_.seek(0))return OpenResult::stale;
        if(found.size>assets::originalUiDatByteLimit50KG)return OpenResult::unsupported;
        readBuffer_.resize(found.size);
        if(!file00_.read(readBuffer_.data(),readBuffer_.size(),count)||count!=readBuffer_.size())return OpenResult::stale;
        const assets::OriginalUiMemberView50KG owned{authenticated.identity,readBuffer_.data(),readBuffer_.size()};
        if(!assets::OriginalUiDat50KG::parseRetainedMember(owned,records_,d,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return OpenResult::unsupported;
        parsed_=true;return OpenResult::opened;
    }catch(...){return OpenResult::allocation;}
}
bool OriginalUiGarageSceneConfig50KG::current()const noexcept {
    return !closed_&&file00_.current();
}
const assets::OriginalUiDat50KG* OriginalUiGarageSceneConfig50KG::parsed()const noexcept {
    return parsed_&&current()?&records_:nullptr;
}
void OriginalUiGarageSceneConfig50KG::close1573B0()noexcept {
    if(closed_)return;
    file00_.close131040(); //Actual file closes before owned parser buffers.
    records_={};std::vector<std::uint8_t>{}.swap(readBuffer_);parsed_=false;closed_=true;
}
OriginalUiGarageSceneConfig50KG::~OriginalUiGarageSceneConfig50KG()noexcept {
    if(!closed_)std::terminate(); //Owner must execute explicit source cleanup.
}
}
