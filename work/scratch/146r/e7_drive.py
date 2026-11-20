from edlib import edit
edit('tests/native/terrain_interactive_drive50du.cpp', [
('''  if(exit==SessionExit50DU::Restart&&consumedCourseLaunch&&
     consumedCourseLaunch->targetsTrack(rr::platform::HostTrack50IM::TA)){
   const bool renewed=consumedCourseLaunch->renewForRestart(sessionIndex+1,restartLaunch);
   std::cerr<<"consumed_launch_restart track=TA renewed="<<renewed<<'\\n';
  }''',
 '''  // Common route (146r): a registry course is reached only through its consumed
  // launch, so its Restart renews that launch the same way (same profile, same
  // port-owned add-on selection; target() must still answer the same target).
  if(exit==SessionExit50DU::Restart&&consumedCourseLaunch&&
     (consumedCourseLaunch->targetsTrack(rr::platform::HostTrack50IM::TA)||
      consumedCourseLaunch->targetsTrack(rr::platform::HostTrack50IM::Course))){
   const bool renewed=consumedCourseLaunch->renewForRestart(sessionIndex+1,restartLaunch);
   std::cerr<<"consumed_launch_restart track="<<rr::platform::hostTrackCode50IM(effectiveTrack)<<" renewed="<<renewed<<'\\n';
  }'''),
('''  if(restartLaunch&&effectiveTrack==rr::platform::HostTrack50IM::TA)consumedCourseLaunch=std::move(restartLaunch);''',
 '''  if(restartLaunch&&(effectiveTrack==rr::platform::HostTrack50IM::TA||effectiveTrack==rr::platform::HostTrack50IM::Course))
   consumedCourseLaunch=std::move(restartLaunch);'''),
])
