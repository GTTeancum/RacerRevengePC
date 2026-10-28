#pragma once
// Concrete270900 pre-static ownership. No Page, draw, Scene completion or
// renderer capability is granted by these descriptors or their allocations.
#include "original_ui_garage_visibility50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
namespace ra_pod_scene50LA {class Owner;}
namespace rr::platform {
class OriginalUiGaragePodScene50KG;
struct OriginalUiMainApplication50KG;
class OriginalUiGarageInstanceMaster50KG final {
    friend class OriginalUiGaragePodScene50KG;
    friend class ::ra_pod_scene50LA::Owner; //Same actual444078 domain; private RA load/retirement only.
    OriginalUiGarageInstanceMaster50KG();
    //2BFEA0 allocates RAW capacity, not ten constructed Instance records.
    std::unique_ptr<std::uint8_t[]> records00_;
    std::int32_t capacity04_=10,growth08_=100000,used0C_=0,count10_=0;
    void close2BFD40()noexcept;
public:
    ~OriginalUiGarageInstanceMaster50KG()noexcept=default;
    OriginalUiGarageInstanceMaster50KG(const OriginalUiGarageInstanceMaster50KG&)=delete;
    OriginalUiGarageInstanceMaster50KG& operator=(const OriginalUiGarageInstanceMaster50KG&)=delete;
};
class OriginalUiGarageSceneSetup50KG final {
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGarageSceneLighting50KG;
    using C=game::OriginalWreckComponent50LA;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using Dat=assets::OriginalUiDat50KG;
    // Unwritten bytes are never read. Unlike value-initializing a char buffer,
    // this does not invent source initialization of260 bytes for a short name.
    struct Text260 final {
        std::array<char,260> bytes;
        bool defined=false;
        void set(const std::string&);
        const char* get()const noexcept{return defined?bytes.data():nullptr;}
    };
    struct StartData final {
        // Actual275B80 copy of the single Application-retained shared vectors.
        //275BC0/158C00 replace XYZ only; the constructor W lanes survive.
        std::array<float,4> position00,direction10;
        StartData(const std::array<float,4>& origin,const std::array<float,4>& forward)
            :position00(origin),direction10(forward){}
    };
    struct Environment final {
        std::int32_t cubeCount00=0;
        std::size_t allocatedCubeRows=0;
        std::unique_ptr<std::unique_ptr<char[]>[]> cubeFiles04;
        std::unique_ptr<std::int32_t[]> zoneCube08;
        Text260 terrain0C,water110,unused214,ecosystem318,particle41C;
        float width520=0,breadth524=0,widthScale528=0,breadthScale52C=0,border530=0;
        float xmin534=0,xmax538=0,zmin53C=0,zmax540=0;
        std::optional<float> ecoDistance2D544,ecoDistance3D548;
        Environment();
    };
    struct Fog final {
        std::array<float,4> color00{};
        float visibility10=1,haziness14=.5f;
    };
    struct LightTemp final {
        std::int32_t type00=4;
        std::array<float,4> color04{};
        std::unique_ptr<std::array<float,4>> position14,look18,attenuation1C;
        std::int32_t zone20=0;
        float range24=0;
        std::uint8_t flags28=4; //Only low7 bits are defined by ctor/read.
        std::optional<float> innerCone2C,outerCone30;
        std::array<std::uint8_t,5> lensLayers34{};
        std::array<float,4> lensColor3C{};
        float lensBrightness4C=1;
        std::string lensTexture50; //Only first NUL initialized by ctor.
        std::array<float,4> lightFlareColor90{};
        float lightFlareSizeA0=1;
        std::optional<std::string> lightFlareTextureA4;
        std::int32_t lightFlareTurningE4=1;
        std::int32_t flashPatternE8=-1;
        float flashPeriodEC=1,flashFilterF0=0;
    };
    struct Light final {
        C* liveLight00=nullptr; //NOT created by270900.
        std::unique_ptr<LightTemp> temp04;
    };
    struct DirectionalColor final {
        //278290 writes ONLY x=-256 when absent. Optional marks defined lanes,
        // not a synthetic zero-filled renderer color or separate value copy.
        std::array<std::optional<float>,4> lanes;
    };
    struct Lights final {
        C entity;
        bool entityLive=false;
        bool pageRetiring=false,pageRetired=false; //Native normal callback custody, not source fields.
        std::int32_t count30=0;
        std::optional<std::int32_t> zoneCount34;
        std::unique_ptr<Light[]> lights38;
        std::unique_ptr<DirectionalColor[]> zoneColors3C;
    };
    struct Zone final {
        std::int32_t index00=0;
        Text260 name04;
        std::int32_t staticCount108=0;
        std::optional<std::int32_t> collisionCount10C;
        //dynamic visibility, dynamic collision, static visibility, static collision
        std::array<std::int32_t,4> search110{{2,2,2,2}};
        std::unique_ptr<std::int32_t[]> exclusions120;
        std::int32_t exclusionCount124=0,reverb128=0;
        Fog fog12C;
        std::uint8_t flags148=7; //Only low3 source bits defined.
        std::array<std::optional<float>,4> bounds14C;
        Zone();
    };
    struct Lists final {
        std::int32_t count00=0;
        std::unique_ptr<std::int32_t[]> capacities04,used08;
        std::unique_ptr<std::unique_ptr<C*[]>[]> rows0C;
    };
    std::unique_ptr<StartData> startData348_;
    std::unique_ptr<Environment> environment3A4_;
    std::unique_ptr<Lights> lights3A8_;
    std::unique_ptr<Fog> fog3AC_;
    std::unique_ptr<Zone[]> zones350_;
    std::size_t zoneCount_=0;
    std::unique_ptr<Lists> lists3D4_;
    std::shared_ptr<OriginalUiGarageInstanceMaster50KG> instanceMaster_;
    std::int32_t instanceCount3E0_=0;
    //Absent for authored count0. Nonzero264-byte descriptor read is unsupported.
    std::unique_ptr<std::uint8_t[]> instanceDescriptors3DC_;
    bool perZoneFog4F0_=false,complete_=false,closed_=false;
    OriginalUiGarageSceneSetup50KG()noexcept=default;
    bool readStartData275BC0(const Dat&,D&);
    bool readEnvironment276C00(const Dat&,std::int32_t,D&);
    bool readLights278290(const Dat&,D&);
    bool readFog278570(const Dat&,D&);
    bool readZones275E30(const Dat&,D&);
    void initLists27C110(std::int32_t);
public:
    ~OriginalUiGarageSceneSetup50KG()noexcept;
    OriginalUiGarageSceneSetup50KG(const OriginalUiGarageSceneSetup50KG&)=delete;
    OriginalUiGarageSceneSetup50KG& operator=(const OriginalUiGarageSceneSetup50KG&)=delete;
};
}
