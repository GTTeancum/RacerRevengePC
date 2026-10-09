#include "collision_mesh48i.h"
#include "collision_runtime48h.h"
#include "import_collision48g.h"
#include <cmath>
#include <new>
#include <utility>

namespace rr::assets {
namespace {
float add(float a, float b) noexcept { volatile float value = a + b; return value; }
float sub(float a, float b) noexcept { volatile float value = a - b; return value; }
float mul(float a, float b) noexcept { volatile float value = a * b; return value; }
bool normalize(const std::array<float, 3>& v, std::array<float, 3>& out) noexcept {
    for (float f : v) if (!std::isfinite(f)) return false;
    const float square = add(add(mul(v[0], v[0]), mul(v[1], v[1])), mul(v[2], v[2]));
    if (!(square > 0) || !std::isfinite(square)) return false;
    volatile float root = static_cast<float>(std::sqrt(static_cast<double>(square)));
    volatile float reciprocal = 1.0f / root;
    for (unsigned i = 0; i != 3; ++i) {
        out[i] = mul(v[i], reciprocal);
        if (!std::isfinite(out[i])) return false;
    }
    return true;
}
bool affine(const std::array<float, 16>& m) noexcept {
    for (float f : m) if (!std::isfinite(f)) return false;
    return m[3] == 0 && m[7] == 0 && m[11] == 0 && m[15] == 1;
}
bool project(Collision48G&& source, CollisionMesh48I& output, std::string& error) {
        CollisionMesh48I mesh;
        mesh.local_matrix = source.matrix;
        mesh.reverse_mode = source.reverse_mode;
        mesh.vertices = std::move(source.vertices);
        mesh.vertex_normals = std::move(source.vertex_normals);
        mesh.nodes.reserve(source.nodes.size());
        mesh.faces.reserve(source.faces.size());
        for (const auto& n : source.nodes)
            mesh.nodes.push_back({n.volume, n.center, n.half, n.children});
        for (const auto& f : source.faces) {
            CollisionMeshFace48I face;
            face.d = f.d;
            if (!packCollisionFaceNormal48H(f.normal, face.normal)) {
                error = "collision.runtime_normal_overflow";
                return false;
            }
            for (unsigned i = 0; i != 3; ++i)
                face.vertices[i] = static_cast<std::uint16_t>(f.vertices[i]);
            mesh.faces.push_back(face);
        }
        output=std::move(mesh);return true;
}
}
static_assert(sizeof(CollisionMeshFace48I) == 16, "Portable compact face record");
static_assert(sizeof(CollisionMeshNode48I) == 36, "Portable ordered node record");
bool loadCollisionMesh48I(const std::vector<std::uint8_t>& cache,
                         CollisionMesh48I& output, std::string& error) {
    error.clear();
    try {
        Collision48G source;
        if (!parseCollisionAsset48G(cache, source, error)) return false;
        return project(std::move(source),output,error);
    } catch (const std::bad_alloc&) {
        error = "collision.runtime_allocation";
        return false;
    }
}
bool loadCollisionCompoundMesh50IJ(const std::vector<std::uint8_t>& original,
    CollisionCompoundMesh50IJ& output,std::string& error,const CollisionCompoundLimits50IJ& limits) {
    error.clear();try {
        CollisionCompound50IJ source;DecodeError48C decode;
        if(!decodeCollisionCompound50IJ(original,source,decode,nullptr,limits)){
            error=decode.code+std::string(" at ")+std::to_string(decode.offset)+": "+decode.detail;return false;
        }
        CollisionCompoundMesh50IJ result;result.reverse_mode=source.reverse_mode;
        result.members.reserve(source.members.size());
        for(auto& member:source.members){
            CollisionCompoundMeshMember50IJ part;
            part.scene_node_index=member.scene_node_index;part.source_offset=member.source_offset;
            part.body_offset=member.body_offset;part.source_end=member.source_end;
            if(!project(std::move(member.tree),part.mesh,error))return false;
            result.members.push_back(std::move(part));
        }
        output=std::move(result);return true;
    }catch(const std::bad_alloc&){error="collision.compound_allocation";return false;}
}
bool placementCollision48I(const std::array<float, 3>& position,
                          const std::array<float, 3>& look,
                          const std::array<float, 3>& up,
                          std::array<float, 16>& output) noexcept {
    for (float f : position) if (!std::isfinite(f)) return false;
    std::array<float, 3> u, l;
    if (!normalize(up, u) || !normalize(look, l)) return false;
    std::array<float, 16> m;
    for (unsigned i = 0; i != 3; ++i) {
        m[i] = sub(mul(u[(i + 1) % 3], l[(i + 2) % 3]), mul(l[(i + 1) % 3], u[(i + 2) % 3]));
        m[4 + i] = u[i]; m[8 + i] = l[i]; m[12 + i] = position[i];
    }
    m[3] = m[7] = m[11] = sub(0, 0);
    m[15] = add(sub(0, 0), 1);
    // Parallel authored axes cannot form a usable collision transform.
    if ((m[0] == 0 && m[1] == 0 && m[2] == 0) || !affine(m)) return false;
    output = m;
    return true;
}
bool composeCollisionWorld48I(const std::array<float, 16>& local,
                             const std::array<float, 16>& placement,
                             std::array<float, 16>& output) noexcept {
    if (!affine(local) || !affine(placement)) return false;
    std::array<float, 16> m;
    for (unsigned row = 0; row != 4; ++row) for (unsigned column = 0; column != 4; ++column) {
        float value = mul(placement[column], local[row * 4]);
        value = add(value, mul(placement[4 + column], local[row * 4 + 1]));
        value = add(value, mul(placement[8 + column], local[row * 4 + 2]));
        m[row * 4 + column] = add(value, mul(placement[12 + column], local[row * 4 + 3]));
    }
    if (!affine(m)) return false;
    output = m;
    return true;
}
}
