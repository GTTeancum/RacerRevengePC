#pragma once
#include "scene_asset48d.h"
#include "native_asset48b.h"
#include <filesystem>
#include <string>
#include <vector>

namespace rr { namespace windows {
struct RuntimeTexture48E {
    std::string source_name;
    // Original smallest-to-largest mip order, RGBA bytes unchanged.
    std::vector<assets::Texture48B> levels;
};
struct RuntimeTextureSlot48E { unsigned slot = 0; std::string name; int texture = -1; };
struct RuntimeMaterialPass48E { std::string program; std::vector<RuntimeTextureSlot48E> textures; };
struct RuntimeMaterial48E { std::string name; std::vector<RuntimeMaterialPass48E> passes; };
struct RuntimeScene48E {
    assets::Scene48D scene;
    std::vector<RuntimeMaterial48E> materials;
    std::vector<RuntimeTexture48E> textures;
    std::vector<std::string> warnings;
};
// Resolves a scene from the verified active native cache, never original files.
// Every selected native object is hash/structure checked. Unsupported bindings
// stay explicitly unresolved; this function supplies no shader approximations.
bool loadCachedScene48E(const std::filesystem::path& cache, const std::string& container,
    const std::string& member, RuntimeScene48E& output, std::string& error);
} }
