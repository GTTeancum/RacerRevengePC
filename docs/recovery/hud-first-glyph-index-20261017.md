# Retain the lowest present font slot during CEL decoding

UNTESTED. No build, syntax check, test, smoke or benchmark executed.

The font decoder already visits and validates every original CEL glyph. A private
uint16 index, initialized to sentinel 256, is updated with the minimum accepted
ASCII immediately after the existing present flag write. The decoder still works
on its fresh private temporary and publishes only after every glyph, optional
metric and trailing-byte check succeeds. Failed decoding preserves the old font.

A const accessor returns null for undecoded/empty fonts and otherwise returns the
indexed glyph in that font's own storage. Default copying copies both arrays and
index, so no self-pointer lifetime issue is added. Optional metrics cannot add
presence or change UVs; arbitrary CEL record order still chooses the lowest ASCII.

Text layout replaces only its first-present-slot search. Exact UV subtraction,
atlas-height multiplication, finite failure check and empty-font -1 line-height
sentinel are retained. For a nonempty font the prior search made lowestASCII+1
lookups; for an empty font it made256. The new accessor does constant work. Actual
font distributions and timing have not been measured.

Root inspected all23 src/platform type references and searched repository source
for sizeof/offsetof/raw copying/reinterpretation/serialization uses; no binary
layout dependency was found. This changes private native metadata, not CEL disk
format. Rebuild all native consumers; no external precompiled ABI guarantee.
Two independent design reviews accepted the invariants. Successor validation
should cover out-of-order slots, ASCII0/255, empty fonts, copies, failed redecode,
optional metric errors, and identical line-height/layout outputs.
