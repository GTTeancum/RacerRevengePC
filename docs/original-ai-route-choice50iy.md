# Original AI route-choice continuation

Status: separate source-only follow-on to all-AI main1c96fb76 / runtime323d8055. Active candidate remains unchanged for the parent's scheduled test. No build, syntax check, test, gameplay or original execution was performed.

## Joined gameplay path

The former1719B0 boundary for an unremembered multi-successor node is replaced by the actual171A90/1719B0 decision chain. Native navigation17005C and forward desired-position lookahead172234 both call the same selector with the same mutable per-racer1080/1084 and actual application shared RNG. No closest-node, shortest-only, safe-road or authored-Next0 fallback is introduced.

Exactly one successor returns Next0 without consulting flags, choices or RNG. Multiple successors first scan for either existing remembered node in original successor order. A new selected node writes1080 only if it is-1, otherwise1084 only if it is-1; if both are occupied the source still returns its new selection without replacing either.

1719B0 first tries the original shortest-distance selection when genuine FE4 is nonzero. With ordinary nonattack FE8, genuine packing policy1000=4 similarly tries its source shortest-distance helper. These use actual candidate DistanceToFinish60 and source list order, retaining the source-1 sentinel and strict-less tie behavior. A nonnegative result returns immediately; if no candidate qualifies, flow reaches the ordinary draw.

The unjoined attack-state owner1050 is rejected only when reached at171A04 (after any successful FE4 early return). No fabricated attack field or target-route decision is supplied. For the admitted real eight-pod all-AI roster, FE4=0, startup/ordinary FE8 is0/4/2 and genuine no-human packing policy is0, so the original171750 ordinary selector is reached.

## Exact ordinary selector

Each successor gets source weight0 when NodeBlocked79 bit0 is set or Pretty79 bit6 is set; otherwise it gets weight1. SecretBranch79 bit5 by itself is not excluded. The R5900 shift helper2A2620 extracts bit6, consistent with the authored metadata loader. This distinction was independently checked from the actual source fields.

After writing all weights,171750 calls the actual shared308030 stream exactly once, even when no successor is eligible. It converts the masked31-bit integer and eligible count to binary32, multiplies the converted draw by2^-31, then multiplies by the converted count with separately rounded operations. Selection uses threshold<=cumulative weight, preserving every zero-weight position and original order. No modulo draw, filtered eligible list, clamp below1 or extra random draw is used. A draw of0 can select the first zero-weight entry; when all weights are0 and count is positive, the original comparison selects ordinal0. These are original branches, not invented fallbacks.

A zero-successor malformed graph still consumes the source draw and returns-1 from the selector, then stops at the caller's171B28 indexing boundary instead of fabricating a valid next node. Existing native graph/node/successor capacity admission remains explicit. Scratch weights use bounded native stack storage instead of the original transient heap allocation.

## Live ownership and source scheduling

The route context borrows references to actual FE4, FE8 and policy1000 plus the same frame.noise.sharedRandomState used by other original consumers. Navigation runs before the current state and packing updates, so it sees those prior live values. Later far and near lookahead runs after them and reads their updated live values. Neither lookahead may use a stale scalar context or private RNG.

Lookahead may select and store future choices even when gate distance exactly equals remaining horizon. It does not consume the chosen entries. Real gate crossing writesC80, applies the selected node's BoostScale-triggered109C reset, then consumes1080 into1084 in the existing source order. The ordinary steering far-lookahead call precedes the near-lookahead call; their changes and random draws are shared in that order.

Persistent observation-only counters route_selections and route_rng_draws, plus route1080/1084, are exposed in existing per-racer AI telemetry. They do not influence driving. No generated paths or original resource bytes are published.

Other source boundaries, including special driving states, attack target routing, original lap/finish/reset lifecycle and the declared missing GC world triggers/effects, remain unchanged. Source integration is not evidence of a completed track or runtime parity.
