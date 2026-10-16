# RA lighting: bounded original loader contract

Static source research only. No runtime admission or lighting readiness is supplied. Sky selection was delivered independently in ra-sky-candidate and integrated by root.

## Actual iteration and missing sections

Authenticated RA SCB declares NumberOfLights=185. It contains 185 light-named sections, but Light90 and Light107 are absent, and Light186 and Light187 exist. Enumerating all named sections would produce the wrong original construction inputs.

2782B8 selects Lights. 2782E0 reads NumberOfLights. 278300..278320 constructs that many 12-byte reader wrappers. 2783D0 initializes index zero; 2783DC..278408 invokes wrapper load using the SAME parser and indices 0..184. Wrapper277D10 allocates its reader and forwards that parser/index at277D50..277D60. Reader277390 formats Light%d(index+1), calls157D00 at2773CC, and ignores its return before reading Type at2773EC.

157D00 calls section search157AF0. A missing search returns -1, and157DD0..157DDC returns false before ANY selected-section fields are replaced. Packed mode replaces selected name at157E88..157E9C, selected bytes at157EA0..157F50, and selected field count at157F38, only on success. Text mode clears/replaces fields at157F90..157FC4 and rebuilds them through158214, also only on success. Section search does not write selected data. Value lookup158240 consumes the retained selected count and buffer. Thus failure does not mean an empty block or defaults.

Exact effective source mapping:
- Runtime numeric slot90 loads Light89 again: RGB -80,-80,-80; zone6; position6301.121,2276.681,6463.46; look0,-1,0; cone60/90; attenuation1.0,0.000001,0.000002.
- Runtime numeric slot107 loads Light106 again: RGB186,186,165; zone6; position5158.353,2312.805,7683.679; look0,-1,0; cone60/90; same attenuation.
- Light186 and Light187 are never reached by this185-record loop.
- Every other numeric slot uses its same-numbered source block.

Keep requested numeric slot and effective source block identity separate. Do not collapse duplicated objects, substitute empty fields, renumber records, or load beyond185. Effective counts remain4 Ambient,1 Directional,15 ObjectPoint,165 Spotlight because the two excluded and two duplicated records are all spotlights. Equal counts do not prove equal source identity.

## Defaults and actual zero cone

Type string fallback is literal none at3BA330.277B70 maps Ambient4, Directional2, ObjectPoint1, Spotlight3; unknown0 diagnoses and falls back4 at27748C. These unknown/default paths do not apply to missing90/107 because their predecessor has a valid Type.

InitialZone defaults1 at2774C8 then decrements at2774FC; reader defaults inner30 at277694, outer45 at2776B0. RA129 explicitly overrides outer to0, so default45 must not be substituted.

Light129 is the in-range relevant block: Spotlight, RGB -80,-80,-80, zone7, position7969.993,2862.167,4864.221, look0,-1,0, inner60, outer0, attenuation1.0,0.000001,0.000002.

277FAC..277FB8 forwards authored inner/outer to146F30. That setter stores two independent binary32 products with constant0x3C8EFA34 (0.01745329052209854) to light+98/+9C. There is no inner<outer rejection or clamping.

149F20 computes the cone denominator penalty.149F54..149F5C initializes penalty constant5.149FE4..149FF4 evaluates cosine of outerRadians*0.5. Word4615B034 at149FFC is ordered C.OLT.S f22,f21. If dot<outerCos,14A00C..14A014 selects5. Otherwise14A018..14A028 evaluates inner cosine; word4600B036 at14A02C is ordered C.OLE.S f22,f0. If dot>innerCos,14A03C..14A048 selects1. Otherwise blend follows14A050..14A074. This is a denominator penalty, not zeroed illumination.

For authored outer0, inner60, finite dot<1 yields penalty5. Finite dot>=1 yields penalty1, since innerCos<1. The interpolation division is not reached. Do not clamp dot to manufacture a wider cone, reject the source, or replace0 with45. Preserve negative RGB and source selection order.

Existing native selectAuthored uses the same ordered branch shape and can represent this input once its RA cold parser admits it. Existing MA/SA cold guards should remain unchanged. Existing cosine initialization uses one product with pi/360; original uses degrees*0x3C8EFA34 then *0.5 followed by13BBA0. Exact bit-parity requires retaining these stages and checking the existing native mathematical boundary, not claiming them equivalent.

## Smallest next implementation boundary

Extend the existing actual lighting Owner through a dedicated authenticated RA preparation path, retaining its real CameraVisibilityLease/DrawLease/world and renderer bindings. Do not create a separate ambient override or diagnostic light bank.

The RA parser must iterate185 requested slots and resolve the two proven retained-selector aliases. Preserve runtime slot order, all signed RGB values, exact zone assignments and defaults. Existing240-local-light storage is sufficient for180 locals. Real visibility/contact/camera ownership remains required. Four ambient records are Light1 global and173/174/175 zones1/2/8; direct Lights directional overrides for those zones are20,20,20. Keep global ambient separate from per-zone/negative-light-adjusted rows.

No lighting implementation is promoted here. Remaining work is the dedicated RA path, exact constructor/default handling audit, and source/native cosine boundary. Existing native authored-zone/top-two query is an explicit host policy, not proof of original Light+D0/manager+88 traversal. Lens-flare resources and actual sky renderer remain separate owners.
