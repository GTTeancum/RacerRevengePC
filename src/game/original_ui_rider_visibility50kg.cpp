#include "original_ui_rider_visibility50kg.h"
#include "camera_view49f.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>

namespace rr::game {
namespace {
using Matrix=std::array<float,16>;
using Vector=std::array<float,4>;
using Error=OriginalUiRiderVisibilityError50KG;
using Diagnostic=OriginalUiRiderVisibilityDiagnostic50KG;
bool fail(Diagnostic& d,Error e,std::uint32_t pc) noexcept {d={e,pc};return false;}
bool arithmeticReady() noexcept {
    return sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&std::fegetround()==FE_TONEAREST;
}
struct Arithmetic {
    bool valid=true;
    float keep(float x) noexcept {if(!std::isfinite(x))valid=false;return x;}
    float add(float x,float y) noexcept {volatile float z=x+y;return keep(z);}
    float sub(float x,float y) noexcept {volatile float z=x-y;return keep(z);}
    float mul(float x,float y) noexcept {volatile float z=x*y;return keep(z);}
    float div(float x,float y) noexcept {
        if(y==0||!std::isfinite(x)||!std::isfinite(y)){valid=false;return 0;}
        volatile float z=x/y;return keep(z);
    }
};
bool finite(const Matrix& m) noexcept {for(float f:m)if(!std::isfinite(f))return false;return true;}
Matrix product(Arithmetic& a,const Matrix& x,const Matrix& y) noexcept {
    Matrix z{};
    for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){
        float f=a.mul(y[c],x[4*r]);
        f=a.add(f,a.mul(y[4+c],x[4*r+1]));
        f=a.add(f,a.mul(y[8+c],x[4*r+2]));
        z[4*r+c]=a.add(f,a.mul(y[12+c],x[4*r+3]));
    }
    return z;
}
Vector transform(Arithmetic& a,const Vector& p,const Matrix& m) noexcept {
    Vector out{};
    for(unsigned c=0;c<4;++c){
        float f=a.mul(m[c],p[0]);f=a.add(f,a.mul(m[4+c],p[1]));
        f=a.add(f,a.mul(m[8+c],p[2]));out[c]=a.add(f,a.mul(m[12+c],p[3]));
    }
    return out;
}
std::uint8_t clipMask(const Vector& p) noexcept {
    //2B0580's source clipping plane comparisons use the magnitude of W.
    // Negative W is not a separate invented reject plane. No division occurs.
    const float limit=std::fabs(p[3]);std::uint8_t mask=0;
    for(unsigned c=0;c<3;++c){
        if(p[c]>limit)mask=static_cast<std::uint8_t>(mask|(1u<<(2*c)));
        if(p[c]<-limit)mask=static_cast<std::uint8_t>(mask|(2u<<(2*c)));
    }
    return mask;
}
Matrix matrix(const BodyMatrix48G& m) noexcept {Matrix out{};std::memcpy(out.data(),m.v,sizeof m.v);return out;}
bool widget(std::uint16_t id) noexcept {return id>0&&id<120;}
bool graphReady(const SceneGraph49P& graph,const assets::PsgHierarchyView49O& psg) noexcept {
    return psg.nodeCount>1&&graph.count==psg.nodeCount+1&&validateSceneGraph49P(graph)&&
        graph.nodes[0].parent180==SceneNodeIndex49P::none;
}
}

bool prepareOriginalUiRiderMembershipCamera50KG(const OriginalUiRiderCamera50KG& camera,
    OriginalUiRiderMembershipCamera50KG& output) noexcept {
    if(!arithmeticReady()||!camera.pageIdentity||!finite(camera.core.view))return false;
    OriginalUiRiderMembershipCamera50KG next{};
    PortalZoneViewOutput49F numeric;
    // Same source 280720 operation as gameplay zones, with this real flat UI
    // owner's full viewport. Retain UI identity and separate clip ownership.
    if(!composePortalZoneView280720(camera.core,next.viewport4C,numeric))return false;
    next.projection=numeric.projection;next.matrix60=numeric.matrix60;
    output=next;return true;
}

bool projectOriginalUiRiderBoxClip50KG(const Matrix& transformMatrix,const Vector& center,
    const Vector& extent,OriginalUiRiderBoxClip50KG& output) noexcept {
    if(!arithmeticReady()||!finite(transformMatrix))return false;
    for(float f:center)if(!std::isfinite(f))return false;
    // Source loads extent.W but operates on XYZ only; do not reinterpret W.
    for(unsigned c=0;c<3;++c)if(!std::isfinite(extent[c])||extent[c]<0)return false;
    Arithmetic a;Vector corner=center,doubled=extent;
    for(unsigned c=0;c<3;++c)corner[c]=a.sub(center[c],extent[c]);
    for(unsigned c=0;c<3;++c)doubled[c]=a.add(extent[c],extent[c]);
    OriginalUiRiderBoxClip50KG next{};next.intersection=0x3f;
    const auto visit=[&]() noexcept {
        const auto p=transform(a,corner,transformMatrix);const auto flags=clipMask(p);
        next.intersection=static_cast<std::uint8_t>(next.intersection&flags);
        next.unionMask=static_cast<std::uint8_t>(next.unionMask|flags);
    };
    visit();
    corner[0]=a.add(corner[0],doubled[0]);visit();
    corner[2]=a.add(corner[2],doubled[2]);visit();
    corner[0]=a.sub(corner[0],doubled[0]);visit();
    corner[1]=a.add(corner[1],doubled[1]);visit();
    corner[0]=a.add(corner[0],doubled[0]);visit();
    corner[2]=a.sub(corner[2],doubled[2]);visit();
    corner[0]=a.sub(corner[0],doubled[0]);visit();
    if(!a.valid)return false;
    next.outside=next.intersection!=0;output=next;return true;
}

struct OriginalUiRiderVisibilityOwner50KG::Storage {
    struct Entry {
        std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> characterAssets;
        std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> podAssets;
        OriginalUiRiderCharacterScene50KG* character=nullptr;
        OriginalUiRiderPodScene50KG* pod=nullptr;
        std::uint64_t generation=0,selectionSerial=0;
        std::uint8_t flags03=3,flags191=0,flags1B2=0;
        bool registered=false;
        OriginalUiRiderModelSelection50KG lastSelection{};
    };
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    const assets::OriginalUiMemberIdentity50KG* pageIdentity=nullptr;
    const assets::OriginalUiMemberIdentity50KG* widgetIdentity=nullptr;
    std::array<Entry,120> entries{};
    OriginalUiRiderMembershipCamera50KG membershipCamera{};
    OriginalUiRiderCamera50KG camera{};
    OriginalUiRiderClipCamera50KG guardCamera{};
    OriginalUiRiderVisibilityPolicy50KG policy{};
    std::uint64_t nextGeneration=1,frameSerial=0;
    std::uint32_t count=0,processed278=0;
    std::uint16_t viewportEpochA4=0,searchEpochA6=0;
    bool frameReady=false;
};
OriginalUiRiderVisibilityOwner50KG::OriginalUiRiderVisibilityOwner50KG() noexcept=default;
OriginalUiRiderVisibilityOwner50KG::~OriginalUiRiderVisibilityOwner50KG()=default;
bool OriginalUiRiderVisibilityOwner50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyBound,0x1CCD44);
    if(!archive||!archive->loaded()||
        archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider))
        return fail(d,Error::input,0x1CCD44);
    try {
        auto next=std::make_unique<Storage>();
        next->pageIdentity=archive->find("UI/UIRider/rider.dat").identity;
        next->widgetIdentity=archive->find("UI/UIRider/Widgets.Dat").identity;
        if(!next->pageIdentity||!next->widgetIdentity)return fail(d,Error::input,0x1CCD44);
        next->archive=std::move(archive);storage_=std::move(next);return true;
    }
    catch(...){return fail(d,Error::allocation,0x1CCD34);}
}
bool OriginalUiRiderVisibilityOwner50KG::bindTournament(
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyBound,0x1CCD44);
    if(!archive||!archive->loaded()||
       archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::tournament))
        return fail(d,Error::input,0x1CCD44);
    try {auto next=std::make_unique<Storage>();
        next->pageIdentity=archive->find("UI/UITourn/standing.dat").identity;
        next->widgetIdentity=archive->find("UI/UITourn/Widgets.Dat").identity;
        if(!next->pageIdentity||!next->widgetIdentity)return fail(d,Error::input,0x1CCD44);
        next->archive=std::move(archive);storage_=std::move(next);return true;
    }catch(...){return fail(d,Error::allocation,0x1CCD34);}
}
bool OriginalUiRiderVisibilityOwner50KG::registerModel(
    std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> assets,
    OriginalUiRiderCharacterScene50KG& model,bool shown,OriginalUiRiderVisibilityHandle50KG& output,
    Diagnostic& d) noexcept {
    d={};if(!storage_||!assets||!assets->loaded()||!widget(assets->widgetId())||
        !assets->hierarchy()||assets->scene()!=model.scene()||!model.lifecycle()||
        assets->widgetSource()!=storage_->widgetIdentity||
        !graphReady(model.graph(),*assets->hierarchy()))return fail(d,Error::input,0x28E8A0);
    auto& s=*storage_;auto& e=s.entries[assets->widgetId()];
    if(e.registered||s.nextGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Error::registration,0x2809C0);
    const auto id=assets->widgetId();e={};e.characterAssets=std::move(assets);e.character=&model;
    e.registered=true;e.generation=s.nextGeneration++;
    e.flags03=shown?3:7;++s.count;output={this,e.generation,id};return true;
}
bool OriginalUiRiderVisibilityOwner50KG::registerModel(
    std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> assets,
    OriginalUiRiderPodScene50KG& model,bool shown,OriginalUiRiderVisibilityHandle50KG& output,
    Diagnostic& d) noexcept {
    d={};if(!storage_||!assets||!assets->loaded()||!widget(assets->widgetId())||
        !assets->hierarchy()||!model.loaded()||assets->scene()!=model.scene()||
        assets->widgetSource()!=storage_->widgetIdentity||
        !graphReady(model.graph(),*assets->hierarchy()))return fail(d,Error::input,0x28E8A0);
    auto& s=*storage_;auto& e=s.entries[assets->widgetId()];
    if(e.registered||s.nextGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,Error::registration,0x2809C0);
    const auto id=assets->widgetId();e={};e.podAssets=std::move(assets);e.pod=&model;
    e.registered=true;e.generation=s.nextGeneration++;
    e.flags03=shown?3:7;++s.count;output={this,e.generation,id};return true;
}
bool OriginalUiRiderVisibilityOwner50KG::contains(const OriginalUiRiderVisibilityHandle50KG& handle) const noexcept {
    return storage_&&handle.owner==this&&widget(handle.widget)&&
        storage_->entries[handle.widget].registered&&handle.generation!=0&&
        storage_->entries[handle.widget].generation==handle.generation;
}
bool OriginalUiRiderVisibilityOwner50KG::removeModel(const OriginalUiRiderVisibilityHandle50KG& handle) noexcept {
    if(!contains(handle))return false;
    storage_->entries[handle.widget]={};--storage_->count;return true;
}
bool OriginalUiRiderVisibilityOwner50KG::setShown(const OriginalUiRiderVisibilityHandle50KG& handle,bool shown) noexcept {
    if(!contains(handle))return false;
    auto& e=storage_->entries[handle.widget];
    const auto flags=static_cast<std::uint8_t>((e.flags03&~4u)|(shown?0u:4u));
    if(flags==e.flags03)return true;
    e.flags03=flags;
    // Invalidate an already-borrowed selection when its enable gate changes.
    // No191 store occurs until26EFC0 itself is visited.
    e.lastSelection={};return true;
}
std::uint32_t OriginalUiRiderVisibilityOwner50KG::registeredCount() const noexcept {
    return storage_?storage_->count:0;
}
bool OriginalUiRiderVisibilityOwner50KG::prepareFrame(const OriginalUiRiderCamera50KG& camera,
    const OriginalUiRiderVisibilityPolicy50KG& policy,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::input,0x27F650);
    auto& s=*storage_;s.frameReady=false;
    if(camera.pageIdentity!=s.pageIdentity||
        s.frameSerial==std::numeric_limits<std::uint64_t>::max())return fail(d,Error::camera,0x27F650);
    OriginalUiRiderMembershipCamera50KG membership;OriginalUiRiderClipCamera50KG guard;
    if(!prepareOriginalUiRiderMembershipCamera50KG(camera,membership))return fail(d,Error::camera,0x280720);
    if(!prepareOriginalUiRiderClipCamera50KG(camera,guard))return fail(d,Error::camera,0x236EC0);
    s.camera=camera;s.membershipCamera=membership;s.guardCamera=guard;s.policy=policy;
    ++s.frameSerial;s.viewportEpochA4=static_cast<std::uint16_t>(s.viewportEpochA4+1);
    s.searchEpochA6=static_cast<std::uint16_t>(s.searchEpochA6+1);s.frameReady=true;return true;
}
bool OriginalUiRiderVisibilityOwner50KG::selectModel(const OriginalUiRiderVisibilityHandle50KG& handle,
    OriginalUiRiderModelSelection50KG& output,Diagnostic& d) noexcept {
    d={};if(!contains(handle))return fail(d,Error::staleHandle,0x26EFC0);
    auto& s=*storage_;auto& e=s.entries[handle.widget];
    if(!s.frameReady)return fail(d,Error::camera,0x27F650);
    if(e.selectionSerial==std::numeric_limits<std::uint64_t>::max())return fail(d,Error::registration,0x26EFC0);
    OriginalUiRiderModelSelection50KG next{};next.membership=handle;next.frameSerial=s.frameSerial;
    next.selectionSerial=e.selectionSerial+1;next.sourceModelFlags191=e.flags191;
    next.sourceRenderFlags1B2=e.flags1B2;next.disabled=(e.flags03&4u)!=0;
    if(next.disabled){
        next.sourceModelFlags191=static_cast<std::uint8_t>(next.sourceModelFlags191&~1u);
    }else{
        if(!arithmeticReady())return fail(d,Error::arithmetic,0x240C00);
        const auto graph=e.character?e.character->graph():e.pod->graph();
        const auto* psg=e.character?e.characterAssets->hierarchy():e.podAssets->hierarchy();
        if((e.character&&!e.character->attached())||(e.pod&&!e.pod->loaded())||!psg||
            !graphReady(graph,*psg))return fail(d,Error::registration,0x240C00);
        if(!refreshSceneWorld49P(graph,static_cast<SceneNodeIndex49P>(0)))return fail(d,Error::bounds,0x2897B0);
        next.rootWorld100=graph.nodes[0].world100;
        if(!prepareOriginalUiRiderModelBounds50KG(*psg,graph,next.bounds))return fail(d,Error::bounds,0x26F4B0);
        Vector center{},extent{};
        std::memcpy(center.data(),next.bounds.center160,sizeof next.bounds.center160);
        std::memcpy(extent.data(),next.bounds.halfExtent170,sizeof next.bounds.halfExtent170);
        Arithmetic a;const auto world=matrix(next.rootWorld100);
        const auto combined=product(a,world,s.policy.useMembership3833D8?s.membershipCamera.matrix60:s.camera.core.combined);
        if(!a.valid||!projectOriginalUiRiderBoxClip50KG(combined,center,extent,next.viewportClip))
            return fail(d,Error::arithmetic,0x2B0580);
        auto& flags=next.sourceRenderFlags1B2;
        flags=static_cast<std::uint8_t>((flags&~(8u|128u))|4u);
        const bool selected=!next.viewportClip.outside;
        std::uint8_t remaining=next.viewportClip.unionMask;
        if(selected){
            if(remaining==0)flags=static_cast<std::uint8_t>(flags|8u);
            else{
                const auto guarded=product(a,world,s.guardCamera.combinedClip2D0);
                if(!a.valid||!projectOriginalUiRiderBoxClip50KG(guarded,center,extent,next.guardClip))
                    return fail(d,Error::arithmetic,0x240FE4);
                next.guardChecked=true;remaining=next.guardClip.unionMask;
                //240FF4 only consumes the union from the second clip call;
                // its all-outside return does not alter the first selection.
                if(remaining==0)flags=static_cast<std::uint8_t>(flags|
                    (s.policy.guardInsideSets80_4354F0?128u:8u));
            }
        }
        //241044 removes +Z/far bit10 AFTER the optional guard test. For culled
        // models,1B2 bit0 is preserved;04 stays set,08/80 stay clear.
        remaining=static_cast<std::uint8_t>(remaining&~0x10u);next.remainingClipMask=remaining;
        if(selected)flags=static_cast<std::uint8_t>((flags&~5u)|(remaining?5u:0u));
        next.sourceModelFlags191=static_cast<std::uint8_t>((next.sourceModelFlags191&~1u)|(selected?1u:0u));
    }
    e.flags191=next.sourceModelFlags191;e.flags1B2=next.sourceRenderFlags1B2;
    e.selectionSerial=next.selectionSerial;e.lastSelection=next;
    if(!next.disabled)++s.processed278;
    output=next;return true;
}
bool OriginalUiRiderVisibilityOwner50KG::isCurrentSelection(const OriginalUiRiderModelSelection50KG& selection) const noexcept {
    if(!contains(selection.membership)||!storage_->frameReady||selection.frameSerial!=storage_->frameSerial||
        selection.sourceZoneIndex48!=0)return false;
    const auto& last=storage_->entries[selection.membership.widget].lastSelection;
    return last.frameSerial==selection.frameSerial&&last.selectionSerial!=0&&
        last.selectionSerial==selection.selectionSerial&&last.sourceModelFlags191==selection.sourceModelFlags191&&
        last.sourceRenderFlags1B2==selection.sourceRenderFlags1B2&&last.disabled==selection.disabled;
}
bool OriginalUiRiderVisibilityOwner50KG::recordPacketFog(const OriginalUiRiderModelSelection50KG& selection,
    bool computeRadialFog) noexcept {
    if(!isCurrentSelection(selection)||!selection.selected())return false;
    auto& e=storage_->entries[selection.membership.widget];
    e.flags1B2=static_cast<std::uint8_t>((e.flags1B2&~2u)|(computeRadialFog?2u:0u));return true;
}
std::uint32_t OriginalUiRiderVisibilityOwner50KG::processedModels278() const noexcept {
    return storage_?storage_->processed278:0;
}
const OriginalUiRiderMembershipCamera50KG* OriginalUiRiderVisibilityOwner50KG::membershipCamera() const noexcept {
    return storage_&&storage_->frameReady?&storage_->membershipCamera:nullptr;
}
}
