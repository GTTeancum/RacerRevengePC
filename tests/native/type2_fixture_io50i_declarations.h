#pragma once
// Original diagnostic types and external reader declarations only. Implementations
// remain in type2_fixture_io50i.h, included by exactly one TU in each consumer.
#include "../../src/game/static_type2_producer49x.h"
#include "json.hpp"
#include <vector>
#include <string>
namespace cold50i {
using namespace rr::game;using Json=nlohmann::json;
struct Shape {std::vector<Type2Node49X> nodes;std::vector<Type2Ray49X> rays;Type2ResourceView49X view;};
struct Mesh {rr::assets::CollisionMesh48I mesh;std::vector<float> radii;std::size_t maxDepth=0;};
std::uint32_t bits(float f);
float fp(std::uint32_t u);
void words(void* p,const Json& row);
void compareWords(const void* p,const Json& row,const std::string& name);
BodyMatrix48G matrix(const Json& j);
CollisionSphere48J sphere(const Json& j);
CollisionFilter48J filter(const Json& j);
std::size_t depth(const std::vector<Type2Node49X>& nodes,std::uint16_t tag);
Shape shape(const Json& j);
Mesh mesh(const Json& j);
std::string unhex(const std::string& hex);
}
