#pragma once
// One rule for every course: the cut-camera identity row is read from the
// course archive actually loaded, as the original reads it (174DF0 reads
// [CSCameras].NumberOfCameras into track+3E4; each cut's [CSCameraN]
// InitialZone; tracks\xxNN.vub per cut; audio\introAud.ini and
// audio\scrptLns.ini). The stock table (original_track_cut_identity50la.h)
// is used unchanged for a stock archive; any other archive gets a row derived
// here and registered in the table header's runtime slot (the one place that
// holds it). Everything is host memory; nothing is written into game memory.
// Bounds: at most 7 cuts and 8 members (the row's own arrays).
// FLAG stand-in: intro EVENT byte offsets (introAud.ini layout) are taken
// from the stock row; the intro reader still checks them against the bytes.
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/original_track_cut_identity50la.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/import_world48f.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

namespace course_cut_identity50CI {
namespace a=rr::assets;
inline std::string lower(std::string s){for(auto& c:s){if(c=='/')c='\\';else if(c>='A'&&c<='Z')c=char(c+32);}return s;}
// Builds and registers the row. Returns false (and registers nothing) when
// the archive lacks what the cut cameras need; the caller logs and continues
// (the camera step then refuses that course with its own message).
// A stock code keeps its stock row's trackId/archiveName (and intro event
// receipts); a registry code (the common-route course whose code is not
// stock, PORT EXTENSION) gets the registry slot: its own code, the archive
// name its session row mounts (container, e.g. "TRACKS/NV.RES") and no stock
// intro rows (eventCount 0: the intro reads introAud.ini itself).
inline bool derive(const char* trackId,const a::ResArchive48C& archive,const std::vector<std::uint8_t>& bytes,
    std::string& error,const char* container=nullptr) {
    try {
        if(!trackId){error="no course id";return false;}
        const auto* stock=a::originalTrackCutStockIdentity50LA(trackId);
        auto* slot=a::originalTrackCutWritableSlot50LA(trackId);
        if(!slot||(!stock&&(!container||!*container))){error="no cut row slot for this course id";return false;}
        a::OriginalTrackCutRuntimeRow50LA next;
        auto& id=next.identity;
        if(stock)id=*stock;
        else {
            id={};
            next.code={trackId[0],trackId[1],'\0'};next.archiveName=container;
            id.eventCount=0;id.events={};
        }
        std::string code(trackId);for(auto& c:code)if(c>='A'&&c<='Z')c=char(c+32);
        next.archiveSha=pod_materials50ed_fixture::detail::sha256(bytes);
        id.archiveSha256=nullptr;id.archiveBytes=bytes.size();id.memberCount=archive.entries.size();
        const auto find=[&](const std::string& wanted,std::size_t& ordinal)->const a::ResourceEntry48C*{
            const auto key=lower(wanted);const a::ResourceEntry48C* found=nullptr;
            for(std::size_t i=0;i<archive.entries.size();++i)if(lower(archive.entries[i].name)==key){
                if(found)throw std::runtime_error("ambiguous member "+wanted);found=&archive.entries[i];ordinal=i;}
            return found;
        };
        const auto row=[&](const a::ResourceEntry48C& e,std::size_t ordinal,std::string& name,std::string& sha,
            a::OriginalTrackMemberIdentity50LA& out,std::vector<std::uint8_t>* keep){
            std::vector<std::uint8_t> data;a::DecodeError48C why;
            if(!a::extractResMember48C(archive,e.name,data,why,e.bytes)||data.size()!=e.bytes)
                throw std::runtime_error("unreadable member "+e.name);
            name=e.name;sha=pod_materials50ed_fixture::detail::sha256(data);
            out.name=nullptr;out.size=data.size();out.ordinal=ordinal;out.sha256=nullptr;
            if(keep)*keep=std::move(data);
        };
        std::size_t ordinal=0;std::vector<std::uint8_t> tob;
        const auto* tobEntry=find("tracks\\"+code+".tob",ordinal);
        if(!tobEntry)throw std::runtime_error("archive lacks its TOB");
        id.members={};id.frames={};id.zones={};id.zoneOffsets={};
        row(*tobEntry,ordinal,next.names[0],next.shas[0],id.members[0],&tob);
        a::ParameterDocument48F document;a::DecodeError48C why;
        if(!a::decodeParameters48F(tob,document,why))throw std::runtime_error("TOB decode "+why.code);
        const auto field=[&](const std::string& block,const char* key)->const a::ParameterField48F*{
            const a::ParameterField48F* out=nullptr;
            for(const auto& b:document.blocks)if(b.name==block)for(const auto& f:b.fields)if(f.name==key){
                if(out)throw std::runtime_error("duplicate TOB field");out=&f;}
            return out;
        };
        const auto decimal=[](const a::ParameterField48F& f){
            if(f.value.empty()||f.value.size()>4)throw std::runtime_error("TOB decimal extent");
            std::uint32_t v=0;for(char c:f.value){if(c<'0'||c>'9')throw std::runtime_error("TOB decimal");v=v*10+std::uint32_t(c-'0');}
            return v;
        };
        const auto* cuts=field("cscameras","numberofcameras");
        if(!cuts)throw std::runtime_error("TOB lacks [CSCameras] NumberOfCameras");
        const auto cutCount=decimal(*cuts);
        if(cutCount<1||cutCount>7)throw std::runtime_error("cut count outside the native 1..7");
        id.cutCount=cutCount;id.cutCountOffset=cuts->value_offset;
        for(std::uint32_t k=1;k<=cutCount;++k){
            const auto* zone=field("cscamera"+std::to_string(k),"initialzone");
            if(!zone)throw std::runtime_error("TOB lacks a cut InitialZone");
            id.zones[k-1]=decimal(*zone);id.zoneOffsets[k-1]=zone->value_offset;
            char name[32];std::snprintf(name,sizeof(name),"tracks\\%s%02u.vub",code.c_str(),k);
            const auto* vub=find(name,ordinal);
            if(!vub)throw std::runtime_error(std::string("archive lacks ")+name);
            row(*vub,ordinal,next.names[k],next.shas[k],id.members[k],nullptr);
            // VUB = 12-byte header + 116 bytes per frame.
            if(id.members[k].size<12||(id.members[k].size-12)%116)throw std::runtime_error("VUB extent");
            id.frames[k-1]=static_cast<std::uint32_t>((id.members[k].size-12)/116);
        }
        const char* intro[2]={"audio\\introAud.ini","audio\\scrptLns.ini"};
        for(std::size_t i=0;i<2;++i){
            const auto* e=find(intro[i],ordinal);
            if(!e)throw std::runtime_error(std::string("archive lacks ")+intro[i]);
            row(*e,ordinal,next.introNames[i],next.introShas[i],id.introMembers[i],nullptr);
        }
        next.present=true;
        *slot=std::move(next);slot->bind();
        std::cerr<<"course_cut_identity code="<<trackId<<" cuts="<<cutCount<<" members="<<id.memberCount
                 <<" stock_row="<<(stock?1:0)<<" derived=1\n";
        return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
// The archive label a course mount carries: the digest/size of the archive
// actually admitted at course select (derived row) or, for a stock archive,
// the stock identity. One source for every mount and every later compare.
inline const char* mountDigest(const char* code,const char* stockSha)noexcept{
    const auto* slot=a::originalTrackCutRuntimeSlot50LA(code?code:"");
    return slot&&slot->present?slot->archiveSha.c_str():stockSha;
}
inline std::size_t mountBytes(const char* code,std::size_t stockBytes)noexcept{
    const auto* slot=a::originalTrackCutRuntimeSlot50LA(code?code:"");
    return slot&&slot->present?slot->identity.archiveBytes:stockBytes;
}
}
