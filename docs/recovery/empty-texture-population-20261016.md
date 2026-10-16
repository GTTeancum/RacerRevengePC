# Early race population on the actual empty texture table

The consumed race performs1AA5A0's material14D370 and texture29E340 pair before
loading its children. A freshly created canonical domain owns actual empty
material/texture tables and an unprepared texture resolver. The resolver's
prepareFresh explicitly rejects an empty corpus, and prepared means real texture
sources were admitted. Therefore empty initialization must not pretend it has
source bytes or fabricate a prepared resolver.

The domain now reaches the real material scan before demanding a texture corpus.
After existing texture-table terminal/active checks and genuine live/same-context
and resolver-identity guards,29E340 admits its actual zero-row scan without any
resolver lookup or binding. It publishes no resources, visits no rows, and leaves
the resolver unprepared. Every nonempty table retains the existing prepared
resolver requirement and normal source lookup/construction behavior. If the
preceding material scan creates texture rows, those real rows still face all
existing resolver admission gates, with their reached material effects retained.

This closes only the real empty-table prefix. It does not relax missing-source
handling, populate a texture, change archive sealing, or assert full race load.
Static source review only; no builds/tests/execution.
