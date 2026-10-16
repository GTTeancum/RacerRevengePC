# TimeStats ScreenTexture host geometry subset

Static original-byte inspection only; no game execution, conversion or render.

Constructor2DEF40 sets scale30=1, rotation3C=0, depth40=1, packedRGB44=0x00FFFFFF, alpha48=255, zeroes position10/14 and origin20/24, clears dirty bits0/1 at5C. TimeStats loader changes dimensions34/38 and atlasUVs but does not alter scale/rotation/depth/color/alpha.

2DF0B0 obtains canonical material/texture, derives reciprocal atlas dimensions4C/50 and original width34/height38, allocates owned buffer58, then writes four native-equivalent corners. 2DF384–2DF4E8 initializes each corner RGB from independent packed channels shifted right1, alpha from signed16 arithmetic right2. In this subset:127,127,127,63. Z is actual Video+60 converted to float minus source depth40. Current authenticated native Video16777215 gives16777214, not a guessed HUD depth. W/Q are1.

Source corner order:TL,TR,BL,BR. Position byte offsets50/54,80/84,B0/B4,E0/E4. UV offsets60/64,90/94,C0/C4,F0/F4. InitialUVs0/1 are overwritten by TimeStats: left panelU0.5,U1.78125;rightU0.7890625,U1=1;bothV0.796875,V1.9375. Native TL,TR,BR,BL vertex order must remap original corners0,1,3,2, not flip either atlas axis.

At draw2DF550 the original source render-enabled gate383678 must be live. It copies actual selected viewport width1E8 andheight1EC into draw metadata. Only originalMain actual640x448 single-view origin0,0 is presently established for native rendering; this is an admission limit, not source split-screen parity.

If5C bit0 is set,2DF694–2DF6C0 computes scaledWidth=width34*scale30,scaledHeight=height38*scale30,right=x10+scaledWidth,bottom=y14+scaledHeight. With rotation bit1 clear,2DF80C–2DF848 writes the four unrotated corners. Source then clears bit0 at2DF84C–860 and submits actual primitive54 through246760 at2DF874. A clean quad skips regeneration but still submits. Rotation path2DF6C4–2DF808 stays outside this TimeStats subset; its unknown decoded operations must not be guessed. TimeStats never enables rotation.

Native implementation should retain actual CPU vertex storage and material references, with these source-equivalent mutations. Guest packet byte layouts and guest instruction execution are unnecessary and prohibited for the implementation. These observations do not alone constitute a drawable owner or GPU publication.
