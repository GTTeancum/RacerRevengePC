#include "import_decode48c.h"
#include "indexed_resource48c.h"
#include <cstddef>
#include <zlib.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <utility>

namespace rr { namespace assets {
namespace {
constexpr std::size_t blockBytes = 24576;
constexpr std::size_t maxResourceFile = 64u * 1024u * 1024u;
constexpr std::size_t maxDecoded = 128u * 1024u * 1024u;
constexpr std::size_t maxMeshFile = 8u * 1024u * 1024u;
struct Failure { DecodeError48C error; };
void require(bool condition, DecodeKind48C kind, const char* code, std::size_t offset, const char* detail) {
    if (!condition) throw Failure{{kind, code, offset, detail}};
}
struct Reader {
    const std::vector<std::uint8_t>& bytes;
    void range(std::size_t offset, std::size_t count) const {
        require(offset <= bytes.size() && count <= bytes.size() - offset,
            DecodeKind48C::malformed, "input.truncated", offset, "Read exceeds input extent");
    }
    std::uint32_t u32(std::size_t offset) const {
        range(offset, 4);
        return std::uint32_t(bytes[offset]) | (std::uint32_t(bytes[offset + 1]) << 8) |
            (std::uint32_t(bytes[offset + 2]) << 16) | (std::uint32_t(bytes[offset + 3]) << 24);
    }
    std::uint16_t u16(std::size_t offset) const {
        range(offset, 2);
        return std::uint16_t(std::uint16_t(bytes[offset]) | (std::uint16_t(bytes[offset + 1]) << 8));
    }
    int i16(std::size_t offset) const {
        const unsigned value = u16(offset);
        return value < 32768 ? static_cast<int>(value) : static_cast<int>(value) - 65536;
    }
    int i8(std::size_t offset) const {
        range(offset, 1);
        const unsigned value = bytes[offset];
        return value < 128 ? static_cast<int>(value) : static_cast<int>(value) - 256;
    }
    double f32(std::size_t offset) const {
        static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559, "IEEE754 binary32 required");
        const std::uint32_t bits = u32(offset);
        float value;
        std::memcpy(&value, &bits, 4);
        return value;
    }
    bool zero(std::size_t start, std::size_t length) const {
        range(start, length);
        for (std::size_t i = 0; i < length; ++i) if (bytes[start + i]) return false;
        return true;
    }
    std::string fixedString(std::size_t start, std::size_t length) const {
        range(start, length);
        std::size_t end = start;
        while (end < start + length && bytes[end]) {
            require(bytes[end] < 128, DecodeKind48C::unsupported, "psg.string_non_ascii", end, "Fixed string is not ASCII");
            ++end;
        }
        require(end < start + length, DecodeKind48C::malformed, "psg.string_unterminated", start, "Fixed string has no terminator");
        require(zero(end, start + length - end), DecodeKind48C::unsupported, "psg.string_padding", end, "Nonzero fixed-string suffix");
        return std::string(bytes.begin() + start, bytes.begin() + end);
    }
};
void checkCancel(const DecodeHooks48C* hooks, std::size_t offset) {
    require(!hooks || !hooks->cancelled || !hooks->cancelled(hooks->context), DecodeKind48C::cancelled,
        "decode.cancelled", offset, "Cancelled before output commit");
}
void progress(const DecodeHooks48C* hooks, const char* stage, std::uint32_t done, std::uint32_t total) {
    if (hooks && hooks->progress) hooks->progress(hooks->context, stage, done, total);
}
void appendU32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) out.push_back(static_cast<std::uint8_t>(value >> (8 * i)));
}
void appendF32(std::vector<std::uint8_t>& out, double value) {
    const float stored = static_cast<float>(value);
    require(std::isfinite(stored), DecodeKind48C::unsupported, "psg.output_float_range", 0, "Decoded value does not fit finite binary32");
    std::uint32_t bits;
    std::memcpy(&bits, &stored, 4);
    appendU32(out, bits);
}
bool sameAsciiName(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        const auto fold = [](unsigned char value) { return value >= 'A' && value <= 'Z' ? value + ('a' - 'A') : value; };
        if (fold(static_cast<unsigned char>(a[i])) != fold(static_cast<unsigned char>(b[i]))) return false;
    }
    return true;
}
struct Inflater {
    z_stream stream{};
    bool initialized = false;
    ~Inflater() { if (initialized) inflateEnd(&stream); }
};
void allocationError(DecodeError48C& error) {
    error = {DecodeKind48C::limit, "decode.allocation_failed", 0, "Allocation could not be completed"};
}
struct DecodedVertex {
    std::array<double, 3> position{}, normal{};
    std::array<double, 2> uv{};
    std::array<std::uint8_t, 4> color{};
};
}

namespace {
struct ResourceReadData {
    std::vector<ResourceEntry48C> entries;
    std::vector<ResourceBlock48C> blocks;
    std::vector<std::uint8_t> payload;
    std::vector<std::size_t> compactOffsets;
    std::size_t logicalExtent=0,preparationPeak=0,codecPeak=0;
};
void payloadAdd(std::size_t& n,std::size_t bytes){
    require(bytes<=std::numeric_limits<std::size_t>::max()-n,DecodeKind48C::limit,
        "res.compact_payload_limit",0,"Indexed resource payload sum overflow");n+=bytes;
}
std::size_t resourcePayload(const ResourceReadData& p){
    std::size_t n=sizeof(ResourceReadData);
    payloadAdd(n,p.entries.capacity()*sizeof(ResourceEntry48C));
    payloadAdd(n,p.blocks.capacity()*sizeof(ResourceBlock48C));
    payloadAdd(n,p.payload.capacity());payloadAdd(n,p.compactOffsets.capacity()*sizeof(std::size_t));
    for(const auto& e:p.entries)payloadAdd(n,e.name.capacity()+1);return n;
}
// A charged fixed scratch arena bounds zlib's allocations for the new compact
// path. Legacy decoding retains its original default allocator and error path.
struct CodecArena {
    alignas(std::max_align_t) std::uint8_t bytes[IndexedResArchive48C::codecArenaBytes]{};
    std::size_t used=0;
    void reset() noexcept {used=0;}
    static voidpf allocate(voidpf opaque,uInt count,uInt size) noexcept {
        auto& self=*static_cast<CodecArena*>(opaque);
        if(size&&count>std::numeric_limits<std::size_t>::max()/size)return nullptr;
        const auto n=std::size_t(count)*size;
        const auto alignment=alignof(std::max_align_t);
        const auto at=(self.used+alignment-1)/alignment*alignment;
        if(!n||at>sizeof self.bytes||n>sizeof self.bytes-at)return nullptr;
        auto* result=self.bytes+at;std::memset(result,0,n);self.used=at+n;return result;
    }
    static void release(voidpf,voidpf) noexcept {}
};
bool decodeResourceData48C(const std::vector<std::uint8_t>& input, ResourceReadData& output,
    bool compact,std::size_t availablePayloadBytes,std::size_t outerStorageBytes,
    DecodeError48C& error, const DecodeHooks48C* hooks, const DecodeLimits48C& limits) {
    error = {};
    try {
        checkCancel(hooks, 0);
        require(input.size() <= std::min(maxResourceFile, limits.resource_file_bytes), DecodeKind48C::limit,
            "res.file_limit", 0, "Compressed resource exceeds size limit");
        require(input.size() >= 28, DecodeKind48C::malformed, "res.header_truncated", 0, "Resource header is truncated");
        const Reader reader{input};
        require(reader.u32(0) == 3 && reader.u32(8) == 0x340 && reader.u32(12) == 0,
            DecodeKind48C::unsupported, "res.header_variant", 0, "Only observed RES3 codec 0x340 flags0 supported");
        const std::uint32_t meta = reader.u32(4), tableBytes = reader.u32(16);
        require(tableBytes >= 2 && tableBytes % 2 == 0 && 20ull + tableBytes == 8ull + meta,
            DecodeKind48C::malformed, "res.size_table", 16, "Compressed-size table structure is invalid");
        reader.range(20, tableBytes);
        const std::size_t blockCount = tableBytes / 2 - 1;
        require(blockCount <= std::min(maxDecoded, limits.resource_decoded_bytes) / blockBytes,
            DecodeKind48C::limit, "res.decoded_limit", 16, "Decoded resource exceeds size limit");
        const std::size_t decodedBytes = blockCount * blockBytes;
        require(reader.u16(20 + blockCount * 2) == 0, DecodeKind48C::malformed,
            "res.block_terminator", 20 + blockCount * 2, "Size table does not end in zero");
        for (std::size_t i = 0; i < blockCount; ++i)
            require(reader.u16(20 + i * 2) != 0, DecodeKind48C::malformed, "res.zero_block", 20 + i * 2, "Premature zero block size");
        std::size_t offset = 20 + static_cast<std::size_t>(tableBytes);
        const std::uint32_t count = reader.u32(offset), indexBytes = reader.u32(offset + 4);
        require(count <= std::min(std::uint32_t(10000), limits.resource_entries), DecodeKind48C::limit,
            "res.entry_limit", offset, "Resource entry count exceeds limit");
        offset += 8;
        reader.range(offset, indexBytes);
        const std::size_t indexEnd = offset + indexBytes;
        ResourceReadData parsed;
        parsed.logicalExtent=decodedBytes;
        if(compact){
            // Bound metadata before any index/name allocation. The factor2
            // covers the pinned string growth from its initial inline buffer.
            const auto nameCap=std::min(std::uint32_t(1024),limits.resource_name_bytes);
            std::size_t reservation=input.capacity();payloadAdd(reservation,outerStorageBytes);payloadAdd(reservation,sizeof(ResourceReadData));
            payloadAdd(reservation,std::size_t(count)*(sizeof(ResourceEntry48C)+2u*std::max(15u,nameCap)+1u+sizeof(std::size_t)));
            payloadAdd(reservation,blockCount*sizeof(ResourceBlock48C));payloadAdd(reservation,sizeof(CodecArena));payloadAdd(reservation,blockBytes+1);
            require(reservation<=availablePayloadBytes,DecodeKind48C::limit,"res.compact_payload_limit",0,"Indexed resource metadata/scratch exceeds remaining payload budget");
        }
        parsed.entries.reserve(count);
        for (std::uint32_t i = 0; i < count; ++i) {
            require(offset <= indexEnd && indexEnd - offset >= 4, DecodeKind48C::malformed,
                "res.index_truncated", offset, "Truncated filename record");
            const std::uint32_t nameBytes = reader.u32(offset);
            offset += 4;
            require(nameBytes > 0, DecodeKind48C::malformed, "res.empty_name", offset, "Empty resource filename");
            require(nameBytes <= std::min(std::uint32_t(1024), limits.resource_name_bytes), DecodeKind48C::limit,
                "res.name_limit", offset, "Resource filename exceeds limit");
            require(offset <= indexEnd && std::uint64_t(nameBytes) + 8 <= indexEnd - offset, DecodeKind48C::malformed,
                "res.name_record_truncated", offset, "Filename record crosses index boundary");
            for (std::uint32_t j = 0; j < nameBytes; ++j)
                require(input[offset + j] > 0 && input[offset + j] < 128, DecodeKind48C::unsupported,
                    "res.name_encoding", offset + j, "Filename must be ASCII without embedded NUL");
            ResourceEntry48C entry;
            entry.name.assign(input.begin() + offset, input.begin() + offset + nameBytes);
            offset += nameBytes;
            entry.logical_offset = reader.u32(offset); entry.bytes = reader.u32(offset + 4);
            require(std::uint64_t(entry.logical_offset) + entry.bytes <= decodedBytes, DecodeKind48C::malformed,
                "res.member_extent", offset, "Resource member extends outside decoded storage");
            offset += 8;
            parsed.entries.push_back(std::move(entry));
        }
        require(offset == indexEnd, DecodeKind48C::malformed, "res.index_suffix", offset, "Unconsumed filename index bytes");
        std::unique_ptr<CodecArena> arena;
        if(compact){
            std::size_t compactBytes=0;
            for(const auto& entry:parsed.entries){
                require(entry.bytes<=std::numeric_limits<std::size_t>::max()-compactBytes,
                    DecodeKind48C::limit,"res.compact_payload_limit",entry.logical_offset,"Compact member sum overflow");
                compactBytes+=entry.bytes;
            }
            std::size_t reservation=resourcePayload(parsed);payloadAdd(reservation,input.capacity());payloadAdd(reservation,outerStorageBytes);
            payloadAdd(reservation,count*sizeof(std::size_t)+blockCount*sizeof(ResourceBlock48C));
            payloadAdd(reservation,compactBytes);payloadAdd(reservation,sizeof(CodecArena));payloadAdd(reservation,blockBytes+1);
            require(reservation<=availablePayloadBytes,DecodeKind48C::limit,"res.compact_payload_limit",0,"Complete compact resource exceeds remaining payload budget");
            parsed.compactOffsets.reserve(count);std::size_t at=0;
            for(const auto& entry:parsed.entries){parsed.compactOffsets.push_back(at);at+=entry.bytes;}
            parsed.payload.resize(compactBytes);arena=std::make_unique<CodecArena>();
        }
        parsed.blocks.reserve(blockCount);
        if(!compact)parsed.payload.reserve(decodedBytes);
        if(compact){
            parsed.preparationPeak=resourcePayload(parsed);payloadAdd(parsed.preparationPeak,input.capacity());
            payloadAdd(parsed.preparationPeak,outerStorageBytes);payloadAdd(parsed.preparationPeak,sizeof(CodecArena));payloadAdd(parsed.preparationPeak,blockBytes+1);
            require(parsed.preparationPeak<=availablePayloadBytes,DecodeKind48C::limit,"res.compact_payload_limit",0,"Actual indexed capacities exceed remaining payload budget");
        }
        progress(hooks, "res_blocks", 0, static_cast<std::uint32_t>(blockCount));
        for (std::size_t i = 0; i < blockCount; ++i) {
            checkCancel(hooks, offset);
            const std::uint16_t compressedBytes = reader.u16(20 + i * 2);
            reader.range(offset, static_cast<std::size_t>(compressedBytes) + 1);
            require(input[offset + compressedBytes] == 255, DecodeKind48C::malformed,
                "res.block_boundary", offset + compressedBytes, "Compressed block is not followed by 0xff");
            std::array<std::uint8_t, blockBytes + 1> decoded{};
            Inflater inflater;
            if(compact){arena->reset();inflater.stream.zalloc=&CodecArena::allocate;
                inflater.stream.zfree=&CodecArena::release;inflater.stream.opaque=arena.get();}
            const int initialized = inflateInit(&inflater.stream);
            require(initialized == Z_OK, DecodeKind48C::limit, "res.zlib_init", offset, "zlib initialization failed");
            inflater.initialized = true;
            inflater.stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(input.data() + offset));
            inflater.stream.avail_in = compressedBytes;
            inflater.stream.next_out = reinterpret_cast<Bytef*>(decoded.data());
            inflater.stream.avail_out = static_cast<uInt>(decoded.size());
            const int status = inflate(&inflater.stream, Z_FINISH);
            require(status != Z_MEM_ERROR, DecodeKind48C::limit, "res.zlib_allocation", offset, "zlib allocation failed");
            require(status == Z_STREAM_END && inflater.stream.total_out == blockBytes &&
                inflater.stream.total_in == compressedBytes && inflater.stream.avail_in == 0,
                DecodeKind48C::malformed, "res.zlib_block", offset,
                "zlib stream must end after exactly one 24576-byte block with no unused input");
            parsed.blocks.push_back({static_cast<std::uint32_t>(offset), compressedBytes});
            if(!compact)parsed.payload.insert(parsed.payload.end(), decoded.begin(), decoded.begin() + blockBytes);
            else {
                const std::size_t first=i*blockBytes,last=first+blockBytes;
                for(std::size_t member=0;member<parsed.entries.size();++member){
                    const auto& entry=parsed.entries[member];
                    const auto from=std::max(first,std::size_t(entry.logical_offset));
                    const auto to=std::min(last,std::size_t(entry.logical_offset)+entry.bytes);
                    if(from<to)std::memcpy(parsed.payload.data()+parsed.compactOffsets[member]+from-entry.logical_offset,
                        decoded.data()+from-first,to-from);
                }
                parsed.codecPeak=std::max(parsed.codecPeak,arena->used);
            }
            offset += static_cast<std::size_t>(compressedBytes) + 1;
            progress(hooks, "res_blocks", static_cast<std::uint32_t>(i + 1), static_cast<std::uint32_t>(blockCount));
        }
        require(offset == input.size(), DecodeKind48C::malformed, "res.trailing_bytes", offset, "Unconsumed resource suffix");
        checkCancel(hooks, offset);
        output = std::move(parsed);
        return true;
    } catch (const Failure& failure) { error = failure.error; }
    catch (const std::bad_alloc&) { allocationError(error); }
    return false;
}

} // private shared strict resource decoder
bool decodeResource48C(const std::vector<std::uint8_t>& input, ResArchive48C& output,
    DecodeError48C& error,const DecodeHooks48C* hooks,const DecodeLimits48C& limits) {
    ResourceReadData parsed;
    if(!decodeResourceData48C(input,parsed,false,0,0,error,hooks,limits))return false;
    output.entries=std::move(parsed.entries);output.blocks=std::move(parsed.blocks);
    output.decoded=std::move(parsed.payload);return true;
}
struct IndexedResArchive48C::Storage {
    ResourceReadData data;
    std::vector<std::uint8_t> original;
};
IndexedResArchive48C::IndexedResArchive48C() noexcept=default;
IndexedResArchive48C::~IndexedResArchive48C()=default;
bool IndexedResArchive48C::loadOwned(std::vector<std::uint8_t>&& input,
    std::size_t availablePayloadBytes,DecodeError48C& error,const DecodeHooks48C* hooks,
    const DecodeLimits48C& limits) noexcept {
    error={};if(attempted_){error={DecodeKind48C::unsupported,"res.indexed_already_attempted",0,"Indexed resource is one-attempt"};return false;}
    attempted_=true;
    try {
        require(availablePayloadBytes>=sizeof(IndexedResArchive48C)+sizeof(Storage),DecodeKind48C::limit,
            "res.compact_payload_limit",0,"Indexed resource owner exceeds remaining payload budget");
        auto next=std::make_unique<Storage>();
        if(!decodeResourceData48C(input,next->data,true,availablePayloadBytes,
            sizeof(IndexedResArchive48C)+sizeof(Storage),error,hooks,limits))return false;
        next->original=std::move(input);storage_=std::move(next);return true;
    }catch(const Failure& failure){error=failure.error;}
    catch(const std::bad_alloc&){allocationError(error);}
    catch(...){error={DecodeKind48C::unsupported,"res.indexed_exception",0,"Unexpected indexed resource exception"};}
    return false;
}
bool IndexedResArchive48C::loaded() const noexcept{return bool(storage_);}
const std::vector<ResourceEntry48C>& IndexedResArchive48C::entries() const noexcept {
    static const std::vector<ResourceEntry48C> empty;return storage_?storage_->data.entries:empty;
}
const std::vector<ResourceBlock48C>& IndexedResArchive48C::blocks() const noexcept {
    static const std::vector<ResourceBlock48C> empty;return storage_?storage_->data.blocks:empty;
}
const std::vector<std::uint8_t>& IndexedResArchive48C::original() const noexcept {
    static const std::vector<std::uint8_t> empty;return storage_?storage_->original:empty;
}
std::size_t IndexedResArchive48C::decodedExtent() const noexcept{return storage_?storage_->data.logicalExtent:0;}
std::size_t IndexedResArchive48C::compactBytes() const noexcept{return storage_?storage_->data.payload.size():0;}
IndexedResArchive48C::Member IndexedResArchive48C::member(std::size_t i) const noexcept {
    if(!storage_||i>=storage_->data.entries.size())return {};
    const auto& s=storage_->data;const auto& e=s.entries[i];
    return {&e,e.bytes?s.payload.data()+s.compactOffsets[i]:nullptr,e.bytes};
}
bool IndexedResArchive48C::extractMember(const std::string& name,std::vector<std::uint8_t>& output,
    DecodeError48C& error,std::size_t maxBytes) const noexcept {
    error={};try {
        require(bool(storage_),DecodeKind48C::unsupported,"res.indexed_not_loaded",0,"Indexed resource is not published");
        const ResourceEntry48C* found=nullptr;std::size_t ordinal=0;
        for(std::size_t i=0;i<storage_->data.entries.size();++i){const auto& entry=storage_->data.entries[i];
            if(sameAsciiName(entry.name,name)){
                require(found==nullptr,DecodeKind48C::unsupported,"res.member_ambiguous",0,"Multiple case-insensitive filename matches");
                found=&entry;ordinal=i;}}
        require(found!=nullptr,DecodeKind48C::unsupported,"res.member_missing",0,"Requested member is absent");
        require(found->bytes>0&&found->bytes<=std::min(maxMeshFile,maxBytes),DecodeKind48C::limit,
            "res.member_limit",found->logical_offset,"Requested member exceeds extraction size bounds");
        const auto view=member(ordinal);std::vector<std::uint8_t> next(view.data,view.data+view.size);
        output=std::move(next);return true;
    }catch(const Failure& f){error=f.error;}catch(const std::bad_alloc&){allocationError(error);}
    catch(...){error={DecodeKind48C::unsupported,"res.indexed_exception",0,"Unexpected indexed extraction exception"};}
    return false;
}
std::size_t IndexedResArchive48C::retainedPayloadBytes() const noexcept {
    if(!storage_)return sizeof(*this);
    try{auto n=resourcePayload(storage_->data);payloadAdd(n,sizeof(*this));payloadAdd(n,sizeof(Storage));payloadAdd(n,storage_->original.capacity());return n;}
    catch(...){return std::numeric_limits<std::size_t>::max();}
}
std::size_t IndexedResArchive48C::preparationPeakPayloadBytes() const noexcept {
    return storage_?storage_->data.preparationPeak:0;
}
std::size_t IndexedResArchive48C::codecPeakBytes() const noexcept{return storage_?storage_->data.codecPeak:0;}

bool extractResMember48C(const ResArchive48C& archive, const std::string& name,
    std::vector<std::uint8_t>& output, DecodeError48C& error, std::size_t maxBytes) {
    error = {};
    try {
        const ResourceEntry48C* found = nullptr;
        for (const auto& entry : archive.entries) if (sameAsciiName(entry.name, name)) {
            require(found == nullptr, DecodeKind48C::unsupported, "res.member_ambiguous", 0, "Multiple case-insensitive filename matches");
            found = &entry;
        }
        require(found != nullptr, DecodeKind48C::unsupported, "res.member_missing", 0, "Requested member is absent");
        require(found->bytes > 0 && found->bytes <= std::min(maxMeshFile, maxBytes), DecodeKind48C::limit,
            "res.member_limit", found->logical_offset, "Requested member exceeds extraction size bounds");
        require(std::uint64_t(found->logical_offset) + found->bytes <= archive.decoded.size(), DecodeKind48C::malformed,
            "res.member_extent", found->logical_offset, "Member extent exceeds archive storage");
        std::vector<std::uint8_t> parsed(archive.decoded.begin() + found->logical_offset,
            archive.decoded.begin() + found->logical_offset + found->bytes);
        output = std::move(parsed);
        return true;
    } catch (const Failure& failure) { error = failure.error; }
    catch (const std::bad_alloc&) { allocationError(error); }
    return false;
}

bool decodeMesh48C(const std::vector<std::uint8_t>& input, PsgMesh48C& output,
    DecodeError48C& error, const DecodeHooks48C* hooks, const DecodeLimits48C& limits) {
    error = {};
    try {
        checkCancel(hooks, 0);
        require(input.size() <= std::min(maxMeshFile, limits.mesh_file_bytes), DecodeKind48C::limit,
            "psg.file_limit", 0, "Mesh exceeds size limit");
        require(input.size() >= 368, DecodeKind48C::malformed, "psg.header_truncated", 0, "PSG header is truncated");
        const Reader reader{input};
        require(reader.u32(0) == 0x00677370 && reader.u32(4) == 3, DecodeKind48C::unsupported,
            "psg.version", 0, "Only PSG version3 is supported");
        require(reader.u32(8) == 1, DecodeKind48C::unsupported, "psg.hierarchy_count", 8, "Only one static hierarchy object supported");
        PsgMesh48C parsed;
        parsed.object_name = reader.fixedString(12, 128);
        for (unsigned i = 0; i < 16; ++i)
            require(reader.f32(140 + i * 4) == (i % 5 == 0 ? 1.0 : 0.0), DecodeKind48C::unsupported,
                "psg.hierarchy_transform", 140 + i * 4, "Non-identity hierarchy transform requires separate recovery");
        std::array<double, 7> bounds{};
        for (unsigned i = 0; i < bounds.size(); ++i) {
            bounds[i] = reader.f32(204 + i * 4);
            require(std::isfinite(bounds[i]), DecodeKind48C::malformed, "psg.bounds_nonfinite", 204 + i * 4, "Hierarchy bounds must be finite");
            if (i >= 3) require(bounds[i] >= 0, DecodeKind48C::malformed, "psg.bounds_negative", 204 + i * 4, "Negative bounds extent/radius");
        }
        require(reader.u32(232) == 0xffffffffu, DecodeKind48C::unsupported, "psg.parent", 232, "Non-root hierarchy parent unsupported");
        require(reader.u32(236) == 1, DecodeKind48C::unsupported, "psg.material_count", 236, "Only one material supported");
        parsed.material = reader.fixedString(240, 64);
        for (unsigned i = 0; i < parsed.quantization.size(); ++i) {
            parsed.quantization[i] = reader.f32(304 + i * 4);
            require(std::isfinite(parsed.quantization[i]), DecodeKind48C::malformed, "psg.quant_nonfinite", 304 + i * 4, "Quantization must be finite");
            if (i < 4) require(parsed.quantization[i] > 0, DecodeKind48C::malformed, "psg.quant_scale", 304 + i * 4, "Quantization scale must be positive");
        }
        const auto& quant = parsed.quantization;
        const std::uint32_t world = reader.u32(332);
        require(world <= 1, DecodeKind48C::unsupported, "psg.world_flag", 332, "Unknown world-space flag");
        parsed.world_space = world != 0;
        require(parsed.world_space || (quant[4] == 0 && quant[5] == 0 && quant[6] == 0), DecodeKind48C::unsupported,
            "psg.local_origin", 320, "Local mesh with nonzero quantization origin needs recovery");
        require(reader.u32(336) == 1 && reader.u32(340) == 0x7f7fffff && reader.u32(344) == 1,
            DecodeKind48C::unsupported, "psg.lod_layout", 336, "Only observed single LOD layout supported");
        require(reader.u32(348) == 0 && reader.u32(352) == 0x00ff010f, DecodeKind48C::unsupported,
            "psg.surface_flags", 348, "Only observed static surface flags supported");
        parsed.uv_size_uninterpreted = reader.f32(356);
        require(std::isfinite(parsed.uv_size_uninterpreted) && parsed.uv_size_uninterpreted > 0,
            DecodeKind48C::malformed, "psg.uv_size", 356, "UV-size metadata must be positive and finite");
        const std::uint32_t declaredVertices = reader.u16(360), declaredStrips = reader.u16(362);
        require(declaredVertices > 0 && declaredStrips > 0 && declaredStrips <= declaredVertices / 3,
            DecodeKind48C::malformed, "psg.debug_counts", 360, "Invalid declared vertex/strip counts");
        const std::uint32_t payloadBytes = reader.u32(364);
        require(payloadBytes == input.size() - 368 && payloadBytes % 16 == 0, DecodeKind48C::malformed,
            "psg.payload_extent", 364, "Payload length/alignment mismatch");
        std::vector<DecodedVertex> vertices;
        std::vector<std::uint32_t> indices;
        vertices.reserve(declaredVertices);
        indices.reserve(static_cast<std::size_t>(declaredVertices - 2 * declaredStrips) * 3);
        std::size_t offset = 368;
        auto unpack = [&](std::uint32_t command, std::uint32_t count, std::uint32_t address, std::size_t elementBytes) {
            const std::size_t at = offset;
            const std::uint32_t expected = (command << 24) | (count << 16) | address;
            require(reader.u32(offset) == expected, DecodeKind48C::unsupported,
                "psg.unpack_command", offset, "Packed upload command differs from supported static layout");
            offset += 4;
            const std::size_t bytes = elementBytes * count, padded = (bytes + 3) & ~std::size_t(3);
            reader.range(offset, padded);
            require(reader.zero(offset + bytes, padded - bytes), DecodeKind48C::unsupported,
                "psg.unpack_padding", offset + bytes, "Nonzero packed upload alignment padding");
            offset += padded;
            return at + 4;
        };
        progress(hooks, "psg_vertices", 0, declaredVertices);
        while (offset < input.size() && reader.u32(offset) != 0) {
            checkCancel(hooks, offset);
            const std::size_t tag = unpack(0x6c, 1, 0x8000, 16);
            const std::uint32_t count = reader.u32(tag) & 0x7fff;
            require(count >= 3 && count <= 128, DecodeKind48C::malformed, "psg.group_count", tag, "Packed group vertex count outside 3..128");
            require(reader.u32(tag) == (0x8000 | count) && reader.u32(tag + 4) == 0x30024000 &&
                reader.u32(tag + 8) == 0x412 && reader.u32(tag + 12) == 0, DecodeKind48C::unsupported,
                "psg.gif_template", tag, "Unknown static packed group template");
            const std::size_t xyz = unpack(0x6d, count, 0x8001, 8);
            const std::size_t normal = unpack(0x6a, count, 0x8001 + count, 3);
            const std::size_t uv = unpack(0x65, count, 0x8001 + 2 * count, 4);
            const std::size_t color = unpack(0x6e, count, 0xc001 + 3 * count, 4);
            require(reader.u32(offset) == 0x17000000, DecodeKind48C::unsupported, "psg.group_end", offset, "Missing supported group-end marker");
            offset += 4;
            const std::uint32_t base = static_cast<std::uint32_t>(vertices.size());
            require(base + count <= declaredVertices, DecodeKind48C::malformed,
                "psg.vertex_limit", xyz, "Groups exceed declared vertex count");
            std::uint32_t start = 0;
            while (start < count) {
                require(start + 2 < count, DecodeKind48C::unsupported, "psg.short_strip", xyz + start * 8, "Strip has fewer than three vertices");
                require(reader.i16(xyz + start * 8 + 6) == -16 && reader.i16(xyz + (start + 1) * 8 + 6) == -16,
                    DecodeKind48C::unsupported, "psg.strip_start", xyz + start * 8 + 6, "Strip must begin with two suppressed vertices");
                std::uint32_t end = start + 2;
                while (end < count && reader.i16(xyz + end * 8 + 6) == 16) {
                    std::uint32_t a = base + end - 2, b = base + end - 1;
                    if ((end - start) & 1) std::swap(a, b);
                    indices.push_back(a); indices.push_back(b); indices.push_back(base + end);
                    ++end;
                }
                require(end > start + 2, DecodeKind48C::unsupported, "psg.empty_strip", xyz + start * 8, "Strip emits no triangle");
                ++parsed.strips;
                start = end;
            }
            for (std::uint32_t i = 0; i < count; ++i) {
                const int adc = reader.i16(xyz + i * 8 + 6);
                require(adc == -16 || adc == 16, DecodeKind48C::unsupported, "psg.adc_encoding", xyz + i * 8 + 6, "Unknown bone/ADC encoding");
                DecodedVertex vertex;
                for (unsigned axis = 0; axis < 3; ++axis) {
                    // Match the frozen Python path's binary64 multiply then add.
                    const double scaled = reader.i16(xyz + i * 8 + axis * 2) * quant[0];
                    vertex.position[axis] = scaled + quant[4 + axis];
                    vertex.normal[axis] = reader.i8(normal + i * 3 + axis) * quant[1];
                }
                for (unsigned axis = 0; axis < 2; ++axis)
                    vertex.uv[axis] = reader.i16(uv + i * 4 + axis * 2) * quant[2 + axis];
                for (unsigned channel = 0; channel < 4; ++channel) vertex.color[channel] = input[color + i * 4 + channel];
                vertices.push_back(vertex);
            }
            ++parsed.groups;
            progress(hooks, "psg_vertices", static_cast<std::uint32_t>(vertices.size()), declaredVertices);
        }
        require(input.size() - offset < 16 && reader.zero(offset, input.size() - offset), DecodeKind48C::unsupported,
            "psg.trailing_data", offset, "Unexpected trailing packed data");
        require(vertices.size() == declaredVertices, DecodeKind48C::malformed,
            "psg.header_vertex_mismatch", 360, "Decoded vertex count differs from header");
        require(parsed.strips == declaredStrips, DecodeKind48C::unsupported,
            "psg.strip_count_variant", 362, "Source-strip and packed-group strip accounting differ; outside frozen 48B subset");
        require(indices.size() == 3ull * (declaredVertices - 2 * declaredStrips), DecodeKind48C::malformed,
            "psg.topology_count", 360, "Triangle-list count differs from strip accounting");
        for (unsigned axis = 0; axis < 3; ++axis) {
            parsed.bounds_min[axis] = parsed.bounds_max[axis] = vertices[0].position[axis];
            for (const auto& vertex : vertices) {
                parsed.bounds_min[axis] = std::min(parsed.bounds_min[axis], vertex.position[axis]);
                parsed.bounds_max[axis] = std::max(parsed.bounds_max[axis], vertex.position[axis]);
            }
            parsed.max_bounds_error = std::max(parsed.max_bounds_error,
                std::max(std::abs(parsed.bounds_min[axis] - (bounds[axis] - bounds[axis + 3])),
                         std::abs(parsed.bounds_max[axis] - (bounds[axis] + bounds[axis + 3]))));
        }
        require(parsed.max_bounds_error <= quant[0] * 1.01 + 0.002, DecodeKind48C::unsupported,
            "psg.bounds_disagree", 204, "Decoded positions disagree with stored hierarchy bounds");
        for (std::size_t i = 0; i < indices.size(); i += 3) {
            const auto& a = vertices[indices[i]];
            const auto& b = vertices[indices[i + 1]];
            const auto& c = vertices[indices[i + 2]];
            std::array<double, 3> u{}, v{}, n{};
            for (unsigned axis = 0; axis < 3; ++axis) {
                u[axis] = b.position[axis] - a.position[axis]; v[axis] = c.position[axis] - a.position[axis];
                n[axis] = (a.normal[axis] + b.normal[axis]) + c.normal[axis];
            }
            const std::array<double, 3> cross{{u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0]}};
            const double dot = (cross[0] * n[0] + cross[1] * n[1]) + cross[2] * n[2];
            require(std::isfinite(dot) && dot >= 0, DecodeKind48C::unsupported,
                "psg.normal_winding", 0, "Recovered winding disagrees with stored normals");
            if (dot > 0) ++parsed.positive_normal_alignment; else ++parsed.degenerate_normal_alignment;
        }
        parsed.vertices = declaredVertices;
        parsed.triangles = static_cast<std::uint32_t>(indices.size() / 3);
        parsed.rrmesh.reserve(16 + vertices.size() * 24 + indices.size() * 4);
        const char magic[] = "RRMESH01";
        parsed.rrmesh.insert(parsed.rrmesh.end(), magic, magic + 8);
        appendU32(parsed.rrmesh, declaredVertices);
        appendU32(parsed.rrmesh, static_cast<std::uint32_t>(indices.size()));
        for (const auto& vertex : vertices) {
            for (double value : vertex.position) appendF32(parsed.rrmesh, value);
            for (double value : vertex.uv) appendF32(parsed.rrmesh, value);
            parsed.rrmesh.insert(parsed.rrmesh.end(), vertex.color.begin(), vertex.color.end());
        }
        for (std::uint32_t index : indices) appendU32(parsed.rrmesh, index);
        checkCancel(hooks, offset);
        output = std::move(parsed);
        return true;
    } catch (const Failure& failure) { error = failure.error; }
    catch (const std::bad_alloc&) { allocationError(error); }
    return false;
}
} }
