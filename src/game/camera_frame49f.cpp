#include "camera_frame49f.h"
#include <cstring>

namespace rr::game {
bool updateTVCameraView49F(const assets::TVCameraView49E& configuration,
    TVCameraSelectorState49E& selected,TVCameraUpdateState49F& updateState,
    const TVCameraUpdateInput49F& input,TVCameraRandom49E& random,
    CameraSceneState49F& scene,TVCameraUpdateOutput49F& liveOutput,
    CameraViewOutput49F& viewOutput,CameraViewContext49F context) noexcept {
    if((context.rootFlags228&4u)||(context.sceneFlags29&4u))return false;
    TVCameraSelectorState49E nextSelected;
    TVCameraUpdateState49F nextUpdate;
    TVCameraUpdateOutput49F numeric;
    std::memcpy(&nextSelected,&selected,sizeof(nextSelected));
    std::memcpy(&nextUpdate,&updateState,sizeof(nextUpdate));
    std::memcpy(&numeric,&liveOutput,sizeof(numeric));
    std::memcpy(&numeric.look,&scene.look,sizeof(numeric.look));
    std::memcpy(&numeric.up,&scene.up,sizeof(numeric.up));
    auto nextRandom=random;
    if(!updateTVCamera49F(configuration,nextSelected,nextUpdate,input,nextRandom,numeric))
        return false;
    CameraViewOutput49F result;
    if(!composeCameraView49F(scene,numeric,result))return false;
    std::memcpy(&selected,&nextSelected,sizeof(selected));
    std::memcpy(&updateState,&nextUpdate,sizeof(updateState));
    random=nextRandom;
    std::memcpy(&scene,&result.sceneAfter,sizeof(scene));
    std::memcpy(&liveOutput,&numeric,sizeof(liveOutput));
    std::memcpy(&viewOutput,&result,sizeof(viewOutput));
    return true;
}
}
