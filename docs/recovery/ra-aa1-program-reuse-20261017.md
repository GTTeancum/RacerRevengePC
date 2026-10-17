# Reuse the direct AA1 RA program within a selected draw

UNTESTED. Source-only review; no execution or measured FPS gain.

bindRaWorldMaterial now binds its program when uploadUniforms is true or program
is zero. Its only callsites are the AA0 blend path with program0 and the direct
path with program=blend?1:0. Thus program1 implies a blended AA1 selected draw.

A selected draw fixes blend/AA mode, resets firstPrimitive=true, and passes that
flag as uploadUniforms. It advances false only after the primitive error gate
succeeds. Later admitted primitives retain program1: snapshot/copy operations,
texture/framebuffer binds, coverage/depth setup and draw calls do not switch
programs. This direct branch never invokes the coverage composition program.
The loop is synchronous without callbacks or program relinking. Skips cannot
advance the flag and snapshot or primitive failure terminates drawing.

All program0 binds remain, including AA0 payload draws whose composition does
switch programs. Every new selected draw still binds its program before uniform
writes; all existing error checks and final restoration remain unchanged. The
helper is private and requires re-audit if additional callers/programs appear.

This removes one program bind per later successful blended-AA1 primitive in a
selected draw. Opaque draws execute once per selected draw and receive no saving.
A successor may validate alternating AA0/AA1 surfaces and rejection cleanup,
then measure actual eligible incidence and performance.
