#pragma once
// Bounded cold POD01/POD03 audio selection. The fixed sixty cue slots retain
// their semantic bank/key identity; Clegg's absent curse/taunt keys stay empty.
// No Anakin speech or engine sound is substituted for an unauthored Clegg key.
#include "gameplay_audio_provider50he_fixture.h"
#include "pod_speech_authoring50hg_fixture.h"
#include "source_announcer_authoring50ie_fixture.h"
#include "native_racer_rig50hw_fixture.h"
#include "opponent_engine_audio50hx_fixture.h"

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
    namespace a=rr::assets;
    namespace d=pod_materials50ed_fixture::detail;
    using namespace rr::game;
    try {
        d::need(racer60==1||racer60==3,"50IJ selected player must be original POD01 or POD03");
        // TA intro and literal boost remain the same authenticated course/global
        // inputs. All POD03-owned event/debris/wreck/voice/engine rows below are
        // replaced from its own original archive before any bank is published.
        std::vector<a::AudioRequest50HC> baseline;
        if(!a::readRetainedAudioRequests50HC(discRoot,baseline,error))return false;
        Authoring next;
        for(std::size_t i=0;i<next.catalog.size();++i){
            const auto& cue=gameplay_audio50HE::catalog[i];
            if(racer60==3&&cue.bank!=AudioBank50HE::TrackIntro&&
                !(cue.bank==AudioBank50HE::PodEngine&&cue.key==3))continue;
            const a::AudioRequest50HC* found=nullptr;
            for(const auto& request:baseline)if(native_racer50HW::rig_detail::lower(request.name)==
                native_racer50HW::rig_detail::lower(cue.filename))found=&request;
            d::need(found,"50IJ original baseline cue absent");next.catalog[i]=*found;
        }
        if(racer60==1){
            if(!pod_speech50HG::readPod01Counts(next.speech,error,
                discRoot/"PODS/POD01/POD01.RES"))return false;
        }else{
            d::Inputs input;input.capacity=328378;
            const auto bytes=input.read(discRoot/"PODS/POD03/POD03.RES",328378);
            d::need(bytes.size()==328378&&d::sha256(bytes)==
                "4ad6a07ac02bc9ef65e336777ff27e6823060d333b0a885e99f14f58bd3ee26e",
                "50IJ original POD03 archive identity differs");
            a::ResArchive48C archive;a::DecodeError48C decode;a::DecodeLimits48C limits;
            limits.resource_file_bytes=328378;limits.resource_decoded_bytes=16u*1024u*1024u;
            limits.resource_entries=512;limits.resource_name_bytes=256;
            d::need(a::decodeResource48C(bytes,archive,decode,nullptr,limits),"50IJ original POD03 archive decode failed");
            const auto member=[&](const char* name,const char* digest){
                std::vector<std::uint8_t> result;
                d::need(a::extractResMember48C(archive,name,result,decode,8192),
                    std::string("50IJ original POD03 member missing: ")+name);
                d::need(d::sha256(result)==digest,std::string("50IJ original POD03 member identity differs: ")+name);
                return result;
            };
            const auto put=[&](AudioBank50HE bank,unsigned key,const a::AudioRequest50HC& request){
                const auto index=gameplay_audio50HE::cueIndex({bank,key});
                d::need(index<next.catalog.size()&&next.catalog[index].name.empty(),
                    "50IJ original selected cue is duplicate or exceeds native bank capacity");
                next.catalog[index]=request;
            };
            const auto events=[&](AudioBank50HE bank,const char* name,const char* digest,unsigned count){
                const auto rows=source_announcer_authoring50IE::detail::events(member(name,digest),count);
                for(unsigned key=1;key<=count;++key)put(bank,key,rows[key-1]);
            };
            events(AudioBank50HE::PodEvent,"audio\\p_event.ini",
                "49856d31023a3c57d4792df3675ef83928b82d25332e1b0d9b7f9fd80f0e5dfd",6);
            events(AudioBank50HE::PodDebris,"audio\\p_debris.ini",
                "243e211ea41df694d0c7f4e21d1f3ff0073da6dd70d34d00f2b0c659a645ddd5",4);
            events(AudioBank50HE::PodCurses,"audio\\speech\\pod03\\curses.ini",
                "3495668c8acf694239c31818e534cd6ed67392252c5c3f0a2a35524f91b13455",11);
            events(AudioBank50HE::PodTaunts,"audio\\speech\\pod03\\taunts.ini",
                "6c14175eb500b4843b0bf18dfcd2e71a9f554b34c4fc564261ed986c9e8b1d7d",8);
            events(AudioBank50HE::PodSpecial,"audio\\speech\\pod03\\special.ini",
                "3df60c54984509b6ca231d414ee59e9647406a0341676220b94007a9d4af230c",9);
            const auto wrecks=native_racer50HW::rig_detail::ini(member("audio\\wrecks.txt",
                "0043194bf11fece83668b5dda8b25f928e0c338f2a60b87fc29e71b39be463e6"));
            d::need(native_racer50HW::rig_detail::value(wrecks,"vehiclesoundemitter","numsounds")=="4",
                "50IJ original POD03 wreck count differs");
            for(unsigned key=1;key<=4;++key)put(AudioBank50HE::PodWreck,key,
                {native_racer50HW::rig_detail::value(wrecks,"sound"+std::to_string(key),"filename"),false,false,false});
            const auto counts=native_racer50HW::rig_detail::ini(member("audio\\speech\\pod03\\sounds.ini",
                "f0b3b4950bd40f71736964ee6cdd9c6e97424ff84e6b57ca464170cb2ab72be2"));
            constexpr const char* keys[]={"numberoflowdamagesounds","numberofwallhitsounds","numberofpassedsounds",
                "numberofattackedsounds","numberofpassingsounds","numberofattackingsounds",
                "numberofdeathsounds","numberofwinsounds","numberoflosesounds"};
            constexpr unsigned expected[]={2,3,3,3,4,4,3,3,3};
            for(unsigned i=0;i<9;++i)d::need(native_racer50HW::rig_detail::value(counts,"indices",keys[i])==
                std::to_string(expected[i]),"50IJ original POD03 speech count differs");
            next.speech={2,3,3,3,4,4,3,3,3};
            const auto phy=member("Pods\\Pod03\\Pod03.phy",
                "147a9ed4d2449592bc6752fc6b44bdacff56120f1b19f1aa29864d3baf694d47");
            const auto mapping=opponent_audio50HX::detail::readMapping({"POD03",&phy});
            next.engine=mapping.parameters;
            put(AudioBank50HE::PodEngine,1,{mapping.engineName,false,false,false});
            put(AudioBank50HE::PodEngine,2,{mapping.binderName,false,false,false});
        }
        for(const auto& request:next.catalog)if(!request.name.empty())next.requests.push_back(request);
        d::need(next.requests.size()==(racer60==1?60u:48u),"50IJ original selected catalog count differs");
        out=std::move(next);error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="50IJ selected original audio preparation failed";}catch(...){}return false;}
}
} // namespace selected_player_audio50IJ
