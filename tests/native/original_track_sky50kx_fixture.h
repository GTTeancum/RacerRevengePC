#pragma once
// The ONE track sky owner (common rewrite R6, PLAN B-C14 / 2.2 row "sky";
// common_track50CR::Track::roles.sky, step 05.11 274AE0), every course.
//  276C00 EnvironmentInfo: NumberOfSkyCubes (default 1; 276C60 skips both
//         allocations and the zone reads for a count <= 0), CubeFile1 then the
//         unnumbered CubeFile (276CD8), a sole 'none' -> zero cubes (276E40),
//         CubeFileForZoneN (default 1, 276E24 subtract 1: authored 0 disables).
//  274AE0 one SkyCube per authored cube (skies\<CubeFile1>), loaded by
//         sky50gw_fixture::Owner from the course's own archive.
//  275854..275964 per camera zone: disable every cube, enable the selected one.
//  13E830 viewport clear = World3EC colour = the SCB [Fog] ColorRGB (278570).
// A track that authors zero cubes has no images and every zone selects none
// (the former SC zero-sky owner). No course is named; the stock zone rows,
// cube names and CUB/SCB digests are receipts (sky.zone_rows, sky.cub_bytes,
// sky.cubes, fog.color.*), never pins.
#include "sky50gw_fixture.h"
#include "sky_cube_sequences50sf.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/game/original_track_fog50cr.h"
#include <array>
#include <charconv>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace original_track_sky50KX {
class Owner final {
    std::vector<std::int32_t> zoneCube_; // one row per SCB zone, sized once
    std::uint32_t cubes_=0;              // effective cube count (after 'none')
    std::string cubeFile_;
    std::array<float,4> clear_{{0.f,0.f,0.f,1.f}};
    bool clearAuthored_=false;
    std::unique_ptr<sky50gw_fixture::Owner> images_;
    // 1299A0 cube sequences (overlay "flash" cubes) of the same CUB; null when
    // the sky authors none (no .acb) or their data was refused (base sky kept).
    std::unique_ptr<sky_cube_sequences50SF::Owner> sequences_;
    bool prepared_=false;
    static bool extractMember(const rr::assets::ResArchive48C& archive,const std::string& member,
        std::vector<std::uint8_t>& out,rr::assets::DecodeError48C& error){
        return rr::assets::extractResMember48C(archive,member,out,error);
    }
    static bool extractMember(const rr::assets::IndexedResArchive48C& archive,const std::string& member,
        std::vector<std::uint8_t>& out,rr::assets::DecodeError48C& error){
        return archive.extractMember(member,out,error,8u*1024u*1024u);
    }
    static bool field(const rr::assets::ParameterBlock48F& block,const std::string& key,
        const std::string*& value) noexcept {
        value=nullptr;
        for(const auto& f:block.fields)if(f.name==key){if(value)return false;value=&f.value;}
        return true;
    }
    static bool integer(const std::string* text,std::int32_t fallback,std::int32_t& value) noexcept {
        value=fallback;if(!text)return true;
        const auto r=std::from_chars(text->data(),text->data()+text->size(),value);
        return !text->empty()&&r.ec==std::errc{}&&r.ptr==text->data()+text->size();
    }
public:
    // PORT STAND-IN (flagged): allocation ceiling of the zone rows (= the
    // decodeWorld48F zone cap); stock maximum 30 (SC). Not a size or a gate.
    static constexpr std::size_t zoneLimit=256;
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // 276C00 selection rows and the 13E830 clear colour from the track's own
    // decoded SCB. Cold one-shot; transactional (nothing published on refusal).
    bool prepareFromCourseData(const rr::assets::World48F& world,std::string& error){
        if(prepared_){error="source sky selection is cold one-shot";return false;}
        if(world.zones.empty()||world.zones.size()>zoneLimit){
            error="source sky selection zone count exceeds its row ceiling";return false;
        }
        const rr::assets::ParameterBlock48F* environment=nullptr;
        for(const auto& b:world.parameters.blocks)if(b.name=="environment"){
            if(environment){error="duplicate source Environment";return false;}environment=&b;
        }
        std::vector<std::int32_t> indices(world.zones.size(),-1);
        std::int32_t count=1;std::string cubeFile;
        if(environment){
            const std::string* text=nullptr;
            if(!field(*environment,"numberofskycubes",text)||!integer(text,1,count)){
                error="source sky NumberOfSkyCubes unreadable";return false;}
            if(count<0){error="source sky NumberOfSkyCubes negative";return false;}
            // PORT STAND-IN (flagged): 274AE0 builds one SkyCube per authored cube;
            // the image loader builds one. More than one is refused, never truncated.
            if(count>1){error="source sky authors more than one cube: multi-cube sky not built";return false;}
            if(count==1){
                const std::string* cube=nullptr;
                if(!field(*environment,"cubefile1",cube)){error="duplicate source CubeFile1";return false;}
                if(!cube&&!field(*environment,"cubefile",cube)){error="duplicate source CubeFile";return false;}
                cubeFile=cube?*cube:std::string("none");
                for(std::uint32_t i=0;i<world.zones.size();++i){
                    std::int32_t authored=0;
                    if(!field(*environment,"cubefileforzone"+std::to_string(i+1),text)||!integer(text,1,authored)||
                       authored<0||authored>count){error="source sky zone cube index rejected";return false;}
                    indices[i]=authored-1;
                }
                //276E40..276E88 runs AFTER the zone array: a sole 'none' is no cube.
                if(cubeFile=="none"){count=0;cubeFile.clear();indices.assign(world.zones.size(),-1);}
            }
        }else{
            // PORT STAND-IN (flagged): without [Environment] the World has no
            // EnvironmentInfo; treated as zero cubes (no sky drawn).
            count=0;
        }
        if(count==0)indices.assign(world.zones.size(),-1);
        // 13E830: renderer+28 (viewport clear) = World3EC colour, from [Fog].
        std::array<float,4> clear{{0.f,0.f,0.f,1.f}};bool clearAuthored=false;
        {
            rr::game::OriginalTrackFog50CR fog;rr::game::OriginalTrackFogDiagnostic50CR fd;
            if(rr::game::readOriginalTrackFog50CR(world,fog,fd)){
                for(unsigned k=0;k<3;++k)clear[k]=fog.global3AC.color00[k]/255.f;
                clearAuthored=true;
            }else if(fd.status!=rr::game::OriginalTrackFogStatus50CR::noFogBlock){
                error=std::string("source sky clear colour: [Fog] rejected (")+
                    rr::game::originalTrackFogStatusName50CR(fd.status)+")";return false;
            }
            // PORT STAND-IN (flagged): no Fog block (World3AC NULL) leaves the
            // clear colour at opaque black; the original's value is not recovered.
        }
        zoneCube_=std::move(indices);cubes_=static_cast<std::uint32_t>(count);cubeFile_=std::move(cubeFile);
        clear_=clear;clearAuthored_=clearAuthored;prepared_=true;
        return true;
    }
    // 274AE0: the cube images from the course's own archive (skies\<CubeFile1>).
    // A zero-cube track loads nothing and succeeds.
    template<class Archive> bool loadImages(const Archive& archive,const rr::assets::World48F& world,
        sky50gw_fixture::Diagnostic& d){
        if(!prepared_||images_){d.message="sky images need a prepared selection and load once";return false;}
        if(!cubes_)return true;
        auto images=std::make_unique<sky50gw_fixture::Owner>();
        if(!images->loadFromCourseData(archive,world,d))return false;
        // 1295B0 -> 1299A0: the same CUB's sequences from the same archive.
        auto sequences=std::make_unique<sky_cube_sequences50SF::Owner>();
        sky_cube_sequences50SF::Diagnostic sd;
        const bool sequenced=sequences->load([&](const std::string& member,std::vector<std::uint8_t>& out,
            rr::assets::DecodeError48C& error){return extractMember(archive,member,out,error);},*images,sd);
        // PORT bound (flagged): refused sequence data keeps the base sky (the
        // original asserts); the refusal reason is logged by the owner.
        if(sequenced&&sequences->loaded())sequences_=std::move(sequences);
        images_=std::move(images);return true;
    }
    // The cube sequences (null when the sky authors none or they were refused).
    sky_cube_sequences50SF::Owner* sequences()noexcept {return sequences_.get();}
    const sky_cube_sequences50SF::Owner* sequences()const noexcept {return sequences_.get();}
    bool prepared()const noexcept {return prepared_;}
    std::uint32_t cubeCount()const noexcept {return prepared_?cubes_:0u;}
    const std::string& cubeFile()const noexcept {return cubeFile_;}
    std::uint32_t zoneCount()const noexcept {return static_cast<std::uint32_t>(zoneCube_.size());}
    bool clearAuthored()const noexcept {return clearAuthored_;}
    // Viewport clear colour (normalized RGBA); null before prepare.
    const float* clearRgba()const noexcept {return prepared_?clear_.data():nullptr;}
    // The loaded cube images (null for a zero-cube track or before loadImages).
    const sky50gw_fixture::Owner* images()const noexcept {return images_.get();}
    // The context seam's owning slot (std::unique_ptr<sky50gw_fixture::Owner>&).
    std::unique_ptr<sky50gw_fixture::Owner>& imageStorage()noexcept {return images_;}
    bool select(std::uint32_t actualCameraZone48,bool& enabled)const noexcept {
        if(!prepared_||actualCameraZone48>=zoneCube_.size())return false;
        //275894 maps camera scene38->zone48;2758BC disables every cube,
        //275914 skips negative index;27591C..275964 enables the selected cube.
        enabled=cubes_&&images_&&zoneCube_[actualCameraZone48]==0;return true;
    }
};
}
