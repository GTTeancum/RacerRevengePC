#pragma once
#include "../../../src/assets/audio_assets50hc.h"
#include "../../../src/assets/original_ta_intro_authoring_candidate.h"
#include "../../../src/assets/original_race_intro_pcm_identity50la.h"
#include <array>
#include <memory>
namespace rr::platform {
// Cold native PCM preparation only, not177750 or a source Sound factory.
// AUDIO root is the user's selected original disc root, never a fallback.
inline bool prepareOriginalTaIntroPcm50LA(
    const std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate>& authoring,
    const std::filesystem::path& audioRoot,
    std::shared_ptr<const assets::AudioBank50HC>& out,std::string& error)noexcept {
    if(out||!authoring||!authoring->providerOwner()||!authoring->metadataOwner()||
       !authoring->providerOwner()->sealed()||
       authoring->providerOwner()->revision()!=authoring->capturedProviderRevision()){
        error="race intro original authoring/provider lifetime is unavailable";return false;
    }
    try{
        std::vector<assets::AudioRequest50HC> requests;requests.reserve(authoring->eventCount());
        if(!authoring->eventCount()||authoring->eventCount()>rr::assets::originalRaceIntroEventsMax50LA||(authoring->trackId()!="TA"&&authoring->trackId()!="RA"&&authoring->trackId()!="GC"&&authoring->trackId()!="SA"&&authoring->trackId()!="MA"&&authoring->trackId()!="GA"&&authoring->trackId()!="MB"&&authoring->trackId()!="SC"&&authoring->trackId()!="GB"&&authoring->trackId()!="RC"&&authoring->trackId()!="TC"&&authoring->trackId()!="MC"&&authoring->trackId()!="TB")){error="Original race intro event count is outside authenticated race catalog";return false;}
        for(unsigned id=1;id<=authoring->eventCount();++id){const auto* event=authoring->event(id);
            if(!event||event->eventId!=id||
               event->factoryFlags!=2||!event->isStream||event->isStereo||event->isLoop||
               !event->metadata||event->metadata!=authoring->metadataOwner()->findForStreamRequest(true,event->filename)){
                error="race intro authenticated event metadata differs";return false;
            }
            requests.push_back({std::string(event->filename),false,false,true});
        }
        auto next=std::make_shared<assets::AudioBank50HC>();
        if(!assets::loadAudioBank50HC(audioRoot,requests,*next,error))return false;
        if(next->clips.size()!=authoring->eventCount()||next->ready!=authoring->eventCount()||next->missing||next->unsupported||next->invalid){
            error="All authored original race intro VAG streams are required";return false;
        }
        for(std::size_t n=0;n<authoring->eventCount();++n){const auto& clip=next->clips[n];
            const auto* identity=assets::originalRaceIntroPcmIdentity50LA(authoring->event(n+1)->filename);
            if(!identity||clip.state!=assets::AudioAssetState50HC::ready||
               clip.request.name!=requests[n].name||!clip.request.stream||clip.request.loop||clip.request.stereo||
               clip.sourceBytes!=identity->bytes||clip.sourceSha256!=identity->sha256||clip.pcm.sourceSha256!=identity->sha256||
               clip.pcm.sampleRate!=identity->sampleRate||clip.pcm.channels!=1||!clip.pcm.frames||
               clip.pcm.samples.size()!=clip.pcm.frames){
                error="Original race intro VAG identity/header/decoded shape differs";return false;
            }
        }
        out=std::move(next);error.clear();return true;
    }catch(...){error="race intro native preload allocation failed";return false;}
}
}
