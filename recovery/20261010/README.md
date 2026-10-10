# Source recovery

The project root is the exact source tree of remote commit51c6e7071df6b54d472f1aa16a86db0e6090f54a. Its Git tree is09dace21929d3802556d48f375163806e8025776. This repository starts new local recovery history; it does not recreate missing commit history.

The subdirectories preserve recovered newer source separately. Their manifests distinguish exact historical hashes, complete but unverified payloads, older variants and isolated work. They are not integrated into the project root. The latest historical local commit64a4aca is not restored.

`ui-renderer-reconstruction` contains NEW untested implementation, not recovered historical bytes. Original UI archives and missing code are still needed for complete screens. No build, syntax check or gameplay test has been run on this recovery branch.
