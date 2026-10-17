# Bound temporary RA static draw capacity from prior demand

UNTESTED source change. No builds, tests, syntax checks or gameplay executed.

The RA frame preparation collects fresh NativeRaStaticDraw50LA rows into a new
vector and publishes it only after final camera lease validation. Its owner keeps
the previous output vector while invalidating the prepared flag before the next
call. Immediately before the first selected row is appended, reserve the smaller
of output.size() and the current staticSelection.size(). This is a capacity hint,
not a selection count limit or cached selection decision.

All portal traversal, model bounds, LOD arithmetic, scissor/viewport records,
validation and duplicate/order behavior are unchanged. The vector can grow beyond
the hint. The temporary is separate from output and no reference to a temporary
row is retained during this reserve. Zero selected rows perform no reserve.

The hint can avoid intermediate growth reallocations and copies when consecutive
frames have similar demand. It does not remove the fresh per-frame allocation.
A smaller current result may reserve excess memory; the hint is bounded by the
current model count. Allocation failure may occur earlier than before, but is
caught by the existing Status::allocation path and cannot publish partial output.
No arithmetic or publication semantic change is intended. Runtime impact has not
been measured. Successor should validate records and frame timing on the same
course/camera before claiming improvement.
