#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>

namespace native_race_replay50PD {class Owner;}
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
    friend class ::native_race_replay50PD::Owner;
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
    // 287B60: clears +B0 bits 5, 1, 2, 3, 7 (ANDI t2,zero,1 yields zero) in that
    // order, then tail-jumps 287BE0. Reached by the race's 1AC8E0 VCR tail (1ACDA0/1ACDC0).
    bool clear287B60(OriginalSimpleVCRDiagnostic50LA&)noexcept;
    // 1ACDC8..1ACDE0: race78+B0 bit1 = 1 (record), after 287B60, when 11258 != 3.
    bool setRecordBit1ACDE0(OriginalSimpleVCRDiagnostic50LA&)noexcept;
    bool copyBackup288630(OriginalSimpleVCRDiagnostic50LA&)noexcept;
    bool closeDetached2875A0(OriginalSimpleVCRDiagnostic50LA&)noexcept;
    // 2878F0: one float channel (class 0). source/destination are 32-bit
    // binary32 words owned by the caller for this VCR's whole lifetime;
    // destination null aliases source. Returns the channel index, or -1 after
    // the source attempted7C increment (capacity, bit4 or scale <= 0).
    std::int32_t addChannel2878F0(std::uint32_t* source,std::uint32_t* destination,
        std::uint32_t interpolation94,float scale,std::uint32_t enabled90)noexcept;
    // Native channel backing reserved by this VCR for its whole lifetime (the
    // port's stand-in for racer+C00..C78/B5C fields). One reservation only;
    // freed by 2875A0 after the last possible channel access.
    bool reserveNativeChannelBacking(std::size_t words,std::size_t bytes,
        std::uint32_t*& wordBase,std::uint8_t*& byteBase)noexcept;
    // 287A30: one byte channel (class 1, interpolation 4, scale 1).
    std::int32_t addByteChannel287A30(std::uint8_t* source,std::uint8_t* destination,
        std::uint32_t enabled90)noexcept;
    // 287C80 stores the playback rate (+50).
    bool setRate287C80(float rate,OriginalSimpleVCRDiagnostic50LA&)noexcept;
    // 287C90 (vtable+38): App11C pause returns; sets bit4; bit1 records
    // (keyframe 287DD0, delta 288030 every +4C = 1/15 s); bit2 plays 288670.
    bool update287C90(float dt,bool paused11C,OriginalSimpleVCRDiagnostic50LA&)noexcept;
    // The next unpaused 287C90 writes a frame (bit1 and +40 == 0 or +44 <= 0).
    bool recordSampleDue()const noexcept;
    // 1ABB2C..1ABB68 (Results) and 1A81C0..1A81D8 (loop): the exact bit
    // stores on +B0. clearRecord: bit1 = 0; always bit2 = 1.
    bool setPlayback(bool clearRecord,OriginalSimpleVCRDiagnostic50LA&)noexcept;
public:
    struct State {
        bool recording=false,playing=false,ended=false,full=false;
        std::uint32_t channels=0,bytesUsed=0,capacityBytes=0,keyframes=0,deltas=0,escapes=0,
            framesRead=0;
        float time30=0,play34=0;
    };
    State state()const noexcept;
public:
    OriginalSimpleVCR50LA(const OriginalSimpleVCR50LA&)=delete;
    OriginalSimpleVCR50LA& operator=(const OriginalSimpleVCR50LA&)=delete;
    ~OriginalSimpleVCR50LA()noexcept;
};
}
