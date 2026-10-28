# Canonical fidelity reset: compile/link checkpoint (2026-10-28)

Windows and Linux compilation and linking passed against the same frozen source snapshot. Neither executable was launched: no runtime, menu, gameplay, performance, full-lap or campaign verification is claimed for this checkpoint.

- Windows: 612 successful translation units, 611 validated reused objects and 1 freshly compiled object. AMD64 PE32+ executable: 84,684,222 bytes; SHA256 `1f2493b1799855bea0f9aadb6e765c73545cc4594c03f257407e163c8b4f1b31`.
- Linux: 607 successful translation units, 200 validated reused objects and 407 freshly compiled objects. x86-64 ELF64 PIE executable: 48,953,016 bytes; SHA256 `0d7e8bc75ab3023646c6538db8565069d9e3c1d16e45b378a85c533fad7795c5`.
- Both links exited zero. Successful object reuse required matching compiler, normalized ordered arguments, source dependencies and recorded object hashes. External installed headers were qualified against the current unchanged baseline, not retroactively proven at their original compilation times.
- Repairs addressed declarations/includes, private ownership access, a required-field scalar parser and registration of the existing vehicle contact translation unit. This does not establish functional or retail parity.

Canonical work includes World ownership/loading, Results and UI ownership, and ordered vehicle/contact physics phases. Remaining limitations include the outer Race loading barrier, unsupported original geometry paths, incomplete restart/End lifecycle and later secondary-body integration. Separate secondary target/restoration and damping proposals are not included in this built checkpoint.

This publication contains source and documentation only, not original game assets, runtime packs, executables or private build evidence. Development returns to source-only work after this checkpoint; further builds or runtime verification require user authorization.
