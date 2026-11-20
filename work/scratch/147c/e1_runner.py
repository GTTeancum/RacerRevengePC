import pathlib
p = pathlib.Path(r'D:/Programming/GitHub/RacerRevengePC/.claude/worktrees/agent-a931b4f26f2a94abf/tools/capture/run_windows_takeover.py')
s = p.read_text(encoding='utf-8')
def rep(old, new):
    global s
    assert s.count(old) == 1, old
    s = s.replace(old, new)
rep("""    parser.add_argument('--hold-course', action='store_true',""",
"""    parser.add_argument('--addon-course-mode', choices=['single', 'time_trial'],
                        help='With --addon-course: on the Course screen first set the event type (original Down to the '
                             'event-type row, Right until Single Race / Time Trial shows, Up back to the course row), '
                             'then pick the add-on course (sets RR_TEST_ADDON_COURSE_SUBTYPE=1 or 3). Default: the '
                             'event type the Course screen opens on (Single Race on a fresh profile)')
    parser.add_argument('--hold-course', action='store_true',""")
rep("""    if args.hold_course and args.addon_course is None:
        parser.error('--hold-course needs --addon-course')""",
"""    if args.hold_course and args.addon_course is None:
        parser.error('--hold-course needs --addon-course')
    if args.addon_course_mode and args.addon_course is None:
        parser.error('--addon-course-mode needs --addon-course')""")
rep("""        env['RR_TEST_ADDON_COURSE_HOLD'] = '1'
""", """        env['RR_TEST_ADDON_COURSE_HOLD'] = '1'
    if args.addon_course_mode:
        env['RR_TEST_ADDON_COURSE_SUBTYPE'] = '3' if args.addon_course_mode == 'time_trial' else '1'
""")
rep("""                  addonCourseHold=args.hold_course,""",
"""                  addonCourseHold=args.hold_course, addonCourseMode=args.addon_course_mode,""")
rep("""                confirmed=f'addon_course_route confirm key={args.addon_course} ' in texts,""",
"""                confirmed=f'addon_course_route confirm key={args.addon_course} ' in texts,
                eventMoves=re.findall(r'addon_course_route event_move=[^\\r\\n]*', texts),
                launch=re.findall(r'course_route_launch [^\\r\\n]*', texts),
                record=re.findall(r'dlc_course_record [^\\r\\n]*', texts),
                raceAck=re.findall(r'race_ack route=[^\\r\\n]*', texts),
                restore=re.findall(r'dlc_course_carousel action=restore[^\\r\\n]*', texts),""")
p.write_text(s, encoding='utf-8')
print('ok')
