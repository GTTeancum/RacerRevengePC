#pragma once
#include "original_source_world_light_read50la.inl"
namespace ra_cold_aggregate50LA {
inline bool Owner::loadSourceLightDescriptor277390(std::size_t index,std::uint32_t& pc)noexcept {
    if(!nativeLightList3A8_||!nativeLightList3A8_->sourceMode||!sourceWorldConfig_||index>=185)return false;
    auto& l=*nativeLightList3A8_;if(!l.wrappers38||index>=l.sourceWrappersConstructed||l.loaded!=index)return false;
    auto& w=l.wrappers38[index];if(w.sourceDescriptorPublished||w.sourceDescriptor04||w.descriptor04||w.live00)return false;
    std::array<std::uint32_t,4> origin,forward;
    if(!copySourceWorldStartDefaults(origin,forward)){pc=0;return false;}
    pc=0x277d30;try{w.sourceDescriptor04.reset(new SourceLightDescriptor277220);}catch(...){w.sourceDescriptor04.reset();}
    if(w.sourceDescriptor04){auto& x=*w.sourceDescriptor04;
        pc=0x277234;x.vtableF4=0x3d2b40;pc=0x277240;x.type00=4;
        pc=0x27724c;x.color04.fill(0);
        pc=0x277254;x.position14.reset();pc=0x277258;x.look18.reset();pc=0x27725c;x.attenuation1C.reset();
        pc=0x277260;x.range24=0;pc=0x277264;x.zone20=0;
        //Onlylow7 known; preserve unknownbit7 by not giving it a value receipt.
        x.flags28=0;x.knownFlags28=0;
        static constexpr std::uint32_t stores[]={0x277280,0x2772a0,0x2772c0,0x2772dc,0x2772f8,0x277310,0x277324};
        for(unsigned bit=0;bit<7;++bit){pc=stores[bit];if(bit==2)x.flags28|=4;x.knownFlags28|=std::uint8_t(1u<<bit);
            if(bit==4){pc=0x2772fc;x.flareTurningE4=1;}}
        pc=0x277320;x.lensLayers34.fill(0);pc=0x277330;x.lensColor3C.fill(0);
        pc=0x27733c;x.lensBrightness4C=0x3f800000u;
        pc=0x277350;x.lensTexture50[0]=0;x.lensTextureKnown=1;
        pc=0x27734c;x.flareColor90.fill(0);pc=0x277358;x.flareSizeA0=0x3f800000u;
        pc=0x277360;x.lensTexture50[0]=0;
        pc=0x277364;x.flashPatternE8=-1;pc=0x277368;x.flashPeriodEC=0x3f800000u;
        pc=0x27736c;x.flashFilterF0=0;x.constructed=true;
    }
    pc=0x277d48;w.sourceDescriptorPublished=true;
    if(!w.sourceDescriptor04){pc=0x277d54;return false;}
    auto& x=*w.sourceDescriptor04;auto& config=*sourceWorldConfig_;a::OriginalStandardTextureDiagnostic50LA io;
    bool returned=false;pc=0x2773cc;
    if(!config.selectSourceLight157D00(static_cast<std::uint32_t>(index),returned,io)){
        pc=io.sourcePC?io.sourcePC:config.sourcePC_;return false;}
    //Genuine missing90/107 selection leaves prior selected block. Original
    //ignores v0 here; subsequent getters must use that SAME Config state.
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept {
        if(!config.readSourceLightField158240(key,value,found,copy,io)){pc=io.sourcePC?io.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    std::array<char,260> text;pc=0x2773ec;
    if(!read.text("type","none",text.data(),text.size(),returned))return false;
    pc=0x277400;
    x.type00=std::strcmp(text.data(),"Ambient")==0?4u:std::strcmp(text.data(),"Directional")==0?2u:
        std::strcmp(text.data(),"ObjectPoint")==0?1u:std::strcmp(text.data(),"Spotlight")==0?3u:0u;
    if(!x.type00){pc=0x277418;return false;} //Before realprintf/sharedinvalidtypeassert; never fabricate4.
    pc=0x2774a8;if(!read.text("colorrgb","none",text.data(),text.size(),returned))return false;
    std::array<std::uint32_t,4> rgba;pc=0x2774b4;if(!read.color(text.data(),rgba))return false;
    static constexpr std::uint32_t cstores[]={0x2774cc,0x2774dc,0x2774e4,0x2774f0};
    for(unsigned i=0;i<4;++i){pc=cstores[i];x.color04[i]=rgba[i];}
    pc=0x2774ec;if(!read.integer("initialzone",1,&x.zone20,returned))return false;
    if(x.zone20==std::numeric_limits<std::int32_t>::min()){pc=0x2774fc;return false;}
    pc=0x277500;--x.zone20;
    if(x.type00==4){pc=0x277514;x.loaded=true;return true;}
    //Each vector allocation/publication is distinct; no transactional triple.
    pc=0x27751c;try{x.position14.reset(new std::array<std::uint32_t,4>);}catch(...){x.position14.reset();}
    if(x.position14){pc=0x27752c;*x.position14={{0,0,0,0x3f800000u}};}
    pc=0x277538; //source14publication precedes secondmalloc return.
    pc=0x277534;try{x.look18.reset(new std::array<std::uint32_t,4>);}catch(...){x.look18.reset();}
    if(x.look18){pc=0x277544;*x.look18={{0,0,0,0x3f800000u}};}
    pc=0x277550;
    pc=0x27754c;try{x.attenuation1C.reset(new std::array<std::uint32_t,4>);}catch(...){x.attenuation1C.reset();}
    if(x.attenuation1C){pc=0x27755c;*x.attenuation1C={{0,0,0,0x3f800000u}};}
    pc=0x277560;
    pc=0x277578;if(!read.vector("position",origin,x.position14.get(),returned))return false;
    std::array<std::uint32_t,4> scratch{{0,0,0,0x3f800000u}};pc=0x277598;
    if(!read.vector("lookvector",origin,&scratch,returned))return false;
    if(returned){pc=0x2775b4;if(!x.look18)return false;*x.look18=scratch;}
    else{
        pc=0x2775cc;if(!read.vector("targetposition",origin,&scratch,returned))return false;
        if(returned){
            static constexpr std::uint32_t target[]={0x45bc4800,0x44c96000,0x45f3f000};
            static constexpr std::uint32_t position[]={0x42d60000,0x455ae354,0x45f0c600};
            static constexpr std::uint32_t difference[]={0x45b8f000,0xc4ec66a8,0x42ca8000};
            std::array<std::uint32_t,4> delta;
            for(unsigned k=0;k<3;++k){pc=k==0?0x2775fc:k==1?0x277600:0x277608;
                if(index!=1||!x.position14||scratch[k]!=target[k]||(*x.position14)[k]!=position[k])return false;
                delta[k]=difference[k];}
            delta[3]=0x3f800000u;pc=0x277624;if(!x.look18)return false;*x.look18=delta;
        }else{pc=0x277634;if(!x.look18)return false;*x.look18=origin;}
    }
    const std::array<std::uint32_t,4> attenuation{{0x3cf5c28f,0x3b03126f,0x3a83126f,0x3f800000}};
    pc=0x277674;if(!read.vector("attenuation",attenuation,x.attenuation1C.get(),returned))return false;
    pc=0x27768c;if(!read.number("range",0,x.range24,returned))return false;
    std::uint32_t angle=0;pc=0x2776a8;if(!read.number("innerconeangle",0x41f00000u,angle,returned))return false;x.inner2C=angle;
    pc=0x2776c4;if(!read.number("outerconeangle",0x42340000u,angle,returned))return false;x.outer30=angle;
    auto flag=[&](const char* key,unsigned bit,std::int32_t fallback,std::uint32_t call,std::uint32_t store)noexcept {
        std::int32_t value=0;pc=call;if(!read.integer(key,fallback,&value,returned))return false;
        pc=store;const auto mask=std::uint8_t(1u<<bit);x.flags28=std::uint8_t((x.flags28&~mask)|((value&1)?mask:0));return true;};
    if(!flag("placelightatinfinity",0,1,0x2776dc,0x277714)||
       !flag("castsshadows",1,1,0x277710,0x27774c)||
       !flag("emitslight",2,1,0x277748,0x27777c)||
       !flag("haslensflare",3,0,0x277780,0x2777a4))return false;
    if(x.flags28&8){
        pc=0x2777d4;if(!read.text("lensflarecolorrgb","undef",text.data(),text.size(),returned))return false;
        pc=0x2777e0;if(!read.color(text.data(),rgba))return false;
        static constexpr std::uint32_t stores[]={0x2777fc,0x27780c,0x277814,0x277820};
        for(unsigned k=0;k<4;++k){pc=stores[k];x.lensColor3C[k]=rgba[k];}
        pc=0x27781c;if(!read.number("lensflarebrightness",0x3f800000u,x.lensBrightness4C,returned))return false;
        pc=0x277840;if(!read.text("lensflarelayersvisible","T,T,T,T,T",text.data(),text.size(),returned))return false;
        const std::string_view layers(text.data());std::size_t at=0,layer=0;
        while(at<layers.size()){
            pc=0x277860;if(layer>=5)return false;const auto end=layers.find(',',at);
            pc=layers[at]=='T'?0x27787c:0x277884;x.lensLayers34[layer++]=layers[at]=='T'?1u:0u;
            if(end==std::string_view::npos)break;at=end+1;
        }
        pc=0x2778c0;if(!read.text("lensflaretexturemapname","none",x.lensTexture50.data(),64,returned))return false;
        x.lensTextureKnown=std::strlen(x.lensTexture50.data())+1;
    }
    if(!flag("haslightflare",4,0,0x2778d8,0x2778fc))return false;
    if(x.flags28&16){
        pc=0x27792c;if(!read.text("lightflarecolorrgb","undef",text.data(),text.size(),returned))return false;
        pc=0x277938;if(!read.color(text.data(),rgba))return false;
        static constexpr std::uint32_t stores[]={0x277954,0x277964,0x27796c,0x277978};
        for(unsigned k=0;k<4;++k){pc=stores[k];x.flareColor90[k]=rgba[k];}
        pc=0x277974;if(!read.number("lightflaresize",0x3f800000u,x.flareSizeA0,returned))return false;
        pc=0x277994;if(!read.text("lightflaretexturemapname","none",x.flareTextureA4.data(),64,returned))return false;
        x.flareTextureKnown=std::strlen(x.flareTextureA4.data())+1;
        pc=0x2779a0; //Actual retained 'flare1' has no dot;1169D0 returnsNULL.
        if(std::strchr(x.flareTextureA4.data(),'.')){pc=0x2779b4;return false;}
        pc=0x2779dc;if(!read.integer("lightflareturning",1,&x.flareTurningE4,returned))return false;
    }
    if(!flag("hasflasher",5,0,0x2779f4,0x277a18))return false;
    if(x.flags28&32){pc=0x277a4c;return false;} //No authored flasher branch in exactRA cohort.
    if(!flag("showdebugsphere",6,0,0x277b20,0x277b48))return false;
    x.loaded=true;pc=0x277b4c;return true;
}
} //namespace ra_cold_aggregate50LA
