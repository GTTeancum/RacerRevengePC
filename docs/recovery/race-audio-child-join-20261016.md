# Connected race audio child construction and native failure unwind

Based on d958698. Static source inspection only; no checks or runtime calls.

OriginalRaceConstructor50LA now owns the actual six-group loop1A6B5C..1A6BBC.
For each Effects110, Engines114, Music118, Character11C, Announcer120 and
Auxiliary124 entry, it invokes the SAME RaceAudioGroups2350D0 constructor/load,
retains the actual Entity pointer at1A6B88 and attaches that child to the actual
counted race through canonical141960 with maskFFFFFFFF at1A6BA8. Every load
and attachment precedes the next group constructor. Existing empty group
admission, actual process434A10 allocation, same mixer epoch and thread checks
remain in force. No substitute audio object or logical sound membership is
created. The caller must retain the same audio/mixer owners through rollback.

An explicit native failure operation checks that the race has exactly its
admitted group chain. It removes that chain in reverse allocation order using
actual141A70 and the typed empty235150 release. A group constructed just before
an attachment failure remains owned and can retire without inventing an
attachment. If detach succeeds but release refuses, a retry recognizes the
now-detached final group and does not detach it again. Every failed release
retains its backing and count.

World4D8, if constructed, must be retired by the actual world's explicit native
construction-failure operation before the six groups can retire. A surviving
seventh group is an honest refusal. Race base rollback refuses while the group
borrow survives. Last-child141A70 recomputes the race descendant mask to0;
this reached value is tracked instead of restoring constructorFFFFFFFF.

This composes real constructor and audio ownership, but it does not certify
the complete1A6AA0 callback. Earlier load effects include actual Application22C,
renderer2C,2410E0 distance setting, FIFO allocation and1AA5A0's14D370/29E340
resource operation. Later world/racer/manager/HUD load and normal loaded-world
cleanup remain outstanding. No driver activation, Application28C publication,
parent receipt, scheduler completion or normal1A6840 destruction is added.

The existing audio helper fuses construction and load. Native race slot retention
follows that synchronous helper, whereas source1A6B88 precedes virtual load.
The represented2351F0 load only consumes its own group, mixer and process-slot
owner, with no race-slot observer or reentrant callback. Thus the native slot
is retained before attachment without changing an observed source effect; this
does not authorize future load callbacks to observe an unpublished race slot.
