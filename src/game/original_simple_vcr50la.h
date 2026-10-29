#pragma once
#include <cstdint>
#include <memory>

namespace rr::game {
class OriginalApplicationEntities50LA;
class OriginalRaceConstructor50LA;
struct OriginalSimpleVCRDiagnostic50LA {
    std::uint32_t sourcePC=0;
    const char* operation=nullptr;
};
// Actual SimpleVCR287430/287710 storage, not a null Race78 substitute or
// replay-complete claim. Only the genuine Race owner can construct/use it.
// The private Race producer is implemented but UNCALLED:1A9240 and all
// intervening original loaders must return before1A6E18/50 may create it.
class OriginalSimpleVCR50LA final {
    friend class OriginalRaceConstructor50LA;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    OriginalSimpleVCR50LA(std::shared_ptr<OriginalApplicationEntities50LA>,std::uint64_t);
    bool constructed()const noexcept;
    // Exact two inspected Race call shapes. Source selection is made by the
    // future actual caller, never a public subtype/ready input or Session flag.
    bool setup287710(std::uint32_t capacity,std::uint32_t bytes,bool backup,
        OriginalSimpleVCRDiagnostic50LA&)noexcept;
    //Registers the actual canonical Race90 address, never a copied timer or
    //host raceSeconds. No read/write of the registered word occurs here.
    bool registerRaceTime1A6E98(OriginalRaceConstructor50LA&,
        OriginalSimpleVCRDiagnostic50LA&)noexcept;
    bool belongsToRace(const OriginalRaceConstructor50LA&)const noexcept;
    bool readEndCallerBit5(const OriginalRaceConstructor50LA&,bool&,
        OriginalSimpleVCRDiagnostic50LA&)const noexcept;
    bool startRecord1A80C8(const OriginalRaceConstructor50LA&,
        OriginalSimpleVCRDiagnostic50LA&)noexcept;
    // Actual zero-ACK1AB520 caller only; direct channel stores precede the
    // real288630 backup. No public channel list or caller-supplied count.
    bool enableEndChannelsAndBackup1ABA68(const OriginalRaceConstructor50LA&,
        std::int32_t& indexS0,OriginalSimpleVCRDiagnostic50LA&)noexcept;
    bool reset287BE0(OriginalSimpleVCRDiagnostic50LA&)noexcept;
    bool copyBackup288630(OriginalSimpleVCRDiagnostic50LA&)noexcept;
    bool closeDetached2875A0(OriginalSimpleVCRDiagnostic50LA&)noexcept;
public:
    OriginalSimpleVCR50LA(const OriginalSimpleVCR50LA&)=delete;
    OriginalSimpleVCR50LA& operator=(const OriginalSimpleVCR50LA&)=delete;
    ~OriginalSimpleVCR50LA()noexcept;
};
}
