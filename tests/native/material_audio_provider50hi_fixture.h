#pragma once
#include "../../src/assets/material_audio50hi.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "pod_materials50ed_fixture.h"
#include "course_archive_receipt50ci.h"
#include "rc_cold_sources50la.h"
#include "rc_audio_originals50rc.h"
#include "tb_cold_sources50la.h"
#include "tb_audio_originals50tb.h"
#include "gb_cold_sources50la.h"
#include "gb_audio_originals50gb.h"
#include "mc_cold_sources50la.h"
#include "mc_material_audio_originals50mc.h"
#include "audio_retained_payload50mc.h"
#include <array>
#include <filesystem>
#include <string>
namespace material_audio50HI {
inline constexpr std::size_t soundCount=16; // Preserved TA base voice reservation64..79.
inline constexpr std::size_t maximumSoundCount=44,extraFirstVoice=118,maximumExtraVoices=8;
// Keep GC 118..125 and Gate 126 intact. GA exclusively owns 128..151.
inline constexpr std::size_t gaExtraFirstVoice=128,gaMaximumExtraVoices=24;
// MA/SA are selected-track-exclusive. Neither aliases GC/Gate, GA,
// gameplay152..158, camera159, nor original AI engines160..191.
inline constexpr std::size_t maSaExtraFirstVoice=192,maMaximumExtraVoices=12,saMaximumExtraVoices=28;
static_assert(soundCount+saMaximumExtraVoices==maximumSoundCount);
static_assert(gaExtraFirstVoice+gaMaximumExtraVoices<=maSaExtraFirstVoice);
static_assert(maSaExtraFirstVoice+saMaximumExtraVoices<=rr::platform::audioMaximumVoices50HD);
// RA measured12+12 distinct host rows:16 base plus8 extra. This range
// is selected-track-exclusive, with its own bound rather than an MA/SA count.
inline constexpr std::size_t raExtraFirstVoice=192,raMaximumExtraVoices=8;
static_assert(soundCount+raMaximumExtraVoices==24);
static_assert(raExtraFirstVoice+raMaximumExtraVoices<=rr::platform::originalMaterialFirstVoice50HD);
// MB owns32 logical rows, not25 voices: PCM deduplication never merges owners.
// Its16 extra host rows are selected-track-exclusive at192..207.
inline constexpr std::size_t mbExtraFirstVoice=192,mbMaximumExtraVoices=16;
static_assert(soundCount+mbMaximumExtraVoices==32);
static_assert(mbExtraFirstVoice+mbMaximumExtraVoices<=rr::platform::originalMaterialFirstVoice50HD);
// SC retains44 authored sound owners with28 selected-track-exclusive extra slots.
inline constexpr std::size_t scExtraFirstVoice=192,scMaximumExtraVoices=28;
static_assert(soundCount+scMaximumExtraVoices==maximumSoundCount);
static_assert(scExtraFirstVoice+scMaximumExtraVoices<=rr::platform::originalMaterialFirstVoice50HD);
// TC's28 logical sound owners fit16 base slots plus12 exclusive192..203.
inline constexpr std::size_t tcExtraFirstVoice=192,tcMaximumExtraVoices=12;
static_assert(soundCount+tcMaximumExtraVoices==28);
static_assert(tcExtraFirstVoice+tcMaximumExtraVoices<=rr::platform::originalMaterialFirstVoice50HD);
// TB owns44 logical host rows:16 base64..79 plus28 exclusive192..219.
inline constexpr std::size_t tbExtraFirstVoice=192,tbMaximumExtraVoices=28;
static_assert(soundCount+tbMaximumExtraVoices==maximumSoundCount);
static_assert(tbExtraFirstVoice+tbMaximumExtraVoices==rr::platform::originalMaterialFirstVoice50HD);
inline constexpr std::size_t gbExtraFirstVoice=192,gbMaximumExtraVoices=24;
static_assert(soundCount+gbMaximumExtraVoices==40&&gbExtraFirstVoice+gbMaximumExtraVoices<=rr::platform::originalMaterialFirstVoice50HD);
inline constexpr std::size_t mcExtraFirstVoice=192,mcMaximumExtraVoices=4;
static_assert(soundCount+mcMaximumExtraVoices==20&&mcExtraFirstVoice+mcMaximumExtraVoices<=rr::platform::originalMaterialFirstVoice50HD);
enum class Track:std::uint8_t {ta,gc,ga,ma,sa,ra,mb,sc,tc,rc,tb,gb,mc};
struct TrackProfile {
    Track track;const char* archiveName;std::size_t archiveBytes;const char* archiveSha;
    const char* aubName;std::size_t aubBytes;const char* aubSha;
    std::uint32_t collisionRows,driveRows,materials;std::size_t uniqueClips,extraVoiceFirst;
};
inline constexpr TrackProfile trackProfiles[]{
    {Track::ta,"TRACKS/TA.RES",3334562u,"b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8",
     "tracks\\ta.aub",25431u,"7474707d4182f29cc8b24203680aa311c9749e745078f3d80545f93f8d97f017",8,8,1,13,extraFirstVoice},
    {Track::gc,"TRACKS/GC.RES",4205067u,"69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9",
     "tracks\\gc.aub",32438u,"eb4046a25b5b08412c8fa8644107ab826499af754f1093283cc4566e2d879c1d",12,12,4,21,extraFirstVoice},
    {Track::ga,"TRACKS/GA.RES",2834942u,"063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9",
     "tracks\\ga.aub",41363u,"8302ea938033d539feece2a13ebc2f3c2171938be13770afdebc980940a043ec",20,20,4,37,gaExtraFirstVoice},
    {Track::ma,"TRACKS/MA.RES",3691592u,"2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228",
     "tracks\\ma.aub",39850u,"1cbf1814fd87cc684fcca94b6980d8d40c323f12cc2ddff6f2a94d376f938dc4",16,12,3,23,maSaExtraFirstVoice},
    {Track::sa,"TRACKS/SA.RES",3786696u,"34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a",
     "tracks\\sa.aub",44414u,"ec272d864f435354904f87f2a240d6f52e1a23060c0b8c9b8e803cd79d1fe887",32,12,2,23,maSaExtraFirstVoice},
    {Track::ra,"TRACKS/RA.RES",4604561u,"1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2",
     "tracks\\ra.aub",22511u,"c0977667b46b918872c3d351f745ce59260037b9f604b76aea23b11bb5c88c6a",12,12,1,21,raExtraFirstVoice},
    {Track::mb,"TRACKS/MB.RES",4421779u,"6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca",
     "tracks\\mb.aub",57210u,"669a20fc6275a7463a1a4a8f529b41ef45ce971f8f4d49b8631f5279699a0d89",20,12,3,25,mbExtraFirstVoice},
    {Track::sc,"TRACKS/SC.RES",2974353u,"8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d",
     "tracks\\sc.aub",80114u,"869c76400974914421571c715f75be3fc7912e494cacb27c8f01624b4c5447ac",32,12,2,25,scExtraFirstVoice},
    {Track::tc,"TRACKS/TC.RES",3607868u,"7b5486f4029dabe137919127c879faef92b8bd2fb9df5c0d1a1d122e92731490",
     "tracks\\tc.aub",44646u,"a4e331f8ac8a4849e717f6a37588304b1642129b8b690b39d7b818449eab838d",16,12,5,21,tcExtraFirstVoice},
    {Track::rc,"TRACKS/RC.RES",4655141u,"ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b",
     "tracks\\rc.aub",47432u,"8b34867bb6e76e1ddce72ac65c6c4548068d52d30a53b4ca08af87bfe3ca1851",8,8,1,13,extraFirstVoice},
    {Track::tb,"TRACKS/TB.RES",3749557u,"1188206d7b1f64910c119192829fc357b9d4a9f021d64e8a2b7561f2c113a09b",
     "tracks\\tb.aub",50826u,"00729a0e848ca3a28996b312bcd789dbc3a3798c8b803ab3c612f4379dd94a19",24,20,5,33,tbExtraFirstVoice},
    {Track::gb,"TRACKS/GB.RES",4088586u,"e92af24305431dc5a96cebb5ebcf89159adb6d371045b2c5424dacd1f74b7d99",
     "tracks\\gb.aub",29684u,"2d42e759aef7b80193d60581b611c572b836de91145ee18f439d45f46e4f530f",20,20,4,37,gbExtraFirstVoice},
    {Track::mc,"TRACKS/MC.RES",3822171u,"f8854752f639676257d44e33479a0c31c047a17e4cd592d02f483ad593179c81",
     "tracks\\mc.aub",50722u,"bbb238196c131979092c7a5cc5066fdfc131156ecb6ec632786279786f779554",12,8,5,15,mcExtraFirstVoice}
};
inline constexpr const TrackProfile* profile(Track track)noexcept{
    for(const auto& p:trackProfiles)if(p.track==track)return &p;return nullptr;
}
enum class ApplyResult : std::uint8_t { Started,Stopped,Controlled,Inactive,Unavailable,Rejected };
struct Sound {
    rr::platform::AudioClip50HD clip{};
    rr::assets::AudioAssetState50HC availability=rr::assets::AudioAssetState50HC::missing;
};
struct Receipt {
    rr::game::AudioEventStatus50HE status=rr::game::AudioEventStatus50HE::Input;
    std::size_t commandCount=0;
    std::array<rr::game::MaterialAudioCommand50HI,rr::game::maxMaterialAudioCommands50HI> commands{};
    std::array<ApplyResult,rr::game::maxMaterialAudioCommands50HI> results{};
    bool okay()const noexcept{return status==rr::game::AudioEventStatus50HE::Complete;}
};
struct Counters {std::uint64_t updates=0,starts=0,stops=0,controls=0,inactive=0,unavailable=0,rejected=0;};
// Host subscriber only. It does NOT register a synthetic canonical D40, assert
// original virtual dispatch occurred, or acknowledge an absent game callback.
class Provider final {
public:
    Provider()=default;Provider(const Provider&)=delete;Provider&operator=(const Provider&)=delete;
    Provider(Provider&&)=delete;Provider&operator=(Provider&&)=delete;
    bool prepare(const std::filesystem::path&discRoot,const std::filesystem::path&audioRoot,
        rr::platform::AudioMixer50HD&mixer,std::size_t firstVoice,std::string&error,Track track=Track::ta){
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HI requires a fresh stable provider";return false;}attempted_=true;
        try{d::need(firstVoice<=mixer.count()&&soundCount<=mixer.count()-firstVoice,"50HI dedicated16-slot range unavailable");
            const auto* selected=profile(track);d::need(selected&&track!=Track::rc&&track!=Track::tb&&track!=Track::gb&&track!=Track::mc,"50HI unsupported source track or compact source required");
            // Common course-archive rule: read within the shared capacity; the
            // stock identity is a receipt (course_archive_receipt50ci.h).
            d::Inputs files;files.capacity=course_archive_receipt50CI::maximumArchiveBytes();
            auto bytes=files.read(discRoot/selected->archiveName,files.capacity);
            course_archive_receipt50CI::receipt("material_audio",selected->archiveName,bytes,selected->archiveSha,selected->archiveBytes);
            a::ResArchive48C archive;a::DecodeError48C decode;std::vector<std::uint8_t>aub;
            d::need(a::decodeResource48C(bytes,archive,decode,nullptr,course_archive_receipt50CI::limits())&&
                a::extractResMember48C(archive,selected->aubName,aub,decode),"50HI selected original AUB unavailable");
            return prepareCatalog(aub,audioRoot,mixer,firstVoice,error,track,*selected);
        }catch(const std::exception&e){error=e.what();return false;}
    }
    // RC never expands a second complete archive. Retain the same authenticated
    // compact source used by the world/navigation/material domain.
    bool prepareRc(std::shared_ptr<const rc_cold_sources50LA::Owner> source,
        const std::filesystem::path& audioRoot,rr::platform::AudioMixer50HD& mixer,
        std::size_t firstVoice,std::string& error){
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HI requires a fresh stable provider";return false;}attempted_=true;
        try{
            d::need(source&&source->ready(),"RC same-source material audio unavailable");
            d::need(firstVoice==64&&firstVoice<=mixer.count()&&soundCount<=mixer.count()-firstVoice,
                "RC actual material host range64..79 unavailable");
            std::vector<std::uint8_t> aub;a::DecodeError48C decode;const auto* selected=profile(Track::rc);
            // Found by name, bounded by the shared course-archive capacity;
            // the stock AUB size is a receipt (prepareCatalog).
            d::need(source->extractMember(selected->aubName,aub,decode,course_archive_receipt50CI::maximumArchiveBytes()),
                "RC same-source AUB extraction rejected");
            rcSource_=std::move(source);
            return prepareCatalog(aub,audioRoot,mixer,firstVoice,error,Track::rc,*selected);
        }catch(const std::exception&e){error=e.what();return false;}
    }
    const rc_cold_sources50LA::Owner* rcSourceIdentity()const noexcept{return rcSource_.get();}
    bool prepareTb(std::shared_ptr<const tb_cold_sources50LA::Owner> source,
        const std::filesystem::path& audioRoot,rr::platform::AudioMixer50HD& mixer,
        std::size_t firstVoice,std::string& error){
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HI requires a fresh stable provider";return false;}attempted_=true;
        try{
            d::need(source&&source->ready(),"TB same-source material audio unavailable");
            d::need(firstVoice==64&&firstVoice<=mixer.count()&&soundCount<=mixer.count()-firstVoice,
                "TB actual material host range64..79 unavailable");
            std::vector<std::uint8_t> aub;a::DecodeError48C decode;const auto* selected=profile(Track::tb);
            // Found by name, bounded by the shared course-archive capacity;
            // the stock AUB size is a receipt (prepareCatalog).
            d::need(source->extractMember(selected->aubName,aub,decode,course_archive_receipt50CI::maximumArchiveBytes()),
                "TB same-source AUB extraction rejected");
            tbSource_=std::move(source);
            return prepareCatalog(aub,audioRoot,mixer,firstVoice,error,Track::tb,*selected);
        }catch(const std::exception&e){error=e.what();return false;}
    }
    const tb_cold_sources50LA::Owner* tbSourceIdentity()const noexcept{return tbSource_.get();}
    bool prepareGb(std::shared_ptr<const gb_cold_sources50LA::Owner> source,
        const std::filesystem::path& audioRoot,rr::platform::AudioMixer50HD& mixer,
        std::size_t firstVoice,std::string& error){
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HI requires a fresh stable provider";return false;}attempted_=true;
        try{
            d::need(source&&source->ready(),"GB same-source material audio unavailable");
            d::need(firstVoice==64&&firstVoice<=mixer.count()&&soundCount<=mixer.count()-firstVoice,
                "GB actual material host range64..79 unavailable");
            std::vector<std::uint8_t> aub;a::DecodeError48C decode;const auto* selected=profile(Track::gb);
            // Found by name, bounded by the shared course-archive capacity;
            // the stock AUB size is a receipt (prepareCatalog).
            d::need(source->extractMember(selected->aubName,aub,decode,course_archive_receipt50CI::maximumArchiveBytes()),
                "GB same-source AUB extraction rejected");
            gbSource_=std::move(source);
            return prepareCatalog(aub,audioRoot,mixer,firstVoice,error,Track::gb,*selected);
        }catch(const std::exception&e){error=e.what();return false;}
    }
    const gb_cold_sources50LA::Owner* gbSourceIdentity()const noexcept{return gbSource_.get();}
    bool prepareMc(std::shared_ptr<const mc_cold_sources50LA::Owner> source,
        const std::filesystem::path& audioRoot,rr::platform::AudioMixer50HD& mixer,
        std::size_t firstVoice,std::string& error){
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        if(attempted_){error="50HI requires a fresh stable provider";return false;}attempted_=true;
        try{
            d::need(source&&source->ready(),"MC same-source material audio unavailable");
            d::need(firstVoice==64&&firstVoice<=mixer.count()&&soundCount<=mixer.count()-firstVoice,
                "MC actual material host range64..79 unavailable");
            std::vector<std::uint8_t> aub;a::DecodeError48C decode;const auto* selected=profile(Track::mc);
            d::need(source->archive().extractMember(selected->aubName,aub,decode,course_archive_receipt50CI::maximumArchiveBytes()),
                "MC same-source AUB extraction rejected");
            mcSource_=std::move(source);
            return prepareCatalog(aub,audioRoot,mixer,firstVoice,error,Track::mc,*selected);
        }catch(const std::exception&e){error=e.what();return false;}
    }
    const mc_cold_sources50LA::Owner* mcSourceIdentity()const noexcept{return mcSource_.get();}
private:
    bool prepareCatalog(const std::vector<std::uint8_t>& aub,const std::filesystem::path& audioRoot,
        rr::platform::AudioMixer50HD& mixer,std::size_t firstVoice,std::string& error,
        Track track,const TrackProfile& selectedValue){
        namespace a=rr::assets;namespace d=pod_materials50ed_fixture::detail;
        const auto* selected=&selectedValue;
        try{
            course_archive_receipt50CI::memberReceipt("material_audio",selected->aubName,aub,0,0,selected->aubBytes,selected->aubSha);
            std::vector<a::AudioRequest50HC>requests;
            d::need(track==Track::mc?a::readMcMaterialAudioCatalog50HI(aub,catalog_,requests,error):
                track==Track::tb?a::readTbMaterialAudioCatalog50HI(aub,catalog_,requests,error):
                track==Track::tc?a::readTcMaterialAudioCatalog50HI(aub,catalog_,requests,error):
                track==Track::mb?a::readMbMaterialAudioCatalog50HI(aub,catalog_,requests,error):
                a::readMaterialAudioCatalog50HI(aub,catalog_,requests,error),"50HI material authoring rejected");
            collisionRowCount_=catalog_.parameters.rowCount;driveRowCount_=catalog_.parameters.driveRowCount;
            activeSoundCount_=collisionRowCount_+driveRowCount_;
            // Row and material counts come from the AUB, bounded by the native
            // sound store; the stock counts are receipts.
            course_archive_receipt50CI::countReceipt("material_audio","collision_rows",collisionRowCount_,selected->collisionRows);
            course_archive_receipt50CI::countReceipt("material_audio","drive_rows",driveRowCount_,selected->driveRows);
            course_archive_receipt50CI::countReceipt("material_audio","materials",catalog_.materialCount,selected->materials);
            // FLAG stand-in: the native voice layout below assumes at least the 8+8
            // base rows (fewer would underflow its extra-slot arithmetic).
            d::need(collisionRowCount_>=8&&driveRowCount_>=8&&activeSoundCount_<=maximumSoundCount,"50HI material rows outside the native sound layout (8..44)");
            // The original reads DefaultMaterial from the AUB (the catalog reader
            // bounds it to one material byte); the stock value 1 is a receipt.
            if(track==Track::ma||track==Track::sa||track==Track::ra||track==Track::mb||track==Track::sc||track==Track::tc||track==Track::rc||track==Track::tb||track==Track::gb||track==Track::mc)
                course_archive_receipt50CI::countReceipt("material_audio","default_material",catalog_.defaultMaterial,1);
            // RA declares NumMaterials1. Material2 is outside271674..271804.
            if(track==Track::ra){
                d::need(catalog_.orderedObjectMaterials.empty(),"RA material assignment traversal differs");
                for(std::size_t i=0;i<12;++i)d::need(catalog_.collisionSectionKeys[i]==i+1&&
                    catalog_.driveSectionKeys[i]==i+1,"RA exact source sections differ");
            }
            if(track==Track::sc){
                // Stock SC layout reported, not enforced: the catalog reader
                // already resolved every section key from the AUB itself.
                course_archive_receipt50CI::countReceipt("material_audio","sc_object_materials",catalog_.orderedObjectMaterials.size(),54);
            }
            course_archive_receipt50CI::countReceipt("material_audio","unique_clips",requests.size(),selected->uniqueClips);
            d::need(a::loadAudioBank50HC(audioRoot,requests,bank_,error),"50HI real material PCM bank failed");
            // Stock PCM identities and GB/MC traversal counts: receipts (the
            // clips are the disc's own sfx files the AUB names).
            const auto pcmReceipt=[&](const char* site,auto&& authentic){std::size_t other=0;
                for(const auto& clip:bank_.clips)if(!authentic(clip))++other;
                course_archive_receipt50CI::countReceipt(site,"non_stock_pcm",other,0);};
            if(track==Track::rc)pcmReceipt("material_audio_rc",[](const auto& c){return rc_audio_originals50RC::authentic(c);});
            if(track==Track::tb)pcmReceipt("material_audio_tb",[](const auto& c){return tb_audio_originals50TB::authentic(c);});
            if(track==Track::mc){
                course_archive_receipt50CI::countReceipt("material_audio_mc","object_materials",catalog_.orderedObjectMaterials.size(),3);
                pcmReceipt("material_audio_mc",[](const auto& c){return mc_material_audio_originals50MC::authentic(c);});
            }
            if(track==Track::gb){
                course_archive_receipt50CI::countReceipt("material_audio_gb","object_materials",catalog_.orderedObjectMaterials.size(),38);
                std::size_t reread=0;
                for(std::size_t i=0;i<collisionRowCount_;++i)if(catalog_.collisionSectionKeys[i]!=i+1)++reread;
                for(std::size_t i=0;i<driveRowCount_;++i)if(catalog_.driveSectionKeys[i]!=i+1)++reread;
                course_archive_receipt50CI::countReceipt("material_audio_gb","reread_sections",reread,0);
                pcmReceipt("material_audio_gb",[](const auto& c){return gb_audio_originals50GB::authentic(c);});
            }
            // All source owners retain distinct slots, including SA17..32,
            // whose source section selection rereads16. Only PCM is shared.
            extraFirstVoice_=selected->extraVoiceFirst;
            const auto extraCollisionRows=collisionRowCount_-8u;
            const auto extraRows=activeSoundCount_-soundCount;
            if(extraRows)d::need(firstVoice+soundCount<=extraFirstVoice_&&
                extraFirstVoice_+extraRows<=mixer.count(),"50HI exclusive extra material slots unavailable");
            for(std::size_t i=0;i<activeSoundCount_;++i){
                const bool drive=i>=collisionRowCount_;const auto key=drive?i-collisionRowCount_:i;
                slots_[i]=key<8?firstVoice+(drive?8u:0u)+key:
                    extraFirstVoice_+(drive?extraCollisionRows:0u)+key-8u;
                if(track!=Track::ta)d::need(!mixer.hasVoiceGroupGain50IC(slots_[i])&&!mixer.active(slots_[i]),"50HI material voice already belongs to another provider");
                const auto*clip=bank_.find(drive?catalog_.drive[key]:catalog_.collision[key]);
                d::need(clip,"50HI catalog row missing bank record");auto&sound=sounds_[i];sound.availability=clip->state;
                if(clip->state==a::AudioAssetState50HC::ready){const auto&p=clip->pcm;
                    sound.clip={p.samples.data(),p.frames,p.sampleRate,p.channels,p.hasEmbeddedLoop?p.loopStart:0,p.hasEmbeddedLoop?p.loopEnd:0};
                    d::need(rr::platform::AudioMixer50HD::validClip(sound.clip),"50HI ready PCM invalid");}
            }
            firstVoice_=firstVoice;mixer_=&mixer;error.clear();return true;
        }catch(const std::exception&e){error=e.what();return false;}
    }
public:
    ApplyResult apply(const rr::game::MaterialAudioCommand50HI&c)noexcept{
        using namespace rr::game;
        if((rcSource_&&!rcSource_->ready())||(tbSource_&&!tbSource_->ready())||(gbSource_&&!gbSource_->ready())||(mcSource_&&!mcSource_->ready())){++counters_.rejected;return ApplyResult::Rejected;}
        if(!mixer_||(c.kind!=MaterialCueKind50HI::Collision&&c.kind!=MaterialCueKind50HI::DriveOn)){++counters_.rejected;return ApplyResult::Rejected;}
        const bool drive=c.kind==MaterialCueKind50HI::DriveOn;
        if(c.key<1||c.key>(drive?driveRowCount_:collisionRowCount_)){++counters_.rejected;return ApplyResult::Rejected;}
        const auto i=(drive?collisionRowCount_:0u)+c.key-1;const auto slot=slots_[i];
        if(c.operation==AudioOperation50HE::Stop){mixer_->stop(slot);++counters_.stops;return ApplyResult::Stopped;}
        if(sounds_[i].availability!=rr::assets::AudioAssetState50HC::ready){++counters_.unavailable;return ApplyResult::Unavailable;}
        if(c.operation==AudioOperation50HE::Controls){if(!mixer_->active(slot)){++counters_.inactive;return ApplyResult::Inactive;}if(mixer_->controls(slot,c.gain,c.pitch)){++counters_.controls;return ApplyResult::Controlled;}}
        else if(c.operation==AudioOperation50HE::Start){
            // Actual292B80/292DA0 do not restart a sound owner already playing.
            if(mixer_->active(slot)){++counters_.inactive;return ApplyResult::Inactive;}
            if(mixer_->play(slot,sounds_[i].clip,c.gain,c.pitch,c.loop)){++counters_.starts;return ApplyResult::Started;}
        }
        ++counters_.rejected;return ApplyResult::Rejected;
    }
    Receipt update(const rr::game::MaterialAudioInput50HI&input,std::uint32_t&sharedRandomState)noexcept{
        Receipt r;if(!mixer_||(rcSource_&&!rcSource_->ready())||(tbSource_&&!tbSource_->ready())||(gbSource_&&!gbSource_->ready())||(mcSource_&&!mcSource_->ready()))return r;
        auto next=state_;r.status=rr::game::updateMaterialAudio50HI(catalog_.parameters,input,state_,next,sharedRandomState,r.commands.data(),r.commands.size(),r.commandCount);
        if(!r.okay())return r;state_=next;++counters_.updates;
        for(std::size_t i=0;i<r.commandCount;++i){r.results[i]=apply(r.commands[i]);if(r.results[i]==ApplyResult::Rejected)r.status=rr::game::AudioEventStatus50HE::Input;}return r;
    }
    void reset()noexcept{if((rcSource_&&!rcSource_->ready())||(tbSource_&&!tbSource_->ready())||(gbSource_&&!gbSource_->ready())||(mcSource_&&!mcSource_->ready()))std::terminate();if(mixer_)for(std::size_t i=0;i<activeSoundCount_;++i)mixer_->stop(slots_[i]);state_={};}
    std::size_t collisionRowCount()const noexcept{return collisionRowCount_;}
    std::size_t driveRowCount()const noexcept{return driveRowCount_;}
    std::size_t extraVoiceFirst()const noexcept{return extraFirstVoice_;}
    std::size_t extraVoiceCount()const noexcept{return activeSoundCount_>soundCount?activeSoundCount_-soundCount:0;}
    bool prepared()const noexcept{return mixer_!=nullptr&&(!rcSource_||rcSource_->ready())&&(!tbSource_||tbSource_->ready())&&(!gbSource_||gbSource_->ready())&&(!mcSource_||mcSource_->ready());}
    // Includes this Provider's inline storage once, excludes retained shared source.
    std::size_t retainedPayloadBytes()const noexcept{
        namespace payload=audio_retained_payload50MC;
        std::size_t bytes=sizeof(*this)+payload::bankDynamicBytes(bank_)+
            catalog_.orderedObjectMaterials.capacity()*sizeof(rr::assets::MaterialAudioCatalog50HI::ObjectMaterial);
        for(const auto& name:catalog_.collision)bytes+=payload::externalStringBytes(name);
        for(const auto& name:catalog_.drive)bytes+=payload::externalStringBytes(name);
        for(const auto& row:catalog_.orderedObjectMaterials)bytes+=payload::externalStringBytes(row.object);
        return bytes;
    }
    std::size_t retainedPcmCapacityBytes()const noexcept{return audio_retained_payload50MC::bankPcmCapacityBytes(bank_);}
    const rr::assets::AudioBank50HC&bank()const noexcept{return bank_;}
    const rr::assets::MaterialAudioCatalog50HI&catalog()const noexcept{return catalog_;}
    const rr::game::MaterialAudioState50HI&state()const noexcept{return state_;}
    const Counters&counters()const noexcept{return counters_;}
private:
    std::shared_ptr<const rc_cold_sources50LA::Owner> rcSource_;
    std::shared_ptr<const tb_cold_sources50LA::Owner> tbSource_;
    std::shared_ptr<const gb_cold_sources50LA::Owner> gbSource_;
    std::shared_ptr<const mc_cold_sources50LA::Owner> mcSource_;
    rr::assets::AudioBank50HC bank_{};
    rr::assets::MaterialAudioCatalog50HI catalog_{};
    std::array<Sound,maximumSoundCount>sounds_{};
    std::array<std::size_t,maximumSoundCount>slots_{};
    rr::platform::AudioMixer50HD*mixer_=nullptr;
    std::size_t firstVoice_=0,extraFirstVoice_=extraFirstVoice,collisionRowCount_=8,driveRowCount_=8,activeSoundCount_=soundCount;
    rr::game::MaterialAudioState50HI state_{};
    Counters counters_{};bool attempted_=false;
};
}
