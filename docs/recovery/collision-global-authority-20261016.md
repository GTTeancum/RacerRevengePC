# One process collision invalidation byte

Source-only migration. No build, test, syntax check, decoder execution or gameplay was performed.

Source11B570 changes query F8 flags and clears process byte3E0730 at11B5F4 before its141C20 tail return. Existing native Follow, death-camera and wreck paths already write the represented byte at their reached constructor boundaries. Their statements and addresses are preserved through a reference member.

## Ownership

OriginalApplicationEntities50LA owns the byte with process startup value zero. The authenticated entry100008..100030 clears BSS from003D5A00 through0044F600, covering003E0730. Each track_runtime_storage50IG::Storage strongly retains that same actual Application as its first member and binds globalValid3E0730 by reference to the private byte. Constructor admission validates the supplied live Application before any dereference. No default Storage constructor can create a replacement byte. All Storage construction paths now require this owner, including the legacy driver, RA retained roster, raw RA runtime entry, and closed static registry.

The strong Application retain is initialized before the reference and outlives every Storage member during destruction. ApplicationEntities has no owning pointer back to Storage or its aggregate, so this adds no owner cycle. The native driver's existing process Application outlives serial sessions. Replacing a Storage does not reset the byte or invalidate its process identity. Existing query lifetimes still must end before their Storage is destroyed; retaining the byte alone is not permission to retain other query backing.

## Reached effects and gates

No write is added during Storage construction, world construction, close, or session replacement. Existing writes in original_follow_query_bridge50kx.h, original_death_camera_query.cpp and original_wreck_queries50la.h continue to address the identical process member. Current pointer-equality guards remain unchanged.

The actual driver passes originalMainApplication.entities() at both active storage creation sites. A cold world already retaining an Application rejects a different process owner at aggregate admission. The scene also retains collisionApplication_ before any storage attempt, including failed attempts; a later counted world constructor checks that pin before effects. Thus storage-before-world and world-before-storage acquisition orders must use the identical process owner. Null or released root admission fails before reference initialization; private constructors retain their own defensive check as well.

This permits a future genuine cold world query constructor to clear the same process field before runtime Storage allocation. It does not execute that constructor, create a collision shape, change type tags, or certify existing registration as source27B040 completion. Candidate type-registry agreement, actual3C4 parent enrollment and normal cleanup remain separate gates.
