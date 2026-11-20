import sys; sys.path.insert(0, r'D:/Programming/GitHub/RacerRevengePC/.claude/worktrees/agent-a931b4f26f2a94abf/work/scratch/147c')
from res import res
def f(n, a, b):
    old = 'humanCourseEnabled&&dynamicProfile.game34==2&&nativeOrdinaryLaunch11258==1'
    assert old in b
    return b.replace(old, 'humanCourseEnabled&&(dynamicProfile.game34==2u||dynamicProfile.game34==0u)&&nativeOrdinaryLaunch11258==1')
res('tests/native/terrain_interactive_world50du.cpp', f)
