# Original selectable planets and the Malastare request

Static original-data inspection, 2026-10-14. No test or gameplay run.

The original `UI/UICourse/track.dat` labels Group 1 the Full Track List.
Its 13 controls map to five planets:

- Tatooine: The Mos Espa Open, The Badlands, Boonta Eve Classic.
- Sullust: Soro Suub Facility, Serres Sarrano.
- Mon Calamari: Orotoru G'am, The Ballast Complex, The Grand Reefs.
- Gamorr: Watchtower Run, Ruins of Carnuss Gorguul, The Citadel.
- Ryloth: The Brightlands, The Nightlands.

Malastare is absent from this original selection catalog. MA is Mon Calamari:
Control 5 authors `CourseIndex=4`, `Planet="mon calamari"`, and `WidgetID2=32`.
Widget 32 names `STR_TRACK_MON_CALAMARI_A`; original English localization
resolves it to `OROTORU G'AM`. Archive abbreviations must never be expanded
into an assumed planet name.

The native parser's six-name planet-index array includes Coruscant because
that label exists in the original domain. It is not a six-planet availability
list: no Coruscant course belongs to Group 1. Likewise, the unused Sullust B
control and localized The Mining Sector do not belong to that 13-course list.
Preserve original numeric planet indices; do not remove the unused label and
shift the later indices merely to make the parser resemble the available list.

Original identities freshly verified by the course-source worker:

- UICOURSE.RES: 1,227,424 bytes;
  f45585dd7fe7ba2a24cfc5b6ce4d3fb5efa6bc279f636e071aecf5ccf970c855
- track.dat: 4,706 bytes;
  d445012d2342ba33c685e3b2b1f78e3b2d648c4d8057fa1427d5f58639ea7268
- Widgets.Dat: 12,846 bytes;
  3ea165fa6bf0071786e454cf79982f0f5e438694d98264d724ff7c29a1ec75e6
- LOC.RES: 35,925 bytes;
  b02e9c437311fd18b9dcd52c0d02222320be4eb7e5bfa17a60f564b05e6d4a0e
- English UI.loc: 17,688 bytes;
  c57ac0eebc9eba4742d7b9bc4a840ca6bb3de23b673353f7cc558d15d229f9b3
- L_UI_ENG.bin: 10,637 bytes;
  48fa45093b373dc587637fb7656ad6cb7fb9c4a5d19956cd5b4c427887f1f4d2

This establishes original course identity only. It does not establish native
runtime readiness, profile unlocks, or Gasgano readiness. The user's requested
Gasgano/Malastare test cannot be represented by relabeling or substituting a
different course. Root has asked for a real selectable planet and continues
independent Gasgano preparation while awaiting that choice.
