#pragma once
#include <string>
namespace rr::assets::detail {
// Original1169D0/24DA98..24DAD4: last dot anywhere, no separator parsing.
// Spelling only; no provider lookup, readiness or fallback authority is granted.
inline std::string originalPstCandidate24DADC(const std::string& requested) {
    std::string candidate=requested;
    const auto dot=candidate.find_last_of('.');
    if(dot==std::string::npos)candidate+=".pst";
    else candidate.replace(dot,std::string::npos,".pst");
    return candidate;
}
}
