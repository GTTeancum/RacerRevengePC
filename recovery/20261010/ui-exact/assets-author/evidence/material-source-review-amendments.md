# Source-only review amendments

Independent source review confirmed the 35 constructor defaults, template and
pass getter PCs, startup owner identities, inputAlphaD/inputColorD precedence,
pass-local context default and runtime override handling.

Three conservative admission changes were incorporated before freezing:

1. Equal effective minification/magnification texel filters are required. The
   original default MMAG=1/MMIN=5 is linear in either case. MXL=0 does not
   justify blindly exporting disparate MMAG/MMIN to desktop sampling. The
   reviewer additionally identified PCSX2 GSVertexTrace.cpp's tested-PS2
   MXL=0/MMAG-only handling; this packet does not rely on broader admission.
2. Policy validates the actual atlas dimensions as power-of-two16..1024,
   matching the original indexed decoder's admitted subset.
3. Context1 is rejected by native policy. Source 2476A0 emits fixed GS
   context0 register addresses: TEX0=0x06 at247948, ALPHA=0x42 at247AE8,
   CLAMP=0x08 at247C18, FBA=0x4A at247C4C, TEST=0x47 at247F24,
   TEX1=0x14 at248030, ZBUF=0x4E at248094. An authored PRMODE context1
   chooses separately owned context state instead of these writes. The raw
   resolver preserves the authored context but does not misrepresent it as
   an immediately usable native policy.

No compiler, syntax check, tests, importer or original game code was run in
this review. CPU source admission is not original-renderer readiness.
