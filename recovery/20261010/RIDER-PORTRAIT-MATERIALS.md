# Authentic Rider portrait ScreenMT construction

Source-only continuation from integrated stat fallback5782af7, with exact preimages. No project parser/build/syntax check/test.

Original1E8D54..1E8D74 omits riders5,6,13 from the1..26 loop.1E9418..1E9428 formats exact string3AF720 `%02d_flrg`;1E9448 constructs ScreenTexture at page1920+rider*60 through2DF0B0, whose explicit ScreenMT owner is3833F0. All23 actual portraits exist as PSTs in authenticated UIRIDER; no corresponding PSMs exist. Each needs the same actual2475C0 null-config material path as the panels, rather than invented shader metadata.

This narrow continuation adds an independently typed portrait fallback origin and function, constrained to those23 exact names/canonical member identities and source caller1E9448. Existing stat3 helper and all other material branches are preserved. The common ScreenMT fallback builder remains shared. Portrait RGB/alpha initialization and live sprite rendering remain original ScreenTexture-owned; this material function does not apply guessed profile tint.

Static evidence is the already frozen sibling stat-material-fallback/evidence/create-static.txt (1E8D54 and1E9418 ranges) and sprite-create-static.txt. New packet or destination readiness is not claimed.
