# Relink the observer only; require section-byte equality before applying this
# map to the already frozen 47O executable.
target_link_options(racer_gsprofile47o_fixed_probe PRIVATE "/MAP:${RR_SHARED_ROOT}/build/gsProfile47O/profile47p.map")
