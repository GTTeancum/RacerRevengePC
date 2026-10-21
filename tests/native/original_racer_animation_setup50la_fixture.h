#pragma once
#include "native_racer_rig50hw_fixture.h"
#include "../../src/game/original_racer_animation_setup50la.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include <cerrno>
#include <cstdlib>
#include <limits>

namespace original_racer_animation_setup50LA {
using Rig=native_racer50HW::Rig;
namespace detail {
inline void need(bool b,const char* why){setup_detail::need(b,why);}
inline std::string canonical(std::string s){
    //132820/308F00 folds case only. Slash spelling remains source identity.
    for(auto& c:s)if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');
    return s;
}
inline std::string value(const Ini& ini,const std::string& section,const std::string& key,const std::string& fallback){
    const auto s=ini.find(section);if(s==ini.end())return fallback;
    const auto k=s->second.find(key);return k==s->second.end()?fallback:k->second;
}
inline std::int32_t integer(const std::string& s){
    const auto text=canonical(s);need(!text.empty(),"original empty integer outside admission");
    //158920 tests first-character T/F/Y/N, exact ON/OFF, then base0 strtol.
    if(text.front()=='t'||text.front()=='y'||text=="on")return 1;
    if(text.front()=='f'||text.front()=='n'||text=="off")return 0;
    errno=0;char* end=nullptr;const auto n=std::strtoll(text.c_str(),&end,0);
    need(errno!=ERANGE&&end!=text.c_str(),"original integer conversion outside admission");
    while(*end==' '||*end=='\t'||*end=='\r'||*end=='\n')++end;
    need((!*end||*end==';')&&n>=std::numeric_limits<std::int32_t>::min()&&
        n<=std::numeric_limits<std::int32_t>::max(),"original animation integer outside source admission");
    return static_cast<std::int32_t>(n);
}
}
// Actual original RES directory, independent of extracted-member lists.
class Archive final {
    a::ResArchive48C archive_;
    std::string digest_;
    std::string originalPath_;
    std::size_t encodedBytes_=0;
public:
    explicit Archive(const Rig& rig){
        const auto bytes=runtime_startup50fl::readBytes(rig.resourceFile);
        originalPath_=rig.resourceFile.generic_string();encodedBytes_=bytes.size();
        digest_=pod_materials50ed_fixture::detail::sha256(bytes);
        detail::need(digest_==rig.manifest.at("source_container_sha256").get<std::string>(),
            "original animation RES identity differs from actual Rig");
        a::DecodeError48C d;
        detail::need(a::decodeResource48C(bytes,archive_,d),"original animation RES directory rejected");
    }
    std::optional<std::string> originalMemberName(const std::string& name)const{
        std::optional<std::string> found;const auto key=detail::canonical(name);
        for(const auto& entry:archive_.entries)if(detail::canonical(entry.name)==key){
            detail::need(!found,"original animation RES name ambiguous");found=entry.name;
        }
        return found;
    }
    std::optional<std::vector<std::uint8_t>> lookup(const std::string& name)const{
        const a::ResourceEntry48C* found=nullptr;const auto key=detail::canonical(name);
        for(const auto& entry:archive_.entries)if(detail::canonical(entry.name)==key){
            detail::need(!found,"original animation RES name ambiguous");found=&entry;
        }
        if(!found)return std::nullopt;
        std::vector<std::uint8_t> result;a::DecodeError48C d;
        detail::need(a::extractResMember48C(archive_,found->name,result,d),"original authenticated extraction failed");return result;
    }
    const std::string& digest()const noexcept{return digest_;}
    bool mount(a::OriginalStandardArchiveProvider50LA& provider,
        a::OriginalStandardArchiveProvider50LA::Diagnostic& d)const noexcept {
        return provider.mountAuthenticatedRes(originalPath_,digest_,encodedBytes_,archive_,d);
    }
};
// Produces table INPUT only. Call Setup::prepareTables154140 after actual model
// load/append. Passing a real cache prototype selects156300's static copies.
inline Input readInput1538B0(const Rig& rig,unsigned model,const Archive& archive,const Setup* prototype=nullptr){
    using namespace detail;using setup_detail::upper;
    need(model<rig.modelCount&&!rig.mcf[model].empty()&&!rig.psg[model].empty(),"original Rig model missing");
    need(archive.digest()==rig.manifest.at("source_container_sha256").get<std::string>(),"original archive belongs to another Rig");
    const auto mcf=archive.lookup(rig.mcfMembers[model]),psg=archive.lookup(rig.psgMembers[model]);
    need(mcf&&*mcf==rig.mcf[model]&&psg&&*psg==rig.psg[model],"original Rig and authenticated members differ");
    Input input;input.actualRig=&rig;input.model=model;input.hierarchy=rig.hierarchy[model];
    input.sourceNodes=rig.sourceNodes[model];input.sceneNodeCount=rig.nodeCount;
    input.parsedConfiguration=native_racer50HW::rig_detail::ini(*mcf);
    const auto& ini=input.parsedConfiguration;need(ini.count("general info")!=0,"original General info absent");
    auto& c=input.configuration;c.definition48=rig.mcfMembers[model].substr(0,79);
    c.slt138=value(ini,"general info","sltfile","");
    if(prototype){
        need(prototype->prepared()&&prototype->captured155730(),"original warm prototype lacks loaded setup");
        const auto& source=prototype->configuration();
        //1563B4..156424 copies only these static fields.20/24/28/2C/0C retain
        // constructor defaults.10/18/38/30 belong to the animation owner.
        c.slt138=source.slt138;c.definition48=source.definition48;c.vut98=source.vut98;
        c.mirrorE8=source.mirrorE8;c.directory190=source.directory190;c.motionCount14=source.motionCount14;
    }else{
        c.directory190=value(ini,"general info","contentdirectory","");
        need(!c.directory190.empty()&&c.directory190.size()<80,"empty ContentDirectory37AED0 fallback outside roster admission");
        c.vut98=value(ini,"general info","vutfilename","\\");c.mirrorE8=value(ini,"general info","mirfilename","\\");
        c.motionCount14=integer(value(ini,"general info","numberofmotions","0"));
        need(c.motionCount14>=0&&c.motionCount14<=static_cast<std::int32_t>(Rig::MotionCount),"original motion count outside roster admission");
        c.interpolatePosition20=integer(value(ini,"general info","interpolateposition","1"));
        c.interpolateRotation24=integer(value(ini,"general info","interpolaterotation","1"));
        c.rotationType28=upper(value(ini,"general info","rotationtype","BYTEAXIS"))=="SHORTAXIS"?1u:0u;
        c.positionType2C=upper(value(ini,"general info","positiontype","BEZIER"))=="LINEAR"?0u:1u;
        //153E94 writes sp+60;153EAC/CC/F0 compare prior sp+20. All known
        // roster MCFs omit MirrorAxis. Never silently fix the guest scratch bug.
        need(ini.at("general info").count("mirroraxis")==0,"authored MirrorAxis scratch path requires separate admission");
    }
    need(!c.slt138.empty()&&c.slt138.size()<80&&c.vut98.size()<80&&c.mirrorE8.size()<80,"original filename extent rejected");
    auto selected=canonical(c.directory190+c.slt138);const auto dot=selected.find_last_of('.');
    selected=selected.substr(0,dot)+".psg";
    need(selected==canonical(rig.psgMembers[model]),"original SLT selects another Rig PSG");
    input.archiveDigest=archive.digest();input.mirrorMember=c.directory190+c.mirrorE8;input.vutMember=c.directory190+c.vut98;
    input.mirror=archive.lookup(input.mirrorMember);input.vut=archive.lookup(input.vutMember);
    if(rig.podId=="POD18"&&model==Rig::chariot){
        // The live canonical Setup reads its own archive-backed input, rather
        // than Rig::load's already projected bindings. Apply the same exact
        // source-proved unknown-LHS skip before the unchanged strict parser.
        need(archive.digest()=="db065fdd6dd1c8dc9996191a07ff549f2fad1706701b69cbc881a470812834a1"&&
            canonical(input.vutMember)=="pods\\pod18\\chariot.vut"&&
            canonical(rig.psgMembers[model])=="pods\\pod18\\chariot.psg"&&input.vut,
            "POD18 live VUT projection requires original archive and chariot role");
        const auto prefix=native_racer50HW::rig_detail::sebulba18ChariotVutPrefix(*psg,*input.vut);
        need(*input.vut==rig.vut[model],"POD18 live VUT differs from retained original Rig bytes");
        // Only this transient parsing input is projected. Rig and both real
        // archive owners retain the complete original 377-byte member.
        input.vut->resize(prefix);
    }
    return input;
}
}
