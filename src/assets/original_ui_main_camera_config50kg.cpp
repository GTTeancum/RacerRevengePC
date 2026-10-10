#include "original_ui_main_camera_config50kg.h"
#include <cstring>

namespace rr::assets {
namespace {
constexpr const char* unusedKeys[]={"CameraPosition1","CameraLook1","CameraUp1","CameraFOV1","CameraRoll1"};
bool mainMember(const OriginalUiMemberIdentity50KG* identity,const char* name) noexcept {
    if(!identity)return false;
    const auto* archive=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::mainMenu);
    if(!archive)return false;
    for(std::size_t i=0;i<archive->memberCount;++i) {
        const auto* member=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::mainMenu,i);
        if(member==identity)return std::strcmp(member->name,name)==0;
    }
    return false;
}
char lower(char c) noexcept {return c>='A'&&c<='Z'?char(c+'a'-'A'):c;}
bool same(const std::string& a,const char* b) noexcept {
    std::size_t i=0;for(;i<a.size()&&b[i];++i)if(lower(a[i])!=lower(b[i]))return false;
    return i==a.size()&&!b[i];
}
}
bool isOriginalUiMainCameraConfig50KG(const OriginalUiMainCameraConfig50KG& config) noexcept {
    return mainMember(config.identity,"UI\\UIMain\\ui.dat");
}
bool readOriginalUiMainCameraConfig50KG(const OriginalUiDat50KG& ui,
    OriginalUiMainCameraConfig50KG& output,OriginalUiPageDiagnostic50KG& d) noexcept {
    if(!mainMember(ui.identity(),"UI\\UIMain\\ui.dat")) {
        d={OriginalUiPageError50KG::missingMember,0,0x1CF4F0};return false;
    }
    try {
        OriginalUiMainCameraConfig50KG next;next.identity=ui.identity();
        const auto* s=ui.section("CameraInfo");
        if(s) {
            next.hasCameraInfo=true;
            if(!originalUiTriple50KG(*s,"CameraPosition",{{0,0,0}},next.position,d,0x1CF55C)||
                !originalUiTriple50KG(*s,"CameraLook",{{0,-1,0}},next.look,d,0x1CF58C)||
                !originalUiTriple50KG(*s,"CameraUp",{{0,0,1}},next.up,d,0x1CF5B8)||
                !originalUiNumber50KG(*s,"CameraFOV",35,next.fovDegrees,d,0x1CF5D4)||
                !originalUiNumber50KG(*s,"CameraRoll",0,next.rollRadians,d,0x1CF5EC))return false;
            constexpr const char* keys[]={"CameraPosition","CameraLook","CameraUp","CameraFOV","CameraRoll"};
            for(std::size_t i=0;i<next.authoredFields.size();++i)next.authoredFields[i]=s->find(keys[i])!=nullptr;
            // Unrecognized CameraInfo fields retain their raw DAT entries;
            // schema admission remains the Main composition owner's task.
        }
        output=next;d={};return true;
    }catch(...) {d={OriginalUiPageError50KG::allocation,0,0x1CF4F0};return false;}
}
bool isOriginalUiMainUnusedCameraField50KG(const std::string& key) noexcept {
    for(const auto* candidate:unusedKeys)if(same(key,candidate))return true;
    return false;
}
bool retainOriginalUiMainUnusedCameraMetadata50KG(const OriginalUiDat50KG& main,
    OriginalUiMainUnusedCameraMetadata50KG& output,OriginalUiPageDiagnostic50KG& d) noexcept {
    if(!mainMember(main.identity(),"UI\\UIMain\\main.dat")) {
        d={OriginalUiPageError50KG::missingMember,0,0x1E5CF0};return false;
    }
    try {
        const auto* info=main.section("PageInfo");
        if(!info){d={OriginalUiPageError50KG::missingSection,0,0x1D64CC};return false;}
        OriginalUiMainUnusedCameraMetadata50KG next;next.pageIdentity=main.identity();
        for(std::size_t i=0;i<next.fields.size();++i)next.fields[i]=info->find(unusedKeys[i]);
        output=next;d={};return true;
    }catch(...) {d={OriginalUiPageError50KG::allocation,0,0x1E5CF0};return false;}
}
}
