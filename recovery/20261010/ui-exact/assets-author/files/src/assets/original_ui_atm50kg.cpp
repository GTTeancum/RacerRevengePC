#include "original_ui_atm50kg.h"
#include <algorithm>
#include <new>
#include <utility>

namespace rr::assets {
namespace {
struct Failure { DecodeError48C error; };
void require(bool ok, DecodeKind48C kind, const char* code,
    std::size_t at, const char* detail) {
    if (!ok) throw Failure{{kind, code, at, detail}};
}
void cancel(const DecodeHooks48C* hooks, std::size_t at) {
    require(!hooks || !hooks->cancelled || !hooks->cancelled(hooks->context),
        DecodeKind48C::cancelled, "atm.cancelled", at, "Cancelled before output commit");
}
struct Reader {
    const std::uint8_t* input;
    std::size_t bytes;
    void range(std::size_t at, std::size_t count) const {
        require(at <= bytes && count <= bytes - at, DecodeKind48C::malformed,
            "atm.truncated", at, "Read exceeds ATM member extent");
    }
    std::uint16_t u16(std::size_t at) const {
        range(at, 2);
        return std::uint16_t(unsigned(input[at]) | (unsigned(input[at + 1]) << 8));
    }
    std::uint32_t u32(std::size_t at) const {
        range(at, 4);
        return std::uint32_t(input[at]) | (std::uint32_t(input[at + 1]) << 8) |
            (std::uint32_t(input[at + 2]) << 16) | (std::uint32_t(input[at + 3]) << 24);
    }
};
bool powerOfTwo(std::uint32_t value) { return value && !(value & (value - 1)); }
}

bool splitOriginalUiAtm50KG(const std::uint8_t* input, std::size_t bytes,
    OriginalUiAtm50KG& output, DecodeError48C& error,
    const DecodeHooks48C* hooks, const OriginalUiAtmLimits50KG& limits) {
    error = {};
    try {
        cancel(hooks, 0);
        require(input != nullptr, DecodeKind48C::malformed,
            "atm.null", 0, "ATM input is null");
        require(bytes <= std::min(limits.inputBytes, std::size_t(8u * 1024u * 1024u)),
            DecodeKind48C::limit, "atm.input_limit", 0, "ATM member exceeds byte ceiling");
        const Reader read{input, bytes};
        read.range(0, 12);
        require(read.u32(0) == 1, DecodeKind48C::unsupported,
            "atm.revision", 0, "Only original ATM revision 1 is supported");
        const auto count = read.u32(4);
        require(count != 0, DecodeKind48C::malformed,
            "atm.empty", 4, "ATM must contain at least one authored frame");
        require(count <= std::min(limits.frames, std::uint32_t(64)), DecodeKind48C::limit,
            "atm.frame_limit", 4, "ATM frame count exceeds bounded subset");
        require(read.u32(8) == 0, DecodeKind48C::unsupported, "atm.storage", 8,
            "Nonzero ATM selector uses the separate unimplemented TGA frame path");
        OriginalUiAtm50KG parsed;
        parsed.frames.reserve(count);
        std::size_t at = 12;
        // These checks deliberately mirror decodeTexture48E's admitted subset.
        // No search for plausible headers, alignment padding, or size division.
        for (std::uint32_t ordinal = 0; ordinal < count; ++ordinal) {
            cancel(hooks, at);
            OriginalUiAtmFrame50KG frame;
            frame.pstOffset = at;
            read.range(at, 12);
            require(read.u16(at) == 1, DecodeKind48C::unsupported,
                "atm.pst_revision", at, "Stored frame is not PST revision 1");
            require(read.u16(at + 2) == 0, DecodeKind48C::unsupported,
                "atm.pst_flags", at + 2, "Stored PST flags are outside observed subset");
            frame.kind = read.u32(at + 4);
            require(frame.kind == 0 || frame.kind == 1, DecodeKind48C::unsupported,
                "atm.pst_kind", at + 4, "Only source-backed indexed PST kinds 0 and 1 are supported");
            frame.mipCount = read.u32(at + 8);
            require(frame.mipCount >= 1 && frame.mipCount <= 7, DecodeKind48C::unsupported,
                "atm.pst_mips", at + 8, "Only one through seven stored mips are supported");
            const std::size_t paletteBytes = frame.kind == 0 ? 64u : 1024u;
            read.range(at + 12, paletteBytes);
            at += 12 + paletteBytes;
            std::uint32_t previousW = 0, previousH = 0;
            for (std::uint32_t mip = 0; mip < frame.mipCount; ++mip) {
                cancel(hooks, at);
                read.range(at, 16);
                const auto height = read.u32(at), width = read.u32(at + 4);
                require(width && height, DecodeKind48C::malformed,
                    "atm.pst_dimensions", at, "Zero stored texture dimension");
                require(width >= 16 && height >= 16 && width <= 1024 && height <= 1024 &&
                    powerOfTwo(width) && powerOfTwo(height), DecodeKind48C::unsupported,
                    "atm.pst_dimension_subset", at, "PST expansion requires power-of-two dimensions 16 through 1024");
                require(read.u32(at + 8) == 0 && read.u32(at + 12) == 0,
                    DecodeKind48C::unsupported, "atm.pst_surface_words", at + 8,
                    "Unrecognized stored PST mip header words");
                require(mip == 0 || (width == previousW * 2 && height == previousH * 2),
                    DecodeKind48C::malformed, "atm.pst_mip_order", at,
                    "Original stored mip dimensions must ascend by factors of two");
                // Bounded dimensions make this multiplication fit size_t even
                // on 32-bit hosts. Original 24EDC0 reads texels * bpp / 8.
                const std::size_t texels = std::size_t(width) * height;
                const std::size_t pixelBytes = frame.kind == 0 ? texels / 2 : texels;
                read.range(at + 16, pixelBytes);
                at += 16 + pixelBytes;
                previousW = width; previousH = height;
            }
            frame.pstBytes = at - frame.pstOffset;
            frame.width = previousW; frame.height = previousH;
            parsed.frames.push_back(frame);
            if (hooks && hooks->progress)
                hooks->progress(hooks->context, "atm.frames", ordinal + 1, count);
        }
        require(at == bytes, DecodeKind48C::malformed,
            "atm.trailing_bytes", at, "Unconsumed ATM suffix after authored frame count");
        cancel(hooks, at);
        output = std::move(parsed);
        return true;
    } catch (const Failure& failure) { error = failure.error; }
    catch (const std::bad_alloc&) {
        error = {DecodeKind48C::limit, "atm.allocation", 0, "Allocation failed within bounded frame splitter"};
    }
    return false;
}
}
