#include "original_world_environment50kg.h"
#include "../assets/track_start50ad.h"
#include <cmath>
#include <cfenv>
#include <optional>
#include <string_view>
#include <limits>
#include <utility>
#include <vector>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "EnvironmentInfo bounds require strict finite IEEE arithmetic"
#endif

namespace rr::game::original_world_environment50KG {
namespace {
// Native bounded admission, not an original fixed sky-cube capacity.
constexpr std::uint32_t maximumCubes=1024;
constexpr std::size_t sourceFilenameBytes=0x104;
bool fail(Diagnostic& d,Status s,std::uint32_t pc,std::size_t offset=0) noexcept {
    d.status=s;d.sourcePC=pc;d.sourceOffset=offset;return false;
}
bool field(const assets::ParameterBlock48F& b,std::string_view name,
    const assets::ParameterField48F*& out,Diagnostic& d,std::uint32_t pc) noexcept {
    out=nullptr;
    for(const auto& f:b.fields)if(f.name==name){
        if(out)return fail(d,Status::duplicateField,pc,f.entry_offset);
        out=&f;
    }
    return true;
}
// Deliberately bounded admitted integer spelling: nonnegative decimal, with
// optional leading '+', no leading zero on multi-digit values. These spellings
// agree with the original base-zero reader; unseen negative/octal/hex/annotated
// forms reject rather than claiming full158B30/158920 malformed-input parity.
bool integer(const assets::ParameterField48F* f,std::int32_t fallback,
    std::int32_t& out,Diagnostic& d,std::uint32_t pc) noexcept {
    if(!f){out=fallback;return true;}
    const auto& text=f->value;
    std::size_t at=!text.empty()&&text.front()=='+'?1u:0u;
    if(at==text.size()||(text[at]=='0'&&at+1u<text.size()))
        return fail(d,Status::integerSyntax,pc,f->value_offset);
    std::uint32_t value=0;
    for(;at<text.size();++at){
        const auto c=text[at];
        if(c<'0'||c>'9')return fail(d,Status::integerSyntax,pc,f->value_offset);
        const auto digit=static_cast<std::uint32_t>(c-'0');
        const auto limit=static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max());
        if(value>(limit-digit)/10u)return fail(d,Status::integerSyntax,pc,f->value_offset);
        value=value*10u+digit;
    }
    out=static_cast<std::int32_t>(value);return true;
}
}
struct Owner::Storage {
    // All source inputs remain private for this owner's whole lifetime.
    assets::World48F world;
    std::vector<std::string> cubeFiles;
    std::vector<std::int32_t> zoneIndices;
    std::optional<SpatialBounds> spatialBounds;
};
Owner::Owner() noexcept=default;
Owner::~Owner()=default;
bool Owner::prepare(const assets::World48F& source,Diagnostic& d) noexcept {
    d={};
    if(storage_)return fail(d,Status::alreadyPrepared,0x270a68);
    try {
        auto next=std::make_unique<Storage>();
        next->world=source;
        const auto& w=next->world;
        std::vector<std::uint8_t> encoded;
        if(w.parameters.original_bytes.empty()||
           !assets::encodeParameters48F(w.parameters,encoded,d.decode)||
           encoded!=w.parameters.original_bytes||
           !assets::validateWorldSemantics48F(w,d.decode))
            return fail(d,Status::sourceShape,0x270a84);
        if(w.zones.empty()||w.zones.size()>256u)
            return fail(d,Status::storageLimit,0x270a84);
        for(std::size_t i=0;i<w.zones.size();++i)
            if(w.zones[i].zone!=i+1u)return fail(d,Status::sourceShape,0x270a84);
        const assets::ParameterBlock48F* environment=nullptr;
        for(const auto& b:w.parameters.blocks)if(b.name=="environment"){
            if(environment)return fail(d,Status::sourceShape,0x276c2c,b.name_offset);
            environment=&b;
        }
        if(!environment)return fail(d,Status::missingEnvironment,0x276c2c);
        const assets::ParameterField48F* f=nullptr;
        std::int32_t count=0;
        if(!field(*environment,"numberofskycubes",f,d,0x276c54)||
           !integer(f,1,count,d,0x276c54))return false;
        if(static_cast<std::uint32_t>(count)>maximumCubes)
            return fail(d,Status::storageLimit,0x276c68,f?f->value_offset:environment->offset);
        //276C60 jumps over BOTH allocations/zone reads for count<=0. Negative
        // source counts are outside this admitted domain, never clamped here.
        if(count>0){
            next->cubeFiles.reserve(static_cast<std::size_t>(count));
            for(std::int32_t i=0;i<count;++i){
                const auto key="cubefile"+std::to_string(i+1);
                if(!field(*environment,key,f,d,0x276cd0))return false;
                //276CD8..276D1C: ONLY missing first numbered field falls back.
                // A present literal 'none' is not a failed read.
                if(!f&&i==0&&!field(*environment,"cubefile",f,d,0x276d18))return false;
                const std::string name=f?f->value:"none";
                if(name.size()>=sourceFilenameBytes||name.find('\0')!=std::string::npos)
                    return fail(d,Status::storageLimit,0x276c7c,f?f->value_offset:environment->offset);
                next->cubeFiles.push_back(name);
            }
            next->zoneIndices.reserve(w.zones.size());
            for(std::size_t i=0;i<w.zones.size();++i){
                const auto key="cubefileforzone"+std::to_string(i+1u);
                std::int32_t authored=0;
                if(!field(*environment,key,f,d,0x276d84)||
                   !integer(f,1,authored,d,0x276d84))return false;
                if(authored>count)return fail(d,Status::sourceRange,0x276ddc,
                    f?f->value_offset:environment->offset);
                next->zoneIndices.push_back(authored-1); //276E24/276E28.
            }
            //276E40..276E88 runs AFTER the zone array is initialized.
            if(count==1&&next->cubeFiles.front()=="none")next->cubeFiles.clear();
        }
        storage_=std::move(next);d.status=Status::complete;return true;
    }catch(...){return fail(d,Status::allocation,0x276c68);}
}
bool Owner::prepareSpatialBounds276F3C(Diagnostic& d) noexcept {
    d={};
    if(!storage_)return fail(d,Status::unprepared,0x276f3c);
    if(storage_->spatialBounds)return fail(d,Status::alreadyPrepared,0x276f3c);
    if(std::fegetround()!=FE_TONEAREST)return fail(d,Status::arithmetic,0x277014);
    const assets::ParameterBlock48F* environment=nullptr;
    for(const auto& b:storage_->world.parameters.blocks)if(b.name=="environment")environment=&b;
    if(!environment)return fail(d,Status::missingEnvironment,0x276c2c);
    SpatialBounds next;
    auto read=[&](const char* name,float& value,std::uint32_t pc,bool& present)->bool {
        const assets::ParameterField48F* f=nullptr;
        if(!field(*environment,name,f,d,pc))return false;
        present=bool(f);value=0.f; // Every source158B80 default here is zero.
        if(!f)return true;
        if(!assets::parseTrackNumber50AD(f->value.data(),f->value.size(),value)||!std::isfinite(value))
            return fail(d,Status::numberSyntax,pc,f->value_offset);
        //The bounded decimal leaf canonicalizes zero. Restore the reader's
        //signed zero rather than changing an explicit negative-zero override.
        if(value==0.f&&!f->value.empty()&&f->value.front()=='-')value=-0.f;
        return true;
    };
    bool present=false;
    if(!read("terrainwidth",next.terrainWidth520,0x276f4c,present)||
       !read("terrainbreadth",next.terrainBreadth524,0x276f64,present)||
       !read("terrainwidthscale",next.terrainWidthScale528,0x276f7c,present)||
       !read("terrainbreadthscale",next.terrainBreadthScale52C,0x276f94,present)||
       !read("terrainborderwidth",next.border530,0x276fac,present))return false;
    if(!read("xmin",next.xMin534,0x276fc4,present))return false;
    if(!present)next.xMin534=-next.border530;
    if(!read("xmax",next.xMax538,0x276ff0,present))return false;
    if(!present){
        volatile float extent=next.terrainWidth520*next.terrainWidthScale528;
        volatile float scaled=256.f*extent;
        volatile float maximum=next.border530+scaled;
        next.xMax538=maximum; //277014..277020, never fused or reassociated.
    }
    if(!read("zmin",next.zMin53C,0x277034,present))return false;
    if(!present)next.zMin53C=-next.border530;
    if(!read("zmax",next.zMax540,0x277060,present))return false;
    if(!present){
        volatile float extent=next.terrainBreadth524*next.terrainBreadthScale52C;
        volatile float scaled=256.f*extent;
        volatile float maximum=next.border530+scaled;
        next.zMax540=maximum;
    }
    volatile float width=next.xMax538-next.xMin534;
    volatile float breadth=next.zMax540-next.zMin53C;
    //2770D4/27714C convert each extent to signed integer for diagnostics.
    //Admit only ordinary positive, safely representable extents. No clamping,
    //fallback box, suppressed assertion, or native spatial-tree claim.
    if(!std::isfinite(next.xMin534)||!std::isfinite(next.xMax538)||
       !std::isfinite(width)||width<1.f||static_cast<double>(width)>2147483647.0)
        return fail(d,Status::sourceRange,0x2770d4);
    if(!std::isfinite(next.zMin53C)||!std::isfinite(next.zMax540)||
       !std::isfinite(breadth)||breadth<1.f||static_cast<double>(breadth)>2147483647.0)
        return fail(d,Status::sourceRange,0x27714c);
    storage_->spatialBounds=next;d.status=Status::complete;return true;
}
const SpatialBounds* Owner::spatialBounds534() const noexcept {
    return storage_&&storage_->spatialBounds?&*storage_->spatialBounds:nullptr;
}
bool Owner::prepared() const noexcept{return bool(storage_);}
const assets::World48F* Owner::world() const noexcept{return storage_?&storage_->world:nullptr;}
std::size_t Owner::zoneCount() const noexcept{return storage_?storage_->world.zones.size():0u;}
std::uint32_t Owner::cubeCount() const noexcept {
    return storage_?static_cast<std::uint32_t>(storage_->cubeFiles.size()):0u;
}
const std::string* Owner::cubeFile(std::uint32_t index) const noexcept {
    return storage_&&index<storage_->cubeFiles.size()?&storage_->cubeFiles[index]:nullptr;
}
const std::int32_t* Owner::zoneCubeIndex(std::uint32_t zone) const noexcept {
    return storage_&&zone<storage_->zoneIndices.size()?&storage_->zoneIndices[zone]:nullptr;
}
bool Owner::selectCameraZone48(std::uint32_t zone,SkySelection& out,Diagnostic& d) const noexcept {
    d={};if(!storage_)return fail(d,Status::unprepared,0x275894);
    if(zone>=storage_->world.zones.size())return fail(d,Status::cameraZone,0x275894);
    SkySelection next{};next.cameraZone48=zone;
    if(!storage_->cubeFiles.empty()){
        if(zone>=storage_->zoneIndices.size())return fail(d,Status::sourceShape,0x275914);
        const auto index=storage_->zoneIndices[zone];
        if(index>=0){
            if(static_cast<std::size_t>(index)>=storage_->cubeFiles.size())
                return fail(d,Status::sourceRange,0x27591c);
            next.cubeIndex=index;next.enabled=true;
        }
    }
    out=next;d.status=Status::complete;return true;
}
} // namespace rr::game::original_world_environment50KG
