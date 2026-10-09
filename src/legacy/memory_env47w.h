#pragma once
#include <cstdlib>
#include <optional>
#include <string>
namespace rr47w {
inline bool cacheDiagnostics() {
    static const bool enabled=[] {const char* p=std::getenv("RR_CACHE_MEMORY_DIAGNOSTICS47W");return p&&p[0]=='1'&&p[1]=='\0';}();
    return enabled;
}
}
// Launch-fixed diagnostics only. Preserve absence and own the returned string.
// Disabled mode retains the original live lookup at every call site.
#define RR47W_MEMORY_ENV(name) ([]()->const char* { \
    if(!rr47w::cacheDiagnostics())return std::getenv(name); \
    static const std::optional<std::string> value=[]()->std::optional<std::string> { \
        const char* p=std::getenv(name);if(p)return std::string(p);return std::nullopt; \
    }(); \
    return value?value->c_str():nullptr; \
}())
