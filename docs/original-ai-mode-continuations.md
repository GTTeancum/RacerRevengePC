# Original AI mode continuations: source-only first slice

## Scope and evidence

This stage extends the existing native Chariot lifecycle and static-impact
projection for genuine vehicle58=1. It is only the mode-dependent continuation
of the cohesive original-AI slice; its frame/controller/cold joins are owned by
the surrounding integration. It does not itself turn on an AI racer.

The active frozenGC53d44876 tree is unchanged. Six source preimages were copied
before staging these changes. Source inspection used the original SLUS_202.68
ELF, SHA-256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30,
through static reads and LLVM disassembly. This stage contains no build, syntax
check, test, gameplay, original instruction execution, emulation, importer run,
ee.bin mutation, held49C change or 48U change. No runtime parity is asserted.

The sibling source-evidence directory contains exact words/disassembly and range
hashes for21A660,222770,223030/2234E0 and1AA684. The GC authoring excerpt is the
actual original gc.tun, not a constructed tuning fixture. Its AI environment
multiplier token is1.0; the code requires that token and never defaults to1.

## Chariot21A660

The existing source prefix is unchanged: lazy shared globals, virtual response,
noise/RNG, physical/virtual display selection and the old-elapsed blend all run
before the mode selector. In particular an AI physical request does not skip a
virtual response or a blend already selected at entry. All the existing graph,
secondary-body, activation, dirty-world and arithmetic admission remains.

At21BEB8 a nonzero vehicle58 selects the AI route. This first slice supports
exactly58=1, rejecting other nonzero modes. It consumes the live application
app28C->B4 byte58, bound explicitly in ChariotLifecycleInput49V. Source21BFEC
DSLL32(30) and21BFF0 DSRL32(31) extract bit1.

- Bit1 clear:21BFFC checks physical838. When zero, copy model1B4 localC0 to
  physical19C, call the original represented secondary activation reset, copy
  main velocity70 and angular velocityB0 including the actual W lanes, clear
  elapsed84C, and set838. Then21C05C/21C064 always write838=1 and83C=0, including
  when838 was already set. Existing blend/virtual state is cleared at this point.
- Bit1 set:21C068 checks83C after the prefix, including any blend expiration.
  Only when83C=0, copy model local to hidden198, derive position relative to194,
  derive orientation from the relative matrix, copy zero434630 into the two
  response velocities, and write838=0/83C=1. Existing83C!=0 preserves838 and all
  seed fields; it is not an unconditional reset.
- Both routes finish by copying probe348+10C to840 at21C0F4. Neither inserts an
  eager physical-world refresh or changes the source stale-world publication.

The ordinary58=0 path retains its speed threshold selector and its existing
activation/virtual-seed behavior. vehicle40!=0, missing model/graph bindings,
(P,V)=(0,0), unbound AI application flags and previously unsupported numeric
conditions fail atomically. This extension is not a proof for state40 recovery.

## Static impact223030 ->222770

The CD8 bit0/state40=3/state40=4 skip remains first and consumes none of the new
AI fields. The admitted non-skipped routes are58=0 or1 with body20C=0 and an
already-ready main scene190. Dynamic body contact partners still return
unsupported; neither CFC nor D34+4 is substituted for body20C.

The original magnitude, inverse direction, normal, strict thresholds, reduction,
directional maxima and independent BA0 coordinate maxima remain. The new AI
branches are:

1. First threshold match wins, even for unsorted authored thresholds. XL/L/M/S
   branch directly to22286C after loading severity. No match also branches
   there. Only an XS match falls through222820.
2. In that XS fallthrough alone, AI58=1 with nonzero vehicleD34->+4 calls2234E0.
   It takes the source scalar maximum of7D0,7D4 and7DC, excluding7D8. Above0.9,
   selected severity becomes zero; equality0.9 keeps it. The intervening source
   MAX.S7DC,7DC is retained. Finite signed-zero ordering uses scalarMax48O.
3. At22286C, AI58=1 with CFC=0 multiplies selected severity by actual track
   tuning+60, even when selected severity is zero. CFC!=0 bypasses this multiply.
   The existing factor(1-CF0) is applied afterward in source order.
4. Rear directionalD18 requires localZ<-0.1, normalizedZ<-0.75 and a value
   exceeding the old D18. Only then, when AI58=1 and CFC!=0, the extra strict
   normalizedZ<-0.85 gate applies. Other directions and unscaledD20/D24 remain
   unchanged. Later BA0 accumulation remains independent of severity.

New input observations are explicitly marked aiOwnersBound. The caller must
observe the real existing CFC, partnerState and damage owners at the call, not
seed parallel persistent copies. Damage values are interpreted only on the XS
partner branch. The multiplier is interpreted only when AI and CFC=0, and then
aiEnvironmentMultiplierBound is mandatory. Arithmetic/unsupported results retain
output and nextVf9W. Copy-only lanes and skipped paths retain the old contract.

## Authoring ownership

The existing readEnvironmentImpact49Y requires the same10 EnvDamage keys and
leaves aiEnvironmentMultiplierBound=false. Thus legacy ordinary callers remain
valid and cannot accidentally pass their zero-initialized extra field as AI
provenance.

New readAIEnvironmentImpact49Y requires those10 keys plus
[PodDamage] AIEnvPointsMultiplier, stores it as aiEnvironmentMultiplier60 and
marks it bound only after all11 values parse. Original1AA790 selects PodDamage;
1AA8E4 references key3AAEA0 and1AA8E8/1AA8EC writes tuning+60. It is distinct
from AIPointsMultiplier at+58. Missing or malformed data fails atomically. The
existing bounded decimal conversion, duplicate rules, section/key limits and
optional token provenance are retained, with11-entry AI provenance.

## Exact integration joins (not edited in this stage)

In graphicsVehicleFrame49W, append the live f.applicationByte58 and a true
binding observation to the ChariotLifecycleInput49V aggregate. The last fields
are now impactBA0, applicationByte58, applicationByte58Bound. Do not invent a
separate manager flag or copy the vehicle58 mode into this byte. This flag must
already have been refreshed from the actual application owner before graphics.

In the unskipped static-impact projection in integrateVehicleMain49W, populate:

    in.vehicleCFC = r.fields.vehicleCFC;
    in.partnerState = r.impactSupplement.partnerState;
    in.damage7D0 = r.fields.damage7D0;
    in.damage7D4 = r.fields.damage7D4;
    in.damage7DC = r.fields.damage7DC;
    in.aiOwnersBound = true; // Only with those actual canonical observations.

Keep bodyContactPartnerCount20C=c.body20C and the current authored CF0 owner.
No new persistent partner/damage fields are needed. The AI reader's returned
StaticImpactTuning49Y must reach p.impactTuning intact, including its bound bit.

At cold authoring, use readAIEnvironmentImpact49Y for genuine originalAI. The
ordinary mode can keep readEnvironmentImpact49Y. Do not invoke the ordinary
reader afterward into the same AI tuning because that intentionally clears the
AI bound bit. Copying the already-read tuning into all participating actual
native racer parameter owners preserves provenance; fabricating+60=1 does not.

The common49W admission may admit58=1 only in conjunction with the complete AI
entry and other recovered branches. This stage does not authorize globally
weakening existing player-entry guards or relabeling HOST controls as original.

## Deferred central verification

No commands to compile or execute the staged implementation were run. Once
separately authorized, central verification should cover AI bit1 on/off with
physical-only, virtual-only, active blend and just-expired blend, preserving the
prefix RNG/activation order and dirty scene state. Impact cases should exercise
each threshold tier, XS/no-match distinction, partner state0/nonzero,0.9 boundary,
CFC0/nonzero, non-unit authored multipliers, strict-0.75/-0.85 rear boundaries,
all skip paths and rejection of unbound/dynamic partners. The10-key ordinary
reader and11-key AI reader need separate regression cases. Existing mutation
scripts contain exact source-text needles for the old lifecycle stage macros;
the conditional source-PC expressions will require coordinated needle updates
before those scripts are meaningful. This is a verification dependency, not a
claim that any check passed.
