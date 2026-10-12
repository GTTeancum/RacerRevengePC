# Rider initial graph acquisition correction

Source-only correction following the 16:22 UTC Linux failure recorded in recovery/20261012/course-second-integration-receipt.json. Not built or runtime-validated.

The original manager's restart tick submits a closed Rider frame before the following opening callback creates the initial Rider19 graphs. The backend had used any prior submitted frame as its acquisition cutoff, blocking the required first graph population.

The backend now records the actual dynamic-material ledger frontier at authenticated source submission and uses that monotonic value in the four initial-acquisition guards. The total submitted watermark remains unchanged for real renderer retirement and destruction. Initial widgets27/53, exact assets, immutable GPU bank, one-shot graph ownership, collector generation checks and failed-submission poisoning remain enforced. This does not enable other riders or directional selection.

Original source evidence: manager2A3C90 restart branch2A3FB0, later opening notification2A3D8C, aperture1D750C virtualAC, Rider1E9E70 and group entry1E9FDC. Independent source review covered initial population, both material-group updates, GPU bank identity and renderer-first teardown. The nested Rider error fields are now included in failures.

The menu observer's destination and earliest time are advanced to session03; its route and cleanup requirements are unchanged. Next validation remains one centralized Main/Rider19/Course/Back observation, no driving or race launch, after17:22:32.320233 UTC and the current shared ledger check.
