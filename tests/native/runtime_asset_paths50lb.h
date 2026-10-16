#pragma once
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

// Native cold-input paths only. The process working directory never changes.
// Explicit absolute inputs remain authoritative; no search, fallback, copying,
// symlink creation, original execution or cache generation occurs here.
namespace runtime_asset_paths50LB {
namespace fs=std::filesystem;
inline fs::path configuredRoot(const char* name) {
    const char* value=std::getenv(name);
    if(!value)return {};
    if(!*value)throw std::runtime_error(std::string(name)+" must name a nonempty directory");
    return fs::absolute(fs::path(value));
}
inline fs::path relativeInput(const fs::path& input) {
    fs::path out;
    for(const auto& component:input){
        if(component==".")continue;
        // Do not collapse parent traversals across filesystem/symlink owners.
        if(component=="..")throw std::runtime_error("Remapped native input cannot contain parent traversal: "+input.string());
        out/=component;
    }
    return out;
}
inline bool suffixAfter(const fs::path& input,const fs::path& prefix,fs::path& suffix) {
    auto at=input.begin();
    for(const auto& component:prefix){
        if(at==input.end()||*at!=component)return false;
        ++at;
    }
    suffix.clear();for(;at!=input.end();++at)suffix/=*at;
    return true;
}
inline fs::path resolve(const fs::path& input) {
    if(input.is_absolute()||input.has_root_path())return input;
    const auto original=configuredRoot("RR_ORIGINAL_ASSET_ROOT");
    const auto generated=configuredRoot("RR_GENERATED_RUNTIME_ROOT");
    if(original.empty()&&generated.empty())return input;
    const auto relative=relativeInput(input);
    fs::path suffix;
    // Original archives must never fall through to a copied runtime disc tree.
    if(suffixAfter(relative,"work/native/project/disc",suffix))
        return original.empty()?input:original/suffix;
    const auto first=relative.begin();
    if(!generated.empty()&&first!=relative.end()&&(*first=="evidence"||*first=="work"))
        return generated/relative;
    return input;
}
// CORPUS_JSON is a generated-input argument, including a bare relative name.
// Its JSON geometry/shape rows are inline data, not filesystem path descriptors.
inline fs::path generatedInput(const fs::path& input) {
    if(input.is_absolute()||input.has_root_path())return input;
    const auto root=configuredRoot("RR_GENERATED_RUNTIME_ROOT");
    return root.empty()?input:root/relativeInput(input);
}
} // namespace runtime_asset_paths50LB
