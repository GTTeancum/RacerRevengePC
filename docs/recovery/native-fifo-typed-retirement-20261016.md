# Actual443848 typed retirement, with no invented drain

Source-only implementation based d83ae96. No builds, tests, syntax checks,
project decoder execution or guest execution.

Race1A6940 reads the actual global collector's table400C, then1A6948 invokes
slot8 with A1=1. Constructor24B6FC installs3D2198; its slot8 is24B720.
The concrete destructor frees optional4010 backing, calls29C300(this,0),
then frees this when the original destruction argument is positive.
It does NOT invoke24B7B0 or any renderer completion operation.

Base29C300 first destroys allocator08 through its actual slot10;29C354/360
clears global443848 only when identity matches;29C370..3A8 then destroys each
registered node in ascending count00 order. Node table20 is3D21A8, slot8
24AED0, which calls29C100 and frees only node storage, without material/pass
release. Source allocations and references are not emulated here.

The SAME native FIFO now has private typed retirement entered only through
Application::retireNativeCollector1A6948. Actual null is the original skip.
Nonempty records, active drain, wrong process/thread/table identity, table pins
or active collector/population/demand operations reject before any mutation.
Every queued renderer/camera/source/allocation lifetime therefore survives an
unsupported retirement. No completion issuer, epoch advance, or blanket queue
clear is introduced. Private real drain remains unsupported.

The admitted empty case retires no live allocator records, clears the actual
Application slot while retaining this owner locally, then invalidates the same
canonical material collector seal and releases registration records. It does
not call29BEA0 or release material/pass references. Old retained FIFO handles
are closed against acceptance, publication and drain. Native scissor/table
borrows then retire. No native4010 lowering allocation currently exists; a
future lowering implementation must provide its concrete4010 retirement before
extending this path. This is not a proof of complete source allocator ABI.

No runtime call is added. The caller still owns preceding real race cleanup
ordering and any required actual draw completion. An empty destructor is not
an empty24B7B0 drain: even the latter has mandatory renderer/global work.
