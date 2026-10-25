#pragma once
#include "../../../src/assets/audio_assets50hc.h"
#include "../../../src/assets/original_ta_intro_authoring_candidate.h"
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
        error="TA intro original authoring/provider lifetime is unavailable";return false;
    }
    try{
        std::vector<assets::AudioRequest50HC> requests;requests.reserve(3);
        for(unsigned id=1;id<=3;++id){const auto* event=authoring->event(id);
            if(!event||event->eventId!=id||event->cameraNumber!=id||event->span||
               event->factoryFlags!=2||!event->isStream||event->isStereo||event->isLoop||
               !event->metadata||event->metadata!=authoring->metadataOwner()->findForStreamRequest(true,event->filename)){
                error="TA intro authenticated three-event metadata differs";return false;
            }
            requests.push_back({std::string(event->filename),false,false,true});
        }
        auto next=std::make_shared<assets::AudioBank50HC>();
        if(!assets::loadAudioBank50HC(audioRoot,requests,*next,error))return false;
        constexpr std::array<const char*,3> hashes{{
            "5796e1a4874411ceaf65d4a1a846ca05f1da881446e135722e5718a8387f079e",
            "6c7eb6c9e64aec42ee5a23c351e99515d2e8e9d6b97afad45aa2a3427035fde7",
            "2275e85c776d830edaaaea3a6bcd88c58e955c2c79c7d6fe4595c6438584cf5f"}};
        constexpr std::array<std::uint64_t,3> sizes{{188208,62608,62240}};
        if(next->clips.size()!=3||next->ready!=3||next->missing||next->unsupported||next->invalid){
            error="All three original TA intro VAG streams are required";return false;
        }
        for(std::size_t n=0;n<3;++n){const auto& clip=next->clips[n];
            if(clip.state!=assets::AudioAssetState50HC::ready||
               clip.request.name!=requests[n].name||!clip.request.stream||clip.request.loop||clip.request.stereo||
               clip.sourceBytes!=sizes[n]||clip.sourceSha256!=hashes[n]||clip.pcm.sourceSha256!=hashes[n]||
               clip.pcm.sampleRate!=22050||clip.pcm.channels!=1||!clip.pcm.frames||
               clip.pcm.samples.size()!=clip.pcm.frames){
                error="Original TA intro VAG identity/header/decoded shape differs";return false;
            }
        }
        out=std::move(next);error.clear();return true;
    }catch(...){error="TA intro native preload allocation failed";return false;}
}
}
