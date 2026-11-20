W = r'D:/Programming/GitHub/RacerRevengePC/.claude/worktrees/agent-a931b4f26f2a94abf/'
def edit(rel, pairs):
    p = W + rel
    s = open(p, encoding='utf-8', newline='').read()
    for old, new in pairs:
        assert s.count(old) == 1, (rel, old)
        s = s.replace(old, new)
    open(p, 'w', encoding='utf-8', newline='').write(s)
    print('ok', rel)

edit('tests/native/original_ui_capture_navigation50la.h', [
("""        const char* courseSubtype=std::getenv("RR_TEST_ADDON_COURSE_SUBTYPE");
        if(addonCourse_&&courseSubtype&&*courseSubtype){
            if(courseSubtype[0]<'1'||courseSubtype[0]>'3'||courseSubtype[1])
                throw std::runtime_error("RR_TEST_ADDON_COURSE_SUBTYPE must be 1 (Single Race), 2 or 3 (Time Trial)");""",
"""        // Free-play Course event type (147c): add-on or stock TA route alike.
        const char* courseSubtype=std::getenv("RR_TEST_COURSE_SUBTYPE");
        if(enabled_&&!selectedRoute_&&!sa_&&!occo_&&courseSubtype&&*courseSubtype){
            if(courseSubtype[0]<'1'||courseSubtype[0]>'3'||courseSubtype[1])
                throw std::runtime_error("RR_TEST_COURSE_SUBTYPE must be 1 (Single Race), 2 or 3 (Time Trial)");"""),
("""            require(enabled_&&addonCourse_&&awaitingDispatch_&&issued_==2&&!s.faulted&&!s.quitRequested&&
                s.route.screen==6&&s.route.page==1&&s.switches==2&&t.bound&&same(t.current,s.route)&&
                same(t.requested,s.route)&&t.lifecycle==Life::idle&&t.phase==Phase::none&&
                !t.applicationRequestQueued&&!t.initialRestartPending&&!s.deferredQueue,
                "add-on course route event-type move changed the route");""",
"""            require(enabled_&&addonCourseSubtype_&&awaitingDispatch_&&issued_==2&&!s.faulted&&!s.quitRequested&&
                s.route.screen==6&&s.route.page==1&&s.switches==2&&t.bound&&same(t.current,s.route)&&
                same(t.requested,s.route)&&t.lifecycle==Life::idle&&t.phase==Phase::none&&
                !t.applicationRequestQueued&&!t.initialRestartPending&&!s.deferredQueue,
                "Course route event-type move changed the route");"""),
("""    // Event type for the add-on course test (147c, RR_TEST_ADDON_COURSE_SUBTYPE=1..3;""",
"""    // Event type for the free-play Course routes (147c, RR_TEST_COURSE_SUBTYPE=1..3;"""),
])

edit('tools/capture/run_windows_takeover.py', [
("""    parser.add_argument('--addon-course-mode', choices=['single', 'time_trial'],
                        help='With --addon-course: on the Course screen first set the event type (original Down to the '
                             'event-type row, Right until Single Race / Time Trial shows, Up back to the course row), '
                             'then pick the add-on course (sets RR_TEST_ADDON_COURSE_SUBTYPE=1 or 3). Default: the '
                             'event type the Course screen opens on (Single Race on a fresh profile)')""",
"""    parser.add_argument('--course-mode', '--addon-course-mode', dest='course_mode', choices=['single', 'time_trial'],
                        help='Free-play Course (--route main-rider-course-ta, stock TA or with --addon-course): first set '
                             'the event type (original Down to the event-type row, Right until Single Race / Time Trial '
                             'shows, Up back to the course row), then pick the course (sets RR_TEST_COURSE_SUBTYPE=1 or 3). '
                             'Default: the event type the Course screen opens on (Single Race on a fresh profile)')"""),
("""    if args.addon_course_mode and args.addon_course is None:
        parser.error('--addon-course-mode needs --addon-course')""",
"""    if args.course_mode and args.route != 'main-rider-course-ta':
        parser.error('--course-mode works only with --route main-rider-course-ta')"""),
("""    if args.addon_course_mode:
        env['RR_TEST_ADDON_COURSE_SUBTYPE'] = '3' if args.addon_course_mode == 'time_trial' else '1'""",
"""    if args.course_mode:
        env['RR_TEST_COURSE_SUBTYPE'] = '3' if args.course_mode == 'time_trial' else '1'"""),
("""addonCourseMode=args.addon_course_mode,""", """courseMode=args.course_mode,"""),
])
