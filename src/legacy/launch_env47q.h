#pragma once
#include <cstdlib>
#include <optional>
#include <string>
namespace rr47q {
inline bool cacheDiagnostics() {
    static const bool enabled=[] { const char* p=std::getenv("RR_CACHE_DIAGNOSTICS47Q");return p&&p[0]=='1'&&p[1]=='\0'; }();
    return enabled;
}
}
// Only literal RR_TRACE_ and RR_CAPTURE_ settings are replaced. Their launch
// values are owned, so later CRT environment storage changes cannot dangle them.
// The control experiment still performs the original getenv on every call.
#define RR47Q_DIAGNOSTIC_ENV(name) ([]()->const char* { \
    if(!rr47q::cacheDiagnostics())return std::getenv(name); \
    static const std::optional<std::string> value=[]()->std::optional<std::string> { \
        const char* p=std::getenv(name);if(p)return std::string(p);return std::nullopt; \
    }(); \
    return value?value->c_str():nullptr; \
}())
