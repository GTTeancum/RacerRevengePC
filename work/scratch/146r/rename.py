# One-shot identifier rename: the SA race chain becomes the common route (146r).
import pathlib, re, sys
WT = pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a242116fa032b3d0b')
M = {
 'isSaTrack': 'commonRouteTrack', 'isSaTrackType': 'commonRouteTrackType',
 'isMaSaTrack': 'maOrCommonRouteTrack', 'isMaSaTrackType': 'maOrCommonRouteTrackType',
 'saOriginalDisc': 'commonRouteOriginalDisc', 'saOriginalDiscType': 'commonRouteOriginalDiscType',
 'nativeSaSelection': 'nativeCommonRouteSelection', 'nativeSaSelectionType': 'nativeCommonRouteSelectionType',
 'saOriginalWorld': 'commonRouteWorld', 'saOriginalWorldType': 'commonRouteWorldType',
 'consumedSaStage': 'consumedCommonRouteStage', 'consumedSaStageType': 'consumedCommonRouteStageType',
 'original_consumed_sa_stage50DU': 'original_consumed_common_route_stage50DU',
 'sa_original_world50LM': 'common_route_world50LM',
 'saWorld74_': 'commonRouteWorld74_', 'saWorldCounted_': 'commonRouteWorldCounted_',
 'saWorldEntity_': 'commonRouteWorldEntity_', 'saWorldPrefixComplete_': 'commonRouteWorldPrefixComplete_',
 'saWorldListCount50C_': 'commonRouteWorldListCount50C_', 'saWorldList500_': 'commonRouteWorldList500_',
 'saWorldMode524_': 'commonRouteWorldMode524_', 'saWorldMode30_': 'commonRouteWorldMode30_',
 'saWorldListGrowth508_': 'commonRouteWorldListGrowth508_', 'saWorldListCapacity504_': 'commonRouteWorldListCapacity504_',
 'saUseEnvironment35C_': 'commonRouteUseEnvironment35C_',
 'retireNativeSaWorldPrefix': 'retireNativeCommonRouteWorldPrefix',
 'constructSaWorld1A6BC0': 'constructCommonRouteWorld1A6BC0',
 'constructConsumedSaRaceWorld1A6BC0': 'constructConsumedCommonRouteWorld1A6BC0',
 'saTriggerActions': 'commonRouteTriggerActions', 'saTriggerActionsType': 'commonRouteTriggerActionsType',
 'saTriggerAuthoring': 'commonRouteTriggerAuthoring', 'saTriggerAuthoringType': 'commonRouteTriggerAuthoringType',
 'saTriggerMembership': 'commonRouteTriggerMembership', 'saTriggerMembershipType': 'commonRouteTriggerMembershipType',
 'saTriggerCandidateFirst': 'commonRouteTriggerCandidateFirst', 'saTriggerCandidateFirstType': 'commonRouteTriggerCandidateFirstType',
 'nativeSaWorldBacking': 'nativeCommonRouteWorldBacking',
 'createNativeSaCameraSession50LA': 'createNativeCommonRouteCameraSession50LA',
 'prepareSaNative': 'prepareCommonRouteNative',
 'saRace': 'commonRouteRace', 'saWorld': 'commonRouteWorldPin', 'saMap': 'commonRouteMap',
 'maSaAtm': 'maCommonRouteAtm', 'maSaAtmType': 'maCommonRouteAtmType',
}
rx = re.compile(r'\b(' + '|'.join(sorted(map(re.escape, M), key=len, reverse=True)) + r')\b')
dry = '--dry' in sys.argv
changed = 0
for top in ('src', 'tests', 'platform'):
    for p in (WT / top).rglob('*'):
        if not p.is_file() or p.suffix not in ('.h', '.hpp', '.cpp', '.inl', '.inc'):
            continue
        b = p.read_bytes()
        try:
            t = b.decode('utf-8')
        except UnicodeDecodeError:
            t = b.decode('latin-1')
        n, k = rx.subn(lambda m: M[m.group(1)], t)
        if k:
            changed += 1
            print(k, p.relative_to(WT))
            if not dry:
                p.write_bytes(n.encode('utf-8') if b.decode('utf-8', 'ignore') == t else n.encode('latin-1'))
print('files', changed)
