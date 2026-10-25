#pragma once
#include "import_decode48c.h"
#include <memory>
#include <utility>

namespace ta_original_world50LE {class Owner;}
namespace rr::assets {
class OriginalStandardArchiveProvider50LA;
class OriginalTaEncodedArchiveView50LA;
// Exact original TA.RES input only. Minted after the actual TA world owner's
// existing full SHA/decode checks. No decoded data, provider/domain back-link,
// file handles, mutable cursor, cache slots or source service readiness.
class OriginalTaEncodedArchive50LA final {
    friend class ::ta_original_world50LE::Owner;
    friend class OriginalStandardArchiveProvider50LA;
    friend class OriginalTaEncodedArchiveView50LA;
    std::vector<std::uint8_t> bytes_;
    std::vector<ResourceBlock48C> blocks_;
    OriginalTaEncodedArchive50LA()=default;
public:
    ~OriginalTaEncodedArchive50LA()=default;
    OriginalTaEncodedArchive50LA(const OriginalTaEncodedArchive50LA&)=delete;
    OriginalTaEncodedArchive50LA& operator=(const OriginalTaEncodedArchive50LA&)=delete;
};
// Distinct allocation identity for ONE actual provider mount. It owns only
// immutable input, never the provider. A retained view prevents identity reuse
// even if a later provider has the same address, input and numeric epochs.
class OriginalTaEncodedBinding50LA final {
    friend class OriginalStandardArchiveProvider50LA;
    friend class OriginalTaEncodedArchiveView50LA;
    const std::shared_ptr<const OriginalTaEncodedArchive50LA> input_;
    explicit OriginalTaEncodedBinding50LA(std::shared_ptr<const OriginalTaEncodedArchive50LA> input)noexcept
        :input_(std::move(input)){}
public:
    ~OriginalTaEncodedBinding50LA()=default;
    OriginalTaEncodedBinding50LA(const OriginalTaEncodedBinding50LA&)=delete;
    OriginalTaEncodedBinding50LA& operator=(const OriginalTaEncodedBinding50LA&)=delete;
};
// Historical immutable input pin. Only the actual mounted provider issues it.
// Provider identity is comparison-only, never an owning/service pointer.
// Bytes may outlive that provider or a mount epoch; future live consumers must
// retain the actual provider and call encodedTaInputCurrent before using it.
class OriginalTaEncodedArchiveView50LA final {
    friend class OriginalStandardArchiveProvider50LA;
    const OriginalStandardArchiveProvider50LA* provider_=nullptr;
    std::shared_ptr<const OriginalTaEncodedBinding50LA> binding_;
    std::size_t mount_=static_cast<std::size_t>(-1);
    std::uint64_t revision_=0,generation_=0;
    OriginalTaEncodedArchiveView50LA(const OriginalStandardArchiveProvider50LA* provider,
        std::shared_ptr<const OriginalTaEncodedBinding50LA> binding,std::size_t mount,
        std::uint64_t revision,std::uint64_t generation)noexcept
        :provider_(provider),binding_(std::move(binding)),mount_(mount),revision_(revision),generation_(generation){}
public:
    OriginalTaEncodedArchiveView50LA()noexcept=default;
    const OriginalStandardArchiveProvider50LA* providerIdentity()const noexcept{return provider_;}
    const OriginalTaEncodedArchive50LA* inputIdentity()const noexcept{return binding_?binding_->input_.get():nullptr;}
    const std::uint8_t* data()const noexcept{return binding_?binding_->input_->bytes_.data():nullptr;}
    std::size_t size()const noexcept{return binding_?binding_->input_->bytes_.size():0;}
    std::size_t blockCount()const noexcept{return binding_?binding_->input_->blocks_.size():0;}
    bool block(std::size_t index,ResourceBlock48C& out)const noexcept {
        out={};if(!binding_||index>=binding_->input_->blocks_.size())return false;
        out=binding_->input_->blocks_[index];return true;
    }
};
}
