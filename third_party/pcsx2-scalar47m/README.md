# Borrowed PCSX2 scalar handlers, checkpoint 47M

The user-supplied PCSX2 R5900OpcodeImpl.cpp is preserved byte-for-byte. See
provenance.json for its SHA-256 and the 47 selected non-trapping handlers.
The upstream GPL-3.0-or-later license and copyright notices apply. Full license:
../pcsx2-mmi47k/COPYING.GPLv3. This directory is part of the recoverable source
package, not a separately licensed implementation.

The production generator adds an explicit state argument. The differential
reference retains the original global-state handlers and verbatim bodies.
Both share decoding metadata; independent arithmetic and delay-slot tests and
the direct generated-game-function comparison supply additional checks.
SA uses byte units here; the initial native leaf eligibility excludes all SA
instructions and QFSRV rather than assuming the legacy convention matches.

This is not the full EE, a proven 64 MiB game build, or an Xbox gameplay result.
