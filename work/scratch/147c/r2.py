import sys; sys.path.insert(0, r'D:/Programming/GitHub/RacerRevengePC/.claude/worktrees/agent-a931b4f26f2a94abf/work/scratch/147c')
from res import res
def f(n, a, b):
    return b.replace('bool single=true,timeTrial=true,vs=true;          // modes (free play and two-player VS; never the tournament)',
                     'bool single=true,timeTrial=true,vs=true; // modes (free play and two-player VS; never the tournament); "vs" has no effect until two-player races are built')
res('src/assets/dlc_registry50dl.h', f)
