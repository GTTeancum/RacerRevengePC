# RA black material: nonzero original texels

Read-only original-byte inspection, 2026-10-15. No project decoder, game, build, syntax test, image rendering or conversion was executed.

Authenticated RA.RES SHA256:
1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2

Member textures/black.pst:
- 2348 bytes
- SHA256 690874b118836a79a08cebfc4f70eeb47d9e32ff96edba46a639ef0549c9c175
- Version 1, kind 1, two mips
- 16x16 mip: all 256 raw indices equal 18
- 32x32 mip: all 1024 raw indices equal 18
- Raw palette entry 10 is RGBA (18,18,18,255)

The existing native indexed decoder's proven kind1 permutation maps source index 18 to palette entry 10. Thus the original texture samples dark gray, not mathematical zero RGB. This follows from raw source bytes and the existing permutation; no decoded pixel artifact was generated here.

The original black.psm is DiffuseCompressed, SHA256 cbdad0b39874455433389179982b16d8f8a132853011bc90607c419c96796755, with 89 static material-table users in the retained source inventory. A zero-RGB argument cannot justify replacing its normal/light path with constant black or prelit vertices. Genuine static-world DiffuseCompressed lowering remains required.
