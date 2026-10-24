#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rr::platform {
// Linux read-only transport, NOT a PS2 memory-card implementation. Nothing
// constructs this provider in the application. Opening requires explicit opt-in.
class OriginalSaveNativeReader50KG final:
    public std::enable_shared_from_this<OriginalSaveNativeReader50KG> {
public:
    enum class Status:std::uint8_t {
        ok, invalidRoot, rootUnavailable, directoryUnavailable, fileUnavailable,
        wrongKind, wrongSize, readFailure, changedDuringRead, allocationFailure,
        notConfigured
    };
    enum class Kind:std::uint8_t {header186E50,payload186C70};
    struct Diagnostic {Status status=Status::ok;int hostErrno=0;};
    struct Identity {
        std::uint64_t device,inode;
        bool operator==(const Identity& other)const noexcept {
            return device==other.device&&inode==other.inode;
        }
    };
    class Receipt final {
    public:
        Kind kind()const noexcept{return kind_;}
        const std::vector<std::uint8_t>& bytes()const noexcept{return bytes_;}
        Identity rootIdentity()const noexcept;
        Identity directoryIdentity()const noexcept{return directory_;}
        Identity fileIdentity()const noexcept{return file_;}
        const OriginalSaveNativeReader50KG& provider()const noexcept{return *owner_;}
        // Host receipt only: not semantic validity, card presence, source RPC
        // completion, current-path freshness, or permission to commit a load.
    private:
        friend class OriginalSaveNativeReader50KG;
        Receipt(std::shared_ptr<const OriginalSaveNativeReader50KG>,Kind,
            Identity,Identity,std::vector<std::uint8_t>&&);
        std::shared_ptr<const OriginalSaveNativeReader50KG> owner_;
        Kind kind_;
        Identity directory_,file_;
        std::vector<std::uint8_t> bytes_;
    };
    // Exact absolute directory, no '.', '..', empty components or symlinks.
    // No default HOME/XDG/asset-root mapping, mkdir, write, format or delete.
    // Output is unchanged on failure. This function performs IO if called.
    static Status openExplicitRoot(const std::string& absoluteRoot,
        std::shared_ptr<OriginalSaveNativeReader50KG>& out,Diagnostic&) noexcept;
    // Explicit optional RR_NATIVE_SAVE_ROOT only. Absent leaves out unchanged
    // and returns notConfigured; empty/invalid never falls back elsewhere.
    // Uncalled: reading environment/opening a target requires a real caller.
    static Status openConfiguredRoot(
        std::shared_ptr<OriginalSaveNativeReader50KG>& out,Diagnostic&) noexcept;
    ~OriginalSaveNativeReader50KG();
    OriginalSaveNativeReader50KG(const OriginalSaveNativeReader50KG&)=delete;
    OriginalSaveNativeReader50KG& operator=(const OriginalSaveNativeReader50KG&)=delete;
    Identity rootIdentity()const noexcept{return root_;}
    const std::string& requestedRoot()const noexcept{return rootPath_;}
    // Reads BASLUS-20268/BASLUS-20268 relative to the pinned root descriptor.
    // Header: exactly8 returned bytes from file>=8. Payload: file exactly0xF508.
    // A payload receipt contains its OWN header. Never authorize a load from a
    // preceding header receipt, even if device/inode/identity happen to match.
    Status read(Kind,std::unique_ptr<const Receipt>& out,Diagnostic&)const noexcept;
private:
    OriginalSaveNativeReader50KG(int,Identity,std::string);
    int rootFd_;
    Identity root_;
    std::string rootPath_;
};
}
