#include "vehicle_draw_submit50cs.h"
#include <algorithm>
#include <cstring>

namespace rr::game {
namespace {
constexpr VehicleDrawComponent50CM components[] = {
    VehicleDrawComponent50CM::chariot, VehicleDrawComponent50CM::leftEngine,
    VehicleDrawComponent50CM::rightEngine
};
bool reject(VehicleDrawSubmitDiagnostic50CS& diagnostic, VehicleDrawSubmitError50CS error,
    std::uint32_t component = vehicleDrawComponentCount50CS, std::uint32_t span = 0) noexcept {
    diagnostic = {error, component, span};
    return false;
}
std::uint32_t componentIndex(VehicleDrawComponent50CM component) noexcept {
    for (std::uint32_t i = 0; i < vehicleDrawComponentCount50CS; ++i)
        if (components[i] == component) return i;
    return vehicleDrawComponentCount50CS;
}
}

bool preflightVehicleDrawBatch50CS(const VehicleDrawBatch50CO& batch,
    const VehicleDrawCapacity50CS& capacity, VehicleDrawPlan50CS& output,
    VehicleDrawSubmitDiagnostic50CS& diagnostic) noexcept {
    diagnostic = {};
    VehicleDrawPlan50CS next{};
    std::uint32_t vertices = 0, indices = 0;
    for (std::uint32_t c = 0; c < vehicleDrawComponentCount50CS; ++c) {
        const auto& geometry = batch.component[c];
        if (!geometry.vertices || !geometry.indices || !geometry.spans ||
            !geometry.vertexCount || geometry.vertexCount > maxRigidVertices49S ||
            !geometry.indexCount || geometry.indexCount > assets::max_mesh_indices ||
            !geometry.spanCount || geometry.spanCount > maxVehicleDrawSpans50CM)
            return reject(diagnostic, VehicleDrawSubmitError50CS::geometry, c);
        if (geometry.vertexCount > capacity.vertices[c] || geometry.indexCount > capacity.indices[c])
            return reject(diagnostic, VehicleDrawSubmitError50CS::capacity, c);
        std::uint32_t vertex = 0, index = 0;
        for (std::uint32_t s = 0; s < geometry.spanCount; ++s) {
            const auto& span = geometry.spans[s];
            if (span.vertex_start != vertex || span.index_start != index ||
                span.vertex_count > geometry.vertexCount - vertex ||
                span.index_count > geometry.indexCount - index || span.index_count % 3)
                return reject(diagnostic, VehicleDrawSubmitError50CS::spans, c, s);
            vertex += span.vertex_count;
            index += span.index_count;
        }
        if (vertex != geometry.vertexCount || index != geometry.indexCount)
            return reject(diagnostic, VehicleDrawSubmitError50CS::spans, c);
        // 50CM's limits keep these products and all three-component sums in u32.
        next.vertexBytes[c] = geometry.vertexCount * static_cast<std::uint32_t>(sizeof(assets::Vertex48B));
        next.indexBytes[c] = geometry.indexCount * static_cast<std::uint32_t>(sizeof(std::uint32_t));
        next.draws += geometry.spanCount;
        vertices += geometry.vertexCount;
        indices += geometry.indexCount;
    }
    if (vertices != batch.vertices || indices != batch.indices || next.draws != batch.draws)
        return reject(diagnostic, VehicleDrawSubmitError50CS::totals);
    next.commands = vehicleDrawFixedCommands50CS + next.draws;
    if (next.commands > capacity.commands)
        return reject(diagnostic, VehicleDrawSubmitError50CS::capacity);
    output = next;
    return true;
}

bool submitVehicleDrawBatch50CS(const VehicleDrawBatch50CO& batch,
    const VehicleDrawBackend50CS& backend, VehicleDrawSubmitDiagnostic50CS& diagnostic) noexcept {
    diagnostic = {};
    if (!backend.preflight || !backend.begin || !backend.uploadVertices ||
        !backend.bindGeometry || !backend.drawIndexed || !backend.commit || !backend.abort)
        return reject(diagnostic, VehicleDrawSubmitError50CS::callbacks);
    VehicleDrawPlan50CS plan{};
    if (!preflightVehicleDrawBatch50CS(batch, backend.capacity, plan, diagnostic)) return false;
    if (!backend.preflight(backend.context, batch, plan))
        return reject(diagnostic, VehicleDrawSubmitError50CS::backendPreflight);
    const auto abort = [&](VehicleDrawSubmitError50CS error,
        std::uint32_t component = vehicleDrawComponentCount50CS, std::uint32_t span = 0) noexcept {
        backend.abort(backend.context);
        return reject(diagnostic, error, component, span);
    };
    if (!backend.begin(backend.context, batch, plan)) return abort(VehicleDrawSubmitError50CS::begin);
    for (std::uint32_t c = 0; c < vehicleDrawComponentCount50CS; ++c) {
        const auto& geometry = batch.component[c];
        if (!backend.uploadVertices(backend.context, components[c], geometry.vertices, geometry.vertexCount))
            return abort(VehicleDrawSubmitError50CS::upload, c);
    }
    for (std::uint32_t c = 0; c < vehicleDrawComponentCount50CS; ++c) {
        const auto& geometry = batch.component[c];
        if (!backend.bindGeometry(backend.context, components[c], geometry.indices, geometry.indexCount))
            return abort(VehicleDrawSubmitError50CS::bind, c);
        for (std::uint32_t s = 0; s < geometry.spanCount; ++s)
            if (!backend.drawIndexed(backend.context, components[c], s, geometry.spans[s], 0))
                return abort(VehicleDrawSubmitError50CS::draw, c, s);
    }
    if (!backend.commit(backend.context)) return abort(VehicleDrawSubmitError50CS::commit);
    return true;
}

VehicleDrawRecorder50CS::VehicleDrawRecorder50CS(VehicleDrawRecordingStorage50CS first,
    VehicleDrawRecordingStorage50CS second) noexcept : storage_{first, second} {}

VehicleDrawBackend50CS VehicleDrawRecorder50CS::backend() noexcept {
    VehicleDrawBackend50CS result{};
    result.context = this;
    for (std::uint32_t c = 0; c < vehicleDrawComponentCount50CS; ++c) {
        result.capacity.vertices[c] = std::min(storage_[0].vertices[c].capacity, storage_[1].vertices[c].capacity);
        // The recorder borrows the existing indices; it allocates no index copy.
        result.capacity.indices[c] = assets::max_mesh_indices;
    }
    result.capacity.commands = std::min(storage_[0].commandCapacity, storage_[1].commandCapacity);
    result.preflight = preflight;
    result.begin = begin;
    result.uploadVertices = upload;
    result.bindGeometry = bind;
    result.drawIndexed = draw;
    result.commit = commit;
    result.abort = abort;
    return result;
}

const VehicleDrawRecording50CS* VehicleDrawRecorder50CS::recording() const noexcept {
    return hasPublished_ ? &published_ : nullptr;
}

bool VehicleDrawRecorder50CS::preflight(void* context, const VehicleDrawBatch50CO& batch,
    const VehicleDrawPlan50CS& plan) noexcept {
    if (!context) return false;
    const auto& recorder = *static_cast<VehicleDrawRecorder50CS*>(context);
    if (recorder.active_) return false;
    for (const auto& bank : recorder.storage_) {
        if (!bank.commands || bank.commandCapacity < plan.commands) return false;
        for (std::uint32_t c = 0; c < vehicleDrawComponentCount50CS; ++c)
            if (!bank.vertices[c].vertices || bank.vertices[c].capacity < batch.component[c].vertexCount)
                return false;
    }
    return true;
}

bool VehicleDrawRecorder50CS::begin(void* context, const VehicleDrawBatch50CO& batch,
    const VehicleDrawPlan50CS& plan) noexcept {
    auto& recorder = *static_cast<VehicleDrawRecorder50CS*>(context);
    recorder.pendingBank_ = static_cast<std::uint8_t>(recorder.publishedBank_ ^ 1u);
    recorder.pending_ = {};
    const auto& bank = recorder.storage_[recorder.pendingBank_];
    for (std::uint32_t c = 0; c < vehicleDrawComponentCount50CS; ++c) {
        const auto& geometry = batch.component[c];
        recorder.pending_.component[c] = {
            bank.vertices[c].vertices, geometry.vertexCount, geometry.indices, geometry.indexCount
        };
    }
    recorder.pending_.commands = bank.commands;
    recorder.pending_.vertices = batch.vertices;
    recorder.pending_.indices = batch.indices;
    recorder.pending_.draws = batch.draws;
    recorder.pending_.bounds = batch.bounds;
    recorder.expectedCommands_ = plan.commands;
    recorder.uploaded_ = 0;
    recorder.active_ = true;
    return true;
}

bool VehicleDrawRecorder50CS::append(const VehicleDrawCommand50CS& command) noexcept {
    auto& bank = storage_[pendingBank_];
    if (!active_ || pending_.commandCount >= bank.commandCapacity) return false;
    bank.commands[pending_.commandCount++] = command;
    return true;
}

bool VehicleDrawRecorder50CS::upload(void* context, VehicleDrawComponent50CM component,
    const assets::Vertex48B* vertices, std::uint32_t count) noexcept {
    auto& recorder = *static_cast<VehicleDrawRecorder50CS*>(context);
    const auto c = componentIndex(component);
    if (!recorder.active_ || c >= vehicleDrawComponentCount50CS || !vertices ||
        count != recorder.pending_.component[c].vertexCount || (recorder.uploaded_ & (1u << c))) return false;
    auto& target = recorder.storage_[recorder.pendingBank_].vertices[c];
    std::memcpy(target.vertices, vertices, std::size_t(count) * sizeof(assets::Vertex48B));
    if (!recorder.append({VehicleDrawCommandKind50CS::uploadVertices, component})) return false;
    recorder.uploaded_ |= 1u << c;
    return true;
}

bool VehicleDrawRecorder50CS::bind(void* context, VehicleDrawComponent50CM component,
    const std::uint32_t* indices, std::uint32_t count) noexcept {
    auto& recorder = *static_cast<VehicleDrawRecorder50CS*>(context);
    const auto c = componentIndex(component);
    if (!recorder.active_ || c >= vehicleDrawComponentCount50CS || recorder.uploaded_ != 7u ||
        indices != recorder.pending_.component[c].indices || count != recorder.pending_.component[c].indexCount)
        return false;
    return recorder.append({VehicleDrawCommandKind50CS::bindGeometry, component});
}

bool VehicleDrawRecorder50CS::draw(void* context, VehicleDrawComponent50CM component,
    std::uint32_t ordinal, const assets::SceneSpan48D& span, std::int32_t baseVertex) noexcept {
    auto& recorder = *static_cast<VehicleDrawRecorder50CS*>(context);
    if (!recorder.active_ || recorder.uploaded_ != 7u || baseVertex != 0 ||
        componentIndex(component) >= vehicleDrawComponentCount50CS) return false;
    return recorder.append({VehicleDrawCommandKind50CS::drawIndexed, component, ordinal, span, baseVertex});
}

bool VehicleDrawRecorder50CS::commit(void* context) noexcept {
    auto& recorder = *static_cast<VehicleDrawRecorder50CS*>(context);
    if (!recorder.active_ || recorder.uploaded_ != 7u ||
        recorder.pending_.commandCount != recorder.expectedCommands_) return false;
    recorder.published_ = recorder.pending_;
    recorder.publishedBank_ = recorder.pendingBank_;
    recorder.hasPublished_ = true;
    recorder.active_ = false;
    return true;
}

void VehicleDrawRecorder50CS::abort(void* context) noexcept {
    auto& recorder = *static_cast<VehicleDrawRecorder50CS*>(context);
    recorder.pending_ = {};
    recorder.expectedCommands_ = recorder.uploaded_ = 0;
    recorder.active_ = false;
}
}
