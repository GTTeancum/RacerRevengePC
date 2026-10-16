# Tournament stage16 aggregate resource review

2026-10-14, source-only root review against applied stage16 on 089ec54. No builds, tests, syntax checks, decoder executions or game launches.

The ATM extension retains one immutable resolver file and one aggregate cache association. Embedded frame Entries borrow the same stable heap File; moving entry vectors does not move that File. Each child carries its member-relative PST extent and palette offset, so images are not fabricated provider rows. Structural range checks prevent begin+extent overflow through the existing range predicate; palette and level reads remain within each frame's exact extent. Original ordinary PST level order remains unchanged. ATM intentionally admits only single-mip children.

Aggregate allocation skips ordinary registration and owns the actual child prefix. Each child starts with one numeric reference. Reserved vector storage and shared_ptr move preserve the allocated child through a later construction failure. Aggregate teardown releases each child numeric reference before dropping its backing, in ascending order, then clears its own cache. Failed child construction may remain partially registered and is cleaned through the existing ordinary destructor. Ordinary texture lifetime, palette conversion and backing publication are preserved.

Provider fallback requires affirmative PST/IPU/TGA absence before selecting ATM from the same sealed revision. The aggregate cannot supply ordinary format, mip, flags or palette fields. Screen prototype binding selects a real child before ordinary field consumption. General demand on an aggregate remains explicitly unsupported; full Tournament frontend admission remains closed.

No blocking aggregate-resource defect found by manual source inspection. This is not runtime validation. Clock reviewer found a separate composite service failure-contract gap: sampleCanonicalMarker can mutate controller state and then fail binding/image correspondence without a terminal latch. Stage17 correction requested before integration. No speedup, complete Tournament readiness or paused presentation is claimed.

Root applied stage17 correction after source review: first post-effect failure latches this concrete resource owner; repeated calls return the retained diagnostic before clock/reference/projection, and current() rejects. No execution checks.
