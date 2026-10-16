# Direct TimeStats text geometry: isolated native candidate

Static-data/source review only. No build, syntax check, test, decoder run, runtime activation, or GPU readiness claim. Candidate is `src/game/original_time_stats50la.h`; root owns integration.

## Proven admitted subset

TimeStats uses six direct ScreenText objects, 40 glyph capacity each, default scale40=1, rotation44=0, depth48=1, justification0 for labels and4 for values. The 13ingame Application font Entry is the actual metrics owner. Independent alpha58=255 gives packet63; packed FFE5D96C gives RGB114/108/54 through133F20. The 16ingame font remains a separate authentic registry/resource-demand borrow, not an invented text material.

The existing `layoutOriginalUiText50KG` implements the numeric glyph leaf2E0A30. Its real Texture48B input must contain the genuine same-PST decoded pixels; empty dimensions-only or generated atlas substitutes fail its contract. The candidate privately pins an immutable OriginalUiScreenAssets50KG bridge, verifies its PST/CEL identity pair, whole-member range, and canonical font texture dimensions. Geometry uses the Application Entry metrics, not the bridge's separately decoded CEL object. This bridge never acquires canonical resource references and does not certify material/GPU readiness. Root must construct it from actual authenticated OVERLAYS member views, using the existing decoder.

## Exact order and arithmetic

- 2E0F50 first calls2E0880. With dirty bit1,2E0914..0988 sets active batches to ceil(characterCount/36);2E098C..0A04 sets each active batch's glyph count36 then remainder. Capacity40 therefore owns two batches36/4 even when neither is active.
- If bit0 is set,2E13A0 positions text. Justification0 or empty string copies x10/y14 to positioned20/24 at2E1444..1454.
- Nonempty justified text calls width2E16F0 at2E145C and height2E15B0 at2E146C. Justification4 computes float(x10−float(integerWidth)), truncates to signed32, then converts back to float at2E1534..1544; y14 is copied unchanged. It is not a bounding-box width, and fractional negative x must truncate toward zero rather than floor.
- Positioning sets bit1, clears bit0, sets bit2 at2E1550..1594.
- 2E0A80 obtains the font's real texture via281F90;2E0A88/8C reads its source width14/height18. If bit2,2E0AD0/DC divides positioned coordinates by source scale and clears bit2. Scale1 makes this numerically identity in the admitted finite domain.
- Pixel-metric glyph branch2E0B20..0D14 advances by signed bearing, subtracts0.5 from x/y, uses widthMinusOne+1/heightMinusOne+1, applies u*(width−1)/width and v*(height−1)/height, then adds advanceAfterBearing. UV-only branch2E0D18..0E70 preserves raw UVs, advances by atlasWidth*(u1−u0), and sets right=advancedX−1. Existing helper preserves both branches and rejects mixed metric runs and missing glyphs rather than reproducing guest assertions.
- 2E10DC..1110 computes depth: negative depth uses its negation; positive depth uses actualVideo+60 minus depth48. Candidate admits authentic default depth1 and the actual native Video depth16777215, giving16777214.
- Rotation0 skips trigonometry/pivot work.2E1240..1274 multiplies coordinates by source scale1. The two source opposite corners expand into native TL,TR,BR,BL quads without UV remap or Y flip. 2E12E8..1300 clears bit1.
- Before submission, source copies actual viewport dimensions1E8/1EC into primitive transform state. This belongs to root's eventual renderer join and is not supplied by the CPU geometry candidate.

## Candidate ownership and failure behavior

The state kernel owns the single mutable string backing, including its terminator and length. Each TextResource borrows that exact state. Its two TextBatch objects borrow its fixed40 native glyph array. Each geometry pointer is attached BEFORE material acquisition, matching the original partial-failure attachment chronology. Private owner destruction clears batch borrows/releases references before the fixed storage dies; no renderer borrow API exists yet.

The owner retains authentic canonical domain/context, six stats references, both Application font Entry borrows, and two canonical font material references per text. Stats acquisitions invoke exact14D370 then29E340. Font loading remains an actual Application call between stats and text stages. Text construction does not populate pending materials.

CPU geometry validation completes before publishing new coordinates/active counts. Rejected finite-domain/native inputs terminate this isolated owner with diagnostics and retained prior canonical partial effects; they do not publish half-updated quads. This is an explicitly narrower native failure contract, not a claim to reproduce guest assertion recovery. Successful numeric writes preserve the recovered arithmetic. Dirty-mask transformations retain high bits.

The synchronous Record projection now borrows four individual scalar floats CB0/CB4/CB8/CBC plus CC8/CCC/CD8. Root must bind these to a coherently owned scalar snapshot populated by actual Bank read handles, never references to temporary `Bank::read` return values. It is not a second timing owner.

## Later material/collector phase

Race loader1A6AA0 calls HUD construction through virtual+74 at1A74D4, resolving1A8A40, then additional UI1A9100 at1A74DC. Later1A7608 calls1AA5A0, which executes14D370 at1AA5AC then29E340 at1AA5B8. Later in the same race loader,1A7B14 invokes14D440 before return1A7B34. Those are genuine later population/collector boundaries. Intervening UI may also populate tables; no claim is made that1AA5A0 is the first possible population.

## Remaining root integration boundaries

1. Complete the authentic stats ScreenTexture native numeric storage from root's recovered contract, retaining its real primitive/material references and dirty state.
2. Add actual frame projection and call order: source layout/prepare, texture rows, then each visible label and computed value; preserve active/sliding gap asymmetry and literal glyph bytes.
3. Run the genuine later canonical population/collector stage, then resolve each pending font material and verify its authentic PSM/PST/font texture identity before renderer publication. Constructed references alone cannot certify this.
4. Feed actual viewport and canonical collector registration/finalLeaf ordering into the renderer; do not substitute hardcoded registry ranks. Root authenticated stats3051/font3189 source material order, but collector redirect history remains authoritative.
5. Add renderer borrow retirement before mutation/destruction or Video close. No current candidate method allows renderer borrowing.
6. Join the narrow actual resource-demand floor194E20 and actual source lifecycle; never use a pixel-bank owner as canonical demand ownership.

Central domain refactor was source-reviewed: the private pair is14D370 then29E340; publicpopulate reuses that pair and still performs its original14D440. No source-proven ordering regression found. No performance or runtime claim.

## ScreenTexture addition from root's independent proof

The isolated owner now also implements private native stats-quad initialization/regeneration. `stageStats193240` accepts actual Video+60 and rejects values outside the proven16777215 domain. After each genuine stats acquisition/population/resolution, it allocates native quad storage, sets RGB127/127/127 and alpha63 from loader2DF384..2DF4E8, depthVideo60−1, authored dimensions149/32 by pair and19height, and the exact authored UV bounds recorded above. Dimension changes mark bit0. Rotation remains0 and scale remains1.

`prepareStatsGeometry2DF550` rejects nonzero rotation and nonfinite inputs. If dirty, it computes width*1,height*1, right=x+width,bottom=y+height following2DF694..2DF6C0; writes native TL,TR,BR,BL from source corner order0,1,3,2 (source2DF80C..2DF848); clears bit0 as2DF84C..860 does. It does not copy viewport state, submit246760, claim GPU readiness, or admit the unrecovered rotated path. Source draw gates and actual640x448 viewport admission remain root duties.

This resolves item1 of the earlier remaining-boundary list for the proven unrotated TimeStats subset. Root integration still needs actual collector/material resolution, frame projection, viewport/submission and renderer retirement.
