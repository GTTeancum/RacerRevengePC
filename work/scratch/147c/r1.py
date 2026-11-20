import sys; sys.path.insert(0, r'D:/Programming/GitHub/RacerRevengePC/.claude/worktrees/agent-a931b4f26f2a94abf/work/scratch/147c')
from res import res
OLD = 'authoring->eventCount()>rr::assets::originalRaceIntroEventsMax50LA||(authoring->trackId()!="TA"&&authoring->trackId()!="RA"&&authoring->trackId()!="GC"&&authoring->trackId()!="SA"&&authoring->trackId()!="MA"&&authoring->trackId()!="GA"&&authoring->trackId()!="MB"&&authoring->trackId()!="SC"&&authoring->trackId()!="GB"&&authoring->trackId()!="RC"&&authoring->trackId()!="TC"&&authoring->trackId()!="MC"&&authoring->trackId()!="TB")'
NEW = 'authoring->eventCount()>rr::assets::originalRaceIntroEventsMax50LA||!rr::assets::originalRaceTrackIdAdmitted50LA(authoring->trackId())'
def f(n, a, b):
    assert OLD in a
    return a.replace(OLD, NEW).replace('!authoring||!authoring->eventCount()||', '!authoring||')
res('platform/linux/native/native_race_intro_transport50la.inl', f)
