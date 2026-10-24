#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

namespace rr::platform {
class OriginalSaveProfileOwner50KG;
// Value-owned observation only, never authority to mutate an application.
// Source strcpy309230 overwrites a prefix through NUL and preserves the tail.
// Private byte backing is not a guest image: bytes beyond knownByteCount()
// are UNKNOWN and cannot be exported. Fresh reset establishes only byte0.
class OriginalPersistedProfileName50KG final {
public:
    OriginalPersistedProfileName50KG() noexcept=default;
    const char* c_str()const noexcept {return reinterpret_cast<const char*>(bytes_.data());}
    std::size_t knownByteCount()const noexcept {return knownBytes_;}
    // Only complete original backing can be serialized. In particular, a
    // nonempty known C string does not imply a known21-byte save field.
    std::optional<std::array<std::uint8_t,21>> exactBytes()const noexcept {
        if(knownBytes_!=bytes_.size())return std::nullopt;
        return bytes_;
    }
private:
    friend class OriginalSaveProfileOwner50KG;
    bool valid()const noexcept {
        if(!knownBytes_||knownBytes_>bytes_.size())return false;
        for(std::size_t i=0;i<knownBytes_;++i)if(bytes_[i]==0)return true;
        return false;
    }
    bool retainExact(const std::array<std::uint8_t,21>& source)noexcept {
        bool terminated=false;for(const auto b:source)if(!b){terminated=true;break;}
        if(!terminated)return false;
        bytes_=source;knownBytes_=bytes_.size();return true;
    }
    // Caller must be a future authenticated owner writer. Not a public setter
    // or proof of name-entry completion. view excludes its terminating NUL.
    // Reject embedded NUL rather than changing strcpy's observed prefix.
    // Copy via a temporary so even an overlapping source view is well-defined.
    bool replaceCString(std::string_view source)noexcept {
        if(source.size()>=bytes_.size())return false;
        for(const auto c:source)if(c=='\0')return false;
        auto next=bytes_;
        for(std::size_t i=0;i<source.size();++i)next[i]=static_cast<std::uint8_t>(source[i]);
        next[source.size()]=0;
        bytes_=next;
        if(knownBytes_<source.size()+1)knownBytes_=source.size()+1;
        return true;
    }
    std::array<std::uint8_t,21> bytes_{}; // Native backing, NOT exported unknown suffix.
    std::size_t knownBytes_=1;
};
}
