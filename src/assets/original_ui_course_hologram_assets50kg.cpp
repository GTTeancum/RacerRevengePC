// Reconstructed source-only candidate, 2026-10-12; not byte-verified against lost files.
#include "original_ui_course_hologram_assets50kg.h"
#include <new>
#include <utility>

namespace rr::assets {
struct OriginalUiCourseHologramAssets50KG::Storage {
    std::shared_ptr<const OriginalUiArchive50KG> archive;
    std::uint16_t widget=0;
    std::string base;
    const OriginalUiMemberIdentity50KG* widgetSource=nullptr;
    const OriginalUiMemberIdentity50KG* mcfSource=nullptr;
    OriginalUiMemberView50KG psg{},vut{};
    PsgHierarchyView49O hierarchy{};
    MotionNodeBindings49O bindings{};
    Scene48D scene;
    std::array<float,3> position{},look{},up{};
    std::array<OriginalUiCourseHologramMotion50KG,1> motions{};
};
namespace {
using E=OriginalUiCourseHologramAssetError50KG;
bool fail(OriginalUiCourseHologramAssetDiagnostic50KG& d,E e,std::uint32_t pc) noexcept {
    d.error=e;d.sourcePC=pc;return false;
}
bool simpleName(const std::string& name) noexcept {
    if (name.empty() || name.size()>63) return false;
    for (unsigned char c:name)
        if (!((c>='a'&&c<='z') || (c>='A'&&c<='Z') || (c>='0'&&c<='9') || c=='_'))
            return false;
    return true;
}
bool stem(const std::string& name,const char* extension,std::string& result) {
    const auto dot=name.find_last_of('.');
    if (dot==std::string::npos || name.substr(dot)!=extension) return false;
    const auto base=name.substr(0,dot);if (!simpleName(base)) return false;
    result=base;return true;
}
}
OriginalUiCourseHologramAssets50KG::OriginalUiCourseHologramAssets50KG() noexcept=default;
OriginalUiCourseHologramAssets50KG::~OriginalUiCourseHologramAssets50KG()=default;
bool OriginalUiCourseHologramAssets50KG::load(std::shared_ptr<const OriginalUiArchive50KG> archive,
    std::uint16_t widget,OriginalUiCourseHologramAssetDiagnostic50KG& d) noexcept {
    d={};
    if (storage_) return fail(d,E::alreadyLoaded,0x2A8770);
    if (!archive || !archive->loaded() || !archive->identity() ||
        archive->identity()->id!=OriginalUiArchiveId50KG::course) return fail(d,E::archive,0x2A8770);
    try {
        auto next=std::make_unique<Storage>();next->archive=std::move(archive);next->widget=widget;
        OriginalUiDat50KG widgets,mcf;
        if (!readOriginalUiDat50KG(*next->archive,"UI/UICourse/Widgets.Dat",widgets,d.text,
                OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)) return fail(d,E::widget,0x2A8770);
        const auto* section=widgets.section("Widget "+std::to_string(widget));
        if (!section) return fail(d,E::widget,0x2A8770);
        const auto* type=section->find("Type");const auto* reference=section->find("MCF");
        if (!type || type->text!="Soultree_Character" || !reference ||
            !stem(reference->text,".mcf",next->base)) return fail(d,E::widget,0x2A8770);
        constexpr std::array<std::uint16_t,13> admitted{{9,10,12,13,14,15,16,20,21,22,23,25,26}};
        bool member=false;for(auto id:admitted)if(id==widget)member=true;
        std::int32_t dynamic=0,select=0,deselect=0;bool enable=false,disable=false;
        if(!member||section->find("InitialMotion")||
           !originalUiInteger50KG(*section,"DynamicLoad",0,dynamic,d.text,0x1CD298)||dynamic!=0||
           !originalUiInteger50KG(*section,"SelectionMotion",0,select,d.text,0x2A8770)||select!=1||
           !originalUiInteger50KG(*section,"DeSelectionMotion",0,deselect,d.text,0x2A8770)||deselect!=1||
           !originalUiBoolean50KG(*section,"EnableOnSelect",false,enable,d.text,0x2A8770)||!enable||
           !originalUiBoolean50KG(*section,"DisableOnDeSelect",false,disable,d.text,0x2A8770)||!disable)
            return fail(d,E::widget,0x2A8770);
        next->widgetSource=widgets.identity();
        // Native binding to exact authenticated UI/Anim members. This does not
        // emulate the original global resource cache's filename-search policy.
        if (!readOriginalUiDat50KG(*next->archive,"UI/Anim/"+reference->text,mcf,d.text,
                OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)) return fail(d,E::config,0x1538B0);
        next->mcfSource=mcf.identity();
        // Actual Course files author every transform; admit no guessed axes.
        if (!section->find("Position") || !section->find("LookVector") || !section->find("UpVector") ||
            !originalUiTriple50KG(*section,"Position",{},next->position,d.text,0x2A8770) ||
            !originalUiTriple50KG(*section,"LookVector",{},next->look,d.text,0x2A8770) ||
            !originalUiTriple50KG(*section,"UpVector",{},next->up,d.text,0x2A8770))
            return fail(d,E::widget,0x2A8770);
        const auto* general=mcf.section("General info");
        if (!general) return fail(d,E::config,0x1538B0);
        std::int32_t count=0;
        if (!originalUiInteger50KG(*general,"NumberOfMotions",0,count,d.text,0x1538B0) || count!=1)
            return fail(d,E::config,0x1538B0);
        const auto* slt=general->find("SLTFile");const auto* vut=general->find("VUTfilename");
        const auto* directory=general->find("ContentDirectory");
        std::string geometryBase,bindingBase;
        if ((directory && !directory->text.empty()) || !slt || !vut ||
            !stem(slt->text,".slt",geometryBase) || !stem(vut->text,".vut",bindingBase))
            return fail(d,E::config,0x1538B0);
        next->psg=next->archive->find("UI/Anim/"+geometryBase+".psg");
        next->vut=next->archive->find("UI/Anim/"+bindingBase+".vut");
        if (!next->psg.identity || !next->vut.identity) return fail(d,E::member,0x1538B0);
        if (!readPsgHierarchy49O(next->psg.data,next->psg.size,next->hierarchy,d.model))
            return fail(d,E::hierarchy,0x152EA0);
        if (!readMotionNodeBindings49O(next->hierarchy,reinterpret_cast<const char*>(next->vut.data),
                next->vut.size,next->bindings,d.binding)) return fail(d,E::binding,0x153218);
        for (std::size_t i=0;i<next->motions.size();++i) {
            d.motionOrdinal=static_cast<std::uint32_t>(i+1);
            const auto* item=mcf.section("MOTION "+std::to_string(i+1));
            if (!item) return fail(d,E::config,0x1523F0);
            auto& motion=next->motions[i];
            const auto* name=item->find("NAME");const auto* vue=item->find("VUE");std::string vueBase;
            if (!name || !simpleName(name->text) || !vue || !stem(vue->text,".vue",vueBase) ||
                vueBase!=name->text) return fail(d,E::config,0x1523F0);
            motion.name=name->text;
            if (motion.name!=next->base || !item->find("Looping") ||
                !originalUiBoolean50KG(*item,"Looping",false,motion.looping,d.text,0x1523F0) ||
                !originalUiInteger50KG(*item,"FrameCount",0,motion.authoredFrames,d.text,0x1523F0) ||
                !originalUiNumber50KG(*item,"FPS",0,motion.authoredFps,d.text,0x1523F0) ||
                motion.authoredFrames<=0 || motion.authoredFps<=0)
                return fail(d,E::config,0x1523F0);
            motion.source=next->archive->find("UI/Anim/"+motion.name+".mot");
            if (!motion.source.identity) return fail(d,E::member,0x152540);
            if (!readMot49O(motion.source.data,motion.source.size,motion.motion,d.model))
                return fail(d,E::motion,0x152570);
        }
        // Existing semantic decoder keeps original rigid bone association,
        // local normals, materials, surfaces and strip-derived index order.
        // Its accepted format/palette constraints remain unchanged. Fail closed.
        const std::vector<std::uint8_t> bytes(next->psg.data,next->psg.data+next->psg.size);
        if (!decodeScene48D(bytes,next->scene,d.geometry)) return fail(d,E::geometry,0x1538B0);
        if (next->scene.nodes.size()!=next->hierarchy.nodeCount || next->scene.world_space ||
            !next->scene.identity_matrix_palette || next->scene.lods.empty())
            return fail(d,E::geometry,0x1538B0);
        storage_=std::move(next);d={};return true;
    } catch (const std::bad_alloc&) { return fail(d,E::allocation,0x2A8770); }
      catch (...) { return fail(d,E::config,0x2A8770); }
}
bool OriginalUiCourseHologramAssets50KG::loaded() const noexcept { return !!storage_; }
std::uint16_t OriginalUiCourseHologramAssets50KG::widgetId() const noexcept { return storage_?storage_->widget:0; }
const std::string* OriginalUiCourseHologramAssets50KG::baseName() const noexcept { return storage_?&storage_->base:nullptr; }
const OriginalUiMemberIdentity50KG* OriginalUiCourseHologramAssets50KG::widgetSource() const noexcept { return storage_?storage_->widgetSource:nullptr; }
const OriginalUiMemberIdentity50KG* OriginalUiCourseHologramAssets50KG::mcfSource() const noexcept { return storage_?storage_->mcfSource:nullptr; }
const PsgHierarchyView49O* OriginalUiCourseHologramAssets50KG::hierarchy() const noexcept { return storage_?&storage_->hierarchy:nullptr; }
const MotionNodeBindings49O* OriginalUiCourseHologramAssets50KG::bindings() const noexcept { return storage_?&storage_->bindings:nullptr; }
const Scene48D* OriginalUiCourseHologramAssets50KG::scene() const noexcept { return storage_?&storage_->scene:nullptr; }
const std::array<float,3>* OriginalUiCourseHologramAssets50KG::position() const noexcept { return storage_?&storage_->position:nullptr; }
const std::array<float,3>* OriginalUiCourseHologramAssets50KG::look() const noexcept { return storage_?&storage_->look:nullptr; }
const std::array<float,3>* OriginalUiCourseHologramAssets50KG::up() const noexcept { return storage_?&storage_->up:nullptr; }
const OriginalUiCourseHologramMotion50KG* OriginalUiCourseHologramAssets50KG::motion(std::size_t i) const noexcept {
    return storage_ && i<storage_->motions.size()?&storage_->motions[i]:nullptr;
}
const OriginalUiCourseHologramMotion50KG* OriginalUiCourseHologramAssets50KG::findMotion(const std::string& name) const noexcept {
    if (storage_) for (const auto& motion:storage_->motions) if (motion.name==name) return &motion;
    return nullptr;
}
} // namespace rr::assets
