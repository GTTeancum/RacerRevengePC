#pragma once
#include "lap_sound_owner50gh_fixture.h"
#include "../../src/game/pod_speech50hg.h"
#include <array>
#include <string_view>

namespace pod_speech50HG {
// Cold original counts, not an inferred category size from a filename prefix.
inline bool readPod01Counts(rr::game::PodSpeechCounts50HG& out,std::string& error,
    const std::filesystem::path& archivePath="work/native/project/disc/PODS/POD01/POD01.RES") {
    namespace a=rr::assets;namespace shared=pod_materials50ed_fixture::detail;
    try {
        shared::Inputs input;input.capacity=lap_sound50GH::podArchiveBytes;
        const auto bytes=input.read(archivePath,lap_sound50GH::podArchiveBytes);
        shared::need(bytes.size()==lap_sound50GH::podArchiveBytes&&shared::sha256(bytes)==lap_sound50GH::podArchiveSha256,
            "50HG original Pod01 archive identity mismatch");
        a::ResArchive48C archive;a::DecodeError48C decode;a::DecodeLimits48C limits;
        limits.resource_file_bytes=lap_sound50GH::podArchiveBytes;limits.resource_decoded_bytes=16u*1024u*1024u;
        limits.resource_entries=512;limits.resource_name_bytes=256;
        shared::need(a::decodeResource48C(bytes,archive,decode,nullptr,limits),"50HG Pod01 archive decode failed");
        std::vector<std::uint8_t> member;
        shared::need(a::extractResMember48C(archive,"audio\\speech\\pod01\\sounds.ini",member,decode,2048),
            "50HG Pod01 speech counts missing");
        constexpr const char* names[]={"NumberOfLowDamageSounds","NumberOfWallHitSounds","NumberOfPassedSounds",
            "NumberOfAttackedSounds","NumberOfPassingSounds","NumberOfAttackingSounds","NumberOfDeathSounds",
            "NumberOfWinSounds","NumberOfLoseSounds"};
        constexpr unsigned expected[]={4,4,6,5,6,6,4,3,2};
        std::array<bool,9> seen{};std::array<std::uint8_t,9> parsed{};bool section=false;
        std::string_view text(reinterpret_cast<const char*>(member.data()),member.size());
        for(std::size_t at=0;at<text.size();) {
            auto end=text.find('\n',at);if(end==std::string_view::npos)end=text.size();
            auto line=lap_sound50GH::detail::trim(text.substr(at,end-at));at=end+1;
            if(line.empty()||line.front()==';')continue;
            if(line.front()=='['){shared::need(!section&&line=="[Indices]","50HG unknown/repeated counts section");section=true;continue;}
            const auto equals=line.find('=');shared::need(section&&equals!=std::string_view::npos,"50HG malformed counts entry");
            const auto name=lap_sound50GH::detail::trim(line.substr(0,equals));
            const auto value=lap_sound50GH::detail::trim(line.substr(equals+1));
            std::size_t i=0;while(i<9&&name!=names[i])++i;
            shared::need(i<9&&!seen[i]&&value.size()==1&&value[0]>='0'&&value[0]<='9',"50HG unknown/duplicate count");
            parsed[i]=static_cast<std::uint8_t>(value[0]-'0');seen[i]=true;
            shared::need(parsed[i]==expected[i],"50HG unsupported changed Pod01 speech count");
        }
        for(bool present:seen)shared::need(present,"50HG incomplete counts");
        out={parsed[0],parsed[1],parsed[2],parsed[3],parsed[4],parsed[5],parsed[6],parsed[7],parsed[8]};
        error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="50HG speech authoring read failed";}catch(...){}return false;}
}
// The retained TA script maps Line1/2/3 to CameraNumber1/2/3. Span is absent,
// so the actual158B30 default0 is preserved. Authenticate before publishing.
inline bool readTAIntroLines(std::array<rr::game::TrackIntroLine50HG,3>& out,
    std::string& error,const std::filesystem::path& archivePath="work/native/project/disc/TRACKS/TA.RES") {
    namespace a=rr::assets;namespace shared=pod_materials50ed_fixture::detail;
    try {
        shared::Inputs input;input.capacity=3334562;
        const auto bytes=input.read(archivePath,3334562);
        shared::need(bytes.size()==3334562&&shared::sha256(bytes)==
            "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8","50HG TA archive identity mismatch");
        a::ResArchive48C archive;a::DecodeError48C decode;a::DecodeLimits48C limits;
        limits.resource_decoded_bytes=16u*1024u*1024u;limits.resource_entries=512;limits.resource_name_bytes=256;
        shared::need(a::decodeResource48C(bytes,archive,decode,nullptr,limits),"50HG TA archive decode failed");
        std::vector<std::uint8_t> member;
        shared::need(a::extractResMember48C(archive,"audio\\scrptLns.ini",member,decode,2048),"50HG TA intro script missing");
        shared::need(member.size()==119&&shared::sha256(member)==
            "1a325c68bbccaa8dfa94f87a8d1dcb743d029c59d96771797ce7045135cf6a0f","50HG TA intro script identity mismatch");
        out={{{1,0},{2,0},{3,0}}};error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="50HG TA intro script read failed";}catch(...){}return false;}
}

// Effective1A5F90 human chance138 from the newly supplied original file.
// Missing/changed input is explicit; constructor0.5 is not silently reused.
inline bool readPlayerSpeechChance(float& out,std::string& error,
    const std::filesystem::path& path) {
    namespace shared=pod_materials50ed_fixture::detail;
    try {
        shared::Inputs input;input.capacity=255;const auto bytes=input.read(path,255);
        shared::need(bytes.size()==255&&shared::sha256(bytes)==
            "24c2967388a31d04761fb11208faaae43a4ae7b9b88b40291bf64daf195c018c",
            "50HG original SPCHPCT.INI identity mismatch");
        const std::string_view text(reinterpret_cast<const char*>(bytes.data()),bytes.size());
        shared::need(text.find("HumanPercent=0.35f")!=std::string_view::npos,
            "50HG effective human probability missing");
        // Binary32 conversion of the authenticated original decimal0.35.
        out=.35f;error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="50HG speech probability read failed";}catch(...){}return false;}
}

}
