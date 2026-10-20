#include "vehicle_draw_gl50cv.h"
#include <algorithm>
#include <limits>
#include <type_traits>

namespace rr::platform {
namespace {
constexpr std::uint32_t arrayBuffer = 0x8892;
constexpr std::uint32_t elementArrayBuffer = 0x8893;
constexpr std::uint32_t triangles = 0x0004;
constexpr std::uint32_t unsignedInt = 0x1405;
constexpr std::uint32_t componentCount = game::vehicleDrawComponentCount50CS;
constexpr game::VehicleDrawComponent50CM components[] = {
    game::VehicleDrawComponent50CM::chariot, game::VehicleDrawComponent50CM::leftEngine,
    game::VehicleDrawComponent50CM::rightEngine, game::VehicleDrawComponent50CM::bottomLeftEngine,
    game::VehicleDrawComponent50CM::bottomRightEngine
};
static_assert(std::is_standard_layout<assets::Vertex48B>::value, "GL vertex layout");
static_assert(sizeof(assets::Vertex48B) == 24 &&
    offsetof(assets::Vertex48B, position) == 0 &&
    offsetof(assets::Vertex48B, uv) == 12 &&
    offsetof(assets::Vertex48B, color) == 20, "GL Vertex48B layout");
static_assert(sizeof(std::uint32_t) == 4, "GL_UNSIGNED_INT indices");

std::uint32_t componentIndex(game::VehicleDrawComponent50CM component) noexcept {
    for (std::uint32_t c = 0; c < componentCount; ++c)
        if (components[c] == component) return c;
    return componentCount;
}
}

VehicleGlBackend50CV::VehicleGlBackend50CV(const VehicleGlFunctions50CV& gl,
    const VehicleGlResources50CV& resources, const VehicleGlHooks50CV& hooks,
    std::uint8_t initialPublishedBank) noexcept
    : gl_(gl), resources_(resources), hooks_(hooks), publishedBank_(initialPublishedBank) {}

game::VehicleDrawBackend50CS VehicleGlBackend50CV::backend() noexcept {
    game::VehicleDrawBackend50CS result{};
    result.context = this;
    for (std::uint32_t c = 0; c < componentCount; ++c) {
        result.capacity.vertices[c] = std::min(resources_.bank[0].vertexCapacity[c],
            resources_.bank[1].vertexCapacity[c]);
        result.capacity.indices[c] = resources_.indices[c].count;
    }
    result.capacity.commands = game::maxVehicleDrawCommands50CS;
    result.preflight = preflight;
    result.begin = begin;
    result.uploadVertices = upload;
    result.bindGeometry = bind;
    result.drawIndexed = draw;
    result.commit = commit;
    result.abort = abort;
    return result;
}

bool VehicleGlBackend50CV::noGlError() noexcept {
    const auto error = gl_.getError();
    if (error && !glError_) glError_ = error;
    return error == 0;
}

bool VehicleGlBackend50CV::preflight(void* context, const game::VehicleDrawBatch50CO& batch,
    const game::VehicleDrawPlan50CS& plan) noexcept {
    if (!context) return false;
    auto& self = *static_cast<VehicleGlBackend50CV*>(context);
    if (self.active_) return false;
    self.glError_ = 0;
    const auto& gl = self.gl_;
    const auto& hooks = self.hooks_;
    if (self.publishedBank_ > 1 || !gl.bindBuffer || !gl.bufferSubData ||
        !gl.bindVertexArray || !gl.drawElementsBaseVertex || !gl.isBuffer ||
        !gl.isVertexArray || !gl.getError || !hooks.ready || !hooks.materialReady ||
        !hooks.begin || !hooks.bindMaterial || !hooks.commit || !hooks.abort) return false;
    const auto& resources = self.resources_;
    if (!game::validVehicleDrawComponentCount50CM(batch.componentCount) ||
        resources.componentCount != batch.componentCount) return false;
    for (std::uint32_t c = 0; c < batch.componentCount; ++c) {
        const auto& indices = resources.indices[c];
        if (!indices.buffer || !indices.data || indices.data != batch.component[c].indices ||
            indices.count != batch.component[c].indexCount ||
            plan.vertexBytes[c] > static_cast<std::uint64_t>(std::numeric_limits<std::intptr_t>::max()) ||
            plan.indexBytes[c] > static_cast<std::uint64_t>(std::numeric_limits<std::intptr_t>::max()) ||
            indices.count > static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())) return false;
    }
    for (std::uint32_t b = 0; b < 2; ++b) {
        const auto& bank = resources.bank[b];
        for (std::uint32_t c = 0; c < batch.componentCount; ++c) {
            if (!bank.vertexBuffer[c] || !bank.vertexArray[c] ||
                bank.vertexCapacity[c] < batch.component[c].vertexCount) return false;
            // A writable bank must never alias any retained index object or any
            // other component/bank's VBO or VAO, including the visible bank.
            for (std::uint32_t other = 0; other < batch.componentCount; ++other)
                if (bank.vertexBuffer[c] == resources.indices[other].buffer) return false;
            for (std::uint32_t earlier = 0; earlier < b * batch.componentCount + c; ++earlier) {
                const auto& prior = resources.bank[earlier / batch.componentCount];
                const auto p = earlier % batch.componentCount;
                if (bank.vertexBuffer[c] == prior.vertexBuffer[p] ||
                    bank.vertexArray[c] == prior.vertexArray[p]) return false;
            }
        }
    }
    const auto inactive = static_cast<std::uint8_t>(self.publishedBank_ ^ 1u);
    // Do not issue even a GL query until the owner confirms a current context
    // and verifies resource contents/layouts/capacities beyond these names.
    if (!hooks.ready(hooks.context, resources, inactive, batch, plan) || !self.noGlError()) return false;
    for (std::uint32_t c = 0; c < batch.componentCount; ++c) {
        if (!gl.isBuffer(resources.indices[c].buffer)) return false;
        for (const auto& bank : resources.bank)
            if (!gl.isBuffer(bank.vertexBuffer[c]) || !gl.isVertexArray(bank.vertexArray[c])) return false;
        for (std::uint32_t s = 0; s < batch.component[c].spanCount; ++s)
            if (!hooks.materialReady(hooks.context, components[c],
                batch.component[c].spans[s].material_index)) return false;
    }
    return self.noGlError();
}

bool VehicleGlBackend50CV::begin(void* context, const game::VehicleDrawBatch50CO& batch,
    const game::VehicleDrawPlan50CS&) noexcept {
    auto& self = *static_cast<VehicleGlBackend50CV*>(context);
    if (self.active_) return false;
    self.pendingBank_ = static_cast<std::uint8_t>(self.publishedBank_ ^ 1u);
    self.activeComponentCount_ = batch.componentCount;
    self.uploaded_ = self.nextComponent_ = self.nextSpan_ = 0;
    self.boundComponent_ = componentCount;
    for (std::uint32_t c = 0; c < batch.componentCount; ++c) {
        self.vertexCount_[c] = batch.component[c].vertexCount;
        self.spanCount_[c] = batch.component[c].spanCount;
    }
    // Mark active before the fallible hook: 50CS aborts even a failed begin.
    self.active_ = true;
    if (!self.hooks_.begin(self.hooks_.context, self.pendingBank_)) return false;
    return self.noGlError();
}

bool VehicleGlBackend50CV::upload(void* context, game::VehicleDrawComponent50CM component,
    const assets::Vertex48B* vertices, std::uint32_t count) noexcept {
    auto& self = *static_cast<VehicleGlBackend50CV*>(context);
    const auto c = componentIndex(component);
    if (!self.active_ || c >= self.activeComponentCount_ || !vertices || count != self.vertexCount_[c] ||
        (self.uploaded_ & (1u << c)) || self.nextComponent_ || self.boundComponent_ != componentCount)
        return false;
    VehicleGlTimingScope50CV timing(self.hooks_.timingObserver,self.hooks_.context,VehicleGlTimingPhase50CV::upload,c);
    self.gl_.bindBuffer(arrayBuffer, self.resources_.bank[self.pendingBank_].vertexBuffer[c]);
    if (!self.noGlError()) return false;
    self.gl_.bufferSubData(arrayBuffer, 0,
        static_cast<std::intptr_t>(std::size_t(count) * sizeof(assets::Vertex48B)), vertices);
    if (!self.noGlError()) return false;
    self.uploaded_ |= 1u << c;
    return true;
}

bool VehicleGlBackend50CV::bind(void* context, game::VehicleDrawComponent50CM component,
    const std::uint32_t* indices, std::uint32_t count) noexcept {
    auto& self = *static_cast<VehicleGlBackend50CV*>(context);
    const auto c = componentIndex(component);
    if (!self.active_ || c >= self.activeComponentCount_ || self.uploaded_ != ((1u << self.activeComponentCount_) - 1u) ||
        c != self.nextComponent_ || self.nextSpan_ || self.boundComponent_ != componentCount ||
        indices != self.resources_.indices[c].data || count != self.resources_.indices[c].count) return false;
    VehicleGlTimingScope50CV timing(self.hooks_.timingObserver,self.hooks_.context,VehicleGlTimingPhase50CV::geometryBind,c);
    self.gl_.bindVertexArray(self.resources_.bank[self.pendingBank_].vertexArray[c]);
    if (!self.noGlError()) return false;
    self.gl_.bindBuffer(elementArrayBuffer, self.resources_.indices[c].buffer);
    if (!self.noGlError()) return false;
    self.boundComponent_ = c;
    return true;
}

bool VehicleGlBackend50CV::draw(void* context, game::VehicleDrawComponent50CM component,
    std::uint32_t ordinal, const assets::SceneSpan48D& span, std::int32_t baseVertex) noexcept {
    auto& self = *static_cast<VehicleGlBackend50CV*>(context);
    const auto c = componentIndex(component);
    if (!self.active_ || c >= self.activeComponentCount_ || self.uploaded_ != ((1u << self.activeComponentCount_) - 1u) ||
        self.boundComponent_ != c || self.nextComponent_ != c || ordinal != self.nextSpan_ ||
        ordinal >= self.spanCount_[c] || baseVertex != 0 || span.index_count % 3 ||
        span.index_start > self.resources_.indices[c].count ||
        span.index_count > self.resources_.indices[c].count - span.index_start) return false;
    bool materialAccepted=false;
    {
        VehicleGlTimingScope50CV timing(self.hooks_.timingObserver,self.hooks_.context,VehicleGlTimingPhase50CV::materialBind,c);
        materialAccepted=self.hooks_.bindMaterial(self.hooks_.context, component, span.material_index) && self.noGlError();
    }
    if(!materialAccepted)return false;
    VehicleGlTimingScope50CV timing(self.hooks_.timingObserver,self.hooks_.context,VehicleGlTimingPhase50CV::spanDraw,c);
    if (self.hooks_.drawSpan) {
        if (!self.hooks_.drawSpan(self.hooks_.context, component, ordinal, span, baseVertex)) return false;
    } else {
        const auto byteOffset = std::uintptr_t(span.index_start) * sizeof(std::uint32_t);
        self.gl_.drawElementsBaseVertex(triangles, static_cast<std::int32_t>(span.index_count),
            unsignedInt, reinterpret_cast<const void*>(byteOffset), 0);
    }
    if (!self.noGlError()) return false;
    if (++self.nextSpan_ == self.spanCount_[c]) {
        ++self.nextComponent_;
        self.nextSpan_ = 0;
        self.boundComponent_ = componentCount;
    }
    return true;
}

bool VehicleGlBackend50CV::commit(void* context) noexcept {
    auto& self = *static_cast<VehicleGlBackend50CV*>(context);
    if (!self.active_ || self.uploaded_ != ((1u << self.activeComponentCount_) - 1u) || self.nextComponent_ != self.activeComponentCount_ ||
        self.nextSpan_ || self.boundComponent_ != componentCount || !self.noGlError()) return false;
    if (!self.hooks_.commit(self.hooks_.context, self.pendingBank_)) return false;
    // No GL call or fallible check after publication. The hook owns the final
    // readiness/error check and must never publish then report failure.
    self.publishedBank_ = self.pendingBank_;
    self.active_ = false;
    return true;
}

void VehicleGlBackend50CV::abort(void* context) noexcept {
    auto& self = *static_cast<VehicleGlBackend50CV*>(context);
    if (self.active_) self.hooks_.abort(self.hooks_.context, self.pendingBank_);
    self.active_ = false;
    self.uploaded_ = self.nextComponent_ = self.nextSpan_ = 0;
    self.boundComponent_ = componentCount;
}
}
