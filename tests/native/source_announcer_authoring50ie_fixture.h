#pragma once
// Cold metadata only. This does not decode PCM, create mixer voices, select a
// live event, or perform a live announcer request.
#include "pod_materials50ed_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../src/game/shared_announcer50ie.h"
#include "common_race_log50cr.h"
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
// One event-file rule for every pod and every bank (B-P02). Original 2C1170
// reads [ResponseEmitter] NumEvents and each [EventN] through Config; 292650
// selects Event1..NumEvents inclusive. So:
//  - a section EventN with N > NumEvents is never selected (ignored);
//  - an EventN <= NumEvents whose section is absent is a hole: nothing is read
//    for it and nothing may be selected from it (present[N-1] == false).
// Storage is sized once from the file's own NumEvents, bounded by `capacity`
// (the caller's semantic key range); a larger count is refused before any store.
struct EventRows {
    unsigned declared=0;                                // NumEvents
    unsigned presentCount=0;                            // EventN sections present, N <= NumEvents
    std::vector<rr::assets::AudioRequest50HC> rows;     // [declared]; a hole keeps an empty name
    std::vector<bool> present;                          // [declared]
};
inline EventRows eventRows(const std::vector<std::uint8_t>& bytes,unsigned capacity) {
    using pod_materials50ed_fixture::detail::need;
    std::string_view text(reinterpret_cast<const char*>(bytes.data()),bytes.size());
    // Pass 1: NumEvents (the [ResponseEmitter] header, read once).
    unsigned count=0;bool header=false,inEmitter=false;
    for(std::size_t at=0;at<text.size();){
        auto end=text.find('\n',at);if(end==text.npos)end=text.size();
        const auto line=trim(text.substr(at,end-at));at=end+1;
        if(line.empty()||line.front()==';')continue;
        if(line.front()=='['){inEmitter=line=="[ResponseEmitter]";continue;}
        if(!inEmitter)continue;
        const auto eq=line.find('=');need(eq!=line.npos,"50IE malformed response header");
        const auto key=trim(line.substr(0,eq)),value=trim(line.substr(eq+1));
        need(key=="NumEvents"&&!header,"50IE unsupported/repeated response header");
        const auto p=std::from_chars(value.data(),value.data()+value.size(),count);
        need(p.ec==std::errc{}&&p.ptr==value.data()+value.size(),"50IE invalid event count");header=true;
    }
    need(header,"50IE missing event count");
    need(count<=capacity,"50IE event count exceeds the bank's key range");
    EventRows out;out.declared=count;out.rows.resize(count);out.present.assign(count,false);
    std::vector<bool> names(count,false),sections(count,false);
    // Pass 2: the events 1..NumEvents; later sections are unselected.
    unsigned section=0;bool selected=false;
    for(std::size_t at=0;at<text.size();) {
        auto end=text.find('\n',at);if(end==text.npos)end=text.size();
        const auto line=trim(text.substr(at,end-at));at=end+1;
        if(line.empty()||line.front()==';')continue;
        if(line.front()=='['){
            if(line=="[ResponseEmitter]"){section=0;selected=false;continue;}
            need(line.size()>7&&line.substr(0,6)=="[Event"&&line.back()==']',"50IE malformed event section");
            const auto n=line.substr(6,line.size()-7);unsigned id=0;
            const auto p=std::from_chars(n.data(),n.data()+n.size(),id);
            need(p.ec==std::errc{}&&p.ptr==n.data()+n.size()&&id>0,"50IE invalid event key");
            section=id;selected=id<=count;
            if(selected){need(!sections[id-1],"50IE repeated event section");sections[id-1]=true;}
            continue;
        }
        if(!section||!selected)continue;   // header (pass 1) or an unselected EventN > NumEvents
        const auto eq=line.find('=');need(eq!=line.npos,"50IE malformed event value");
        const auto key=trim(line.substr(0,eq)),value=trim(line.substr(eq+1));
        auto&r=out.rows[section-1];
        if(key=="FileName"){need(!names[section-1]&&!value.empty(),"50IE repeated/missing filename");r.name=value;names[section-1]=true;}
        else {need((key=="IsStereo"||key=="IsLoop"||key=="IsStream")&&(value=="0"||value=="1"),"50IE unsupported event flag");
            if(key=="IsStereo")r.stereo=value=="1";
            if(key=="IsLoop")r.loop=value=="1";
            if(key=="IsStream")r.stream=value=="1";}
    }
    for(unsigned i=0;i<count;++i){
        need(sections[i]==names[i],"50IE event section without its FileName");
        if(sections[i]){out.present[i]=true;++out.presentCount;}
    }
    return out;
}
// Strict form for the shared (non-pod) files: exactly `expected` events, no hole.
inline std::vector<rr::assets::AudioRequest50HC> events(
    const std::vector<std::uint8_t>&bytes,unsigned expected) {
    using pod_materials50ed_fixture::detail::need;
    auto r=eventRows(bytes,expected);
    need(r.declared==expected,"50IE event count differs");
    need(r.presentCount==expected,"50IE missing authored event");
    return std::move(r.rows);
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
        // PORT EXTENSION, NOT ORIGINAL: an add-on pod (DLC registry row) has
        // no named announcer INI, so it gets no named bank and no named cues
        // (generic lines only). It never indexes the 27-row original table.
        const auto addonRacer=[](std::uint32_t racer){return original_player_identity50IK::addonPodOf(racer)!=nullptr;};
        std::size_t named=0,addons=0;
        for(std::size_t i=0;i<count;++i){
            const auto racer=roster?actualRacer60[i]:playerRacer60;
            if(addonRacer(racer)){
                d::need(original_player_identity50IK::find(racer)!=nullptr,"50IE add-on announcer identity unsupported");
                for(std::size_t k=0;k<i;++k)d::need((roster?actualRacer60[k]:playerRacer60)!=racer,
                    "50IE duplicate actual announcer racer60");
                ++addons;continue;
            }
            d::need(racer<next.namedBanks.size()&&original_player_identity50IK::find(racer)&&
                rr::game::original_racer_selection50IJ::findByRacer60(racer),
                "50IE actual named announcer identity unsupported");
            d::need(!next.namedBanks[racer],"50IE duplicate actual announcer racer60");
            next.namedBanks[racer]=true;++named;
        }
        d::need(addonRacer(playerRacer60)||(playerRacer60<next.namedBanks.size()&&next.namedBanks[playerRacer60]),
            "50IE selected racer is absent from actual announcer roster");
        if(addons)std::cerr<<"dlc_announcer named_banks="<<named<<" addon_racers="<<addons<<" addon_lines=generic\n";
        // One generic INI, one SPCHPCT, at most eight2496-byte named INIs.
        // These are cold metadata reads only; no selector or RNG is touched.
        d::Inputs input;input.capacity=32768;
        const auto generic=input.read(discRoot/"AUDIO/SPEECH/DAX/INRACE/GEN_ANN.INI",32768);
        const auto percentages=input.read(discRoot/"AUDIO/SPCHPCT.INI",32768);
        // Stock digests are log-only receipts; the files are read by their own rows (C4).
        if(!(generic.size()==11704&&d::sha256(generic)=="24adbcb8784b20fab3d1f97650ffcb3bf12d0c3846b84b7f2477796dfbdcf6ab"))
            std::cerr<<"common_shared_file member=GEN_ANN.INI bytes="<<generic.size()<<" receipt=differs\n";
        if(!(percentages.size()==255&&d::sha256(percentages)=="24c2967388a31d04761fb11208faaae43a4ae7b9b88b40291bf64daf195c018c"))
            std::cerr<<"common_shared_file member=SPCHPCT.INI bytes="<<percentages.size()<<" receipt=differs\n";
        const std::string_view pct(reinterpret_cast<const char*>(percentages.data()),percentages.size());
        // PORT STAND-IN (flagged): DaxGenericPercent / DaxSpeechDelay are required present; the
        // announcer selector still uses the stock values (0.80 / 7.0), not the read ones.
        d::need(pct.find("DaxGenericPercent=")!=pct.npos&&pct.find("DaxSpeechDelay=")!=pct.npos,
            "50IE original announcer fields absent");
        const auto g=detail::events(generic,150);
        next.cues.reserve(roster?74+18*named:42+12*named);
        std::size_t namedCues=0;
        //2C2FBC/2C30CC select generic1..4/5..32 for collision types1/2.
        for(unsigned key=roster?1u:33u;key<=51;++key)next.cues.push_back({0,key,g[key-1]});
        for(unsigned key=128;key<=150;++key)next.cues.push_back({0,key,g[key-1]});
        //2C4B30 creates each bank from actual manager racer60, independently
        // of vehicle58. This also permits the real AI event5 type12 fallback.
        for(std::size_t i=0;i<count;++i){
            const auto racer=roster?actualRacer60[i]:playerRacer60;
            if(addonRacer(racer))continue; // no named bank (see above)
            const auto& identity=*original_player_identity50IK::find(racer);
            const auto pod=input.read(original_player_identity50IK::announcerPath(discRoot,identity),
                identity.announcerBytes);
            d::need(pod.size()==identity.announcerBytes&&d::sha256(pod)==identity.announcerSha256,
                std::string("50IE original named announcer differs: ")+identity.podId);
            // Each pod's own NumEvents (29 or 30 on the stock disc; the receipt
            // identity.announcerEvents is compared, never used to size or gate).
            // A section past NumEvents is never selected (292650); a hole is skipped.
            const auto p=detail::eventRows(pod,256u);
            common_race_log50CR::podReceipt(racer,"ANN.INI.NumEvents",p.declared==identity.announcerEvents?
                common_race_log50CR::Receipt::match:common_race_log50CR::Receipt::differs);
            auto cue=[&](unsigned key){
                d::need(key<=p.declared,std::string("50IE named announcer lacks a selected event: ")+identity.podId);
                if(!p.present[key-1])return;     // hole: nothing to select
                next.cues.push_back({racer,key,p.rows[key-1]});++namedCues;
            };
            for(unsigned key=roster?1u:7u;key<=15;++key)cue(key);
            for(unsigned key=24;key<=26;++key)cue(key);
        }
        d::need(next.cues.size()==(roster?74u:42u)+namedCues,"50IE semantic roster count differs");
        for(const auto&c:next.cues)d::need(!c.request.loop&&!c.request.stereo&&c.request.stream,
            "50IE original announcer delivery flags differ");
        out=std::move(next);error.clear();return true;
    }catch(const std::exception&e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="50IE announcer authoring read failed";}catch(...){}return false;}
}
} // namespace source_announcer_authoring50IE
