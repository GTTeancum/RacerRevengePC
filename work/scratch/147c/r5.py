import sys; sys.path.insert(0, r'D:/Programming/GitHub/RacerRevengePC/.claude/worktrees/agent-a931b4f26f2a94abf/work/scratch/147c')
from res import res
def f(n, a, b):
    if n == 0:
        return ("    # A --hold-rider / --hold-course / --eyecheck-dlc-hall run writes its held frame as an image (eye check).\n"
                "    logs_only = (args.logs_only and not args.hold_rider and not args.hold_course and not args.hold_results and\n"
                "                 not args.hold_results_hall and args.eyecheck_dlc_hall is None)\n")
    assert n == 1
    return ("                  addonCourseHold=args.hold_course,\n"
            "                  eyecheckDlcHall=args.eyecheck_dlc_hall, resultsHold=args.hold_results, resultsHallCapture=args.hold_results_hall,\n"
            "                  seedSlowHall=args.seed_slow_hall,\n"
            "                  resultsExit=args.results_exit,\n")
res('tools/capture/run_windows_takeover.py', f)
