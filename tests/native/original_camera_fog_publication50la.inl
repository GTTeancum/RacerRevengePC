// Included privately inside the actual Session. Copies are observations, never
// independent camera state. Only prepareGameplayFogRender can publish one.
struct FogPreparation {
    const OriginalRaceFogFields* owner=nullptr;
    std::uint64_t revision=0,generation=0,publication=0,frame=0;
    OriginalRaceFogFields source;
    CameraViewOutput49F camera;
    CameraSceneState49F scene;
    TVCameraUpdateOutput49F pose;
    std::array<float,16> screen{},combinedScreen{},guardProjection{},guardCombined{};
    std::array<float,3> position{};
    float nearPlane=0,farPlane=0;
};
FogPreparation fogPreparation_;
std::uint64_t fogPreparationRevision_=0;
void invalidateFogPreparation()noexcept {fogPreparation_={};}
static bool sameFogSource(const OriginalRaceFogFields& a,const OriginalRaceFogFields& b)noexcept {
    return a.color30==b.color30&&same(a.start34,b.start34)&&same(a.end38,b.end38)&&
        same(a.authoredFraction3C,b.authoredFraction3C)&&same(a.authoredFraction40,b.authoredFraction40)&&
        same(a.far50,b.far50)&&same(a.near54,b.near54)&&same(a.minimum58,b.minimum58)&&a.dirty60==b.dirty60;
}
static bool sameFogMatrix(const std::array<float,16>& a,const std::array<float,16>& b)noexcept {
    return std::memcmp(a.data(),b.data(),sizeof(float)*16)==0;
}
static bool sameFogVector(const rr::assets::TVCameraVector49E& a,const rr::assets::TVCameraVector49E& b)noexcept {
    return same(a.x,b.x)&&same(a.y,b.y)&&same(a.z,b.z)&&same(a.w,b.w);
}
static bool sameFogScene(const CameraSceneState49F& a,const CameraSceneState49F& b)noexcept {
    return sameFogVector(a.look,b.look)&&sameFogVector(a.up,b.up)&&same(a.fovDegrees,b.fovDegrees)&&
        same(a.aspect1F8,b.aspect1F8)&&same(a.nearPlane,b.nearPlane)&&same(a.farPlane,b.farPlane)&&
        same(a.rollRadians,b.rollRadians);
}
static bool sameFogPose(const TVCameraUpdateOutput49F& a,const TVCameraUpdateOutput49F& b)noexcept {
    return a.mode==b.mode&&sameFogVector(a.position,b.position)&&sameFogVector(a.target,b.target)&&
        sameFogVector(a.look,b.look)&&sameFogVector(a.up,b.up)&&same(a.fov,b.fov);
}
bool readFogPreparation(FogPreparation& out)const noexcept {
    const auto& p=fogPreparation_;
    if(!p.revision||!coldBound_||closed_||abandoned_||frameStage_!=FrameStage::published||
       !displayInputs_||displayInputs_->trackFog3EC!=p.owner||!p.owner||
       display_.actualTrackFog3EC!=p.owner||p.generation!=generation_||
       p.publication!=publicationCount_||p.frame!=completedFrame_||
       !sameFogSource(*p.owner,p.source)||!sameFogScene(scene_,p.scene)||!sameFogPose(pose_,p.pose)||
       !sameFogScene(matrices_.sceneAfter,p.camera.sceneAfter)||
       !sameFogVector(matrices_.position,p.camera.position)||
       matrices_.targetAccepted!=p.camera.targetAccepted||matrices_.fovWritten!=p.camera.fovWritten||
       !sameFogMatrix(matrices_.view,p.camera.view)||!sameFogMatrix(matrices_.projection,p.camera.projection)||
       !sameFogMatrix(matrices_.combined,p.camera.combined)||!sameFogMatrix(viewport_.screen140,p.screen)||
       !sameFogMatrix(viewport_.combinedScreen250,p.combinedScreen)||
       !sameFogMatrix(viewport_.guardProjection290,p.guardProjection)||
       !sameFogMatrix(viewport_.guardCombined2D0,p.guardCombined))return false;
    out=p;return true;
}
void publishFogPreparation(const OriginalRaceFogFields& fog)noexcept {
    FogPreparation next;next.owner=&fog;next.revision=++fogPreparationRevision_;
    next.generation=generation_;next.publication=publicationCount_;next.frame=completedFrame_;
    next.source=fog;next.camera=matrices_;next.scene=scene_;next.pose=pose_;
    next.screen=viewport_.screen140;next.combinedScreen=viewport_.combinedScreen250;
    next.guardProjection=viewport_.guardProjection290;next.guardCombined=viewport_.guardCombined2D0;
    next.position={{pose_.position.x,pose_.position.y,pose_.position.z}};
    next.nearPlane=scene_.nearPlane;next.farPlane=scene_.farPlane;fogPreparation_=next;
}
