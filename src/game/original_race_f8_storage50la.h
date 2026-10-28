#pragma once
// Implementation-only. Existing ScreenObject representations, never a Pause
// instance/ledger or a second canonical material/font/texture manager.
#include "original_race_f8_50la.h"
#include "original_pause_storage50la.h"
#include "original_race_f8_polygon50la.h"
#include <optional>
namespace original_main_entry50KG { class RaceF8RendererLease50LA; class NativeF8ImageCache50LA; }
namespace rr::game {
struct OriginalRaceF850LA::Impl final {
    using D=OriginalRaceF8Diagnostic50LA;
    using Texture=OriginalPauseStorage50LA::Texture;
    using Text=OriginalPauseStorage50LA::Text;
    using PolygonStorage=OriginalRaceF8PolygonStorage50LA;
    using Polygon=PolygonStorage::Polygon;
    using Packet=OriginalPauseStorage50LA::Packet;
    using Primitive=OriginalPauseStorage50LA::Primitive;
    using Glyph=OriginalPauseStorage50LA::Glyph;
    using TextPrimitive=OriginalPauseStorage50LA::TextPrimitive;
    using TextPacket=OriginalPauseStorage50LA::TextPacket;
    using PacketVertex=OriginalPauseStorage50LA::PacketVertex;
    std::shared_ptr<OriginalApplicationEntities50LA> application;
    OriginalWreckComponent50LA component;
    bool counted=false,constructed=false,loadAttempted=false,loadComplete=false,
        initialVisibility=false,closed=false;
    std::uint32_t nativeBarBorrows=0; //Scoped native packet/material/Frame borrow, not source refs.
    bool retirementStarted=false; //Native resumable104970->198510 custody.
    std::shared_ptr<original_main_entry50KG::RaceF8RendererLease50LA> rendererLease;
    // Immutable private native pixel cache, never a material/pass readiness
    // receipt. Weak source keys avoid extending canonical reference lifetimes.
    std::shared_ptr<const original_main_entry50KG::NativeF8ImageCache50LA> nativeImageCache;
    std::uint32_t mask=0;
    // Native once-per-actual-Root update history; not a source timer/flag lane.
    std::uint64_t lastUpdatePublication=0;
    std::optional<std::array<Texture,2>> textures40;
    std::optional<std::array<Text,2>> texts100;
    std::array<std::unique_ptr<Polygon>,4> polygons30;
    // Source constructor establishes only low5 bits. Upper lanes are unknown
    // and are neither exposed nor filled from the separate native HUD gate.
    struct Fields {float transition1E0=-1.f;std::uint8_t flags1E4Low5=0x10;};
    std::optional<Fields> fields;
    struct Reference {std::int32_t row=-1;bool started=false,retained=false;};
    std::array<Reference,10> references{};
    std::size_t nextReference=0;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context;
    std::shared_ptr<assets::OriginalApplicationFonts50LA> fonts;
    std::shared_ptr<const assets::OriginalPauseSourceData50LA> sourceData;
    assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
    assets::OriginalApplicationFontDiagnostic50LA fontDiagnostic;
    std::uint32_t depth=0;
    explicit Impl(std::shared_ptr<OriginalApplicationEntities50LA>,std::uint64_t);
    bool detached()const noexcept;
    bool currentLeaf()const noexcept;
    bool stop(D&,std::uint32_t,const char*)noexcept;
    bool acquire(std::uint32_t caller,std::uint32_t offset,const std::string&,D&);
    bool polygon(std::size_t,const std::string&,D&);
    bool polygonCurrent(std::size_t)const noexcept;
    bool texture(Texture&,std::size_t,const std::string&,D&);
    bool text(Text&,std::size_t,const std::shared_ptr<const assets::OriginalApplicationFonts50LA::Entry>&,
        std::string&,D&);
    static void position(Texture&,float,float)noexcept;
    static void position(Text&,float,float)noexcept;
    static void color(Text&,std::uint32_t)noexcept;
    bool localized(Text&,std::size_t,D&);
    void release(std::size_t)noexcept;
    void closeText(Text&)noexcept;
    void closeTexture(Texture&)noexcept;
    void closeResources()noexcept;
};
}
