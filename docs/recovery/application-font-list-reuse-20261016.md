# Reusable actual Application font list after282190

Source-only correction, base0ebb9f1. No runtime call or race-constructor claim.

Original282190 keeps the SAME list owner alive. It advances head00 to the old
node's next04 at2821A8..AC, invokes the font destructor when present at2821B0..D0,
frees the node at2821DC, decrements list count04 at2821E4..EC, and loops until
head00 is null. It does not write a terminal-load state or delete the list owner.
The separate deleting destructor282280 invokes282190 and conditionally frees
the list itself at2822C4. Later lookup282210 reads this same head; load282430
can reach the existing append/load operations on an empty list.

Native close282190 previously added terminal_=true even after ordinary successful
cleanup. That prevented the existing TimeStats/PositionStats/SpeedBoost/Pause
font loads from reusing the Application-owned registry after a race cleanup.

The correction removes only that invented successful-close failure state.
All existing outstanding Entry borrow guards remain before mutation. Pending
state is still released; published entries still retire in original insertion
order; publicationCount becomes zero. The same domain/context/registry object
remain retained. Reached private loaders still require actual live canonical
context, valid archives, healthy material/texture population and correct paths.
No automatic reload, replacement registry or altered resource bytes are added.

Crucially an earlier genuine terminal load failure and terminalDiagnostic remain
unchanged by cleanup. Successful emptying does not repair unsupported source,
parse/allocation/population failures or convert a failed registry to ready.
The destructor can still invoke this idempotent clear before object destruction.

No build, test, syntax check, gameplay or decoder execution was performed.
