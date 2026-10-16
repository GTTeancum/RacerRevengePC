#pragma once
#include "vehicle_draw_batch50co.h"

namespace rr::game {
constexpr std::uint32_t vehicleDrawComponentCount50CS = maxVehicleDrawComponents50CM;
constexpr std::uint32_t vehicleDrawFixedCommands50CS = 6; // Legacy three-model uploads/binds.
constexpr std::uint32_t maxVehicleDrawCommands50CS =
    vehicleDrawComponentCount50CS * (2 + maxVehicleDrawSpans50CM);

struct VehicleDrawCapacity50CS {
    std::uint32_t vertices[vehicleDrawComponentCount50CS]{}, indices[vehicleDrawComponentCount50CS]{};
    std::uint32_t commands = 0;
};
struct VehicleDrawPlan50CS {
    std::uint32_t vertexBytes[vehicleDrawComponentCount50CS]{}, indexBytes[vehicleDrawComponentCount50CS]{};
    std::uint32_t draws = 0, commands = 0;
};
enum class VehicleDrawSubmitError50CS : std::uint8_t {
    none, callbacks, geometry, spans, totals, capacity,
    backendPreflight, begin, upload, bind, draw, commit
};
struct VehicleDrawSubmitDiagnostic50CS {
    VehicleDrawSubmitError50CS error = VehicleDrawSubmitError50CS::none;
    // Capacity denotes a batch-level error; span is meaningful for spans/draw.
    std::uint32_t component = vehicleDrawComponentCount50CS, span = 0;
};

// The context owns all device/resource/material/camera decisions. Indices are
// retained immutable component-local R32_UINT buffers, not uploaded per frame.
// Geometry binds use Vertex48B stride, zero vertex/index byte offsets, and
// triangle-list topology. Draw binds the supplied component's existing material
// index then issues DrawIndexed(span.index_count, span.index_start, baseVertex).
// baseVertex is always zero. World-space vertices require view/projection only.
//
// All callbacks are required, noexcept, synchronous and single-threaded. They
// must not mutate the batch, its backing storage, or this callback table.
// preflight is read-only. A device backend verifies ready resources, exact
// retained index-buffer identity/content, capacity and every material plus the
// caller-selected camera and render state. A recorder verifies storage only.
// Nothing here selects defaults or implements visibility. Invoke callbacks only
// through submitVehicleDrawBatch50CS; callbacks must not reenter submission.
//
// begin starts an UNPUBLISHED transaction. On any false result after invoking
// begin (including begin/commit itself), submit calls abort exactly once. abort
// must be safe after a failed begin and preserve the previously committed frame.
// Successful commit alone publishes. A failed commit must remain abortable and
// must not publish partial work. An immediate-context backend must stage an
// inactive buffer set/command list or otherwise satisfy this contract; abort
// cannot undo draws already issued into a visible target. No callback retains
// the transient batch/plan/span references; consume or copy them synchronously.
struct VehicleDrawBackend50CS {
    void* context = nullptr;
    VehicleDrawCapacity50CS capacity{};
    bool (*preflight)(void*, const VehicleDrawBatch50CO&, const VehicleDrawPlan50CS&) noexcept = nullptr;
    bool (*begin)(void*, const VehicleDrawBatch50CO&, const VehicleDrawPlan50CS&) noexcept = nullptr;
    bool (*uploadVertices)(void*, VehicleDrawComponent50CM,
        const assets::Vertex48B*, std::uint32_t count) noexcept = nullptr;
    bool (*bindGeometry)(void*, VehicleDrawComponent50CM,
        const std::uint32_t*, std::uint32_t count) noexcept = nullptr;
    bool (*drawIndexed)(void*, VehicleDrawComponent50CM, std::uint32_t ordinal,
        const assets::SceneSpan48D&, std::int32_t baseVertex) noexcept = nullptr;
    bool (*commit)(void*) noexcept = nullptr;
    void (*abort)(void*) noexcept = nullptr;
};

// Allocation-free structural/capacity preflight. Input must be a successful
// 50CO result with unchanged, previously prepared 50CM indices/spans. This checks
// ordered span coverage, triangle counts and totals, not every index or vertex
// again. Backing-array validity and disjoint input/output storage are caller
// preconditions. Failure preserves plan and changes only diagnostic.
bool preflightVehicleDrawBatch50CS(const VehicleDrawBatch50CO&,
    const VehicleDrawCapacity50CS&, VehicleDrawPlan50CS&,
    VehicleDrawSubmitDiagnostic50CS&) noexcept;

// Preflight, begin, all active world-space uploads, then each component's bind
// and ordered material spans, then commit. No heap, device creation, index
// upload, model transform, camera/material/visibility policy or presentation.
// False means no new committed submission, provided the backend obeys the
// transactional contract. Inputs remain unchanged. A true result establishes
// backend acceptance, not GPU completion, presentation, or a rendered game.
bool submitVehicleDrawBatch50CS(const VehicleDrawBatch50CO&,
    const VehicleDrawBackend50CS&, VehicleDrawSubmitDiagnostic50CS&) noexcept;

// Optional headless backend: copies vertex uploads and records operations, but
// performs no rendering. Both banks are bounded, caller-owned, mutually disjoint
// storage, also disjoint from inputs and this recorder. Neither bank may be
// resized or moved. Indices continue to borrow the immutable prepared meshes.
enum class VehicleDrawCommandKind50CS : std::uint8_t { uploadVertices, bindGeometry, drawIndexed };
struct VehicleDrawCommand50CS {
    VehicleDrawCommandKind50CS kind = VehicleDrawCommandKind50CS::uploadVertices;
    VehicleDrawComponent50CM component = VehicleDrawComponent50CM::chariot;
    std::uint32_t ordinal = 0;
    assets::SceneSpan48D span{}; // Used only by drawIndexed.
    std::int32_t baseVertex = 0;
};
struct VehicleDrawRecordingStorage50CS {
    VehicleDrawBuffer50CM vertices[vehicleDrawComponentCount50CS]{};
    VehicleDrawCommand50CS* commands = nullptr;
    std::uint32_t commandCapacity = 0;
};
struct VehicleDrawRecordedGeometry50CS {
    const assets::Vertex48B* vertices = nullptr;
    std::uint32_t vertexCount = 0;
    const std::uint32_t* indices = nullptr;
    std::uint32_t indexCount = 0;
};
struct VehicleDrawRecording50CS {
    VehicleDrawRecordedGeometry50CS component[vehicleDrawComponentCount50CS]{};
    const VehicleDrawCommand50CS* commands = nullptr;
    std::uint32_t commandCount = 0, vertices = 0, indices = 0, draws = 0;
    assets::Bounds48B bounds{};
    std::uint32_t componentCount = defaultVehicleDrawComponents50CM;
};

class VehicleDrawRecorder50CS {
public:
    VehicleDrawRecorder50CS(VehicleDrawRecordingStorage50CS first,
        VehicleDrawRecordingStorage50CS second) noexcept;
    VehicleDrawRecorder50CS(const VehicleDrawRecorder50CS&) = delete;
    VehicleDrawRecorder50CS& operator=(const VehicleDrawRecorder50CS&) = delete;
    // The returned callback context borrows this nonmovable recorder.
    VehicleDrawBackend50CS backend() noexcept;
    // Null before the first commit. The current record/payload survive every
    // rejected submission and remain valid until the next successful commit.
    // Older copied views may be overwritten by a later begin; consume promptly.
    const VehicleDrawRecording50CS* recording() const noexcept;
private:
    static bool preflight(void*, const VehicleDrawBatch50CO&, const VehicleDrawPlan50CS&) noexcept;
    static bool begin(void*, const VehicleDrawBatch50CO&, const VehicleDrawPlan50CS&) noexcept;
    static bool upload(void*, VehicleDrawComponent50CM, const assets::Vertex48B*, std::uint32_t) noexcept;
    static bool bind(void*, VehicleDrawComponent50CM, const std::uint32_t*, std::uint32_t) noexcept;
    static bool draw(void*, VehicleDrawComponent50CM, std::uint32_t,
        const assets::SceneSpan48D&, std::int32_t) noexcept;
    static bool commit(void*) noexcept;
    static void abort(void*) noexcept;
    bool append(const VehicleDrawCommand50CS&) noexcept;

    VehicleDrawRecordingStorage50CS storage_[2];
    VehicleDrawRecording50CS published_{}, pending_{};
    std::uint32_t expectedCommands_ = 0, uploaded_ = 0;
    std::uint8_t publishedBank_ = 0, pendingBank_ = 1;
    bool hasPublished_ = false, active_ = false;
};
}
