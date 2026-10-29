// Included once by original_ui_main_entry50kg.cpp after actual owners.
namespace original_main_entry50KG {
bool Application::advanceCourseRaLoadingPrefix50LA(Frontend& frontend,std::uint32_t& pc)const noexcept {
    using Phase=CourseLoadingPhase50LA;
    pc=0x17d7a0;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    auto& retained=courseLoadingPrefix50LA_;
    if(retained.attempted){pc=retained.stopPC;return false;} //Never replay any prefix, even after native cleanup.
    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selected;
    std::shared_ptr<RaceDisplayLifetime50LA> display;
    if(!frontend.coordinator.observeCourseRaHandoff50LA(*this,frontend,selected,display))return false;
    retained.frontend=&frontend;retained.display=std::move(display);retained.video=videoOwner_;
    retained.profile=riderProfile;retained.selection=selected;retained.attempted=true;
    // The real flags9 consumer first takes bit8 at1465C0: Video virtual
    // +24/+20, then17DCE0 detaches App2D8 at17DD28, invokes dispatcher
    // virtual+10 at17DD3C and clears App2D8 at17DD44. Only afterward
    // does146628 write App22C=1 and146630 call17D7A0.
    // Current Course owns a Session/collector/resources without source
    // teardown. Generic Storage::retireSource has no Course branch. Neither
    // draining its CPU packets nor native renderer close proves that chain.
    // Retain all exact owners and stop BEFORE its first unrepresented call;
    // do not close audio, reset Aural/pending rows, stage330 or request Video.
    retained.stopPC=pc=0x1465d0;
    retained.phase=Phase::terminal;
    return false;
}
bool Application::withdrawCourseRaLoadingPrefix50LA(Frontend& frontend)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    auto& retained=courseLoadingPrefix50LA_;
    if(!retained.frontend)return true;
    if(retained.frontend!=&frontend||frontend.materialApplicationOwner!=this)return false;
    // Explicit native custody withdrawal, not source state rollback. Normal
    //coordinator.close owns partial UI cleanup while original context survives.
    retained.phase=CourseLoadingPhase50LA::terminal;
    retained.frontend=nullptr;retained.display.reset();retained.video.reset();retained.profile.reset();
    return true;
}
}
