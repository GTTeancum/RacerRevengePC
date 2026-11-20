from edlib import edit
edit('tests/native/terrain_interactive_session50du.cpp', [
(''' const bool commonRouteTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"SA");''',
 ''' // ONE common race route (146r): stock SA (its stock profile = receipts) and every
 // registry (add-on) course. The registry flag is data of the session row
 // (track_session_assets50IG::registrySession); no add-on code is named here.
 // PORT STAND-IN (flagged): the other twelve stock codes still select their own
 // loaders below (isXxTrack); they are to be retired onto this route.
 const bool registryCourse=trackSession->registry;
 require50DU(!registryCourse||loadedTrack==rr::platform::HostTrack50IM::Course,
     "registry course row reached the session without the common-route launch identity");
 const bool commonRouteTrack=track_session_assets50IG::codeEquals(trackSession->world->code,"SA")||registryCourse;
 if(registryCourse)std::cerr<<"course_route_session code="<<trackSession->world->code<<" archive="<<trackSession->archive
     <<" listed_bytes="<<trackSession->archiveBytes<<" tuning="<<(trackSession->tuningArchive?"inherited":"own")
     <<" route=common session="<<sessionIndex+1<<'\\n';'''),
('''"MA/SA require either actual Main/Course Player0 plus seven AI or their retained ALL8 route");''',
 '''"MA/common-route courses require either actual Main/Course Player0 plus seven AI or their retained ALL8 route");
 // The registry course has no retained ALL8 route: only the consumed free-play Course launch reaches it.
 require50DU(!registryCourse||(consumedLaunch&&directOriginalHuman&&humanCourseEnabled&&!allAiEnabled&&trackAdmission.gcMode2),
     "registry course requires the consumed free-play Course launch (Player0 plus seven AI, mode2, original disc)");'''),
('''"MA/SA/RA require the actual recovered camera/visibility owner");''',
 '''"MA/common-route/RA require the actual recovered camera/visibility owner");'''),
])
