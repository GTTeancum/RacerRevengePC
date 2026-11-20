import sys; sys.path.insert(0, r'D:/Programming/GitHub/RacerRevengePC/.claude/worktrees/agent-a931b4f26f2a94abf/work/scratch/147c')
from res import res
def f(n, a, b):
    assert 'consumedSaStage.race()' in a
    return a.replace('consumedSaStage.race()', 'consumedCommonRouteStage.race()')
res('tests/native/terrain_interactive_publication50du.cpp', f)
