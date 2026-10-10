# Exact author-payload recovery

Three source headers were copied literally from complete conversation outputs and match their retained frozen SHA256 pins. They came from the isolated vegetation-raw-dispatch stage, whose final integration patch pin was6c987a3cc8d9b805b4b1596ba117148ec637259a8a75acea195df60ca557fb42. The author never built, tested or syntax-checked this stage. Recovery does not establish runtime readiness or recover missing dependencies.

The already-recovered type3_raw_cursor50jn.h matches the author's frozen20a61694 pin; it was only hashed, not copied again. The existing audit vegetation_raw_dispatch50jl.h has72bcac42 and differs from the author's85e6e6ec source. Both versions are preserved separately. Their difference is not reconciled or explained by this recovery.

Missing final .cpp bodies and mesh50JJ are listed precisely in recovery-manifest.json. Hashes and summaries do not substitute for source bodies. The earlier cursor.cpp draft is deliberately excluded. No payload was reconstructed from summaries, fragments or transformation commands.

Only permitted isolated source writes and SHA256 metadata comparisons occurred. No tests, builds, syntax checks, Git operations, PC access, active repository edits or guest execution occurred.
