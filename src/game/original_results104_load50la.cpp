#include "original_results104_storage50la.h"
#include "../../tests/native/original_results_roster_read50la.h"
#include <bit>
#include <cstring>
#include <limits>
namespace rr::game {
bool OriginalResults10450LA::load1BEEB0(original_results_roster50LA::Read& roster,
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain,
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context,
    std::shared_ptr<assets::OriginalApplicationFonts50LA> fonts,
    std::shared_ptr<const assets::OriginalPauseSourceData50LA> data,
    std::string& directory,const std::string& fontDirectory,std::uint32_t maximum,
    OriginalResults104Diagnostic50LA& d)noexcept {
    d={};if(!constructed())return false;auto& p=*impl_;
    if(p.loadAttempted||!p.detached())return p.stop(d,Impl::S::phase,0x1beeb0,"Results load is one attempt");
    p.loadAttempted=true;
    try {
        if(!roster.current(p.extent)||!p.extent||p.extent>std::size_t(std::numeric_limits<std::int32_t>::max())||
           p.extent>(std::numeric_limits<std::size_t>::max()-126)/7||
           p.extent>(std::numeric_limits<std::size_t>::max()-16)/(3*sizeof(Impl::Text))||
           p.extent>(std::numeric_limits<std::size_t>::max()-16)/sizeof(Impl::Texture)||
           !domain||!domain->prepared()||!context||!context->live()||!fonts||fonts->terminal()||
           fonts->domain_!=domain||fonts->context_!=context||!data||!data->sourceLoaded()||maximum!=0xffffffu)
            return p.stop(d,Impl::S::owner,0x1beed8,"Results same live roster/Application resources");
        p.domain=std::move(domain);p.context=std::move(context);p.fonts=std::move(fonts);
        p.sourceData=std::move(data);p.depth=maximum;
        std::shared_ptr<const assets::OriginalUiArchive50KG> archive(p.sourceData,p.sourceData->overlays());
        if(!p.references.prepareCold(p.domain,p.context,archive,p.extent,p.materialDiagnostic))
            return p.stop(d,Impl::S::material,p.materialDiagnostic.sourcePC,"Results exact private reference backing");
        if(!p.baseLoad(directory,fontDirectory,d)||!p.derivedLoad(roster,directory,fontDirectory,d))return false;
        std::size_t finalCount=0;
        if(!roster.current(finalCount)||finalCount!=p.extent||!p.context->live()||
           p.references.associatedCount()!=126+7*p.extent)
            return p.stop(d,Impl::S::owner,0x1c1580,"Results final same-owner/resource extent validation");
        // Native identity only, published at the existing successful commit.
        // Failure retains no new Operation custody and no load receipt changes.
        p.loadedRoster=roster.roster_;
        p.loadComplete=true;d.sourcePC=0x1c15ac;d.retainedPrimitives=p.references.associatedCount();return true;
    }catch(...){return p.stop(d,Impl::S::allocation,d.sourcePC?d.sourcePC:0x1beeb0,
        "Results partial native allocation retained; no full source destructor receipt");}
}
bool OriginalResults10450LA::Impl::derivedLoad(original_results_roster50LA::Read& roster,
    std::string& directory,const std::string& fontDirectory,D& d) {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive(sourceData,sourceData->overlays());
    if(!fonts->loadResultsDerived1BEF20(archive,directory,fontDirectory,fontDiagnostic))
        return stop(d,S::font,fontDiagnostic.sourcePC,"Results actual382C80 font order");
    struct Icon {std::size_t index;Role role;std::uint32_t pc,xBits;};
    constexpr Icon icons[]={{2,Role::iconSquare,0x1bef38,0x43c30000},
        {1,Role::iconTriangle,0x1befe8,0x438e8000},{0,Role::iconCross,0x1bf0a0,0x44008000},
        {3,Role::iconCircle,0x1bf158,0x41f00000}};
    for(const auto& spec:icons){auto& t=(*icons210)[spec.index];
        if(!texture(t,spec.role,spec.pc,static_cast<std::uint32_t>(0x210+0x60*spec.index),0,0,directory,d))return false;
        position(t,std::bit_cast<float>(spec.xBits),std::bit_cast<float>(std::uint32_t{0x43bf0000}));}
    const auto denmark=fonts->find282210(assets::OriginalApplicationFonts50LA::Name::denmark16);
    for(std::size_t i=0;i<3;++i){auto& t=(*titles490)[i];
        if(!text(t,Role::denmark16,0x1bf23c,static_cast<std::uint32_t>(0x490+0x70*i),0,40,denmark,directory,nullptr,d))return false;
        color(t,base->blue168);justification(t,0);}
    if(!localized((*titles490)[1],426,d))return false;
    position((*titles490)[1],63.f,389.f);
    if(!text((*labels3A0)[0],Role::denmark16,0x1bf440,0x3a0,0,40,denmark,directory,nullptr,d))return false;
    color((*labels3A0)[0],base->yellow16C);justification((*labels3A0)[0],0);
    if(!localized((*labels3A0)[0],428,d))return false;
    if(!text((*labels3A0)[1],Role::denmark16,0x1bf5a0,0x410,0,40,denmark,directory,nullptr,d))return false;
    color((*labels3A0)[1],base->yellow16C);justification((*labels3A0)[1],1);
    if(!localized((*labels3A0)[1],429,d))return false;
    const auto sameCount=[&](std::uint32_t pc){std::size_t n=0;
        return roster.current(n)&&n==extent?true:stop(d,S::owner,pc,"Results live manager count changed");};
    if(!sameCount(0x1bf6ec))return false;
    d.sourcePC=0x1bf708;
    rows5E0=std::make_unique<Text[]>(3*extent);rowsCount=3*extent;published5E0=true; //1BF730.
    for(std::size_t i=0;i<rowsCount;++i){
        if(!sameCount(0x1bf768)||!text(rows5E0[i],Role::denmark16,0x1bf758,0x5e0,i,40,denmark,directory,nullptr,d))return false;}
    if(!sameCount(0x1bf784))return false;
    d.sourcePC=0x1bf7a0;
    portraits480=std::make_unique<Texture[]>(extent);portraitsCount=extent;published480=true; //1BF7C8.
    for(std::size_t i=0;i<portraitsCount;++i){
        std::uint32_t actual60=0;
        if(!sameCount(0x1bf818)||!roster.racer60At1AF330(static_cast<std::int32_t>(i),actual60))
            return stop(d,S::owner,0x1af330,"Results exact live ordered racer lookup");
        if(!texture(portraits480[i],Role::portrait,0x1bf804,0x480,i,actual60,directory,d))return false;
    }
    constexpr Role medalRoles[]={Role::medalGold,Role::medalSilver,Role::medalBronze};
    constexpr std::uint32_t medalPCs[]={0x1bf848,0x1bf860,0x1bf878};
    for(std::size_t i=0;i<3;++i)if(!texture((*medals5F0)[i],medalRoles[i],medalPCs[i],
        static_cast<std::uint32_t>(0x5f0+0x60*i),0,0,directory,d))return false;
    const float enlarged=std::bit_cast<float>(std::uint32_t{0x3f99999a});
    if(!text(*text710,Role::denmark16,0x1bf898,0x710,0,30,denmark,directory,"CHEATS ENABLED!",d))return false;
    justification(*text710,3);color(*text710,base->grey170);scale(*text710,enlarged);
    if(!text(*text780,Role::denmark16,0x1bf9b0,0x780,0,30,denmark,directory,nullptr,d))return false;
    justification(*text780,1);color(*text780,base->yellow16C);scale(*text780,enlarged);
    struct Single {Text* value;std::uint32_t pc,offset;std::uint8_t capacity;std::uint32_t justify;};
    const Single singles[]={{&*text7F0,0x1bfac8,0x7f0,30,4},{&*text940,0x1bfb88,0x940,10,1},
        {&*text860,0x1bfc48,0x860,30,4},{&*text9B0,0x1bfd08,0x9b0,10,1},
        {&*text8D0,0x1bfdc8,0x8d0,40,4},{&*textA20,0x1bfe88,0xa20,2,1}};
    for(const auto& spec:singles){if(!text(*spec.value,Role::denmark16,spec.pc,spec.offset,0,
        spec.capacity,denmark,directory,nullptr,d))return false;
        justification(*spec.value,spec.justify);color(*spec.value,base->yellow16C);}
    d.sourcePC=0x1bff38;nameA90=std::make_unique<Text>();publishedA90=true; //1BFF50 before load.
    if(!text(*nameA90,Role::denmark16,0x1bff68,0xa90,0,10,denmark,directory,nullptr,d))return false;
    color(*nameA90,base->grey170);scale(*nameA90,enlarged);justification(*nameA90,3);
    d.sourcePC=0x1c0074;keysA94=std::make_unique<std::unique_ptr<Text>[]>(42);publishedA94=true; //1C007C.
    //Source309230 copies only the initialized empty name; no unwritten tail is read.
    editA9C=std::string(nameA90->string8.get());
    d.sourcePC=0x1c0090;cursorA98=std::make_unique<Text>();publishedA98=true; //1C00A8.
    if(!text(*cursorA98,Role::denmark16,0x1c00c4,0xa98,0,2,denmark,directory,"_",d))return false;
    color(*cursorA98,base->grey170);scale(*cursorA98,enlarged);justification(*cursorA98,2);
    constexpr const char* labels[]={"A","B","C","D","E","F","G","H","I","J","K","L","M",
        "N","O","P","Q","R","S","T","U","V","W","X","Y","Z","!",".","#",
        "1","2","3","4","5","6","7","8","9","0","SPACE","BACKSPACE","DONE"}; //382C90.
    for(std::size_t i=0;i<42;++i){
        d.sourcePC=0x1c0214;keysA94[i]=std::make_unique<Text>();++keysConstructed; //1C0244 before load.
        if(!text(*keysA94[i],Role::denmark16,i<39?0x1c02a8:0x1c027c,0xa94,i,
            i<39?2:10,denmark,directory,labels[i],d))return false;
        justification(*keysA94[i],3);
    }
    if(!text((*text1150)[0],Role::denmark16,0x1c0330,0x1150,0,40,denmark,directory,nullptr,d))return false;
    color((*text1150)[0],base->yellow16C);scale((*text1150)[0],enlarged);justification((*text1150)[0],3);
    if(!text((*text1150)[1],Role::denmark16,0x1c0444,0x11c0,0,40,denmark,directory,nullptr,d))return false;
    color((*text1150)[1],base->yellow16C);justification((*text1150)[1],3);
    const auto eurostile=fonts->find282210(assets::OriginalApplicationFonts50LA::Name::eurostile13);
    constexpr std::uint32_t statisticPCs[]={0x1c052c,0x1c05f0,0x1c06b0,0x1c0770,0x1c0830,0x1c08f0,0x1c09b0,0x1c0a70};
    for(std::size_t i=0;i<8;++i){auto& t=(*statistics1230)[i];
        if(!text(t,Role::eurostile13,statisticPCs[i],static_cast<std::uint32_t>(0x1230+0x70*i),0,40,eurostile,directory,nullptr,d))return false;
        color(t,base->yellow16C);justification(t,4);}
    //1C0B28 re-queries the SAME denmark font; no replacement registry.
    if(fonts->find282210(assets::OriginalApplicationFonts50LA::Name::denmark16)!=denmark)
        return stop(d,S::font,0x1c0b28,"Results registered font identity changed");
    constexpr std::uint32_t middlePCs[]={0x1c0b54,0x1c0c18,0x1c0cd8,0x1c0d98};
    for(std::size_t i=0;i<4;++i){auto& t=(*text15B0)[i];
        if(!text(t,Role::denmark16,middlePCs[i],static_cast<std::uint32_t>(0x15b0+0x70*i),0,40,denmark,directory,nullptr,d))return false;
        color(t,base->yellow16C);justification(t,i==1?0:4);}
    constexpr std::uint32_t longPCs[]={0x1c0e58,0x1c0f18,0x1c0fd8};
    for(std::size_t i=0;i<3;++i){auto& t=(*text1770)[i];
        if(!text(t,Role::denmark16,longPCs[i],static_cast<std::uint32_t>(0x1770+0x70*i),0,70,denmark,directory,nullptr,d))return false;
        color(t,base->yellow16C);justification(t,3);}
    constexpr std::uint32_t tablePCs[]={0x1c109c,0x1c1164,0x1c122c};
    constexpr std::uint8_t tableCapacities[]={30,8,20};
    for(std::size_t i=0;i<12;++i){auto& t=(*tableAF0)[i];
        if(!text(t,Role::denmark16,tablePCs[i%3],static_cast<std::uint32_t>(0xaf0+0x70*i),0,
            tableCapacities[i%3],denmark,directory,nullptr,d))return false;
        color(t,base->yellow16C);justification(t,i%3?4:0);}
    for(std::size_t i=9;i<12;++i)color((*tableAF0)[i],base->grey170);
    constexpr std::uint32_t separatorPCs[]={0x1c140c,0x1c1424,0x1c143c};
    for(std::size_t i=0;i<3;++i)if(!texture((*separators1030)[i],Role::separator,separatorPCs[i],
        static_cast<std::uint32_t>(0x1030+0x60*i),0,0,directory,d))return false;
    //1C1468..157C samples the real image dimensions, in source binary32 order.
    for(auto& t:*separators1030){for(std::size_t i=0;i<4;++i){
        t.buffer58->vertices[i].stq[0]=(i&1?74.f:71.f)*t.invWidth4C;
        t.buffer58->vertices[i].stq[1]=(i&2?58.f:57.f)*t.invHeight50;}
        color(t,base->yellow16C);}
    return true;
}
}
