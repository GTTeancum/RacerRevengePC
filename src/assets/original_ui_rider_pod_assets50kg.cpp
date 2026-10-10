#include "original_ui_rider_pod_assets50kg.h"
#include <new>
#include <utility>

namespace rr::assets {
struct OriginalUiRiderPodAssets50KG::Storage {
    std::shared_ptr<const OriginalUiArchive50KG> archive;
    std::uint16_t widget=0;
    const OriginalUiMemberIdentity50KG* widgetSource=nullptr;
    OriginalUiMemberView50KG model{};
    OriginalUiRiderPodConfig50KG config{};
    PsgHierarchyView49O hierarchy{};
    Scene48D scene;
};
namespace {
using E=OriginalUiRiderPodError50KG;
bool fail(OriginalUiRiderPodDiagnostic50KG& d,E error,std::uint32_t pc) noexcept {
    d.error=error;d.sourcePC=pc;return false;
}
bool podStem(const std::string& value,std::string& result) {
    const auto dot=value.find_last_of('.');
    if (dot==std::string::npos||value.substr(dot)!=".slt"||dot==0||dot>63) return false;
    result=value.substr(0,dot);
    for (const auto c:result) if (!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||
        (c>='0'&&c<='9')||c=='_')) return false;
    return true;
}
}
OriginalUiRiderPodAssets50KG::OriginalUiRiderPodAssets50KG() noexcept=default;
OriginalUiRiderPodAssets50KG::~OriginalUiRiderPodAssets50KG()=default;
bool OriginalUiRiderPodAssets50KG::load(std::shared_ptr<const OriginalUiArchive50KG> archive,
    std::uint16_t widget,OriginalUiRiderPodDiagnostic50KG& d) noexcept {
    d={};if (storage_) return fail(d,E::alreadyLoaded,0x1D3590);
    if (!archive||!archive->loaded()||!archive->identity()||
        archive->identity()->id!=OriginalUiArchiveId50KG::rider) return fail(d,E::archive,0x1D3590);
    try {
        auto next=std::make_unique<Storage>();next->archive=std::move(archive);next->widget=widget;
        OriginalUiDat50KG widgets;
        if (!readOriginalUiDat50KG(*next->archive,"UI/UIRider/Widgets.Dat",widgets,d.text,
            OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)) return fail(d,E::widget,0x1D31D0);
        next->widgetSource=widgets.identity();
        const auto* section=widgets.section("Widget "+std::to_string(widget));
        if (!section) return fail(d,E::widget,0x1D31D0);
        const auto* type=section->find("Type");const auto* slt=section->find("SLT");
        std::string stem;bool dynamic=false;
        if (!type||type->text!="User_Rotate"||!slt||!podStem(slt->text,stem)||
            !originalUiBoolean50KG(*section,"DynamicLoad",false,dynamic,d.text,0x1CD5C0)||!dynamic)
            return fail(d,E::widget,0x1CD600);
        auto& c=next->config;
        // Rider explicitly authors these vectors. Do not silently manufacture
        // constructor defaults instead of the source missing-field getter route.
        if (!section->find("Position")||!section->find("LookVector")||!section->find("UpVector")||
            !originalUiTriple50KG(*section,"Position",{},c.position,d.text,0x1D31D0)||
            !originalUiTriple50KG(*section,"LookVector",{},c.look,d.text,0x1D31D0)||
            !originalUiTriple50KG(*section,"UpVector",{},c.up,d.text,0x1D31D0)||
            !originalUiBoolean50KG(*section,"IsLit",false,c.lit,d.text,0x1D31D0)||
            !originalUiBoolean50KG(*section,"StartPaused",true,c.startPaused,d.text,0x1D31D0)||
            !originalUiBoolean50KG(*section,"CleanFromMemory",false,c.cleanFromMemory,d.text,0x1D31D0))
            return fail(d,E::widget,0x1D31D0);
        const char* flags[]{"XRotate","YRotate","ZRotate"};
        const char* speeds[]{"XVelocity","YVelocity","ZVelocity"};
        for (unsigned axis=0;axis<3;++axis) if (
            !originalUiBoolean50KG(*section,flags[axis],false,c.rotate[axis],d.text,0x1D31D0)||
            !originalUiNumber50KG(*section,speeds[axis],0,c.velocity[axis],d.text,0x1D31D0))
            return fail(d,E::widget,0x1D31D0);
        next->model=next->archive->find("UI/Geometry/"+stem+".psg");
        if (!next->model.identity) return fail(d,E::member,0x1D35C4);
        if (!readPsgHierarchy49O(next->model.data,next->model.size,next->hierarchy,d.hierarchy))
            return fail(d,E::hierarchy,0x1D35C4);
        const std::vector<std::uint8_t> bytes(next->model.data,next->model.data+next->model.size);
        if (!decodeScene48D(bytes,next->scene,d.geometry)||
            next->scene.nodes.size()!=next->hierarchy.nodeCount||next->scene.world_space||
            !next->scene.identity_matrix_palette||next->scene.lods.empty())
            return fail(d,E::geometry,0x1D35C4);
        storage_=std::move(next);d={};return true;
    } catch (const std::bad_alloc&) {return fail(d,E::allocation,0x1D3590);}
      catch (...) {return fail(d,E::widget,0x1D3590);}
}
bool OriginalUiRiderPodAssets50KG::loaded() const noexcept {return !!storage_;}
std::uint16_t OriginalUiRiderPodAssets50KG::widgetId() const noexcept {return storage_?storage_->widget:0;}
const OriginalUiRiderPodConfig50KG* OriginalUiRiderPodAssets50KG::config() const noexcept {return storage_?&storage_->config:nullptr;}
const PsgHierarchyView49O* OriginalUiRiderPodAssets50KG::hierarchy() const noexcept {return storage_?&storage_->hierarchy:nullptr;}
const Scene48D* OriginalUiRiderPodAssets50KG::scene() const noexcept {return storage_?&storage_->scene:nullptr;}
const OriginalUiMemberIdentity50KG* OriginalUiRiderPodAssets50KG::widgetSource() const noexcept {return storage_?storage_->widgetSource:nullptr;}
OriginalUiMemberView50KG OriginalUiRiderPodAssets50KG::modelSource() const noexcept {return storage_?storage_->model:OriginalUiMemberView50KG{};}
std::shared_ptr<const OriginalUiArchive50KG> OriginalUiRiderPodAssets50KG::archive() const noexcept {return storage_?storage_->archive:nullptr;}
}
