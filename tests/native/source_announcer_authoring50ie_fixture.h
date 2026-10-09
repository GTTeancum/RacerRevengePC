#pragma once
// Cold metadata only. This does not decode PCM, create mixer voices, select a
// live event, or attach the unfinished announcer owner to the native frame.
#include "pod_materials50ed_fixture.h"
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
}
inline bool readAuthoring(const std::filesystem::path&discRoot,Authoring&out,std::string&error) {
    namespace d=pod_materials50ed_fixture::detail;
    try {
        d::Inputs input;input.capacity=16384;
        const auto generic=input.read(discRoot/"AUDIO/SPEECH/DAX/INRACE/GEN_ANN.INI",11704);
        const auto pod=input.read(discRoot/"AUDIO/SPEECH/DAX/INRACE/POD01ANN.INI",2403);
        const auto percentages=input.read(discRoot/"AUDIO/SPCHPCT.INI",255);
        d::need(generic.size()==11704&&d::sha256(generic)==
            "24adbcb8784b20fab3d1f97650ffcb3bf12d0c3846b84b7f2477796dfbdcf6ab","50IE original GEN_ANN.INI differs");
        d::need(pod.size()==2403&&d::sha256(pod)==
            "8c4f9b759c8fe933ab85341eff4d42e45e2c391e14f05ccad01238d04139a591","50IE original POD01ANN.INI differs");
        d::need(percentages.size()==255&&d::sha256(percentages)==
            "24c2967388a31d04761fb11208faaae43a4ae7b9b88b40291bf64daf195c018c","50IE original SPCHPCT.INI differs");
        const std::string_view pct(reinterpret_cast<const char*>(percentages.data()),percentages.size());
        d::need(pct.find("DaxGenericPercent=0.80f")!=pct.npos&&pct.find("DaxSpeechDelay=7.0f")!=pct.npos,
            "50IE original announcer fields absent");
        auto g=detail::events(generic,150),p=detail::events(pod,29);
        Authoring next;next.parameters={.80f,7.f};next.cues.reserve(54);
        for(unsigned key=33;key<=51;++key)next.cues.push_back({0,key,g[key-1]});
        for(unsigned key=7;key<=15;++key)next.cues.push_back({1,key,p[key-1]});
        for(unsigned key=128;key<=150;++key)next.cues.push_back({0,key,g[key-1]});
        for(unsigned key=24;key<=26;++key)next.cues.push_back({1,key,p[key-1]});
        for(const auto&c:next.cues)d::need(!c.request.loop&&!c.request.stereo&&c.request.stream,
            "50IE original announcer delivery flags differ");
        out=std::move(next);error.clear();return true;
    }catch(const std::exception&e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="50IE announcer authoring read failed";}catch(...){}return false;}
}
} // namespace source_announcer_authoring50IE
