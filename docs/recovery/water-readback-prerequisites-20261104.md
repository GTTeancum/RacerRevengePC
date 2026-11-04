# Water transfer CPU prerequisites

These inactive, explicitly native numeric leaves reproduce original CPU work before/after the water GS-to-memory transfer. Neither is connected to startup, rendering, live water resources or Video completion. No compilation, tests or runtime have been performed.

## Packet and callback argument arithmetic

`original_water_readback_packet2b80d0_native50la.h` emits the exact eight-qword2B81A8..2B8288 packet: CNT QWC6; EOP GIF A+D NLOOP5; BITBLTBUF50; TRXREG52; TRXPOS51; FINISH61 with data0; TRXDIR53 with data1; END. Frame FINISH25AB50 uses data1 and is a different producer.

All inputs are raw32-bit source words, not verified source owners. Arithmetic preserves low32 multiply, signed division toward zero, arithmetic versus logical shifts, sign extension, unsigned wrapping and unmasked64-bit OR. In particular a negative source base, width or x can affect upper packet lanes through sign extension. No convenient GS bit masks are substituted.

The calculated receive-QWC is separate from the six-qword DMA CNT length. The source alignment predicate is exposed as numeric data only. Its shared assertion state443E30 and assertion handler are not executed or inferred. A checked native count-only byte extent additionally requires positive dimensions/byte size, nonoverflowing signed byte product, exact whole qwords and the source eight-qword alignment predicate. Failure of this separate native check never changes the exact numeric packet or original QWC result. It does not validate pixel layout, a real allocation, alignment of a host pointer, or source readiness.

Color argument planning preserves the exact low16 sign extension of(Video262E90<<5), pitch640,128x128 transfer, camera x+443C88−1 and y, PSM0 and four bytes per pixel. Depth planning preserves base443CD0, pitch640,width92,height4*trunc((columnsDC+3)/4), camera x+443CF0+92*index, y, PSM48 and four bytes per pixel. It does not choose branches or derive an authorized index from443DC8.

The depth destination stride is formed from unrounded columnsDC through the original32-bit shift/add sequence. For bounded ordinary inputs it is368*columns per band, while transfer length is368*roundedHeight. The planner preserves this difference, including potential overlap when columns are not divisible four. Numeric wrapped offsets remain observable; the separate native extent rejects wrapped offset arithmetic. The actual factory's normal columns are256,136,112,88,232,208,184,160, but the generic arithmetic does not assume those values.

## Boundary-record byte transform

`original_water_readback_fixup2b4778_native50la.h` implements the bounded contiguous nonwrapped memory transform at2B4778..4814. It preflights the complete native span before any write, then visits each band's final three368-byte records in original order. Each record gets exactly240 zero bytes at+0x10 and five qwords of four0x00000FFF lanes at+0x150,+0x140,+0x130,+0x120,+0x110. All other bytes and allocation suffixes are preserved.

Zero bands perform no writes or column/storage admission. Positive bands require columns>=3 and a representable nonwrapped native record span. Rejected input changes no bytes. This narrower native storage policy does not model wrapped guest pointers. The result describes only the byte transform; it neither asserts that preserved input bytes are known nor proves readback completion. No caller-supplied completion Boolean is accepted.

## Distinct resource owners

Water packet443D98 belongs to water-render constructor2B1E60: it allocates a32-byte2592D0 object with64-qword/1024-byte backing, and destructor2B239C clears it. This is not Video+88 or the existing native24-qword encoder backing.

Depth destination443CE8 is geometry provider+140, allocated128-aligned with368*(columns+1)*bands bytes and published at2B8764. It is not the120-cell native wake lattice or a signature texture. The new numeric artifacts own neither allocation and never mask a host pointer into a guest address.

## Authentic completion remains unjoined

After packet construction the source must still write CSR=2, execute2596A0(1,0), poll GS FINISH, set VIF1_STAT/BUSDIR and start actual VIF1 receive DMA. Callback2B4500 waits using242D60's actual VIF1 CHCR status. Continuation2B46D0 first performs its own hardware resets, then schedules later depth bands or applies the terminal fixup, optional Video clear and callback removal. Those effects and their live443DC8 producer are absent from these leaves.

The original packet owner, geometry/readback allocation, real water render caller, Video mode/base fields, DMA/GS ownership and callback installation/removal still need an authentic join. Native GLsync completion, the existing native water presentation policy and these numeric computations do not provide that join, CSR/INTC/frame readiness or sky initialization.
