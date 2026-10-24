#include "original_standard_material_table50la.h"
#include "original_shader_pass_groups50la.h"
#include "original_standard_material_animation50la.h"
#include "original_standard_texture_resource50la.h"
#include "original_standard_texture_demand_math50la.h"
#include "../game/original_application_animation_clock50kg.h"
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <exception>
#include <new>
#include <utility>

namespace rr::assets {
namespace {
using D=OriginalStandardMaterialDiagnostic50LA;
using E=OriginalStandardMaterialStatus50LA;
using Origin=OriginalStandardMaterialValueOrigin50LA;
using Field=OriginalUiGsField50KG;
struct Spec {const char* name;std::int32_t initial;std::uint32_t templatePC,passPC;};
constexpr Spec specs[]={
#include "original_ui_material_fields50kg.inc"
};
static_assert(sizeof(specs)/sizeof(specs[0])==originalUiGsFieldCount50KG,"native material fields");
char lower(char c) noexcept {return c>='A'&&c<='Z'?char(c+('a'-'A')):c;}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)if(lower(a[i])!=lower(b[i]))return false;
    return true;
}
bool same(const std::string& a,const char* b) noexcept {
    std::size_t i=0;for(;i<a.size()&&b[i];++i)if(lower(a[i])!=lower(b[i]))return false;
    return i==a.size()&&!b[i];
}
bool fail(D& d,E e,std::uint32_t pc,const char* field=nullptr) noexcept {
    d.status=e;d.sourcePC=pc;d.field=field;return false;
}
bool demandMultiplyFailure(D& d,std::uint32_t pc,float a,float b,const char* reason) noexcept {
    std::uint32_t aBits=0,bBits=0;
    std::memcpy(&aBits,&a,sizeof(aBits));std::memcpy(&bBits,&b,sizeof(bBits));
    const char* why=reason?reason:"texture-demand multiplication";
    // Failure only: retain bounded numeric operands, not private addresses.
    // Keep the original PC/status/row/pass/map and already committed effects.
    std::fprintf(stderr,"[garage-demand-arithmetic] pc=0x%08x row=%d pass=%zu map=%zu lhs_bits=0x%08x rhs_bits=0x%08x reason=%s\n",
        static_cast<unsigned>(pc),static_cast<int>(d.row),d.pass,d.map,
        static_cast<unsigned>(aBits),static_cast<unsigned>(bBits),why);
    return fail(d,E::arithmetic,pc,why);
}
bool ascii(const std::string& s,std::size_t limit,bool empty=false) noexcept {
    if(s.size()>limit||(!empty&&s.empty()))return false;
    for(unsigned char c:s)if(c<32||c>=127)return false;
    return true;
}
bool integer(const OriginalUiDatSection50KG& section,const char* key,std::int32_t fallback,
    std::int32_t& out,D& d,std::uint32_t pc) {
    if(!originalUiInteger50KG(section,key,fallback,out,d.document,pc))return fail(d,E::document,pc,key);
    return true;
}
bool string(const OriginalUiDatSection50KG& section,const char* key,const std::string& fallback,
    std::string& out,D& d,std::uint32_t pc,std::size_t limit=255) {
    const auto* value=section.find(key);out=value?value->text:fallback;
    return ascii(out,limit,true)||fail(d,E::document,pc,key);
}
bool apply(const OriginalUiDatSection50KG& section,const char* key,
    OriginalStandardMaterialInteger50LA& out,D& d,std::uint32_t pc) {
    std::int32_t n=out.value;
    if(!integer(section,key,n,n,d,pc))return false;
    if(const auto* value=section.find(key))out={n,Origin::authored,nullptr,value->valueOffset,pc};
    return true;
}
void runtime(OriginalStandardMaterialStateContext50LA& state,Field f,std::int32_t n,std::uint32_t pc) noexcept {
    state.fields[static_cast<std::size_t>(f)]={n,Origin::runtime,nullptr,0,pc};
}
bool knownField(const std::string& key,bool dual,bool animated,bool textureAnimated,
    const OriginalStandardProgramTable50LA& programs) {
    for(const auto& spec:specs)if(same(key,spec.name))return true;
    if(dual){
        for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i){
            if(i==static_cast<std::size_t>(Field::colorClampingMethod)||
               i==static_cast<std::size_t>(Field::alphaBlendingControlInUnitsOfPixels)||
               i==static_cast<std::size_t>(Field::environmentContext0or1))continue;
            if(same(key,std::string(specs[i].name)+"_1")||same(key,std::string(specs[i].name)+"_2"))return true;
        }
        constexpr const char* names[]={"inputAlphaD_1","inputAlphaD_2",
            "lodCalculationMethod_1","maximumMipLevel_1","magFilter_1","minFilter_1","L_1","K_1",
            "lodCalculationMethod_2","maximumMipLevel_2","magFilter_2","minFilter_2","L_2","K_2"};
        for(const char* name:names)if(same(key,name))return true;
    }
    for(std::size_t i=0;i<programs.parameterRowCount382B78();++i){
        OriginalStandardProgramParameterRowView50LA row;
        if(programs.parameterRow382B78(static_cast<std::int32_t>(i),row)&&same(key,row.canonicalName))return true;
    }
    if(animated&&same(key,"uvs_animate"))return true;
    if((animated||textureAnimated)&&same(key,"animatesWhilePaused"))return true;
    constexpr const char* textureFields[]={"animatedMapInitialFrame","animatedMapFramesPerSecond",
        "animatedMapLooping","animatedMapReverseLoopOnEnd","animatedMapLoopOnEndToFrame"};
    if(textureAnimated)for(const auto* field:textureFields)if(same(key,field))return true;
    constexpr const char* names[]={"microProgramName","EnableThisShader","totalTextureMaps",
        "TextureMap_1","TextureCoordinateIndex_1","TextureMap_2","TextureCoordinateIndex_2","ShaderPassGroup","inputAlphaD",
        "lodCalculationMethod","maximumMipLevel","magFilter","minFilter","L","K",
        "TextureAnimation","VertexAnimation","PixelAnimation","EnableFillColor",
        "FillColorRed","FillColorGreen","FillColorBlue","FillColorAlpha"};
    for(const char* name:names)if(same(key,name))return true;
    return false;
}
bool parameters249390(const OriginalUiDatSection50KG& section,
    const OriginalStandardProgramTable50LA& programs,
    OriginalStandardMaterialParameters50LA& out,D& d) {
    // Source158D00 reads four comma-delimited scalar components when a name
    // is present. Admit complete bounded decimal vectors only; malformed or
    // truncated input is a terminal document error, never an absent parameter.
    for(std::size_t i=0;i<programs.parameterRowCount382B78();++i){
        OriginalStandardProgramParameterRowView50LA parameter;
        if(!programs.parameterRow382B78(static_cast<std::int32_t>(i),parameter))return fail(d,E::program,0x249390);
        const auto* authored=section.find(parameter.canonicalName);
        if(!authored)continue;
        OriginalStandardMaterialParameter50LA entry;
        entry.row382B78=parameter.index;entry.destination=parameter.value;
        entry.byteOffset=authored->valueOffset;
        const auto& text=authored->text;
        if(text.size()>127)return fail(d,E::unsupportedPass,0x158D24,"parameter vector length");
        std::size_t at=0;
        for(std::size_t lane=0;lane<4;++lane){
            std::size_t end=text.find(',',at);
            if(end==std::string::npos)end=text.size();
            if((lane<3&&end==text.size())||(lane==3&&end!=text.size())){
                d.document={OriginalUiPageError50KG::numberSyntax,authored->valueOffset+at,0x158D00};
                return fail(d,E::document,0x2493C8,parameter.canonicalName);
            }
            auto first=at,last=end;
            while(first<last&&(text[first]==' '||text[first]=='\t'||text[first]=='\r'))++first;
            while(last>first&&(text[last-1]==' '||text[last-1]=='\t'||text[last-1]=='\r'))--last;
            OriginalUiDatSection50KG scalar;
            scalar.values.push_back({parameter.canonicalName,text.substr(first,last-first),
                authored->keyOffset,authored->valueOffset+first,true});
            if(!originalUiNumber50KG(scalar,parameter.canonicalName,0,entry.value[lane],d.document,0x158D00))
                return fail(d,E::document,0x2493C8,parameter.canonicalName);
            at=end+1;
        }
        out.push_back(entry);
    }
    return true;
}
bool admitPass(const OriginalUiDatSection50KG& section,bool dual,bool animated,bool textureAnimated,
    const OriginalStandardProgramTable50LA& programs,D& d) {
    constexpr const char* animation[]={"TextureAnimation","VertexAnimation","PixelAnimation"};
    for(std::size_t kind=0;kind<3;++kind)if(const auto* v=section.find(animation[kind]))
        if(!same(v->text,"none")&&!(((animated&&kind==1)||(textureAnimated&&kind==0))&&same(v->text,"unique")))
            return fail(d,E::unsupportedPass,0x29B890,animation[kind]);
    for(const auto& value:section.values)if(value.sourceValuePresent&&!knownField(value.key,dual,animated,textureAnimated,programs))
        return fail(d,E::unsupportedPass,0x247110,"unrepresented active pass field");
    return true;
}
bool singleState(const OriginalUiDatSection50KG& section,
    const OriginalUiMaterialTemplateState50KG& inherited,
    const OriginalStandardMaterialContext50LA* binding,
    OriginalStandardMaterialState50LA& out,D& d) {
    for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i){
        const auto& source=inherited.gs.fields[i];
        out.fields[i]={source.value,source.member?Origin::templateValue:Origin::sourceDefault,
            source.member,source.byteOffset,source.sourcePC};
    }
    //247C88 missing context is0 even if the associated template differs.
    out.fields[static_cast<std::size_t>(Field::environmentContext0or1)]={0,Origin::sourceDefault,nullptr,0,0x247C88};
    for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i){
        if(i==static_cast<std::size_t>(Field::inputColorD)&&
           !apply(section,"inputAlphaD",out.fields[i],d,0x247A7C))return false;
        if(!apply(section,specs[i].name,out.fields[i],d,specs[i].passPC))return false;
    }
    if(!binding||!binding->live())return fail(d,E::video,0x247AC0,"live native material construction context");
    out.runtime435670=binding->runtime435670();
    if(out.runtime435670){
        runtime(out,Field::inputAlphaC,2,0x247AC4);
        runtime(out,Field::fixedAlphaValue,16,0x247ACC);
        runtime(out,Field::foggingOnOff,0,0x247D6C);
        runtime(out,Field::alphaBlendingOnOff,1,0x247D70);
        runtime(out,Field::depthTestMethod,1,0x247ED4);
    }
    // Bounded disjoint native scalar fields; out-of-width source bit overlap
    // is rejected, never silently normalized using a host raster convention.
    constexpr std::int32_t maxima[]={1,3,255,255,1,3,3,3,3,255,
        3,3,1023,1023,1023,1023,1,1,1,1,1,1,1,1,1,1,1,1,7,255,3,1,1,3,1};
    static_assert(sizeof(maxima)/sizeof(maxima[0])==originalUiGsFieldCount50KG,"native state bounds");
    for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i)
        if(out.fields[i].value<0||out.fields[i].value>maxima[i])
            return fail(d,E::unsupportedPass,out.fields[i].sourcePC,specs[i].name);
    constexpr const char* samplerNames[]={"lodCalculationMethod","maximumMipLevel","magFilter","minFilter","L","K"};
    constexpr std::int32_t defaults[]={0,0,1,5,0,-75},minimum[]={0,0,0,0,0,-2048},maximum[]={1,7,1,7,3,2047};
    constexpr std::uint32_t pcs[]={0x247F70,0x247F88,0x247FA0,0x247FB8,0x247FD0,0x247FE8};
    for(std::size_t i=0;i<6;++i){
        out.sampler[i]={defaults[i],Origin::sourceDefault,nullptr,0,0x247F38};
        if(!apply(section,samplerNames[i],out.sampler[i],d,pcs[i]))return false;
        if(out.sampler[i].value<minimum[i]||out.sampler[i].value>maximum[i])
            return fail(d,E::unsupportedPass,pcs[i],samplerNames[i]);
    }
    if(!binding->live())return fail(d,E::video,0x248060,"actual video443A60 construction context");
    // These are source-data snapshots, NOT native GPU addresses or device
    // packets. In particular format0x31 is a legitimate constructor value.
    out.videoDepthBase2E0=binding->depthBufferBase2E0();
    out.videoDepthFormat5C=static_cast<std::int32_t>(binding->depthBufferFormat5C());
    return true;
}
bool applyContext(const OriginalUiDatSection50KG& section,const std::string& key,
    const char* diagnosticName,OriginalStandardMaterialInteger50LA& out,D& d,std::uint32_t pc) {
    std::int32_t n=out.value;
    if(!originalUiInteger50KG(section,key,n,n,d.document,pc))return fail(d,E::document,pc,diagnosticName);
    if(const auto* value=section.find(key))out={n,Origin::authored,nullptr,value->valueOffset,pc};
    return true;
}
bool dualState2480D0(const OriginalUiDatSection50KG& section,
    const OriginalUiMaterialTemplateState50KG& inherited,
    const OriginalStandardMaterialContext50LA* binding,
    OriginalStandardMaterialState50LA& out,D& d) {
    if(!binding||!binding->live())return fail(d,E::video,0x2480D0,"live native material construction context");
    // Actual2480D0 has its own two-context input names and defaults. It does
    // not call2476A0 twice or inherit the latter's unsuffixed authored values.
    // Shared color clamp/PABE are read once; context selection is literal0/1.
    constexpr std::uint32_t pcs[][2]={
        {0x248494,0x2484F8},{0x2484AC,0x248510},
        {0x24876C,0x2487F8},{0x248784,0x248810},{0x24879C,0x248824},
        {0x248894,0x2489B0},{0x2488AC,0x2489C8},{0x2488C4,0x2489E0},
        {0x2488F4,0x248A10},{0x24890C,0x248A28},
        {0x248AD4,0x248BE4},{0x248AEC,0x248BFC},{0x248B04,0x248C14},
        {0x248B1C,0x248C2C},{0x248B34,0x248C44},{0x248B4C,0x248C5C},
        {0x248424,0x248424},{0x248CCC,0x248D00},{0x248458,0x248458},
        {0x248D64,0x248EC0},{0x248D7C,0x248ED8},{0x248D94,0x248EF0},
        {0x248DAC,0x248F08},{0x248DC4,0x248F20},{0x248DDC,0x248F38},
        {0x248E6C,0x248FCC},{0x248DF4,0x248F50},
        {0x249020,0x24917C},{0x249038,0x249194},{0x249050,0x2491AC},
        {0x249068,0x2491C4},{0x249080,0x2491DC},{0x249098,0x2491F4},
        {0x2490B0,0x24920C},{0x2492A8,0x249314}
    };
    static_assert(sizeof(pcs)/sizeof(pcs[0])==originalUiGsFieldCount50KG,"dual state getters");
    constexpr std::int32_t maxima[]={1,3,255,255,1,3,3,3,3,255,
        3,3,1023,1023,1023,1023,1,1,1,1,1,1,1,1,1,1,1,1,7,255,3,1,1,3,1};
    constexpr const char* samplerNames[]={"lodCalculationMethod","maximumMipLevel","magFilter","minFilter","L","K"};
    constexpr std::int32_t defaults[]={0,0,1,5,0,-75},minimum[]={0,0,0,0,0,-2048},maximum[]={1,7,1,7,3,2047};
    constexpr std::uint32_t samplerPcs[2][6]={
        {0x248570,0x248588,0x2485A0,0x2485B8,0x2485D0,0x2485E8},
        {0x248674,0x24868C,0x2486A4,0x2486BC,0x2486D4,0x2486EC}
    };
    out.secondContext.emplace();
    for(std::size_t c=0;c<2;++c){
        auto& context=c?*out.secondContext:static_cast<OriginalStandardMaterialStateContext50LA&>(out);
        const std::string suffix=c?"_2":"_1";
        for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i){
            const auto& source=inherited.gs.fields[i];
            context.fields[i]={source.value,source.member?Origin::templateValue:Origin::sourceDefault,
                source.member,source.byteOffset,source.sourcePC};
            if(i==static_cast<std::size_t>(Field::environmentContext0or1)){
                context.fields[i]={static_cast<std::int32_t>(c),Origin::sourceDefault,nullptr,0,pcs[i][c]};
                continue;
            }
            const bool shared=i==static_cast<std::size_t>(Field::colorClampingMethod)||
                i==static_cast<std::size_t>(Field::alphaBlendingControlInUnitsOfPixels);
            if(shared&&c){context.fields[i]=out.fields[i];continue;}
            if(i==static_cast<std::size_t>(Field::inputColorD)&&
                !applyContext(section,"inputAlphaD"+suffix,"inputAlphaD",context.fields[i],d,c?0x2489F8:0x2488DC))return false;
            if(!applyContext(section,std::string(specs[i].name)+(shared?"":suffix),
                specs[i].name,context.fields[i],d,pcs[i][c]))return false;
        }
        context.runtime435670=binding->runtime435670();
        if(context.runtime435670){
            runtime(context,Field::inputAlphaC,2,c?0x248A40:0x248924);
            runtime(context,Field::fixedAlphaValue,16,c?0x248A48:0x24892C);
            runtime(context,Field::foggingOnOff,0,c?0x248F6C:0x248E10);
            runtime(context,Field::alphaBlendingOnOff,1,c?0x248F70:0x248E14);
            runtime(context,Field::depthTestMethod,1,c?0x249228:0x2490CC);
        }
        for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i)
            if(context.fields[i].value<0||context.fields[i].value>maxima[i])
                return fail(d,E::unsupportedPass,context.fields[i].sourcePC,specs[i].name);
        for(std::size_t i=0;i<6;++i){
            context.sampler[i]={defaults[i],Origin::sourceDefault,nullptr,0,c?0x24863Cu:0x24853Cu};
            if(!applyContext(section,std::string(samplerNames[i])+suffix,samplerNames[i],
                context.sampler[i],d,samplerPcs[c][i]))return false;
            if(context.sampler[i].value<minimum[i]||context.sampler[i].value>maximum[i])
                return fail(d,E::unsupportedPass,samplerPcs[c][i],samplerNames[i]);
        }
        if(!binding->live())return fail(d,E::video,c?0x249320:0x2492B4,"actual video443A60 construction context");
        context.videoDepthBase2E0=binding->depthBufferBase2E0();
        context.videoDepthFormat5C=static_cast<std::int32_t>(binding->depthBufferFormat5C());
    }
    return true;
}
}
struct OriginalStandardMaterialObject50LA::Storage {
    struct Pass {
        std::int32_t references04=1; //1048F0 ->29AD80 ->246FA0.
        std::int16_t program18=-1,group22=-1;
        std::array<std::int16_t,2> texture1A{{-1,-1}};
        std::array<std::uint8_t,2> uv1E{{0,0}};
        std::uint8_t mapCount20=0,flags21=2;
        std::array<std::uint8_t,4> fillRgba2C{{128,128,128,255}};
        const OriginalUiMaterialTemplateState50KG* constructionTemplate=nullptr;
        std::unique_ptr<OriginalStandardMaterialState50LA> state30;
        std::array<OriginalStandardMaterialNativeBinding50KG,2> nativeBindings{};
        std::uint32_t parameterMask24=0;
        std::int32_t parameterCount28=0;
        std::unique_ptr<const OriginalStandardMaterialParameters50LA> parameters34;
        std::unique_ptr<OriginalStandardMaterialAnimation50LA> animation10;
        std::unique_ptr<OriginalStandardMaterialTextureAnimation50LA> textureAnimation10;
    };
    std::string name00;
    std::int32_t row40=-1,count48=0;
    std::array<std::unique_ptr<Pass>,4> passes4C{};
    std::array<std::int32_t,4> collectors5C{{-1,-1,-1,-1}};
    std::shared_ptr<const OriginalStandardMaterialSource50LA> source;
    std::shared_ptr<const OriginalUiArchive50KG> tournamentMissingPsm;
    std::unique_ptr<OriginalUiMaterial50KG> tournamentFallback;
    std::shared_ptr<const OriginalUiArchive50KG> speedBoostMissingPsm;
    std::unique_ptr<OriginalUiMaterial50KG> speedBoostFallback;
    std::shared_ptr<const OriginalUiArchive50KG> damageMissingPsm;
    std::unique_ptr<OriginalUiMaterial50KG> damageFallback;
    std::shared_ptr<const OriginalUiArchive50KG> pauseBackdropMissingPsm;
    std::shared_ptr<const OriginalGarageScreenFallback50KG> garageFallback;
    OriginalGarageScreenFallback50KG::ConstructionObservation sharedIconConstruction;
    OriginalMemoryCardScreenFallback50KG::Observation memoryCardConstruction;
    std::unique_ptr<OriginalUiMaterial50KG> pauseBackdropFallback;
    std::shared_ptr<const OriginalUiMaterialTemplates50KG> templates;
    std::shared_ptr<const OriginalStandardProgramTable50LA> programs;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> construction;
    //29B410 prologue44/epilogue6C start null; present actions reject before
    //publication, so no fake action object/success callback is necessary.
    bool constructed=false;
};
OriginalStandardMaterialObject50LA::OriginalStandardMaterialObject50LA():storage_(new Storage) {}
OriginalStandardMaterialObject50LA::~OriginalStandardMaterialObject50LA(){
    if(!storage_)return;
    //29B498 releases exactly four pass slots in ascending order. The table
    //retires borrowed collector nodes first; each source pass ref remains1.
    for(auto& owned:storage_->passes4C){
        if(!owned)continue;
        auto& pass=*owned;
        if(pass.references04!=1)std::terminate();
        --pass.references04; //104970: last pass reference invokes247030.
        pass.state30.reset(); //247058..68 precedes both texture releases.
        pass.parameters34.reset(); //24706C..78: parameter owner before maps.
        for(const auto token:pass.texture1A)if(token!=-1&&token!=-2)
            if(!storage_->textures||!storage_->textures->release29E400(token))std::terminate();
        pass.animation10.reset(); //29ADC0: base animation lifetime after maps.
        pass.textureAnimation10.reset();
        owned.reset();
    }
}
bool OriginalStandardMaterialObject50LA::constructed29B550() const noexcept {return storage_&&storage_->constructed;}
const std::int32_t* OriginalStandardMaterialObject50LA::row40() const noexcept {return storage_?&storage_->row40:nullptr;}
const std::int32_t* OriginalStandardMaterialObject50LA::passCount48() const noexcept {return storage_?&storage_->count48:nullptr;}
const std::string* OriginalStandardMaterialObject50LA::name00() const noexcept {return storage_?&storage_->name00:nullptr;}
const OriginalStandardMaterialSource50LA* OriginalStandardMaterialObject50LA::source() const noexcept {return storage_?storage_->source.get():nullptr;}
const OriginalStandardTextureTable50LA* OriginalStandardMaterialObject50LA::textures383400() const noexcept {return storage_?storage_->textures.get():nullptr;}
const std::int32_t* OriginalStandardMaterialObject50LA::collector5C(std::size_t i) const noexcept {
    return storage_&&i<4?&storage_->collectors5C[i]:nullptr;
}
void OriginalStandardMaterialObject50LA::clearCollectors29BEA0() noexcept {
    if(!storage_||!storage_->constructed||storage_->count48<1||storage_->count48>4)
        std::terminate();
    for(std::int32_t i=0;i<storage_->count48;++i)
        storage_->collectors5C[static_cast<std::size_t>(i)]=0; //29BEA0 source clear value.
}
void OriginalStandardMaterialObject50LA::setCollector29BED0(std::size_t pass,std::int32_t index) noexcept {
    if(!storage_||!storage_->constructed||storage_->count48<1||storage_->count48>4||
       pass>=static_cast<std::size_t>(storage_->count48)||index<0||index>=4096)
        std::terminate();
    storage_->collectors5C[pass]=index;
}
void OriginalStandardMaterialObject50LA::commitNativeUntexturedState249BA0(std::size_t slot,
    const OriginalStandardMaterialStateContext50LA& context)noexcept {
    if(!storage_||slot>=4||!storage_->passes4C[slot])std::terminate();
    auto& p=*storage_->passes4C[slot];
    if(p.mapCount20||!p.state30||p.state30->contextCount()!=1u||p.nativeBindings[0].bound()||p.nativeBindings[1].bound())std::terminate();
    static_cast<OriginalStandardMaterialStateContext50LA&>(*p.state30)=context;
    //No texture native history is created. Source TME0 never bound an image.
}
void OriginalStandardMaterialObject50LA::commitNativeBinding249610(std::size_t slot,std::size_t map,
    const OriginalStandardMaterialStateContext50LA& context,
    const OriginalStandardMaterialNativeBinding50KG& binding) noexcept {
    // Table preflights the WHOLE transaction before the first nonallocating
    // store. Alias submitters never receive the final representative's state.
    if(!storage_||slot>=4||!storage_->passes4C[slot])std::terminate();
    auto& p=*storage_->passes4C[slot];
    if(!p.state30||map>=p.state30->contextCount()||map>=p.nativeBindings.size()||!binding.bound())std::terminate();
    if(map==0)static_cast<OriginalStandardMaterialStateContext50LA&>(*p.state30)=context;
    else *p.state30->secondContext=context;
    p.nativeBindings[map]=binding;
}
bool OriginalStandardMaterialObject50LA::pass4C(std::size_t i,OriginalStandardMaterialPassView50LA& out) const noexcept {
    if(!storage_||i>=4||!storage_->passes4C[i])return false;
    const auto& p=*storage_->passes4C[i];
    out={&p.references04,&p.program18,&p.texture1A,&p.uv1E,&p.mapCount20,&p.flags21,
        &p.group22,&p.fillRgba2C,p.constructionTemplate,p.state30.get(),
        &p.parameterMask24,&p.parameterCount28,p.parameters34.get(),p.animation10.get(),p.textureAnimation10.get(),&p.nativeBindings};return true;
}
const OriginalUiMaterialProvenance50KG* OriginalStandardMaterialObject50LA::tournamentFallbackProvenance() const noexcept {
    return storage_&&storage_->tournamentFallback?&storage_->tournamentFallback->provenance:nullptr;
}
const OriginalUiMaterialProvenance50KG* OriginalStandardMaterialObject50LA::speedBoostFallbackProvenance() const noexcept {
    return storage_&&storage_->speedBoostFallback?&storage_->speedBoostFallback->provenance:nullptr;
}
const OriginalUiMaterialProvenance50KG* OriginalStandardMaterialObject50LA::damageFallbackProvenance() const noexcept {
    return storage_&&storage_->damageFallback?&storage_->damageFallback->provenance:nullptr;
}
const OriginalUiMaterialProvenance50KG* OriginalStandardMaterialObject50LA::pauseBackdropFallbackProvenance() const noexcept {
    return storage_&&storage_->pauseBackdropFallback?&storage_->pauseBackdropFallback->provenance:nullptr;
}
bool OriginalStandardMaterialObject50LA::capturedSharedIconName()const noexcept {
    if(!storage_)return false;using P=OriginalStandardArchiveProvider50LA;
    return P::sameName(storage_->name00,"ui\\textures\\icon_x.psm")||
        P::sameName(storage_->name00,"ui\\textures\\icon_tri.psm")||
        P::sameName(storage_->name00,"ui\\textures\\icon_sqr.psm")||
        P::sameName(storage_->name00,"ui\\textures\\icon_cir.psm");
}
bool OriginalStandardMaterialObject50LA::borrowCapturedSharedIconTexture(
    const OriginalStandardTextureResource50LA& texture,
    std::shared_ptr<const OriginalUiArchive50KG>& archive,OriginalUiMemberView50KG& member,D& d)const noexcept {
    if(!storage_||!capturedSharedIconName())return fail(d,E::source,0x2475C0);
    const auto& s=*storage_;
    const auto provider=s.garageFallback?s.garageFallback->provider_:s.memoryCardConstruction.provider;
    if(!preflightCapturedSharedIconBinding(s.row40,s.construction,s.textures,s.programs,provider,texture.resolver_,d))return false;
    const auto& pass=*s.passes4C[0];OriginalStandardTextureResource50LA* actual=nullptr;
    if(!s.textures->resolve298E60(pass.texture1A[0],actual,d.texture)||actual!=&texture)
        return fail(d,E::texture,0x247648,"exact retained representative resource");
    const auto& receipt=texture.firstUiSource_;
    if(!receipt.archive||!receipt.member)return fail(d,E::texture,0x24E880);
    const auto selected=receipt.archive->member(receipt.ordinal);
    if(selected.identity!=receipt.member||selected.data!=receipt.data||selected.size!=receipt.bytes)
        return fail(d,E::texture,0x24E880,"immutable first texture archive member");
    archive=receipt.archive;member=selected;return true;
}
bool OriginalStandardMaterialObject50LA::preflightCapturedSharedIconBinding(std::int32_t row,
    const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
    const std::shared_ptr<OriginalStandardTextureTable50LA>& textures,
    const std::shared_ptr<const OriginalStandardProgramTable50LA>& programs,
    const std::shared_ptr<const OriginalStandardArchiveProvider50LA>& provider,
    const std::shared_ptr<OriginalStandardTextureResolver50LA>& resolver,D& d)const noexcept {
    d={};d.row=row;d.pass=0;
    if(!storage_||!storage_->constructed||storage_->row40!=row||storage_->source||!capturedSharedIconName()||
       (!storage_->garageFallback&&!storage_->memoryCardConstruction.options)||storage_->count48!=1||!storage_->passes4C[0])
        return fail(d,E::source,0x29B638,"captured actual shared-icon first construction");
    const auto& s=*storage_;const auto& pass=*s.passes4C[0];
    using Provider=OriginalStandardArchiveProvider50LA;
    const auto* garage=s.garageFallback.get();const auto& card=s.memoryCardConstruction;
    if(garage&&card.options)return fail(d,E::source,0x29B638,"one actual construction family");
    const std::vector<std::uint8_t>* retained=nullptr;
    if(garage){
        const auto& receipt=s.sharedIconConstruction;
        if(garage->row_!=row||s.name00.size()!=garage->name_.size()+4||
           !Provider::sameName(std::string_view(s.name00).substr(0,garage->name_.size()),garage->name_)||
           !Provider::sameName(std::string_view(s.name00).substr(garage->name_.size()),".psm")||garage->provider_!=provider||garage->context_!=context||garage->textures_!=textures||
           !garage->garage_||!garage->garage_->loaded()||
           garage->garage_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garage)||
           !receipt.selectedArchive.identity()||receipt.providerIdentity!=provider.get()||
           receipt.selectedArchive.providerIdentity()!=provider.get()||
           receipt.selectedArchive.identity()!=originalUiArchiveIdentity50KG(garage->pstArchive_)||
           !receipt.revision||!receipt.bankGeneration||!receipt.selectedBytes||
           receipt.selectedSize!=garage->pstBytes_.size()||
           std::memcmp(receipt.selectedBytes,garage->pstBytes_.data(),receipt.selectedSize))
            return fail(d,E::source,0x29B638,"retained Garage construction observation");
        retained=&garage->pstBytes_;
    }else{
        if(!Provider::sameName(s.name00,"ui\\textures\\icon_x.psm")||!card.options||
           !card.options->loaded()||card.options->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::options)||
           card.options->member(46).identity!=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::options,46)||
           card.options->member(1).identity!=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::options,1)||card.provider!=provider||
           (card.selectedArchive.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::controllerPopup)&&
            card.selectedArchive.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))||
           card.selectedArchive.providerIdentity()!=provider.get()||
           !card.revision||!card.bankGeneration||!card.selectedBytes||card.pstBytes.empty()||
           std::memcmp(card.selectedBytes,card.pstBytes.data(),card.pstBytes.size()))
            return fail(d,E::source,0x29B638,"retained MemoryCard construction observation");
        retained=&card.pstBytes;
    }
    const auto* constructionArchive=garage?s.sharedIconConstruction.selectedArchive.identity():card.selectedArchive.identity();
    const auto constructionOrdinal=garage?s.sharedIconConstruction.ordinal:card.ordinal;
    const auto* constructionMember=constructionArchive?originalUiMemberIdentity50KG(constructionArchive->id,constructionOrdinal):nullptr;
    if(!constructionMember||constructionMember->bytes!=retained->size()||!constructionMember->name||
       std::strlen(constructionMember->name)!=s.name00.size()||
       !Provider::sameName(std::string_view(constructionMember->name,s.name00.size()-4),std::string_view(s.name00).substr(0,s.name00.size()-4))||
       !Provider::sameName(std::string_view(constructionMember->name+s.name00.size()-4),".pst"))
        return fail(d,E::source,0x29B638,"exact first material selected member");
    if(!capturedSharedIconName()||s.programs!=programs||!provider||s.construction!=context||!context||!context->live()||s.textures!=textures||
       !s.templates||!context->materialApplication()||&context->materialApplication()->templates()!=s.templates.get()||
       pass.constructionTemplate!=s.templates->get(OriginalUiTemplate50KG::screenMt)||
       !pass.constructionTemplate||pass.constructionTemplate->microprogram!="ScreenObject"||
       pass.references04!=1||pass.mapCount20!=1||pass.flags21!=2||pass.uv1E[0]||pass.uv1E[1]||
       pass.group22!=-1||pass.texture1A[0]<0||pass.texture1A[1]!=-1||pass.parameterMask24||
       pass.parameterCount28||pass.parameters34||pass.animation10||pass.textureAnimation10||
       !pass.state30||pass.state30->contextCount()!=1u)
        return fail(d,E::reference,0x2475C0,"retained first-origin ScreenMT owner and live pass");
    OriginalStandardProgramRowView50LA program;
    if(!s.programs||!s.programs->row(pass.program18,program)||!program.canonicalName||
       std::strcmp(program.canonicalName,"ScreenObject"))return fail(d,E::program,0x247620);
    OriginalStandardTextureResource50LA* texture=nullptr;OriginalStandardTextureRowView50LA textureRow;
    if(!textures||!textures->row(pass.texture1A[0],textureRow)||!textureRow.references08||*textureRow.references08<=0||
       !textures->resolve298E60(pass.texture1A[0],texture,d.texture)||!texture||textureRow.object04!=texture||
       texture->construction_!=context||texture->resolver_!=resolver||!resolver||
       texture->firstUiSource_.providerIdentity!=provider.get()||
       !texture->firstUiSourceCurrent())return fail(d,E::texture,0x247648,"actual texture first-source receipt");
    // The material's historical PST winner may differ from an earlier cached
    // texture's equal-byte winner. Preserve BOTH receipts; never relabel either.
    const auto* bytes=texture->originalBytes();const auto* member=texture->sourceMember();
    if(!member||member->size()!=s.name00.size()||
       !Provider::sameName(std::string_view(*member).substr(0,member->size()-4),std::string_view(s.name00).substr(0,s.name00.size()-4))||
       !Provider::sameName(std::string_view(*member).substr(member->size()-4),".pst")||!bytes||bytes->size()!=retained->size()||
       std::memcmp(bytes->data(),retained->data(),bytes->size()))
        return fail(d,E::texture,0x24E880,"retained material and resource bytes");
    d.sourcePC=0x2DF158;return true;
}
bool OriginalStandardMaterialObject50LA::borrowGarageScreenFallback2475C0(
    std::size_t slot,const OriginalStandardTextureResource50LA& texture,
    const OriginalUiArchive50KG& archive,const OriginalUiMemberView50KG& member,
    OriginalUiMaterial50KG& output,D& d) const noexcept {
    d={};d.pass=slot;
    if(capturedSharedIconName()){
        if(slot!=0)return fail(d,E::range,0x2475C0);
        std::shared_ptr<const OriginalUiArchive50KG> retained;OriginalUiMemberView50KG selected;
        if(!borrowCapturedSharedIconTexture(texture,retained,selected,d))return false;
        if(retained.get()!=&archive||selected.identity!=member.identity||selected.data!=member.data||selected.size!=member.size)
            return fail(d,E::texture,0x24E880,"exact first-source texture owner, not current provider winner");
        const auto& s=*storage_;const auto& pass=*s.passes4C[0];
        try {
            OriginalStandardProgramRowView50LA program;
            if(!s.programs->row(pass.program18,program)||!program.canonicalName)return fail(d,E::program,0x247620);
            OriginalUiMaterial50KG next;auto& provenance=next.provenance;
            provenance.origin=s.garageFallback?OriginalUiMaterialOrigin50KG::garageCanonicalScreenMissingPsmFallback:
                OriginalUiMaterialOrigin50KG::memoryCardCanonicalScreenMissingPsmFallback;
            provenance.archive=archive.identity();provenance.fallbackTexture=member.identity;
            provenance.fallbackConstructionArchive=s.garageFallback?s.sharedIconConstruction.selectedArchive.identity():
                s.memoryCardConstruction.selectedArchive.identity();
            provenance.fallbackConstructionTexture=originalUiMemberIdentity50KG(provenance.fallbackConstructionArchive->id,
                s.garageFallback?s.sharedIconConstruction.ordinal:s.memoryCardConstruction.ordinal);
            provenance.fallbackPageSource=s.garageFallback?s.garageFallback->garage_->find("UI/UIGarage/garage.dat").identity:
                s.memoryCardConstruction.options->find("UI/UIOption/memcard.dat").identity;
            if(!s.garageFallback){
                provenance.fallbackWidgetSource=s.memoryCardConstruction.options->find("UI/UIOption/Widgets.Dat").identity;
                provenance.sourceCallerPC=0x1FF058;
            }
            provenance.registeredName=s.name00.substr(0,s.name00.size()-4);
            provenance.attemptedPsmMember=s.name00;provenance.creationPC=0x2475C0;
            next.passes.emplace_back();auto& projected=next.passes.back();
            projected.provenance=provenance;projected.pass=1;projected.microprogram=program.canonicalName;
            projected.inheritedTemplate=pass.constructionTemplate->identity;
            projected.textureName=provenance.registeredName.substr(provenance.registeredName.find_last_of('\\')+1);
            projected.shaderPassGroup="none";projected.enabled=1;projected.textureCount=1;projected.textureCoordinateIndex=0;
            for(std::size_t i=0;i<projected.gs.fields.size();++i){const auto& field=pass.state30->fields[i];
                projected.gs.fields[i]={field.value,field.templateMember,field.byteOffset,field.sourcePC};}
            projected.lodCalculationMethod=pass.state30->sampler[0].value;projected.maximumMipLevel=pass.state30->sampler[1].value;
            projected.magFilter=pass.state30->sampler[2].value;projected.minFilter=pass.state30->sampler[3].value;
            projected.lodL=pass.state30->sampler[4].value;projected.lodK=pass.state30->sampler[5].value;
            output=std::move(next);return true;
        }catch(...){return fail(d,E::allocation,0x2475C0,"retained shared-icon current-state projection");}
    }
    if(!storage_||!storage_->constructed||storage_->source||slot!=0||storage_->count48!=1||
       !storage_->garageFallback||!storage_->passes4C[0])
        return fail(d,E::source,0x2475C0,"actual constructed Garage missing-PSM owner");
    const auto& s=*storage_;const auto& proof=*s.garageFallback;const auto& pass=*s.passes4C[0];d.row=s.row40;
    try {
        if(proof.row_!=s.row40||!proof.current(s.name00,s.construction,s.textures)||
           !s.templates||pass.constructionTemplate!=s.templates->get(OriginalUiTemplate50KG::screenMt)||
           !s.construction||!s.construction->materialApplication()||
           &s.construction->materialApplication()->templates()!=s.templates.get()||
           !pass.constructionTemplate||pass.constructionTemplate->microprogram!="ScreenObject"||
           pass.references04!=1||pass.mapCount20!=1||pass.flags21!=2||pass.uv1E[0]||pass.uv1E[1]||
           pass.group22!=-1||pass.texture1A[0]<0||pass.texture1A[1]!=-1||
           pass.parameterMask24||pass.parameterCount28||pass.parameters34||pass.animation10||pass.textureAnimation10||
           !pass.state30||pass.state30->contextCount()!=1u)
            return fail(d,E::reference,0x2475C0,"same live one-map ScreenMT fallback pass");
        OriginalStandardProgramRowView50LA program;
        if(!s.programs||!s.programs->row(pass.program18,program)||!program.canonicalName||
           std::strcmp(program.canonicalName,"ScreenObject"))
            return fail(d,E::program,0x247620,"actual retained ScreenObject program");
        OriginalStandardTextureResource50LA* actual=nullptr;
        if(!s.textures->resolve298E60(pass.texture1A[0],actual,d.texture)||actual!=&texture||
           !texture.constructed24DA30()||texture.animated102070()||!texture.references00()||*texture.references00()<=0)
            return fail(d,E::texture,0x247648,"actual fallback texture token and resource");
        OriginalStandardArchiveProvider50LA::MemberView selected;
        const auto* bytes=texture.originalBytes();const auto* name=texture.sourceMember();
        const auto actualMember=name?archive.find(*name):OriginalUiMemberView50KG{};
        std::size_t offset=0,extent=0;
        if(!archive.loaded()||archive.identity()!=originalUiArchiveIdentity50KG(proof.pstArchive_)||
           !member.identity||!member.data||!member.size||actualMember.identity!=member.identity||
           actualMember.data!=member.data||actualMember.size!=member.size||!name||*name!=proof.pstMember_||
           proof.provider_->lookup132820(*name,selected)!=OriginalStandardArchiveProvider50LA::Lookup::present||
           selected.data!=member.data||selected.size!=member.size||
           !bytes||bytes->size()!=member.size||proof.pstBytes_.size()!=member.size||
           std::memcmp(bytes->data(),member.data,member.size)||std::memcmp(proof.pstBytes_.data(),member.data,member.size)||
           !texture.pstExtent24E880(offset,extent)||offset||extent!=member.size)
            return fail(d,E::texture,0x24E880,"same winning Garage fallback PST and retained bytes");
        OriginalUiMaterial50KG next;auto& provenance=next.provenance;
        provenance.origin=OriginalUiMaterialOrigin50KG::garageCanonicalScreenMissingPsmFallback;
        provenance.archive=archive.identity();provenance.fallbackTexture=member.identity;
        provenance.fallbackPageSource=proof.garage_->find("UI/UIGarage/garage.dat").identity;
        provenance.registeredName=proof.name_;provenance.attemptedPsmMember=s.name00;provenance.creationPC=0x2475C0;
        next.passes.emplace_back();auto& projected=next.passes.back();
        projected.provenance=provenance;projected.pass=1;projected.microprogram=program.canonicalName;
        projected.inheritedTemplate=pass.constructionTemplate->identity;
        projected.textureName=proof.name_.substr(proof.name_.find_last_of('\\')+1);
        projected.shaderPassGroup="none";projected.enabled=1;projected.textureCount=1;projected.textureCoordinateIndex=0;
        // Observe the real state30, including any already-committed249610
        // history. Do not regenerate template defaults or change GS fields.
        for(std::size_t i=0;i<projected.gs.fields.size();++i){const auto& field=pass.state30->fields[i];
            projected.gs.fields[i]={field.value,field.templateMember,field.byteOffset,field.sourcePC};}
        projected.lodCalculationMethod=pass.state30->sampler[0].value;projected.maximumMipLevel=pass.state30->sampler[1].value;
        projected.magFilter=pass.state30->sampler[2].value;projected.minFilter=pass.state30->sampler[3].value;
        projected.lodL=pass.state30->sampler[4].value;projected.lodK=pass.state30->sampler[5].value;
        output=std::move(next);return true;
    }catch(...){return fail(d,E::allocation,0x2475C0,"retained Garage fallback state projection");}
}
bool OriginalStandardMaterialObject50LA::serviceCableBasePrototype29AC90(
    std::size_t slot,D& d) noexcept {
    if(!storage_||!storage_->constructed||!storage_->construction||!storage_->construction->live())
        return fail(d,E::phase,0x29B1D0);
    auto& s=*storage_;
    if(slot>=static_cast<std::size_t>(s.count48)||!s.passes4C[slot])
        return fail(d,E::range,0x29B1D0);
    auto& pass=*s.passes4C[slot];
    if(pass.textureAnimation10&&pass.animation10)return fail(d,E::unsupportedPass,0x29B1D0);
    const auto clock=s.construction->applicationAnimationClock();
    if(pass.textureAnimation10){
        const auto& base=*pass.textureAnimation10;
        if(!base.constructed_||base.material_!=this||base.pass_!=slot)
            return fail(d,E::unsupportedPass,0x29B1D0);
        rr::game::OriginalApplicationAnimationClockSample50KG entry;
        if(!clock.read(entry))return fail(d,E::phase,0x29ACA8);
        if(base.stamp08_==entry.current230)return true;
        if(entry.pause11C&&!base.paused_)return true;
        if(!s.textures)return fail(d,E::texture,0x29B1D0);
        // This actual bounded controller has null callback00, one texture0C
        // child and no vertex10/pixel14 child. Its existing source-ordered
        // leaf service reads the SAME sealed Application clock afresh.
        return serviceTexturePrototype29AC90(slot,*s.textures,clock,d);
    }
    if(!pass.animation10)return true; // Genuine null base-map10:24A750.
    auto& animation=*pass.animation10;
    auto* base=animation.prototype10_.get(); //29B200, irrespective of clones14.
    if(!animation.constructed_||!base||base->prototype_!=&animation||base->model_||
       base->nativeModelId_||base->callback00_||base->callbackOwner04_)
        return fail(d,E::unsupportedPass,0x29B1D0);
    rr::game::OriginalApplicationAnimationClockSample50KG entry;
    if(!clock.read(entry))return fail(d,E::phase,0x29ACA8);
    if(base->frameStamp08_==entry.current230)return true;
    if(entry.pause11C&&!base->animatesWhilePaused18_)return true;
    if(base->revision_==std::numeric_limits<std::uint64_t>::max())
        return fail(d,E::overflow,0x29ACD0);
    // Actual constructed domain: null callback, absent texture/pixel children.
    // Vertex vtable3D2180+10 ->246CC0: its absent uvs_animate subordinate08
    // returns without row writes (246CCC..246CE8). No engine callback/clone.
    rr::game::OriginalApplicationAnimationClockSample50KG commit;
    if(!clock.read(commit))return fail(d,E::phase,0x29AD48);
    base->frameStamp08_=commit.current230;++base->revision_;
    d.sourceEffectsCommitted=true;d.sourcePC=0x29AD58;d.pass=slot;return true;
}
bool OriginalStandardMaterialObject50LA::serviceTournamentMarker24A740(
    const OriginalStandardTextureTable50LA& expectedTextures,const OriginalUiMemberView50KG& psm,
    const OriginalUiMemberView50KG& atm,const rr::game::OriginalApplicationAnimationClockView50KG& forwarded,D& d) noexcept {
    if(!storage_||!storage_->constructed)return fail(d,E::missingMaterialObject,0x24A740);
    auto& s=*storage_;
    if(!s.construction||!s.construction->live())return fail(d,E::video,0x24A740);
    if(s.textures.get()!=&expectedTextures||s.count48!=1||!s.passes4C[0]||!s.source||
       !s.source->originalBytes()||!psm.data||!atm.data)return fail(d,E::source,0x24A740);
    const auto& psmBytes=*s.source->originalBytes();
    if(psmBytes.size()!=psm.size||std::memcmp(psmBytes.data(),psm.data,psm.size))return fail(d,E::source,0x24A740);
    auto& pass=*s.passes4C[0];
    auto* prototype=pass.textureAnimation10.get();
    if(!prototype||prototype->materialOwner()!=this||prototype->passOrdinal()!=0||
       pass.animation10||pass.mapCount20!=1||pass.texture1A[0]<0)
        return fail(d,E::unsupportedPass,0x29B1D0);
    OriginalStandardTextureResource50LA* aggregate=nullptr;
    if(!s.textures->resolve298E60(pass.texture1A[0],aggregate,d.texture))return fail(d,E::texture,d.texture.sourcePC);
    const auto* bytes=aggregate?aggregate->originalBytes():nullptr;
    const auto* count=aggregate?aggregate->frameCount20():nullptr;
    if(!bytes||bytes->size()!=atm.size||std::memcmp(bytes->data(),atm.data,atm.size)||!count||*count<=0)
        return fail(d,E::source,0x29A10C);
    return serviceTexturePrototype29AC90(0,expectedTextures,forwarded,d);
}
bool OriginalStandardMaterialObject50LA::serviceTexturePrototype29AC90(std::size_t slot,
    const OriginalStandardTextureTable50LA& expectedTextures,
    const rr::game::OriginalApplicationAnimationClockView50KG& forwarded,D& d) noexcept {
    // Caller provenance is separate: Tournament keeps its authenticated UI
    // proof above; retained model surfaces enter only through the private table.
    if(!storage_||!storage_->constructed||!storage_->construction||!storage_->construction->live())
        return fail(d,E::phase,0x29B1D0);
    auto& s=*storage_;
    if(s.textures.get()!=&expectedTextures||slot>=static_cast<std::size_t>(s.count48)||!s.passes4C[slot])
        return fail(d,E::source,0x29B1D0);
    auto& pass=*s.passes4C[slot];auto* prototype=pass.textureAnimation10.get();
    if(!prototype||!prototype->unique18()||prototype->materialOwner()!=this||prototype->passOrdinal()!=slot||
       pass.animation10||pass.mapCount20!=1||pass.texture1A[0]<0)
        return fail(d,E::unsupportedPass,0x29B1D0);
    // This concrete prototype representation has no registration table14 or
    // callback00/context04 mutation API. Registered clones/callbacks are not
    // represented or silently replaced by this bounded shared-prototype path.
    OriginalStandardTextureResource50LA* aggregate=nullptr;
    if(!s.textures->resolve298E60(pass.texture1A[0],aggregate,d.texture))return fail(d,E::texture,d.texture.sourcePC);
    const auto* count=aggregate?aggregate->frameCount20():nullptr;
    if(!aggregate||!aggregate->animated102070()||!count||*count<=0)return fail(d,E::texture,0x29A10C);
    //Authoritative reads use this material's sealed Video/Application owner.
    //The forwarded view is a consistency check, NOT an identity proof.
    //No callback/publication can reenter this bounded main-thread service.
    const auto clock=s.construction->applicationAnimationClock();
    rr::game::OriginalApplicationAnimationClockSample50KG entry,forwardedEntry;
    if(!forwarded.read(forwardedEntry)||!clock.read(entry)||
       forwardedEntry.current230!=entry.current230||forwardedEntry.pause11C!=entry.pause11C||
       forwardedEntry.pending120!=entry.pending120)return fail(d,E::phase,0x29ACA8);
    if(prototype->stamp08_==entry.current230)return true; //29ACB0, no store.
    if(entry.pause11C&&!prototype->paused_)return true; //29ACC8, no store.
    //Callback00/context04 are permanently null in this bounded prototype.
    //General callback support must keep callback-false -> final stamp read;
    //it must NOT return as if it were a duplicate/pause early-out.
    rr::game::OriginalApplicationAnimationClockSample50KG leafTime;
    if(!clock.read(leafTime))return fail(d,E::phase,0x29A118);
    auto next=prototype->state_;next.frameCount=static_cast<std::uint32_t>(*count);
    std::uint32_t ordinal=0;
    if(!rr::game::advanceOriginalUiTextureAnimation50KG(next,leafTime.current230,ordinal)||
       !aggregate->frame102050(static_cast<std::int32_t>(ordinal)))return fail(d,E::arithmetic,0x29A020);
    rr::game::OriginalApplicationAnimationClockSample50KG leafCommit;
    if(!clock.read(leafCommit))return fail(d,E::phase,0x29A16C);
    next.previousMilliseconds=leafCommit.current230;
    prototype->state_=next;d.sourceEffectsCommitted=true;
    rr::game::OriginalApplicationAnimationClockSample50KG controllerCommit;
    if(!clock.read(controllerCommit))return fail(d,E::phase,0x29AD48);
    prototype->stamp08_=controllerCommit.current230; //29AD58 fresh owner read.
    d.sourcePC=0x29AD58;d.pass=slot;d.map=0;return true;
}
bool OriginalStandardMaterialObject50LA::selectScreenPrototype24B278(std::size_t slot,std::size_t map,
    const OriginalStandardTextureResource50LA& resource,const OriginalStandardTextureResource50LA*& selected,D& d) const noexcept {
    return selectTexturePrototype29AF90(slot,map,resource,selected,d);
}
bool OriginalStandardMaterialObject50LA::selectModelPrototype24B26C(std::size_t slot,std::size_t map,
    const OriginalStandardTextureResource50LA& resource,const OriginalStandardTextureResource50LA*& selected,D& d) const noexcept {
    selected=nullptr;
    if(!storage_||!storage_->constructed||!storage_->templates||slot>=static_cast<std::size_t>(storage_->count48)||!storage_->passes4C[slot]||
       storage_->passes4C[slot]->constructionTemplate!=storage_->templates->get(OriginalUiTemplate50KG::defaultMt))
        return fail(d,E::source,0x24B26C);
    return selectTexturePrototype29AF90(slot,map,resource,selected,d);
}
bool OriginalStandardMaterialObject50LA::selectTexturePrototype29AF90(std::size_t slot,std::size_t map,
    const OriginalStandardTextureResource50LA& resource,const OriginalStandardTextureResource50LA*& selected,D& d) const noexcept {
    selected=nullptr;
    if(!storage_||!storage_->constructed||slot>=static_cast<std::size_t>(storage_->count48)||
       !storage_->passes4C[slot]||!storage_->construction||!storage_->construction->live())
        return fail(d,E::phase,0x24B278);
    const auto& pass=*storage_->passes4C[slot];
    if(map>=pass.mapCount20)return fail(d,E::range,0x24B254);
    if(!resource.animated102070()){
        if(pass.textureAnimation10)return fail(d,E::texture,0x29A10C);
        selected=&resource;return true; //Ordinary29C6D0.
    }
    if(map!=0||!pass.textureAnimation10||pass.textureAnimation10->materialOwner()!=this||
       pass.textureAnimation10->passOrdinal()!=slot||
       !pass.textureAnimation10->select29AF90(resource,selected))
        return fail(d,E::texture,0x102050);
    return true;
}
bool OriginalStandardMaterialObject50LA::demandTournamentMarker2DF8A0(
    const OriginalStandardTextureTable50LA& expectedTextures,const OriginalUiMemberView50KG& psm,
    const OriginalUiMemberView50KG& atm,D& d) const noexcept {
    if(!storage_||!storage_->constructed)return fail(d,E::missingMaterialObject,0x2DF8BC);
    const auto& s=*storage_;
    if(!s.construction||!s.construction->live())return fail(d,E::video,0x2DF8A0);
    if(s.textures.get()!=&expectedTextures||s.count48!=1||!s.passes4C[0]||
       !s.source||!s.source->originalBytes()||!psm.data||!atm.data)return fail(d,E::source,0x2DF8BC);
    const auto& psmBytes=*s.source->originalBytes();
    if(psmBytes.size()!=psm.size||std::memcmp(psmBytes.data(),psm.data,psm.size))return fail(d,E::source,0x2DF8BC);
    const auto& pass=*s.passes4C[0];
    if(pass.mapCount20!=1||pass.texture1A[0]<0||pass.animation10||!pass.textureAnimation10||
       pass.textureAnimation10->materialOwner()!=this||pass.textureAnimation10->passOrdinal()!=0)
        return fail(d,E::unsupportedPass,0x2DF8C4);
    OriginalStandardTextureResource50LA* aggregate=nullptr;
    if(!s.textures->resolve298E60(pass.texture1A[0],aggregate,d.texture))return fail(d,E::texture,d.texture.sourcePC);
    const auto* bytes=aggregate?aggregate->originalBytes():nullptr;
    if(!aggregate||!aggregate->animated102070()||!bytes||bytes->size()!=atm.size||
       std::memcmp(bytes->data(),atm.data,atm.size))return fail(d,E::source,0x2DF8D0);
    bool written=false;
    if(!aggregate->demandTournamentMarker1F264C(written,d.texture))return fail(d,E::texture,d.texture.sourcePC);
    d.sourcePC=0x1F264C;d.pass=0;d.map=0;d.demandMapsVisited=1;d.texturesDemanded=1;
    d.textureDemandWritten=written;d.sourceEffectsCommitted=written;return true;
}
bool OriginalStandardMaterialObject50LA::demandTournamentOutline2DF8A0(
    const OriginalStandardTextureTable50LA& expectedTextures,const OriginalUiMemberView50KG& psm,
    const OriginalUiMemberView50KG& pst,D& d) const noexcept {
    if(!storage_||!storage_->constructed)return fail(d,E::missingMaterialObject,0x2DF8BC);
    const auto& s=*storage_;
    if(!s.construction||!s.construction->live())return fail(d,E::video,0x2DF8A0);
    if(s.textures.get()!=&expectedTextures||s.count48!=1||!s.passes4C[0]||
        !s.source||!s.source->originalBytes()||!psm.data||!pst.data)
        return fail(d,E::source,0x2DF8BC);
    const auto& bytes=*s.source->originalBytes();
    if(bytes.size()!=psm.size||std::memcmp(bytes.data(),psm.data,psm.size)!=0)
        return fail(d,E::source,0x2DF8BC);
    const auto& pass=*s.passes4C[0];
    if(pass.mapCount20!=1||pass.texture1A[0]<0||pass.animation10||pass.textureAnimation10)
        return fail(d,E::unsupportedPass,0x2DF8C4);
    OriginalStandardTextureResource50LA* texture=nullptr;
    if(!s.textures->resolve298E60(pass.texture1A[0],texture,d.texture))
        return fail(d,E::texture,d.texture.sourcePC);
    // Source dereferences this result. A pending row cannot certify the hook.
    if(!texture)return fail(d,E::texture,0x2DF8D0);
    const auto* original=texture->originalBytes();
    if(!original||original->size()!=pst.size||std::memcmp(original->data(),pst.data,pst.size)!=0)
        return fail(d,E::source,0x2DF8D0);
    bool written=false;
    if(!texture->demandTournamentOutline1F2530(written,d.texture))
        return fail(d,E::texture,d.texture.sourcePC);
    d.pass=0;d.map=0;d.demandMapsVisited=1;d.texturesDemanded=1;
    d.textureDemandWritten=written;d.sourceEffectsCommitted=written;return true;
}
bool OriginalStandardMaterialObject50LA::demandSharedWarning2DF8A0(
    const OriginalStandardTextureTable50LA& expected,const OriginalUiMemberView50KG& psm,
    const OriginalUiMemberView50KG& pst,D& d) const noexcept {
    if(!storage_||!storage_->constructed)return fail(d,E::missingMaterialObject,0x2DF8BC);
    const auto& s=*storage_;
    if(!s.construction||!s.construction->live()||s.textures.get()!=&expected)
        return fail(d,E::source,0x2DF8A0);
    if(!psm.identity||!psm.data||!pst.identity||!pst.data||!s.source||!s.source->originalBytes())
        return fail(d,E::source,0x2DF8BC);
    const auto& original=*s.source->originalBytes();
    if(original.size()!=psm.size||std::memcmp(original.data(),psm.data,psm.size))
        return fail(d,E::source,0x2DF8BC);
    if(s.count48!=1||!s.passes4C[0])return fail(d,E::unsupportedPass,0x2DF8BC);
    const auto& pass=*s.passes4C[0];
    if(pass.mapCount20!=1||pass.texture1A[0]<0||pass.animation10||pass.textureAnimation10)
        return fail(d,E::unsupportedPass,0x2DF8C4);
    OriginalStandardTextureResource50LA* texture=nullptr;
    if(!s.textures->resolve298E60(pass.texture1A[0],texture,d.texture))
        return fail(d,E::texture,d.texture.sourcePC);
    d.pass=0;d.map=0;d.demandMapsVisited=1;
    // Unlike the Garage caller,204A5C explicitly skips a null return.
    if(!texture){d.nullTexturesSkipped=1;d.sourcePC=0x204A5C;return true;}
    const auto* bytes=texture->originalBytes();
    if(!bytes||bytes->size()!=pst.size||std::memcmp(bytes->data(),pst.data,pst.size))
        return fail(d,E::source,0x2DF8D0);
    bool written=false;
    if(!texture->demandSharedWarning204A84(written,d.texture))return fail(d,E::texture,d.texture.sourcePC);
    d.texturesDemanded=1;d.textureDemandWritten=written;d.sourceEffectsCommitted=written;
    d.sourcePC=0x204A84;return true;
}
bool OriginalStandardMaterialObject50LA::demandGarage2DF8A0(
    const OriginalStandardTextureTable50LA& expected,std::uint32_t pc,D& d) const noexcept {
    if(pc!=0x1FDDA8&&pc!=0x1FDDE8)return fail(d,E::source,pc);
    if(!storage_||!storage_->constructed)return fail(d,E::missingMaterialObject,0x2DF8BC);
    const auto& s=*storage_;
    if(!s.construction||!s.construction->live()||s.textures.get()!=&expected)
        return fail(d,E::source,0x2DF8A0);
    if(s.count48<1||!s.passes4C[0])return fail(d,E::unsupportedPass,0x2DF8BC);
    const auto& pass=*s.passes4C[0];
    if(pass.mapCount20<1||pass.texture1A[0]<0)return fail(d,E::unsupportedPass,0x2DF8C4);
    OriginalStandardTextureResource50LA* texture=nullptr;
    if(!s.textures->resolve298E60(pass.texture1A[0],texture,d.texture))
        return fail(d,E::texture,d.texture.sourcePC);
    if(!texture)return fail(d,E::texture,0x2DF8D0);
    bool written=false;
    if(!texture->demandGarageFloor1FDD60(pc,written,d.texture))
        return fail(d,E::texture,d.texture.sourcePC);
    d.sourcePC=pc;d.pass=0;d.map=0;d.demandMapsVisited=1;d.texturesDemanded=1;
    d.textureDemandWritten=written;d.sourceEffectsCommitted=written;return true;
}
bool OriginalStandardMaterialObject50LA::demandTournamentFallback2DF8A0(
    const OriginalStandardTextureTable50LA& expectedTextures,const OriginalUiArchive50KG& archive,
    std::uint32_t storePC,D& d) const noexcept {
    try {
    if(!storage_||!storage_->constructed)return fail(d,E::missingMaterialObject,0x2DF8BC);
    const auto& s=*storage_;
    if(!s.construction||!s.construction->live())return fail(d,E::video,0x2DF8A0);
    if(s.textures.get()!=&expectedTextures||s.tournamentMissingPsm.get()!=&archive||
        !s.tournamentFallback||s.source||s.count48!=1||!s.passes4C[0])
        return fail(d,E::source,0x2DF8BC);
    const auto& proof=s.tournamentFallback->provenance;
    const auto origin=proof.origin;
    bool caller=false;
    if(origin==OriginalUiMaterialOrigin50KG::tournamentPortraitMissingPsmFallback)
        caller=storePC==0x1F24FC;
    else if(origin==OriginalUiMaterialOrigin50KG::tournamentNotchMissingPsmFallback)
        caller=storePC==0x1F2690;
    else if(origin==OriginalUiMaterialOrigin50KG::tournamentTrophyMissingPsmFallback){
        const auto& name=s.tournamentFallback->passes[0].textureName;
        const bool icon=name=="trphgldi"||name=="trphslvi"||name=="trphbrzi";
        const bool award=name=="trphglda"||name=="trphslva"||name=="trphbrza";
        caller=(icon&&storePC==0x1F2564)||(award&&
            (storePC==0x1F25AC||storePC==0x1F25E4||storePC==0x1F261C));
    }
    if(!caller||proof.archive!=archive.identity()||archive.find(s.name00).identity)
        return fail(d,E::source,0x2DF8BC);
    const auto pst=archive.find(proof.registeredName+".pst");
    if(!pst.identity||pst.identity!=proof.fallbackTexture||!pst.data||pst.size!=pst.identity->bytes)
        return fail(d,E::source,0x2DF8D0);
    const auto& pass=*s.passes4C[0];
    if(pass.mapCount20!=1||pass.texture1A[0]<0||pass.animation10||pass.textureAnimation10)
        return fail(d,E::unsupportedPass,0x2DF8C4);
    OriginalStandardTextureResource50LA* texture=nullptr;
    if(!s.textures->resolve298E60(pass.texture1A[0],texture,d.texture))
        return fail(d,E::texture,d.texture.sourcePC);
    if(!texture)return fail(d,E::texture,0x2DF8D0);
    const auto* bytes=texture->originalBytes();
    if(!bytes||bytes->size()!=pst.size||std::memcmp(bytes->data(),pst.data,pst.size)!=0)
        return fail(d,E::source,0x2DF8D0);
    bool written=false;
    if(!texture->demandTournamentFallbackFloor10(storePC,written,d.texture))
        return fail(d,E::texture,d.texture.sourcePC);
    d.sourcePC=storePC;d.pass=0;d.map=0;d.demandMapsVisited=1;d.texturesDemanded=1;
    d.textureDemandWritten=written;d.sourceEffectsCommitted=written;return true;
    }catch(...){return fail(d,E::allocation,0x2DF8A0);}
}
bool OriginalStandardMaterialObject50LA::select29BFF0(D& d) noexcept {
    if(!storage_||!storage_->constructed)return fail(d,E::missingMaterialObject,0x29bff0u);
    auto& s=*storage_;
    if(!s.construction||!s.construction->live())return fail(d,E::video,0x29c014u);
    if(s.count48<0||s.count48>4)return fail(d,E::passCount,0x29c030u);
    for(std::int32_t n=0;n<s.count48;++n){
        d.pass=static_cast<std::size_t>(n);const auto* pass=s.passes4C[d.pass].get();
        if(!pass)return fail(d,E::phase,0x29c014u);
        // Actual246FA0 vtable3D2110 slot20=249580. All declared passes,
        // regardless of enabled21, program, collector, animation or draw state.
        for(std::size_t map=0;map<pass->mapCount20;++map){
            d.map=map;++d.demandMapsVisited;
            if(map>=pass->texture1A.size()||!s.textures)return fail(d,E::texture,0x2495a4u);
            //298E60 BLTZ returns null for EVERY negative token on this route;
            // selection does not interpret draw-only -2 RenderTarget semantics.
            if(pass->texture1A[map]<0){++d.nullTexturesSkipped;continue;}
            OriginalStandardTextureResource50LA* texture=nullptr;
            if(!s.textures->resolve298E60(pass->texture1A[map],texture,d.texture))
                return fail(d,E::texture,d.texture.sourcePC);
            if(!texture){++d.nullTexturesSkipped;continue;}
            bool written=false;
            if(!texture->demandSelection249580(written,d.texture))return fail(d,E::texture,d.texture.sourcePC);
            ++d.texturesDemanded;
            if(written){d.textureDemandWritten=true;d.sourceEffectsCommitted=true;}
        }
    }
    d.sourcePC=0x29c050u;return true;
}
bool OriginalStandardMaterialObject50LA::demand24A430(const float* sizes,std::uint8_t count,
    float projectedRadius,float scale,float applicationScale,bool& firstTexture,
    std::uint32_t& assertUv,D& d) const noexcept {
    if(!storage_||!storage_->constructed)return fail(d,E::missingMaterialObject,0x24A430);
    const auto& s=*storage_;
    if(!s.construction||!s.construction->live())return fail(d,E::video,0x24A430);
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
    return fail(d,E::arithmetic,0x24A430);
#endif
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!sizes||!count||projectedRadius<0.f||scale<0.f||
        !Math::ordinary(projectedRadius)||!Math::ordinary(scale)||!Math::ordinary(applicationScale))
        return fail(d,E::arithmetic,0x24A430);
    //24A4A0 has no stored result outside this call. If its numeric domain is
    //unsupported, keep that explicit and reject only if a nonnull texture
    //actually consumes the cap. Zero-map/null-texture paths have no arithmetic
    //output or texture store that could be fabricated by this unused value.
    float cap=0;
    const char* capReason=nullptr;
    const bool capSupported=Math::multiplyTowardZero(projectedRadius,applicationScale,cap,&capReason);
    for(std::int32_t n=0;n<s.count48;++n){
        d.pass=static_cast<std::size_t>(n);
        const auto* pass=s.passes4C[d.pass].get();
        if(!pass)return fail(d,E::phase,0x24A4AC);
        //The source does NOT test enabled bit21 or resolve a program here.
        for(std::size_t map=0;map<pass->mapCount20;++map){
            d.map=map;++d.demandMapsVisited;
            if(map>=pass->texture1A.size()||!s.textures)return fail(d,E::texture,0x24A4C8);
            OriginalStandardTextureResource50LA* texture=nullptr;
            if(!s.textures->resolve298E60(pass->texture1A[map],texture,d.texture))
                return fail(d,E::texture,d.texture.sourcePC);
            //24A4D4: a real pending row is skipped. Do not manufacture an image
            //or read its UV selector/assertion state before this null branch.
            if(!texture){++d.nullTexturesSkipped;continue;}
            if(!capSupported)return demandMultiplyFailure(d,0x24A4A0,projectedRadius,applicationScale,capReason);
            const std::uint8_t uv=pass->uv1E[map];
            if(!firstTexture){assertUv=1;firstTexture=true;d.sourceEffectsCommitted=true;}
            //Always retain the native bounds guard, even should a future real
            //debug-global writer disable the source assertion. No padding lane.
            if(uv>=count)return fail(d,E::range,assertUv?0x24A51C:0x24A574,"UV-size selector");
            const auto* width=texture->sourceWidth14();
            if(!width||*width<=0||!Math::ordinary(sizes[uv])||sizes[uv]<0.f)return fail(d,E::texture,0x24A578);
            //Source operation order: scale*uvSize, then *float(texture14).
            float scaledUv=0,request=0;
            const char* reason=nullptr;
            if(!Math::multiplyTowardZero(scale,sizes[uv],scaledUv,&reason))
                return demandMultiplyFailure(d,0x24A584,scale,sizes[uv],reason);
            if(!Math::multiplyTowardZero(scaledUv,static_cast<float>(*width),request,&reason))
                return demandMultiplyFailure(d,0x24A588,scaledUv,static_cast<float>(*width),reason);
            if(!(request<=cap))request=cap; //24A58C..5A0, preserve comparison order.
            bool written=false;
            if(!texture->demand104A00(request,written,d.texture))return fail(d,E::texture,d.texture.sourcePC);
            ++d.texturesDemanded;
            if(written){d.textureDemandWritten=true;d.sourceEffectsCommitted=true;}
        }
    }
    return true;
}
bool OriginalStandardMaterialObject50LA::construct29B550(std::int32_t row,const std::string& canonical,
    std::shared_ptr<const OriginalUiMaterialTemplates50KG> templates,
    const OriginalUiMaterialTemplateState50KG* association,
    const OriginalStandardMaterialPopulationInput50LA& input,
    std::shared_ptr<const OriginalUiArchive50KG> tournamentMissingPsm,
    std::shared_ptr<const OriginalUiArchive50KG> speedBoostMissingPsm,
    std::shared_ptr<const OriginalUiArchive50KG> damageMissingPsm,
    std::shared_ptr<const OriginalUiArchive50KG> pauseBackdropMissingPsm,
    std::shared_ptr<const OriginalGarageScreenFallback50KG> garageFallback,
    std::shared_ptr<const OriginalMemoryCardScreenFallback50KG> memoryCardFallback,D& d) noexcept {
    try {
        auto& s=*storage_;
        if(s.row40!=-1)return fail(d,E::phase,0x29B550);
        s.row40=row; //29B5C0; table has already performed actual298C80.
        s.name00=canonical;for(char& c:s.name00)c=lower(c);
        const auto dot=s.name00.find_last_of('.'); //1169D0 searches entire path.
        if(dot!=std::string::npos)s.name00.resize(dot);
        s.name00+=".psm";
        // Material name00 is the actual64-byte field preceding row40.
        if(!ascii(s.name00,63)||s.name00.find('/')!=std::string::npos)
            return fail(d,E::name,0x29B5F8,"material name00");
        if(!input.sources)return fail(d,E::source,0x29B628,"resolved original PSM sources");
        for(const auto& source:*input.sources){
            if(!source||!source->prepared()||!source->member()||!same(*source->member(),s.name00))continue;
            if(s.source)return fail(d,E::source,0x29B628,"ambiguous resolved PSM member");
            s.source=source;
        }
        if((tournamentMissingPsm&&speedBoostMissingPsm)||
           (damageMissingPsm&&(tournamentMissingPsm||speedBoostMissingPsm))||
           (pauseBackdropMissingPsm&&(tournamentMissingPsm||speedBoostMissingPsm||damageMissingPsm)))return fail(d,E::source,0x29B638);
        if(garageFallback||memoryCardFallback){
            if(s.source||tournamentMissingPsm||speedBoostMissingPsm||damageMissingPsm||pauseBackdropMissingPsm||
               (garageFallback&&memoryCardFallback))return fail(d,E::source,0x29B638,"one actual pending construction family");
            if(garageFallback&&(garageFallback->row_!=row||
               !garageFallback->current(s.name00,input.construction,input.actualTextures383400,&s.sharedIconConstruction)))
                return fail(d,E::source,0x29B638,"Garage canonical absent-PSM proof");
            if(memoryCardFallback&&(memoryCardFallback->row_!=row||
               !memoryCardFallback->captureAt29B638(s.name00,input.construction,input.actualTextures383400,input.actualPrograms382B70,s.memoryCardConstruction)))
                return fail(d,E::source,0x29B638,"actual MemoryCard first-X absent-PSM construction");
            if(!templates||association!=templates->get(OriginalUiTemplate50KG::screenMt)||
               !input.construction||!input.construction->live()||
               &input.construction->materialApplication()->templates()!=templates.get())
                return fail(d,E::templates,0x2475F8);
            s.templates=std::move(templates);s.programs=input.actualPrograms382B70;
            s.textures=input.actualTextures383400;s.construction=input.construction;
            s.garageFallback=std::move(garageFallback);
            s.count48=1;s.passes4C[0]=std::make_unique<Storage::Pass>();
            auto& pass=*s.passes4C[0];pass.constructionTemplate=association;
            if(!s.programs||!s.programs->prepared()||
               !s.programs->find298C80(association->microprogram,pass.program18,d.program))
                return fail(d,E::program,0x247620);
            pass.mapCount20=1; //247630; flags21 remain2 and UV1E remains0.
            if(!s.textures||!s.textures->prepared()||!s.textures->bindMaterialContext(s.construction,d.texture)||
               !s.textures->acquire299330AndRetain298A30(memoryCardFallback?memoryCardFallback->name_:s.garageFallback->name_,pass.texture1A[0],d.texture))
                return fail(d,E::texture,0x247648);
            const OriginalUiDatSection50KG noAttributes{}; //NULL document at247670, never a fabricated PSM.
            auto state=std::make_unique<OriginalStandardMaterialState50LA>();
            if(!singleState(noAttributes,*association,s.construction.get(),*state,d))return false;
            pass.state30=std::move(state);s.constructed=true;return true;
        }
        if(!s.source&&speedBoostMissingPsm){
            // Only a retained concrete Speed/Boost primitive reference can
            // attach this archive proof to the canonical row. No input flag.
            if(!templates||association!=templates->get(OriginalUiTemplate50KG::screenMt)||
                !input.construction||!input.construction->live()||
                &input.construction->materialApplication()->templates()!=templates.get())
                return fail(d,E::templates,0x2475F8);
            const auto& archive=*speedBoostMissingPsm;
            if(!archive.loaded()||archive.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)||
                archive.find(s.name00).identity)return fail(d,E::source,0x29B638);
            const std::string prefix="ui\\textures\\";
            if(s.name00.compare(0,prefix.size(),prefix)!=0||s.name00.size()<prefix.size()+5)
                return fail(d,E::name,0x29B638);
            const auto name=s.name00.substr(prefix.size(),s.name00.size()-prefix.size()-4);
            auto proof=std::make_unique<OriginalUiMaterial50KG>();OriginalUiMaterialDiagnostic50KG proofDiagnostic;
            bool proven=false;
            constexpr const char* images[]{"spdbost1","boost_a1","boost_a2","boost_a3","boost_a4",
                "boost_a5","boost_a6","boost_a7","boost_a8","boost_a9"};
            for(unsigned n=0;n<10;++n)if(name==images[n])
                proven=readOriginalSpeedBoostFallbackMaterial50LA(archive,
                    static_cast<OriginalSpeedBoostImage50LA>(n),*templates,OriginalUiTemplate50KG::screenMt,
                    input.construction->materialRuntime(),*proof,proofDiagnostic);
            if(!proven||proof->passes.size()!=1)return fail(d,E::source,0x29B638);
            s.templates=std::move(templates);s.programs=input.actualPrograms382B70;
            s.textures=input.actualTextures383400;s.construction=input.construction;
            s.speedBoostMissingPsm=std::move(speedBoostMissingPsm);s.speedBoostFallback=std::move(proof);
            s.count48=1;s.passes4C[0]=std::make_unique<Storage::Pass>();
            auto& pass=*s.passes4C[0];pass.constructionTemplate=association;
            if(!s.programs||!s.programs->prepared()||
                !s.programs->find298C80(association->microprogram,pass.program18,d.program))
                return fail(d,E::program,0x247620);
            pass.mapCount20=1; //247630; constructor flags21 stay2, UV1E stays0.
            if(!s.textures||!s.textures->prepared()||
                !s.textures->bindMaterialContext(s.construction,d.texture)||
                !s.textures->acquire299330AndRetain298A30(prefix+name,pass.texture1A[0],d.texture))
                return fail(d,E::texture,0x247648);
            //247670 passes NULL document. Empty attribute view supplies only
            //source defaults; it is never a PSM/source object or identity.
            const OriginalUiDatSection50KG noAttributes{};
            auto state=std::make_unique<OriginalStandardMaterialState50LA>();
            if(!singleState(noAttributes,*association,s.construction.get(),*state,d))return false;
            pass.state30=std::move(state); //247678. No parameters/animation path.
            s.constructed=true;return true;
        }
        if(!s.source&&damageMissingPsm){
            // Only a retained concrete Damage primitive reference can
            // attach this archive proof to the canonical row. No input flag.
            if(!templates||association!=templates->get(OriginalUiTemplate50KG::screenMt)||
                !input.construction||!input.construction->live()||
                &input.construction->materialApplication()->templates()!=templates.get())
                return fail(d,E::templates,0x2475F8);
            const auto& archive=*damageMissingPsm;
            if(!archive.loaded()||archive.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)||
                archive.find(s.name00).identity)return fail(d,E::source,0x29B638);
            const std::string prefix="ui\\textures\\";
            if(s.name00.compare(0,prefix.size(),prefix)!=0||s.name00.size()<prefix.size()+5)
                return fail(d,E::name,0x29B638);
            const auto name=s.name00.substr(prefix.size(),s.name00.size()-prefix.size()-4);
            auto proof=std::make_unique<OriginalUiMaterial50KG>();OriginalUiMaterialDiagnostic50KG proofDiagnostic;
            if(name!="dmgradar"||!readOriginalDamageFallbackMaterial50LA(archive,*templates,
                OriginalUiTemplate50KG::screenMt,input.construction->materialRuntime(),*proof,proofDiagnostic)||
                proof->passes.size()!=1)return fail(d,E::source,0x29B638);
            s.templates=std::move(templates);s.programs=input.actualPrograms382B70;
            s.textures=input.actualTextures383400;s.construction=input.construction;
            s.damageMissingPsm=std::move(damageMissingPsm);s.damageFallback=std::move(proof);
            s.count48=1;s.passes4C[0]=std::make_unique<Storage::Pass>();
            auto& pass=*s.passes4C[0];pass.constructionTemplate=association;
            if(!s.programs||!s.programs->prepared()||
                !s.programs->find298C80(association->microprogram,pass.program18,d.program))
                return fail(d,E::program,0x247620);
            pass.mapCount20=1; //247630; constructor flags21 stay2, UV1E stays0.
            if(!s.textures||!s.textures->prepared()||
                !s.textures->bindMaterialContext(s.construction,d.texture)||
                !s.textures->acquire299330AndRetain298A30(prefix+name,pass.texture1A[0],d.texture))
                return fail(d,E::texture,0x247648);
            //247670 passes NULL document. Empty attribute view supplies only
            //source defaults; it is never a PSM/source object or identity.
            const OriginalUiDatSection50KG noAttributes{};
            auto state=std::make_unique<OriginalStandardMaterialState50LA>();
            if(!singleState(noAttributes,*association,s.construction.get(),*state,d))return false;
            pass.state30=std::move(state); //247678. No parameters/animation path.
            s.constructed=true;return true;
        }
        if(!s.source&&pauseBackdropMissingPsm){
            // Only a retained concrete Pause backdrop primitive reference can
            // attach this archive proof to the canonical row. No input flag.
            if(!templates||association!=templates->get(OriginalUiTemplate50KG::screenMt)||
                !input.construction||!input.construction->live()||
                &input.construction->materialApplication()->templates()!=templates.get())
                return fail(d,E::templates,0x2475F8);
            const auto& archive=*pauseBackdropMissingPsm;
            if(!archive.loaded()||archive.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)||
                archive.find(s.name00).identity)return fail(d,E::source,0x29B638);
            const std::string prefix="ui\\textures\\";
            if(s.name00.compare(0,prefix.size(),prefix)!=0||s.name00.size()<prefix.size()+5)
                return fail(d,E::name,0x29B638);
            const auto name=s.name00.substr(prefix.size(),s.name00.size()-prefix.size()-4);
            auto proof=std::make_unique<OriginalUiMaterial50KG>();OriginalUiMaterialDiagnostic50KG proofDiagnostic;
            if((name!="results1"&&name!="results2")||!readOriginalPauseBackdropFallbackMaterial50LA(archive,
                name=="results1"?0:1,*templates,
                OriginalUiTemplate50KG::screenMt,input.construction->materialRuntime(),*proof,proofDiagnostic)||
                proof->passes.size()!=1)return fail(d,E::source,0x29B638);
            s.templates=std::move(templates);s.programs=input.actualPrograms382B70;
            s.textures=input.actualTextures383400;s.construction=input.construction;
            s.pauseBackdropMissingPsm=std::move(pauseBackdropMissingPsm);s.pauseBackdropFallback=std::move(proof);
            s.count48=1;s.passes4C[0]=std::make_unique<Storage::Pass>();
            auto& pass=*s.passes4C[0];pass.constructionTemplate=association;
            if(!s.programs||!s.programs->prepared()||
                !s.programs->find298C80(association->microprogram,pass.program18,d.program))
                return fail(d,E::program,0x247620);
            pass.mapCount20=1; //247630; constructor flags21 stay2, UV1E stays0.
            if(!s.textures||!s.textures->prepared()||
                !s.textures->bindMaterialContext(s.construction,d.texture)||
                !s.textures->acquire299330AndRetain298A30(prefix+name,pass.texture1A[0],d.texture))
                return fail(d,E::texture,0x247648);
            //247670 passes NULL document. Empty attribute view supplies only
            //source defaults; it is never a PSM/source object or identity.
            const OriginalUiDatSection50KG noAttributes{};
            auto state=std::make_unique<OriginalStandardMaterialState50LA>();
            if(!singleState(noAttributes,*association,s.construction.get(),*state,d))return false;
            pass.state30=std::move(state); //247678. No parameters/animation path.
            s.constructed=true;return true;
        }
        if(!s.source&&tournamentMissingPsm){
            // Only a retained concrete Tournament ScreenTexture reference can
            // attach this archive proof to the canonical row. No input flag.
            if(!templates||association!=templates->get(OriginalUiTemplate50KG::screenMt)||
                !input.construction||!input.construction->live()||
                &input.construction->materialApplication()->templates()!=templates.get())
                return fail(d,E::templates,0x2475F8);
            const auto& archive=*tournamentMissingPsm;
            if(!archive.loaded()||archive.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament)||
                archive.find(s.name00).identity)return fail(d,E::source,0x29B638);
            const std::string prefix="ui\\textures\\";
            if(s.name00.compare(0,prefix.size(),prefix)!=0||s.name00.size()<prefix.size()+5)
                return fail(d,E::name,0x29B638);
            const auto name=s.name00.substr(prefix.size(),s.name00.size()-prefix.size()-4);
            auto proof=std::make_unique<OriginalUiMaterial50KG>();OriginalUiMaterialDiagnostic50KG proofDiagnostic;
            bool proven=false;
            constexpr const char* trophies[]{"trphgldi","trphslvi","trphbrzi","trphglda","trphslva","trphbrza"};
            for(unsigned n=0;n<6;++n)if(name==trophies[n])
                proven=readOriginalUiTournamentTrophyFallbackMaterial50KG(archive,
                    static_cast<OriginalUiTournamentTrophy50KG>(n),*templates,OriginalUiTemplate50KG::screenMt,
                    input.construction->materialRuntime(),*proof,proofDiagnostic);
            if(name=="notch")proven=readOriginalUiTournamentNotchFallbackMaterial50KG(archive,*templates,
                OriginalUiTemplate50KG::screenMt,input.construction->materialRuntime(),*proof,proofDiagnostic);
            if(name.size()==7&&name.substr(2)=="_flrg"&&name[0]>='0'&&name[0]<='9'&&name[1]>='0'&&name[1]<='9'){
                const auto career=static_cast<std::uint8_t>((name[0]-'0')*10+name[1]-'0');
                proven=readOriginalUiTournamentPortraitFallbackMaterial50KG(archive,career,*templates,
                    OriginalUiTemplate50KG::screenMt,input.construction->materialRuntime(),*proof,proofDiagnostic);
            }
            if(!proven||proof->passes.size()!=1)return fail(d,E::source,0x29B638);
            s.templates=std::move(templates);s.programs=input.actualPrograms382B70;
            s.textures=input.actualTextures383400;s.construction=input.construction;
            s.tournamentMissingPsm=std::move(tournamentMissingPsm);s.tournamentFallback=std::move(proof);
            s.count48=1;s.passes4C[0]=std::make_unique<Storage::Pass>();
            auto& pass=*s.passes4C[0];pass.constructionTemplate=association;
            if(!s.programs||!s.programs->prepared()||
                !s.programs->find298C80(association->microprogram,pass.program18,d.program))
                return fail(d,E::program,0x247620);
            pass.mapCount20=1; //247630; constructor flags21 stay2, UV1E stays0.
            if(!s.textures||!s.textures->prepared()||
                !s.textures->bindMaterialContext(s.construction,d.texture)||
                !s.textures->acquire299330AndRetain298A30(prefix+name,pass.texture1A[0],d.texture))
                return fail(d,E::texture,0x247648);
            //247670 passes NULL document. Empty attribute view supplies only
            //source defaults; it is never a PSM/source object or identity.
            const OriginalUiDatSection50KG noAttributes{};
            auto state=std::make_unique<OriginalStandardMaterialState50LA>();
            if(!singleState(noAttributes,*association,s.construction.get(),*state,d))return false;
            pass.state30=std::move(state); //247678. No parameters/animation path.
            s.constructed=true;return true;
        }
        if(damageMissingPsm)return fail(d,E::source,0x29B638,"PSM contradicts Damage absence proof");
        if(pauseBackdropMissingPsm)return fail(d,E::source,0x29B638,"PSM contradicts Pause backdrop absence proof");
        if(speedBoostMissingPsm)return fail(d,E::source,0x29B638,"PSM contradicts Speed/Boost absence proof");
        if(tournamentMissingPsm)return fail(d,E::source,0x29B638,"PSM contradicts retained absence proof");
        if(!s.source)return fail(d,E::source,0x29B628,"unresolved PSM; missing-file fallback not established");
        s.templates=std::move(templates);s.programs=input.actualPrograms382B70;s.textures=input.actualTextures383400;
        s.construction=input.construction;
        if(!s.templates)return fail(d,E::templates,0x24714C);
        if(s.source->section("Prologue"))return fail(d,E::unsupportedAction,0x29B6D8,"Prologue");
        const auto* general=s.source->section("General");
        if(!general)return fail(d,E::document,0x29B700,"General");
        if(!integer(*general,"TotalShaderPasses",0,s.count48,d,0x29B718))return false;
        if(s.count48<1||s.count48>4)return fail(d,E::passCount,0x29B718,"native four-pass layout");
        //29B720..79C recognizes only the last literal backslash.
        const auto slash=s.name00.find_last_of('\\');
        const std::string prefix=slash==std::string::npos?std::string{}:s.name00.substr(0,slash+1);
        for(std::int32_t n=0;n<s.count48;++n){
            d.pass=static_cast<std::size_t>(n);
            auto& owned=s.passes4C[static_cast<std::size_t>(n)];
            owned=std::make_unique<Storage::Pass>(); //29B7C8 installs actual246FA0 owner.
            auto& pass=*owned;
            const auto* inherited=association;
            if(!inherited)inherited=s.templates->get(OriginalUiTemplate50KG::screenMt); //247160.
            if(!inherited||(inherited!=s.templates->get(OriginalUiTemplate50KG::defaultMt)&&
                inherited!=s.templates->get(OriginalUiTemplate50KG::screenMt)))return fail(d,E::templates,0x24714C);
            pass.constructionTemplate=inherited;
            const auto* section=s.source->section("ShaderPass_"+std::to_string(n+1));
            if(!section)return fail(d,E::document,0x24718C,"ShaderPass_N");
            if(!s.programs||!s.programs->prepared())return fail(d,E::program,0x2471BC,"actual fresh program382B70");
            std::string program;
            if(!string(*section,"microProgramName",inherited->microprogram,program,d,0x2471A8,63))return false;
            if(!s.programs->find298C80(program,pass.program18,d.program))return fail(d,E::program,0x2471BC,"microProgramName");
            std::int32_t value=1;
            if(!integer(*section,"EnableThisShader",1,value,d,0x2471DC))return false;
            pass.flags21=static_cast<std::uint8_t>((pass.flags21&~2u)|((static_cast<std::uint32_t>(value)&1u)<<1));
            if(!integer(*section,"totalTextureMaps",1,value,d,0x247214))return false;
            if(value<0||value>2)return fail(d,E::unsupportedPass,0x247214,"zero/one/two texture maps");
            pass.mapCount20=static_cast<std::uint8_t>(value);
            if(pass.mapCount20){
                for(std::size_t map=0;map<pass.mapCount20;++map){
                d.map=map;
                const char* textureKey=map?"TextureMap_2":"TextureMap_1";
                const char* uvKey=map?"TextureCoordinateIndex_2":"TextureCoordinateIndex_1";
                std::string texture;
                if(!string(*section,textureKey,"",texture,d,0x247260))return false;
                if(texture.empty()){
                    //13DA50 fallback uses original MATERIAL name, not PSM name.
                    auto begin=canonical.find_last_of("\\/");begin=begin==std::string::npos?0:begin+1;
                    if(begin==0&&canonical.size()>1&&canonical[1]==':')begin=2;
                    auto end=canonical.find_last_of('.');if(end==std::string::npos||end<begin)end=canonical.size();
                    texture=canonical.substr(begin,end-begin);
                }
                if(!ascii(texture,255)||same(texture,"RenderTarget")||texture.find_first_of("\\/.:\"")!=std::string::npos)
                    return fail(d,E::unsupportedPass,0x2472A8,textureKey);
                const auto full=prefix+texture;
                if(full.size()>255)return fail(d,E::name,0x2472D4,"texture registry name");
                if(!s.textures||!s.textures->prepared())return fail(d,E::texture,0x2472F0,"actual texture383400");
                if(!s.textures->bindMaterialContext(s.construction,d.texture))
                    return fail(d,E::texture,d.texture.sourcePC,"same actual texture construction context");
                if(!s.textures->acquire299330AndRetain298A30(full,pass.texture1A[map],d.texture))
                    return fail(d,E::texture,d.texture.sourcePC,"texture registry retain");
                if(!integer(*section,uvKey,0,value,d,0x247338))return false;
                //24734C stores through fixed s5, not incremented s1: map2's
                //selector overwrites byte1E and constructor byte1F stays0.
                pass.uv1E[0]=static_cast<std::uint8_t>(static_cast<std::uint32_t>(value)&255u);
                }
            }else{
                if(!integer(*section,"EnableFillColor",0,value,d,0x24737C))return false;
                pass.flags21=static_cast<std::uint8_t>((pass.flags21&~4u)|((static_cast<std::uint32_t>(value)&1u)<<2));
                if(pass.flags21&4u){
                    constexpr const char* names[]={"FillColorAlpha","FillColorRed","FillColorGreen","FillColorBlue"};
                    constexpr std::size_t indexes[]={3,0,1,2};
                    for(std::size_t c=0;c<4;++c){
                        if(!integer(*section,names[c],0,value,d,0x2473C8+static_cast<std::uint32_t>(c*24)))return false;
                        if(value<0||value>255)return fail(d,E::unsupportedPass,0x247418,names[c]);
                        pass.fillRgba2C[indexes[c]]=static_cast<std::uint8_t>(value);
                    }
                }
            }
            //247444..480: only count==2 sets bit0 and selects2480D0.
            const bool dual=pass.mapCount20==2;
            pass.flags21=static_cast<std::uint8_t>((pass.flags21&~1u)|(dual?1u:0u));
            OriginalStandardMaterialParameters50LA parameters;
            if(!parameters249390(*section,*s.programs,parameters,d))return false;
            pass.parameterCount28=static_cast<std::int32_t>(parameters.size()); //247484.
            auto state=std::make_unique<OriginalStandardMaterialState50LA>();
            if(dual){if(!dualState2480D0(*section,*inherited,s.construction.get(),*state,d))return false;}
            else if(!singleState(*section,*inherited,s.construction.get(),*state,d))return false;
            pass.state30=std::move(state); //2474AC/2474C0, before animation.
            //2474C8 ->29B890 visits texture, vertex, then pixel leaves before
            //249420 may create parameters34. Even an unsupported non-vertex
            //leaf must stop here, without publishing those later seed effects.
            const auto* textureAnimation=section->find("TextureAnimation");
            if(textureAnimation&&!same(textureAnimation->text,"none")){
                const auto* vertexLeaf=section->find("VertexAnimation");
                const auto* pixelLeaf=section->find("PixelAnimation");
                //29B890/29B950 constructs one prototype per actual pass;
                //neither the leaf nor29AAA0 depends on the draw program.
                //Preserve the existing ScreenTexture subset, and admit the
                //proven single-map DefaultMT model programs independently of
                //material pass count (e.g. finishline's second Diffuse pass).
                const bool screenPrototype=s.count48==1&&same(program,"ScreenObject")&&
                    inherited==s.templates->get(OriginalUiTemplate50KG::screenMt);
                const bool modelPrototype=inherited==s.templates->get(OriginalUiTemplate50KG::defaultMt)&&
                    (same(program,"PrelitCompressed")||same(program,"Diffuse"));
                if(!same(textureAnimation->text,"unique")||pass.mapCount20!=1||
                   (!screenPrototype&&!modelPrototype)||
                   (vertexLeaf&&!same(vertexLeaf->text,"none"))||
                   (pixelLeaf&&!same(pixelLeaf->text,"none"))||!parameters.empty())
                    return fail(d,E::unsupportedPass,0x29B950,"bounded per-pass texture prototype");
                //This constructs no model registration table or callback.
                //Actual model draw/service must join its retained material
                //slot and this pass before using the Application clock.
                //Constructed material state alone is not animated draw readiness.
                auto animation=std::make_unique<OriginalStandardMaterialTextureAnimation50LA>();
                OriginalStandardMaterialAnimationDiagnostic50LA animationDiagnostic;
                if(!animation->construct29B950(*section,*this,static_cast<std::size_t>(n),animationDiagnostic)){
                    d.document=animationDiagnostic.document;
                    return fail(d,E::unsupportedPass,animationDiagnostic.sourcePC,animationDiagnostic.field);
                }
                pass.textureAnimation10=std::move(animation);
            }
            const auto* vertex=section->find("VertexAnimation");
            if(vertex&&!same(vertex->text,"none")){
                // Only the concrete bounded unique owner can admit this path.
                // There is no no-op fallback or caller-supplied success flag.
                auto animation=std::make_unique<OriginalStandardMaterialAnimation50LA>();
                OriginalStandardMaterialAnimationDiagnostic50LA animationDiagnostic;
                if(!animation->construct29B890(*section,parameters,*s.programs,animationDiagnostic)){
                    d.document=animationDiagnostic.document;
                    return fail(d,E::unsupportedPass,animationDiagnostic.sourcePC,animationDiagnostic.field);
                }
                pass.animation10=std::move(animation);
            }
            const auto* pixelAnimation=section->find("PixelAnimation");
            if(pixelAnimation&&!same(pixelAnimation->text,"none"))
                return fail(d,E::unsupportedPass,0x29BBD0,"PixelAnimation");
            if(pass.parameterCount28){
                //249420 owns immutable seed rows independently of animation.
                for(const auto& parameter:parameters)
                    pass.parameterMask24|=std::uint32_t{1}<<parameter.row382B78;
                pass.parameters34=std::make_unique<const OriginalStandardMaterialParameters50LA>(std::move(parameters));
            }
            //2474F0 calls247530 AFTER parameters34 publication;2474F8 stores
            //its signed result. Unique is the literal -2 nonmerge sentinel,
            //not a named group or a per-primitive material allocation.
            std::string group;
            if(!string(*section,"ShaderPassGroup","none",group,d,0x247550,127))return false;
            if(same(group,"none"))pass.group22=-1;
            else if(same(group,"unique"))pass.group22=-2;
            else if(!OriginalShaderPassGroups50LA::instance().acquire299330(group,pass.group22))
                return fail(d,E::unsupportedPass,0x2475AC,"named ShaderPassGroup index capacity");
            if(!admitPass(*section,dual,pass.animation10!=nullptr,pass.textureAnimation10!=nullptr,*s.programs,d))return false;
        }
        if(s.source->section("Epilogue"))return fail(d,E::unsupportedAction,0x29B828,"Epilogue");
        s.constructed=true;return true; //Table performs actual29B854/14D3FC publication.
    }catch(const std::bad_alloc&){return fail(d,E::allocation,0x29B7AC);}
    catch(...){return fail(d,E::input,0x29B550);}
}
}
