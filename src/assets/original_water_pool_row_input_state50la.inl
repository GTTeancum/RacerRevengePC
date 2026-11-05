    struct RowInputRead {
        std::vector<std::uint8_t> bytes;
        bool entered=false,prepared=false,beforeObserved=false,attempted=false;
        bool nativeReturned=false,afterObserved=false,resultAdmitted=false;
        std::size_t copied=0,clampedAmount=0;
        std::uint32_t pc=0;
        std::optional<std::int32_t> sourceItems;
        ServiceObservation before,after;
        D helperDiagnostic{},admissionDiagnostic{};
    };
    struct RowInputs {
        bool entered=false,prepared=false;
        const void* dataIdentity=nullptr;
        const void* objectIdentity=nullptr;
        const void* rowsIdentity=nullptr;
        const void* rowIdentity=nullptr;
        std::uint64_t dataEpoch=0;
        std::uint32_t count=0;
        RowInputRead vertices;
        std::vector<RowInputRead> normals;
    } rowInputs_;
    bool prepareRowInputsNative(const void*,const void*,const void*,const void*,std::uint64_t,std::uint32_t,D&)noexcept;
    bool readRowInput12FB30(bool,std::uint32_t,D&)noexcept;
    bool rowInputsHistoryNative()const noexcept;
    const RowInputRead* latestRowInputReadNative()const noexcept {
        const RowInputRead* latest=rowInputs_.vertices.attempted?&rowInputs_.vertices:nullptr;
        for(const auto& read:rowInputs_.normals)if(read.attempted)latest=&read;
        return latest;
    }
