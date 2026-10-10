# Original Rider clipping camera

New source-only pair; no builds/tests/syntax/project parsing. Existing camera and
render-context packages remain frozen. Consumes renderContext.cameraAfterFog.

236DD0 updates normal camera matrices and screen140, then237240 derives original
camera334/338. For X, abs(screen140[0]) divided by min(screen140[12]+abs(scaleX),
4095-(screen140[12]+abs(scaleX))); Y uses140[5]/140[13]. The4095 constant is exact
source457FF000. Source uses MIN.S, not guessed viewport clipping. Actual640x448
center2048 gives334=320/1727,338=224/1823 with separately rounded float arithmetic.

236EC0 copies projection80 to290;236EEC..236F74 scales all four X and Y column
lanes by334/338.236F78..236FE4 computes viewC0*clip290 into2D0.237000 repeats this
for near/far callback changes without recomputing viewport guard factors. Therefore
render-boundary preparation after fog must use updated projection near1/far1200.

Global packet24BBAC uploads290 to rows4..7 while24BBDC uploads80 to rows8..11.
Do not substitute either matrix for the other. Renderer owns actual source clip
flags, strip orientation, packet depth, fog, perspective UV and original programs.
This leaf grants no destination readiness or render admission.
