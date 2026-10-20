#pragma once
// Cold metadata only. This does not decode PCM, create mixer voices, select a
// live event, or perform a live announcer request.
#include "pod_materials50ed_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../src/game/shared_announcer50ie.h"
#include <charconv>
#include <string_view>

namespace source_announcer_authoring50IE {
struct AuthoredCue {
    std::uint32_t bankRacer=0,key=0;
    rr::assets::AudioRequest50HC request;
};
struct Authoring {
    rr::game::AnnouncerParameters50IE parameters{};
    std::vector<AuthoredCue> cues;
    std::array<bool,27> namedBanks{};
    bool collisionRequests=false;
};
namespace detail {
inline std::string_view trim(std::string_view s)noexcept {
    const auto a=s.find_first_not_of(" \t\r");if(a==s.npos)return {};
    return s.substr(a,s.find_last_not_of(" \t\r")-a+1);
}
inline std::vector<rr::assets::AudioRequest50HC> events(
    const std::vector<std::uint8_t>&bytes,unsigned expected) {
    using pod_materials50ed_fixture::detail::need;
    std::vector<rr::assets::AudioRequest50HC> out(expected);
    std::vector<bool> names(expected,false);unsigned section=0,count=0;
    std::string_view text(reinterpret_cast<const char*>(bytes.data()),bytes.size());
    for(std::size_t at=0;at<text.size();) {
        auto end=text.find('\n',at);if(end==text.npos)end=text.size();
        const auto line=trim(text.substr(at,end-at));at=end+1;
        if(line.empty()||line.front()==';')continue;
        if(line.front()=='['){
            if(line=="[ResponseEmitter]"){section=0;continue;}
            need(line.size()>7&&line.substr(0,6)=="[Event"&&line.back()==']',"50IE malformed event section");
            const auto n=line.substr(6,line.size()-7);unsigned id=0;
            const auto p=std::from_chars(n.data(),n.data()+n.size(),id);
            need(p.ec==std::errc{}&&p.ptr==n.data()+n.size()&&id>0&&id<=expected,"50IE invalid event key");
            section=id;continue;
        }
        const auto eq=line.find('=');need(eq!=line.npos,"50IE malformed event value");
        const auto key=trim(line.substr(0,eq)),value=trim(line.substr(eq+1));
        if(!section){need(key=="NumEvents"&&count==0,"50IE unsupported/repeated response header");
            const auto p=std::from_chars(value.data(),value.data()+value.size(),count);
            need(p.ec==std::errc{}&&p.ptr==value.data()+value.size()&&count==expected,"50IE event count differs");continue;}
        auto&r=out[section-1];
        if(key=="FileName"){need(!names[section-1]&&!value.empty(),"50IE repeated/missing filename");r.name=value;names[section-1]=true;}
        else {need((key=="IsStereo"||key=="IsLoop"||key=="IsStream")&&(value=="0"||value=="1"),"50IE unsupported event flag");
            if(key=="IsStereo")r.stereo=value=="1";
            if(key=="IsLoop")r.loop=value=="1";
            if(key=="IsStream")r.stream=value=="1";}
    }
    need(count==expected,"50IE missing event count");for(bool n:names)need(n,"50IE missing authored event");return out;
}

// Original292650 selects Event1..NumEvents inclusive. Exact POD17 declares29
// while retaining an unselected Event30. Keep the generic text reader strict.
inline std::vector<rr::assets::AudioRequest50HC> scorch17DeclaredEvents(
    const std::vector<std::uint8_t>& bytes) {
    using pod_materials50ed_fixture::detail::need;
    need(bytes.size()==2478&&pod_materials50ed_fixture::detail::sha256(bytes)==
        "ff897119ce1d9eba85e7f9b69f1811bd9d182dfb87db6afe3f022442ec6a2398",
        "50IE exact POD17 declared-count source differs");
    constexpr std::size_t unselectedEvent30=2405;
    const std::string_view text(reinterpret_cast<const char*>(bytes.data()),bytes.size());
    need(text.substr(unselectedEvent30,9)=="[Event30]"&&
        text.find("NumEvents=29")!=text.npos,"50IE exact POD17 source boundary differs");
    return events(std::vector<std::uint8_t>(bytes.begin(),bytes.begin()+unselectedEvent30),29);
}

}
inline bool readAuthoring(const std::filesystem::path&discRoot,Authoring&out,std::string&error,
    std::uint32_t playerRacer60=1,const std::uint32_t* actualRacer60=nullptr,
    std::size_t actualRacerCount=0) {
    namespace d=pod_materials50ed_fixture::detail;
    try {
        const bool roster=actualRacerCount!=0;
        d::need((actualRacer60!=nullptr)==roster&&actualRacerCount<=8,
            "50IE invalid actual announcer roster extent");
        Authoring next;next.parameters={.80f,7.f};next.collisionRequests=roster;
        const std::size_t count=roster?actualRacerCount:1;
        for(std::size_t i=0;i<count;++i){
            const auto racer=roster?actualRacer60[i]:playerRacer60;
            d::need(racer<next.namedBanks.size()&&original_player_identity50IK::find(racer)&&
                rr::game::original_racer_selection50IJ::findByRacer60(racer),
                "50IE actual named announcer identity unsupported");
            d::need(!next.namedBanks[racer],"50IE duplicate actual announcer racer60");
            next.namedBanks[racer]=true;
        }
        d::need(playerRacer60<next.namedBanks.size()&&next.namedBanks[playerRacer60],
            "50IE selected racer is absent from actual announcer roster");
        // One generic INI, one SPCHPCT, at most eight2496-byte named INIs.
        // These are cold metadata reads only; no selector or RNG is touched.
        d::Inputs input;input.capacity=32768;
        const auto generic=input.read(discRoot/"AUDIO/SPEECH/DAX/INRACE/GEN_ANN.INI",11704);
        const auto percentages=input.read(discRoot/"AUDIO/SPCHPCT.INI",255);
        d::need(generic.size()==11704&&d::sha256(generic)==
            "24adbcb8784b20fab3d1f97650ffcb3bf12d0c3846b84b7f2477796dfbdcf6ab","50IE original GEN_ANN.INI differs");
        d::need(percentages.size()==255&&d::sha256(percentages)==
            "24c2967388a31d04761fb11208faaae43a4ae7b9b88b40291bf64daf195c018c","50IE original SPCHPCT.INI differs");
        const std::string_view pct(reinterpret_cast<const char*>(percentages.data()),percentages.size());
        d::need(pct.find("DaxGenericPercent=0.80f")!=pct.npos&&pct.find("DaxSpeechDelay=7.0f")!=pct.npos,
            "50IE original announcer fields absent");
        const auto g=detail::events(generic,150);
        next.cues.reserve(roster?74+18*count:54);
        //2C2FBC/2C30CC select generic1..4/5..32 for collision types1/2.
        for(unsigned key=roster?1u:33u;key<=51;++key)next.cues.push_back({0,key,g[key-1]});
        for(unsigned key=128;key<=150;++key)next.cues.push_back({0,key,g[key-1]});
        //2C4B30 creates each bank from actual manager racer60, independently
        // of vehicle58. This also permits the real AI event5 type12 fallback.
        for(std::size_t i=0;i<count;++i){
            const auto racer=roster?actualRacer60[i]:playerRacer60;
            const auto& identity=*original_player_identity50IK::find(racer);
            const auto pod=input.read(original_player_identity50IK::announcerPath(discRoot,identity),
                identity.announcerBytes);
            d::need(pod.size()==identity.announcerBytes&&d::sha256(pod)==identity.announcerSha256,
                std::string("50IE original named announcer differs: ")+identity.podId);
            // Original POD01/POD11 author29 events, other retained pods30.
            const auto p=racer==17?detail::scorch17DeclaredEvents(pod):
                detail::events(pod,identity.announcerEvents);
            for(unsigned key=roster?1u:7u;key<=15;++key)next.cues.push_back({racer,key,p[key-1]});
            for(unsigned key=24;key<=26;++key)next.cues.push_back({racer,key,p[key-1]});
        }
        d::need(next.cues.size()==(roster?74+18*count:54),"50IE semantic roster count differs");
        for(const auto&c:next.cues)d::need(!c.request.loop&&!c.request.stereo&&c.request.stream,
            "50IE original announcer delivery flags differ");
        out=std::move(next);error.clear();return true;
    }catch(const std::exception&e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="50IE announcer authoring read failed";}catch(...){}return false;}
}
} // namespace source_announcer_authoring50IE
