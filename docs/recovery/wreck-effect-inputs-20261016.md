# First full wreck occurrence: actual effect constructor inputs

Source-only continuation of b474e5f. No runtime activation or validation is claimed.

## Connected native stage

`OriginalWreckRacerStorage50LA::bindFirstEffectConstructorInputs` follows the
completed mutual filters/derived callback at 2CB610. The input contains addresses
of the actual resource-context and particle-parent fields. Both addresses are
mandatory. A missing address cannot stand in for a source null argument.

The count comes from the same adopted PHY inventory and exact first collection /
first piece already under construction: EmitSmoke selects the authored
MaxNumberOfDamageEmitters; false selects zero. The source count is committed even
when either actual context value is null. No scalar zero default is used as proof
that 174 was initialized. Read-only inspection distinguishes committed count from
cold backing, and can verify that the retained caller fields still have their
captured values.

The non-null pair stops at 2CB628 before count testing and allocations. The real
resource-null branch stops at 2CB8C4; parent-null stops at 2CB8C8. These are explicit
return boundaries, not route-completed receipts. In particular this change does
not set pieceRouteClosed or pieceNullRouteClosed, append the piece, or skip the
count-zero dust/splash constructors for a non-null pair.

## Authenticated source anchors

Read-only original SLUS_202.68, VA file offset = VA - 100000 + 80.

- 22C064 reads MaxNumberOfDamageEmitters into stack318, default4.
- 22C0EC reads EmitSmoke%d into stack31C. 22C0F8 branches on that boolean;
  delay-slot22C0FC sets s1=0. If true,22C100 replaces s1 with stack318.
- 22C188 writes s1 to outgoing argument +20 for 2CB5A0.
- 22BFD0's incoming t3 is retained in s3 and forwarded at 22C180 to outgoing +10.
  2CB5C4 reads that resource context into s3.
- 22BFD0's incoming stack+0 supplies the particle parent, forwarded at 22C184
  to outgoing +18; 2CB5CC reads it into s2.
  At the racer caller,212A50 loads actual racer7FC and212A60 passes it as
  outgoing stack0. Resource context is saved racer-setup s1 passed as t3
  at212A94; its owning-field offset is not yet proven and must not be guessed.
- 2CB614 loads incoming count; 2CB618 tests resource context, and its delay slot
  2CB61C always stores piece174. 2CB620 tests particle parent; its delay slot
  moves the piece into the return register. Branch targets are 2CB8C4/2CB8C8.
- Non-null continuation 2CB628 loads174. Positive count begins 160 allocation
  at 2CB634 and 164 at 2CB644; 170 is separately conditional. Count zero still
  reaches the dust/splash segment rather than source return.

## Lifetime and remaining joins

No effect allocation or external Entity publication occurs in this stage. The
existing exact E8, E4, D8/DC, E0, gravity/body, model, piece and collection
retirement chain admits both new stopped cursors. Only after genuine piece
104970/2CB500/165BD0/1413E0 retirement are the native caller-field borrows cleared.
The committed source174 word and reached-stage history survive. This does not
invent source writes to clear arguments or source174.

The caller must keep these field addresses and referenced owners alive through
the piece lifetime. A later continuation must revalidate the captured field
values before borrowing them. Full effect work still requires:

1. The authentic application resource context and external particle forest,
   with stable component/control identities for this occurrence.
2. Actual positive-count160/164/170 allocation and original child factories.
3. Matched external effect retirement before the current no-effect retirement
   admission can be extended past these boundaries.
4. For a genuine null branch, source return followed by actual22C1B8 append and
   the remaining collection tuning/exclusions. Neither is certified here.
5. Full type3 producer/result synchronization and subsequent runtime ownership
   joins remain separate activation prerequisites.

The existing one-piece effect owner is not duplicated or claimed to support
positive emitter arrays. No missing context is manufactured to bypass it.
