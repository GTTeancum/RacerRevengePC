from edlib import edit
edit('tests/native/terrain_interactive_publication50du.cpp', [
('''    const bool effects=racerRigs[0]&&native_wreck_debris50PD::loadEffects(trackSession->archive,*racerRigs[0],effectsLog)&&''',
 '''    // inherit.particles (146r): a registry course without particle\\demoprt.* reads
    // them from the named stock course's archive (members in the archive win).
    const std::string effectsArchive=[&]()->std::string{
     rr::game::dlc50DL::AddonCourseLoad row;
     if(trackSession->registry&&rr::game::dlc50DL::addonCourseLoadByCode(trackSession->world->code,row)&&row.particlesInherited)
      return runtime_asset_paths50LB::resolve("work/native/project/disc/"+row.particleArchive).string();
     return trackSession->archive;
    }();
    if(trackSession->registry)std::cerr<<"course_route_wreck_effects code="<<trackSession->world->code
        <<" source="<<(effectsArchive==trackSession->archive?"own":"inherited")<<'\\n';
    const bool effects=racerRigs[0]&&native_wreck_debris50PD::loadEffects(effectsArchive.c_str(),*racerRigs[0],effectsLog)&&'''),
('''    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection course;
    bool bound=originalMainApplication.riderProfile&&
        originalMainApplication.riderProfile->courseRaceSnapshot(course)&&course.course<13&&
        originalMainApplication.finishPauseDrawCold50LA(sourcePause)&&
        renderer.bindOriginalPauseDerived(sourcePause.drawing(),rendererDiagnostic);
    bound=bound&&sourcePauseObservation.bindHost(originalMainApplication,sourcePause.storage(),
        sourceCameraSession,trackTitles[course.course]);''',
 '''    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection course;
    // Registry course (146r): the title is the row's name; the profile's stock
    // course field is not this race's course and is never used as an index here.
    rr::game::dlc50DL::AddonCourseLoad titleRow;
    const bool registryTitle=trackSession->registry&&rr::game::dlc50DL::addonCourseLoadByCode(trackSession->world->code,titleRow);
    bool bound=originalMainApplication.riderProfile&&
        originalMainApplication.riderProfile->courseRaceSnapshot(course)&&(registryTitle||course.course<13)&&
        originalMainApplication.finishPauseDrawCold50LA(sourcePause)&&
        renderer.bindOriginalPauseDerived(sourcePause.drawing(),rendererDiagnostic);
    bound=bound&&(registryTitle?
        sourcePauseObservation.bindHostNamed(originalMainApplication,sourcePause.storage(),sourceCameraSession,titleRow.name):
        sourcePauseObservation.bindHost(originalMainApplication,sourcePause.storage(),sourceCameraSession,trackTitles[course.course]));
    if(registryTitle)std::cerr<<"course_route_pause_title code="<<titleRow.code<<" name=\\""<<titleRow.name<<"\\" stock=0\\n";'''),
])
