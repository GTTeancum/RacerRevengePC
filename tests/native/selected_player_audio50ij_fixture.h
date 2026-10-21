#pragma once
// Cold audio selection for the retained originals, including POD19. Semantic
// slots retain bank/key identity; absent original keys reject commands. Every
// pod-owned member and engine parameter comes from that pod's authenticated RES.
#include "gameplay_audio_provider50he_fixture.h"
#include "source_announcer_authoring50ie_fixture.h"
#include "native_racer_rig50hw_fixture.h"
#include "opponent_engine_audio50hx_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "../../src/game/pod_speech50hg.h"

namespace selected_player_audio50IJ {
using Catalog=std::array<rr::assets::AudioRequest50HC,gameplay_audio50HE::cueCount>;
struct Authoring {
    Catalog catalog{};
    std::vector<rr::assets::AudioRequest50HC> requests;
    rr::game::PodSpeechCounts50HG speech{};
    rr::game::PodEngineParameters50HE engine{};
};
inline bool read(const std::filesystem::path& discRoot,std::uint32_t racer60,
    Authoring& out,std::string& error) {
    namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
    namespace id=original_player_identity50IK;
    using namespace rr::game;
    try {
        const auto* identity=id::find(racer60);
        d::need(identity,"50IK selected original player identity unsupported");
        // Preserve authenticated TA intro and literal boost from the existing
        // shared course/global reader. No pod-owned row is borrowed from its
        // POD01 input. This eight-racer package still requires POD01 data.
        std::vector<a::AudioRequest50HC> baseline;
        if(!a::readRetainedAudioRequests50HC(discRoot,baseline,error))return false;
        Authoring next;
        for(std::size_t i=0;i<next.catalog.size();++i){
            const auto& cue=gameplay_audio50HE::catalog[i];
            if(cue.bank!=AudioBank50HE::TrackIntro&&
                !(cue.bank==AudioBank50HE::PodEngine&&cue.key==3))continue;
            const a::AudioRequest50HC* found=nullptr;
            for(const auto& request:baseline)if(native_racer50HW::rig_detail::lower(request.name)==
                native_racer50HW::rig_detail::lower(cue.filename))found=&request;
            d::need(found,"50IK original course/global cue absent");next.catalog[i]=*found;
        }
        d::Inputs input;input.capacity=identity->archiveBytes;
        const auto bytes=input.read(id::archivePath(discRoot,*identity),identity->archiveBytes);
        d::need(bytes.size()==identity->archiveBytes&&d::sha256(bytes)==identity->archiveSha256,
            std::string("50IK original archive identity differs: ")+identity->podId);
        a::ResArchive48C archive;a::DecodeError48C decode;a::DecodeLimits48C limits;
        limits.resource_file_bytes=identity->archiveBytes;
        limits.resource_decoded_bytes=16u*1024u*1024u;limits.resource_entries=512;limits.resource_name_bytes=256;
        d::need(a::decodeResource48C(bytes,archive,decode,nullptr,limits),
            std::string("50IK original archive decode failed: ")+identity->podId);
        const auto member=[&](const std::string& name,const char* digest){
            std::vector<std::uint8_t> result;
            d::need(a::extractResMember48C(archive,name,result,decode,8192),
                std::string("50IK original member missing: ")+identity->podId+"/"+name);
            d::need(d::sha256(result)==digest,
                std::string("50IK original member identity differs: ")+identity->podId+"/"+name);
            return result;
        };
        const auto put=[&](AudioBank50HE bank,unsigned key,const a::AudioRequest50HC& request){
            const auto index=gameplay_audio50HE::cueIndex({bank,key});
            d::need(index<next.catalog.size()&&next.catalog[index].name.empty(),
                "50IK selected cue is duplicate or exceeds native semantic capacity");
            next.catalog[index]=request;
        };
        const auto events=[&](AudioBank50HE bank,const std::string& name,const char* digest,unsigned count){
            const auto bytes=member(name,digest);
            if(racer60==26&&bank==AudioBank50HE::PodCurses){
                // Authenticated POD26 declares14, omits Event2, and retains
                // Event3..14. Preserve exact source keys: never compact the
                // hole or synthesize another racer's line for it. Speech-count
                // selection stays independently authored (sum13).
                const auto ini=native_racer50HW::rig_detail::ini(bytes);
                d::need(count==13&&native_racer50HW::rig_detail::value(ini,"responseemitter","numevents")=="14"&&
                    !ini.count("event2")&&ini.size()==14,"POD26 original sparse curses structure differs");
                for(unsigned key=1;key<=14;++key){if(key==2)continue;
                    const auto section="event"+std::to_string(key);
                    const auto& row=ini.at(section);
                    d::need(row.size()==2&&row.at("isstream")=="1","POD26 sparse event flags differ");
                    put(bank,key,{row.at("filename"),false,false,true});
                }
                return;
            }
            const auto rows=source_announcer_authoring50IE::detail::events(bytes,count);
            for(unsigned key=1;key<=count;++key)put(bank,key,rows[key-1]);
        };
        const auto speechPrefix="audio\\speech\\"+native_racer50HW::rig_detail::lower(identity->podId)+"\\";
        const auto expected=id::speechCounts(*identity);
        events(AudioBank50HE::PodEvent,"audio\\p_event.ini",
            "49856d31023a3c57d4792df3675ef83928b82d25332e1b0d9b7f9fd80f0e5dfd",6);
        events(AudioBank50HE::PodDebris,"audio\\p_debris.ini",
            "243e211ea41df694d0c7f4e21d1f3ff0073da6dd70d34d00f2b0c659a645ddd5",4);
        events(AudioBank50HE::PodCurses,speechPrefix+"curses.ini",identity->cursesSha256,
            unsigned(expected[0])+expected[1]+expected[2]+expected[3]);
        events(AudioBank50HE::PodTaunts,speechPrefix+"taunts.ini",identity->tauntsSha256,
            unsigned(expected[4])+expected[5]);
        events(AudioBank50HE::PodSpecial,speechPrefix+"special.ini",identity->specialSha256,
            unsigned(expected[6])+expected[7]+expected[8]);
        const auto wrecks=native_racer50HW::rig_detail::ini(member("audio\\wrecks.txt",
            "0043194bf11fece83668b5dda8b25f928e0c338f2a60b87fc29e71b39be463e6"));
        d::need(native_racer50HW::rig_detail::value(wrecks,"vehiclesoundemitter","numsounds")=="4",
            "50IK original selected wreck count differs");
        for(unsigned key=1;key<=4;++key)put(AudioBank50HE::PodWreck,key,
            {native_racer50HW::rig_detail::value(wrecks,"sound"+std::to_string(key),"filename"),false,false,false});
        const auto counts=native_racer50HW::rig_detail::ini(member(speechPrefix+"sounds.ini",id::speechCountsSha256(*identity)));
        constexpr const char* keys[]={"numberoflowdamagesounds","numberofwallhitsounds","numberofpassedsounds",
            "numberofattackedsounds","numberofpassingsounds","numberofattackingsounds",
            "numberofdeathsounds","numberofwinsounds","numberoflosesounds"};
        for(unsigned i=0;i<9;++i)d::need(native_racer50HW::rig_detail::value(counts,"indices",keys[i])==
            std::to_string(expected[i]),"50IK original selected speech count differs");
        next.speech={expected[0],expected[1],expected[2],expected[3],expected[4],expected[5],expected[6],expected[7],expected[8]};
        const auto phy=member(std::string("Pods\\")+identity->podId+"\\"+identity->podId+".phy",identity->phySha256);
        const auto mapping=opponent_audio50HX::detail::readMapping({identity->podId,&phy});
        next.engine=mapping.parameters;
        put(AudioBank50HE::PodEngine,1,{mapping.engineName,false,false,false});
        put(AudioBank50HE::PodEngine,2,{mapping.binderName,false,false,false});
        std::size_t authoredSlots=0;
        for(const auto& request:next.catalog)if(!request.name.empty()){
            ++authoredSlots;bool exists=false;
            for(const auto& previous:next.requests)if(native_racer50HW::rig_detail::lower(previous.name)==
                native_racer50HW::rig_detail::lower(request.name)){
                d::need(previous.loop==request.loop&&previous.stereo==request.stereo&&previous.stream==request.stream,
                    "50IK aliased selected sample flags differ");exists=true;break;
            }
            if(!exists)next.requests.push_back(request);
        }
        std::size_t speechSlots=0;for(const auto count:expected)speechSlots+=count;
        d::need(authoredSlots==20u+speechSlots,"50IK selected semantic catalog count differs");
        out=std::move(next);error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="50IK selected original audio preparation failed";}catch(...){}return false;}
}
} // namespace selected_player_audio50IJ
