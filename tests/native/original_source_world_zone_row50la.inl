#pragma once
#include "original_source_world_zones50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::loadSourceWorldZone275E30(std::size_t i,std::uint32_t& pc)noexcept {
    if(i>=10||!sourceZones350_||sourceZonesLoaded_!=i||!sourceWorldConfig_)return false;
    auto& z=sourceZones350_[i];if(!z.constructed||z.loaded)return false;
    static constexpr const char* names[]={"Zone1","Zone2","Zone3","Zone4","Zone5","Zone6","Zone7","Zone8","Zone9","Zone10"};
    pc=0x275e50;z.index00=static_cast<std::int32_t>(i);
    pc=0x275e6c;const auto nameBytes=std::strlen(names[i])+1;
    std::memcpy(z.name04.data(),names[i],nameBytes);z.nameKnown=nameBytes;
    auto& config=*sourceWorldConfig_;a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    pc=0x275e78;if(!config.selectSourceZone157D00(static_cast<std::uint32_t>(i),0,returned,d)){
        pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}
    if(!returned){pc=0x275e80;return false;} //Before unjoined fallback/log; authenticated all10 exist.
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept {
        if(!config.readSourceZoneField158240(key,value,found,copy,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    std::int32_t flag=0;pc=0x275ef4;if(!read.integer("environmentnodevisible",1,&flag,returned))return false;
    pc=0x275f1c;z.flags148=std::uint8_t((z.flags148&~2u)|((flag&1)?2u:0u));
    pc=0x275f68;z.flags148&=~4u; //Actual ZoneN != StaticModels comparison.
    pc=0x275f6c;if(!read.integer("numberofstaticmodels",0,&z.staticCount108,returned))return false;
    std::array<char,256> text;pc=0x275f8c;
    if(!read.text("searchtype","None",text.data(),text.size(),returned))return false;
    if(!returned){pc=0x275fc8;return false;} //Actual per-role fallback getters deliberately unjoined.
    pc=0x276530;
    std::int32_t kind=std::strcmp(text.data(),"QuadTree")==0?2:std::strcmp(text.data(),"List")==0?1:0;
    pc=0x275fa8;z.search110[0]=kind;pc=0x275fb0;z.search110[2]=z.search110[0];
    pc=0x275fb8;z.search110[1]=z.search110[0];pc=0x275fc4;z.search110[3]=z.search110[0];
    if(!kind){pc=0x276148;return false;} //Before actual diagnostic/shared assertion, no invented validkind.
    static constexpr const char* bounds[]={"xmin","xmax","zmin","zmax"};
    static constexpr std::uint32_t calls[]={0x2761d4,0x2761ec,0x276204,0x27621c};
    for(unsigned k=0;k<4;++k){std::uint32_t bits=0;pc=calls[k];
        if(!read.number(bounds[k],0,bits,returned))return false;z.bounds14C[k]=bits;}
    pc=0x276234;if(!read.integer("zonereverb",0,&z.reverb128,returned))return false;
    pc=0x276254;if(!read.text("fogcolorrgb","undef",text.data(),text.size(),returned))return false;
    //The pointer lane, not native backing alone, supplies actual World3AC.
    //Known NULL and unknown history are different source cuts.
    auto fog=[&](std::uint32_t pointerPC,std::uint32_t dereferencePC)noexcept -> const NativeFogDescriptor3AC* {
        pc=pointerPC;if(!sourceFog3AC_)return nullptr;
        pc=dereferencePC;if(!*sourceFog3AC_)return nullptr;
        const auto* f=*sourceFog3AC_;
        if(!nativeFogDescriptor3AC_||f!=nativeFogDescriptor3AC_.get()||!f->sourceMode||
           !f->sourceConstructed||!f->sourcePublished||sourceFogPhase_!=SourceFogPhase::returnedOne||
           f->sourceApplication!=worldApplication_.get()||f->sourceGeneration!=worldEntity_.sessionGeneration||
           f->sourceThread!=std::this_thread::get_id())return nullptr;
        return f;
    };
    if(std::strcmp(text.data(),"undef")==0){
        const auto* f=fog(0x276274,0x276278);if(!f)return false;
        static constexpr std::uint32_t stores[]={0x27627c,0x276284,0x27628c,0x276298};
        for(unsigned k=0;k<4;++k){pc=stores[k];z.fogColor12C[k]=f->sourceColor00[k];}
    }else{
        std::array<std::uint32_t,4> rgba;pc=0x2762a0;if(!read.color(text.data(),rgba))return false;
        static constexpr std::uint32_t stores[]={0x2762ac,0x2762b4,0x2762bc,0x2762c4};
        for(unsigned k=0;k<4;++k){pc=stores[k];z.fogColor12C[k]=rgba[k];}
    }
    //Defaults are unconditionally read BEFORE the getter, even when authored.
    const auto* f=fog(0x2762c8,0x2762d8);if(!f)return false;
    pc=0x2762dc;if(!read.number("fogvisibility",f->sourceVisibility10,z.fogVisibility13C,returned))return false;
    f=fog(0x2762e4,0x2762f4);if(!f)return false;
    pc=0x2762f8;if(!read.number("foghaziness",f->sourceHaziness14,z.fogHaziness140,returned))return false;
    f=fog(0x276300,0x278480);if(!f)return false;
    //278480 compares six finite scalar lanes, not an arithmetic transform.
    //Exact bit equality agrees with EQ.S; signed zero is explicitly equivalent.
    std::array<std::uint32_t,6> left{{f->sourceColor00[0],f->sourceColor00[1],f->sourceColor00[2],f->sourceColor00[3],f->sourceVisibility10,f->sourceHaziness14}};
    std::array<std::uint32_t,6> right{{z.fogColor12C[0],z.fogColor12C[1],z.fogColor12C[2],z.fogColor12C[3],z.fogVisibility13C,z.fogHaziness140}};
    bool equal=true;
    for(unsigned k=0;k<6;++k){pc=0x278498;
        if((left[k]&0x7f800000u)==0x7f800000u||(right[k]&0x7f800000u)==0x7f800000u)return false;
        if(left[k]!=right[k]&&((left[k]|right[k])&0x7fffffffu))equal=false;}
    if(!equal){pc=0x276328;world4F0KnownValue_|=1u;world4F0KnownMask_|=1u;}
    pc=0x27634c;z.exclusionCount124=0; //Getter call delay slot, BEFORE getter effects.
    std::array<char,128> first;pc=0x276348;
    if(!read.text("exclusionlist","",first.data(),first.size(),returned))return false;
    if(returned){
        std::array<char,128> second;pc=0x27635c;std::memcpy(second.data(),first.data(),std::strlen(first.data())+1);
        pc=0x276368;source_world_zone50LA::Tokens countTokens(first.data());
        pc=0x276378;char* token=countTokens.next();
        while(token){pc=0x276390;token=countTokens.next();pc=0x2763a0;++z.exclusionCount124;}
        if(z.exclusionCount124<0||z.exclusionCount124>128){pc=0x2763b4;return false;}
        pc=0x2763b4;try{z.exclusions120.reset(new std::int32_t[static_cast<std::size_t>(z.exclusionCount124)]);}catch(...){z.exclusions120.reset();}
        pc=0x2763c8;z.exclusionAllocationReturned=true;
        pc=0x2763c4;source_world_zone50LA::Tokens valueTokens(second.data());
        pc=0x2763d8;token=valueTokens.next();
        std::size_t next=0;
        while(token){pc=0x2763e8;std::int32_t value=0;
            if(!source_world_light50LA::Reader<decltype(fetch)>::integerToken(token,value)||value==std::numeric_limits<std::int32_t>::min())return false;
            pc=0x2763f4;--value;
            pc=0x276410;if(!z.exclusions120||next>=static_cast<std::size_t>(z.exclusionCount124))return false;
            z.exclusions120[next]=value;z.exclusionsProduced=++next;
            pc=0x27640c;token=valueTokens.next(); //Actual store is the call delay slot.
        }
    }
    pc=0x276438;if(!config.selectSourceZone157D00(static_cast<std::uint32_t>(i),1,returned,d)){
        pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}
    if(!returned){pc=0x276440;return false;} //Before actual missing-section fallback/log.
    std::int32_t collision=-1;pc=0x2764d4;
    if(!read.integer("numberofcollisionobjects",-1,&collision,returned))return false;
    z.collisionCount10C=collision;
    if(collision==-1){pc=0x276500;z.flags148&=~1u;pc=0x276508;z.collisionCount10C=z.staticCount108;}
    z.loaded=true;pc=0x276510;return true;
}
} //namespace ra_cold_aggregate50LA
