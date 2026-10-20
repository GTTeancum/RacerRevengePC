#pragma once
#include "import_decode48c.h"
#include <memory>

namespace rr::assets {
// Complete strict RES3 input with compact immutable storage for EVERY indexed
// member. Original logical offsets remain source offsets, never compact ones.
// This is format validation only. A course owner separately authenticates it.
class IndexedResArchive48C final {
public:
    struct Member {
        const ResourceEntry48C* identity=nullptr;
        const std::uint8_t* data=nullptr;
        std::size_t size=0;
    };
    IndexedResArchive48C() noexcept;
    ~IndexedResArchive48C();
    IndexedResArchive48C(const IndexedResArchive48C&)=delete;
    IndexedResArchive48C& operator=(const IndexedResArchive48C&)=delete;
    IndexedResArchive48C(IndexedResArchive48C&&)=delete;
    IndexedResArchive48C& operator=(IndexedResArchive48C&&)=delete;
    // One attempt, transactional input ownership: success moves the exact
    // original allocation; failure leaves input unchanged. No mutable aliases
    // may survive successful transfer. Every block (including gaps) is checked.
    bool loadOwned(std::vector<std::uint8_t>&&,std::size_t availablePayloadBytes,
        DecodeError48C&,const DecodeHooks48C* = nullptr,
        const DecodeLimits48C& = DecodeLimits48C{}) noexcept;
    bool loaded() const noexcept;
    const std::vector<ResourceEntry48C>& entries() const noexcept;
    const std::vector<ResourceBlock48C>& blocks() const noexcept;
    const std::vector<std::uint8_t>& original() const noexcept;
    std::size_t decodedExtent() const noexcept;
    std::size_t compactBytes() const noexcept;
    Member member(std::size_t ordinal) const noexcept;
    bool extractMember(const std::string&,std::vector<std::uint8_t>&,
        DecodeError48C&,std::size_t maxBytes=8u*1024u*1024u) const noexcept;
    // Conservative capacity/descriptor payload accounting. Includes inline
    // string capacities again; excludes malloc metadata/control-block overhead.
    std::size_t retainedPayloadBytes() const noexcept;
    std::size_t preparationPeakPayloadBytes() const noexcept;
    std::size_t codecPeakBytes() const noexcept;
    static constexpr std::size_t blockScratchBytes=24577;
    static constexpr std::size_t codecArenaBytes=65536;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
    bool attempted_=false;
};
}
