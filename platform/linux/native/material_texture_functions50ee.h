#pragma once
#include "material_texture_gl50eb.h"
#include <cstring>

namespace rr::platform {
using MaterialGlProc50EE = void (*)();
using MaterialGlResolver50EE = MaterialGlProc50EE (*)(void*,const char*) noexcept;
// Context owner supplies lookup; this performs no GL operation or installation.
// Missing symbols leave the destination unchanged, with a static symbol name.
inline bool loadMaterialTextureFunctions50EE(void* context,
    MaterialGlResolver50EE resolve, MaterialTextureGlFunctions50EB& output,
    const char*& missing) noexcept {
    missing=nullptr;
    if(!resolve){missing="procedure resolver";return false;}
    MaterialTextureGlFunctions50EB next{};
    auto load=[&](auto& field,const char* name) noexcept {
        const auto symbol=resolve(context,name);
        if(!symbol){missing=name;return false;}
        static_assert(sizeof(field)==sizeof(symbol),"Linux GL function pointer size");
        std::memcpy(&field,&symbol,sizeof(field));return true;
    };
    if(!load(next.genTextures,"glGenTextures")||
       !load(next.deleteTextures,"glDeleteTextures")||
       !load(next.bindTexture,"glBindTexture")||
       !load(next.texParameteri,"glTexParameteri")||
       !load(next.texImage2D,"glTexImage2D")||
       !load(next.getTexLevelParameteriv,"glGetTexLevelParameteriv")||
       !load(next.bindBuffer,"glBindBuffer")||
       !load(next.pixelStorei,"glPixelStorei")||
       !load(next.getIntegerv,"glGetIntegerv")||
       !load(next.getError,"glGetError"))return false;
    output=next;return true;
}
}
