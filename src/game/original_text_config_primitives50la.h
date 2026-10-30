#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <new>
#include <utility>

namespace rr::assets {class OriginalParticleSourceConfig50LA;}
namespace rr::platform {class OriginalWorldSourceConfig50LA;}
namespace rr::game {
class OriginalWreckSparksConfig50LA;
// Private, UNJOINED primitives. No FileManager handle, open/read completion,
// allocator receipt or Config parse success is manufactured by this class.
class OriginalTextConfigPrimitives50LA final {
    friend class rr::assets::OriginalParticleSourceConfig50LA;
    friend class OriginalWreckSparksConfig50LA;
    friend class rr::platform::OriginalWorldSourceConfig50LA;
    enum class Stop { complete, unknownByte, unknownCtype, nativeBounds, nativeStorage, needsAllocation,
                      nullTableDereference, duplicateSection };
    struct Result { Stop stop; std::uint32_t pc; std::size_t produced; };
    struct Bytes {
        std::array<std::uint8_t, 0x400> value; // Unwritten bytes stay unread.
        std::array<bool, 0x400> known{};      // Native provenance, not guest bytes.
        void put(std::size_t i, std::uint8_t v) noexcept { value[i]=v; known[i]=true; }
    };
    struct Row {
        std::uint64_t position00;
        std::uint32_t line08;
        Bytes name0C;
        bool positionKnown=false, lineKnown=false;
        // Native semantic row, NOT a guest-layout struct: the original stride
        // is 0x418. Original trailing 12 bytes are never read or initialized.
    };
    struct Table {
        std::uint32_t count14=0, capacity18=0;
        Row* rows=nullptr; // Native adapter storage, not a guest address.
        std::size_t rowExtent=0;
        bool allocationPending=false;
        std::uint32_t pendingCapacity=0;
        bool pendingReallocation=false;
        // Native custody is separate from source text1C (rows). Row metadata
        // changes host sizeof(Row); source logical stride remains exactly0x418.
        std::unique_ptr<Row[]> owned;
        std::unique_ptr<Row[]> orphan;
        void* sourceStorage=nullptr;
        void* orphanStorage=nullptr;
        bool allocationFault=false, sourceCleaned=false;
        bool rowNameCopied=false;std::size_t rowNameLength=0; //Native staged-call custody.
        Table()noexcept=default;
        Table(const Table&)=delete;
        Table& operator=(const Table&)=delete;
        // Native cancellation releases both allocations WITHOUT claiming the
        // source destructor ran or resetting source count/capacity fields.
        ~Table(){std::free(sourceStorage);std::free(orphanStorage);}
    };
    struct AllocationRequest {
        bool reallocate;
        std::uint32_t bytes;
        std::uint32_t callPc;
    };
    OriginalTextConfigPrimitives50LA()=delete;

    // 159468 strlen occurs BEFORE 159470 output[0]=0. Unknown input cannot
    // be treated as a NUL, whitespace or EOF. Distinct source stack buffers.
    static Result split159440(const Bytes& input, Bytes& output) noexcept {
        if(&input==&output)return {Stop::nativeBounds,0x159440u,0};
        std::size_t length=0;
        for(;length<input.value.size();++length){
            if(!input.known[length])return {Stop::unknownByte,0x159468u,0};
            if(input.value[length]==0)break;
        }
        if(length==input.value.size())return {Stop::nativeBounds,0x159468u,0};
        output.put(0,0);
        std::uint32_t quote=0,depth=0;
        std::size_t written=0;
        for(std::size_t i=0;i<length;++i){
            const auto ch=input.value[i];
            bool keep=true;
            if(ch==']')--depth; // ADDIU wrap, including unmatched closing bracket.
            else if(ch=='"')quote=1u-quote;
            else if(ch=='[')++depth;
            else if(ch=='\r'||ch=='\n'||ch=='\t'||ch==' ')
                keep=(depth!=0&&(depth&0x80000000u)==0)||quote!=0;
            if(keep)output.put(written++,ch);
        }
        output.put(written,0);
        // produced is NATIVE metadata, not an invented original v0 return.
        return {Stop::complete,0x159554u,written};
    }

    // Pure suffix AFTER the real 130560 read. 'requested' is the original
    // min(remaining, limit-2), NOT read return length. Caller must supply the
    // exact known-byte mask of the destination after the real read. No caller
    // is integrated yet. 'nonzeroReadReturn' is not itself a read receipt.
    static Result lineSuffix12F1B4(Bytes& buffer, std::uint32_t requested,
                                  bool nonzeroReadReturn, std::uint32_t& cursor) noexcept {
        if(requested>=buffer.value.size())return {Stop::nativeBounds,0x12f1b4u,0};
        if(!nonzeroReadReturn){
            buffer.put(0,0); // 12F1BC: zero return, cursor remains unchanged.
            return {Stop::complete,0x12f1c0u,0};
        }
        buffer.put(requested,0); // 12F1D0 does NOT use the actual read result.
        std::size_t i=0;
        for(;;++i){
            if(!buffer.known[i])return {Stop::unknownByte,0x12f1e0u,i};
            if(buffer.value[i]==0||buffer.value[i]=='\n')break;
        }
        std::uint32_t consumed=requested;
        if(buffer.value[i]=='\n'){
            consumed=static_cast<std::uint32_t>(i)+1u;
            if(i>0){
                // This preceding byte was already read by the scan.
                if(buffer.value[i-1]=='\r'){
                    buffer.put(i-1,'\n');buffer.put(i,0); // CRLF -> LF NUL.
                }else{
                    // 12F22C BNEL targets 12F244; its delay advances a0 once.
                    buffer.put(i+1,0);
                }
            }else buffer.put(i+1,0);
        }
        cursor+=consumed; // Original SW low32; no-LF embedded NUL consumes request.
        return {Stop::complete,0x12f254u,consumed};
    }

    // 309348 uses exact literal 3A6220: "]\n\r"; no required closing ']'.
    static Result sectionName157548(const Bytes& split, Bytes& name) noexcept {
        if(&split==&name)return {Stop::nativeBounds,0x157548u,0};
        if(!split.known[0])return {Stop::unknownByte,0x157538u,0};
        if(split.value[0]!='[')return {Stop::nativeBounds,0x157540u,0};
        std::size_t n=0;
        for(std::size_t i=1;i<split.value.size();++i){
            if(!split.known[i])return {Stop::unknownByte,0x157550u,n};
            const auto ch=split.value[i];
            if(ch==0||ch==']'||ch=='\n'||ch=='\r'){
                // 309980 copies only the span; 157574 appends NUL.
                for(std::size_t j=0;j<n;++j)name.put(j,split.value[j+1]);
                name.put(n,0);return {Stop::complete,0x157574u,n};
            }
            ++n;
        }
        return {Stop::nativeBounds,0x157550u,n};
    }

    struct CompareResult { Stop stop; std::uint32_t pc; std::int32_t difference; };
    struct LookupResult { Stop stop; std::uint32_t pc; std::int32_t index; };
    // Original3C6391 unsigned-index table has bit1 ONLY for ASCII A..Z.
    // This is the exact observed classification, not host locale case folding.
    static std::int32_t foldUnsigned308F7C(std::uint8_t byte)noexcept {
        return byte>='A'&&byte<='Z'?std::int32_t(byte)+0x20:std::int32_t(byte);
    }
    static CompareResult compareKnown308F00(const Bytes& query,const Bytes& row)noexcept {
        for(std::size_t i=0;i<query.value.size();++i){
            //308F08/60 LB query, then branch before row LB.
            if(!query.known[i])return {Stop::unknownByte,i?0x308f60u:0x308f08u,0};
            const auto left=query.value[i];
            if(left==0){
                //The final return path uses UNSIGNED indices, unlike loop.
                if(!row.known[i])return {Stop::unknownByte,0x308f80u,0};
                return {Stop::complete,0x308fb0u,-foldUnsigned308F7C(row.value[i])};
            }
            if(!row.known[i])return {Stop::unknownByte,0x308f18u,0};
            const auto right=row.value[i];
            //The loop sign-extends BOTH bytes before indexing3C6391. Negative
            //indices read preceding data, outside the classified unsigned table.
            //Do not substitute host locale or guess that preceding data's live
            //provenance. ASCII paths are complete; these paths remain blocked.
            if(left&0x80u)return {Stop::unknownCtype,0x308f34u,0};
            if(right&0x80u)return {Stop::unknownCtype,0x308f3cu,0};
            const auto a=foldUnsigned308F7C(left),b=foldUnsigned308F7C(right);
            if(a!=b)return {Stop::complete,0x308fb0u,a-b};
            //Equal, non-NUL query advances both pointers; even an unknown next
            //byte stays unknown. No whole-buffer strlen or zero-fill admission.
        }
        return {Stop::nativeBounds,0x308f60u,0};
    }
    // ONLY the text branch157C44 of157AF0, not its binary-name binary search.
    // Genuine Table storage+knownness produced by the retained allocator/row
    // primitives is required for every row read. No file invocation is implied.
    static LookupResult lookupText157C44(const Table& table,const Bytes& name)noexcept {
        if(table.sourceCleaned||table.allocationPending||table.allocationFault)
            return {Stop::nativeBounds,0x157c44u,-1};
        //157C84 signed SLT against0: zero/high-bit count returns -1 unread.
        if(!table.count14||(table.count14&0x80000000u))
            return {Stop::complete,0x157c90u,-1};
        for(std::uint32_t index=0;index<table.count14;++index){
            if(!table.rows||!table.sourceStorage||table.rows!=table.owned.get()||
               table.orphan||table.orphanStorage||index>=table.rowExtent)
                return {Stop::nativeBounds,0x157c50u,-1};
            const auto compared=compareKnown308F00(name,table.rows[index].name0C);
            if(compared.stop!=Stop::complete)
                return {compared.stop,compared.pc,-1};
            if(compared.difference==0)
                return {Stop::complete,0x157c70u,static_cast<std::int32_t>(index)};
            // Original increments byte offset by0x418 and index by1; Row[]
            // is the separate semantic sidecar for that same raw row stride.
        }
        return {Stop::complete,0x157c90u,-1};
    }

    // Called only AFTER authentic 157AF0 returns -1. Existing-name path is
    // a different log/assertion route and is deliberately not implemented here.
    // Stages the exact request; continueAllocation makes a real native nullable
    // allocation analogue. Neither step proves a FileManager/parser invocation.
    static Result prepareNewRow15758C(Table& table, AllocationRequest& request) noexcept {
        if(table.allocationPending||table.allocationFault||table.sourceCleaned||table.rowNameCopied||
           table.orphan||table.orphanStorage||table.rows!=table.owned.get()||
           (bool(table.rows)!=bool(table.sourceStorage)))
            return {Stop::nativeBounds,0x15758cu,0};
        if(!table.rows){
            table.allocationPending=true;table.pendingCapacity=10;
            table.pendingReallocation=false;
            request={false,0x28f0u,0x1575e0u};
            return {Stop::needsAllocation,0x1575e0u,0};
        }
        // Source signed SLT. Native bounds refusal does not impersonate guest failure.
        if((table.count14&0x80000000u)||(table.capacity18&0x80000000u))
            return {Stop::nativeBounds,0x1575a0u,0};
        if(table.count14<table.capacity18)return {Stop::complete,0x1575f8u,0};
        if(table.capacity18>0xffffffffu/0x418u-10u)
            return {Stop::nativeBounds,0x1575acu,0};
        table.capacity18+=10u; // Published BEFORE realloc, never rolled back here.
        table.allocationPending=true;table.pendingCapacity=table.capacity18;
        table.pendingReallocation=true;
        request={true,table.capacity18*0x418u,0x1575ccu};
        return {Stop::needsAllocation,0x1575ccu,0};
    }

    // Native realloc adaptation: semantic rows carry byte-knownness metadata.
    // Copy ONLY known values, never C++ copy an indeterminate source row.
    static void copyKnownRow(const Row& source,Row& target)noexcept {
        if(source.positionKnown){target.position00=source.position00;target.positionKnown=true;}
        if(source.lineKnown){target.line08=source.line08;target.lineKnown=true;}
        for(std::size_t i=0;i<source.name0C.value.size();++i)
            if(source.name0C.known[i])target.name0C.put(i,source.name0C.value[i]);
    }
    static void syncKnownRow(Table& table,std::size_t index)noexcept {
        auto* bytes=static_cast<std::uint8_t*>(table.sourceStorage)+index*0x418u;
        const auto& row=table.rows[index];
        if(row.positionKnown)for(unsigned i=0;i<8;++i)
            bytes[i]=static_cast<std::uint8_t>(row.position00>>(i*8));
        if(row.lineKnown)for(unsigned i=0;i<4;++i)
            bytes[8+i]=static_cast<std::uint8_t>(row.line08>>(i*8));
        for(std::size_t i=0;i<row.name0C.value.size();++i)
            if(row.name0C.known[i])bytes[0xcu+i]=row.name0C.value[i];
        // Never touch original trailing12 bytes or unknown name tail.
    }
    static Result continueAllocation(Table& table)noexcept {
        const auto callPc=table.pendingReallocation?0x1575ccu:0x1575e0u;
        if(!table.allocationPending||table.allocationFault||table.sourceCleaned||
           table.orphan||table.orphanStorage||table.rows!=table.owned.get()||
           (bool(table.rows)!=bool(table.sourceStorage))||!table.pendingCapacity||
           table.pendingCapacity>0xffffffffu/0x418u||
           table.pendingCapacity>std::numeric_limits<std::size_t>::max()/sizeof(Row))
            return {Stop::nativeBounds,callPc,0};
        if(table.pendingReallocation){
            if(!table.rows||table.rowExtent>table.pendingCapacity||
               table.capacity18!=table.pendingCapacity)
                return {Stop::nativeBounds,callPc,0};
        }else if(table.rows||table.rowExtent||table.pendingCapacity!=10u)
            return {Stop::nativeBounds,callPc,0};
        std::unique_ptr<Row[]> replacement;
        // Sidecar allocation precedes the real source-size allocation analogue.
        // Its failure is ONLY native refusal: pending state, already-published
        // capacity and existing source pointer/custody remain intact.
        try { replacement.reset(new Row[table.pendingCapacity]); }
        catch(const std::bad_alloc&){return {Stop::nativeStorage,callPc,0};}
        // Clone native knownness BEFORE realloc; no throwing work follows it.
        // Includes unused/partial rows, not merely count14 committed rows.
        for(std::size_t i=0;i<table.rowExtent;++i)
            copyKnownRow(table.rows[i],replacement[i]);
        const auto bytes=std::size_t(table.pendingCapacity)*0x418u;
        void* const allocated=table.pendingReallocation?
            std::realloc(table.sourceStorage,bytes):std::malloc(bytes);
        // Actual nullable host malloc/realloc result, not a supplied success bit.
        // No claim about PS2 heap addresses, locks or fragmentation is made.
        if(allocated){
            table.sourceStorage=allocated; //Original-size backing, copied by realloc.
            table.owned=std::move(replacement); //Old sidecar released on success.
            table.rows=table.owned.get(); //1575D8 or1575E8 pointer publication.
            table.rowExtent=table.pendingCapacity;
        }else{
            // Original3084BC..4C8 returns NULL without freeing old storage.
            // Keep that orphan under native custody BEFORE overwriting source1C.
            table.orphan=std::move(table.owned);
            table.orphanStorage=table.sourceStorage;
            table.sourceStorage=nullptr;
            table.rows=nullptr; //Unconditional1575D8/1575E8, NOT rollback.
            table.rowExtent=0;
            table.allocationFault=true; //Native stop before invalid strcpy15761C.
        }
        if(!table.pendingReallocation)table.capacity18=10u; //1575F0 AFTER pointer.
        // On failed growth count/capacity survive; failure is terminal for this
        // parser occurrence, so no second realloc can overwrite orphan custody.
        table.allocationPending=false;
        return table.allocationFault?Result{Stop::nullTableDereference,0x15761cu,0}:
                                     Result{Stop::complete,0x1575f8u,0};
    }
    // Table-only slice of1573B0. Other Config resources are NOT retired here.
    // Native orphan disposal is deliberately not represented as305728(source1C).
    static Result cleanupText157414(Table& table)noexcept {
        if(table.sourceCleaned)return {Stop::complete,0x157438u,0};
        if(table.rows!=table.owned.get()||bool(table.rows)!=bool(table.sourceStorage))
            return {Stop::nativeBounds,0x157414u,0};
        if(table.rows){
            std::free(table.sourceStorage);table.sourceStorage=nullptr; //305728.
            table.owned.reset(); //Native knownness sidecar, not a second source free.
            table.rows=nullptr; //157428.
            table.count14=0; //15742C, only in NONNULL branch.
            table.rowExtent=0;
        }
        // NULL path preserves count; both paths preserve source capacity18.
        // Pending pre-call growth already published capacity: do not undo it.
        table.allocationPending=false;table.rowNameCopied=false;table.sourceCleaned=true;
        return {Stop::complete,0x157438u,0};
    }

    //15761C strcpy is BEFORE the actual15762C FileManager tell call. Keep
    //that committed name even when the genuine tell operation later bounds.
    static Result copyRowName15761C(Table& table,const Bytes& name)noexcept {
        if(table.sourceCleaned||table.rowNameCopied)return {Stop::nativeBounds,0x15761cu,0};
        if(table.allocationPending)return {Stop::needsAllocation,0x1575f8u,0};
        if(!table.rows)return {Stop::nullTableDereference,0x15761cu,0};
        if(!table.sourceStorage||table.rows!=table.owned.get()||
           table.count14>=table.capacity18||table.count14>=table.rowExtent)
            return {Stop::nativeBounds,0x15761cu,0};
        std::size_t n=0;
        for(;n<name.value.size();++n){
            if(!name.known[n])return {Stop::unknownByte,0x15761cu,n};
            if(name.value[n]==0)break;
        }
        if(n==name.value.size())return {Stop::nativeBounds,0x15761cu,n};
        auto& row=table.rows[table.count14];
        for(std::size_t i=0;i<=n;++i)row.name0C.put(i,name.value[i]);
        syncKnownRow(table,table.count14);
        table.rowNameCopied=true;table.rowNameLength=n;
        return {Stop::complete,0x157624u,n};
    }
    //Only follows copied name and the real caller's12F520 return. This helper
    //cannot itself mint a tell receipt; World orchestration supplies actual v0.
    static Result finishRow157654(Table& table,std::uint64_t actualTell,std::uint32_t line)noexcept {
        if(table.sourceCleaned||table.allocationPending||table.allocationFault||
           !table.rowNameCopied||!table.rows||!table.sourceStorage||
           table.rows!=table.owned.get()||table.count14>=table.rowExtent)
            return {Stop::nativeBounds,0x157654u,0};
        auto& row=table.rows[table.count14];
        row.position00=actualTell;row.positionKnown=true; //157654 SD.
        row.line08=line;row.lineKnown=true; //157678.
        syncKnownRow(table,table.count14);
        //Prior-row read is after all current-row writes, even when unknown.
        if(table.count14&&!table.rows[table.count14-1].lineKnown)
            return {Stop::unknownByte,0x1576a4u,0};
        if(table.count14){
            table.rows[table.count14-1].line08=line-table.rows[table.count14-1].line08-1u;
            syncKnownRow(table,table.count14-1);
        }
        ++table.count14;table.rowNameCopied=false;
        return {Stop::complete,0x1576c4u,table.rowNameLength};
    }
    //Retained pure convenience primitive; it is not an I/O operation.
    static Result appendRow15761C(Table& table,const Bytes& name,
                                 std::uint64_t actualTell,std::uint32_t line)noexcept {
        const auto copied=copyRowName15761C(table,name);
        return copied.stop==Stop::complete?finishRow157654(table,actualTell,line):copied;
    }
    static Result finishLastRow157770(Table& table,std::uint32_t line) noexcept {
        if(table.sourceCleaned||table.allocationPending||table.allocationFault||table.rowNameCopied)
            return {Stop::nativeBounds,0x157770u,0};
        // 157774 BLEZ compares signed32 count; high-bit values also skip.
        if(!table.count14||(table.count14&0x80000000u))
            return {Stop::complete,0x157774u,0};
        if(!table.rows||!table.sourceStorage||table.rows!=table.owned.get()||
           table.count14>table.rowExtent)
            return {Stop::nativeBounds,0x157798u,0};
        auto& row=table.rows[table.count14-1];
        if(!row.lineKnown)return {Stop::unknownByte,0x157798u,0};
        row.line08=line-row.line08-1u;
        syncKnownRow(table,table.count14-1);
        return {Stop::complete,0x1577a4u,0};
    }
};
} // namespace rr::game
