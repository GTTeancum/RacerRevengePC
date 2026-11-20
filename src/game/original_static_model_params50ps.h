#pragma once
// One reader of the track TOB "staticmodelparams" table for every course.
// Original: 271A00 (called from the race world TOB prefix) applies each row's
// AllowShadowReceiving to every loaded type-1 static model whose name matches;
// 27A208/27A234 first default every model's +148 to 1, and 27AC74..27AC94
// copy +148 into the model's +1B3 bit 4, which the shadow receiver gather
// 2DE900 tests. Courses differ only in the rows their TOB carries (MC sets
// all of its models to F, SA has none); no course name is read here.
//
// Replaces the nine per-course owners (MA, MB, SC, TB, GB, MC, RC, TC, GC)
// for this flag and fixes TA, GA and SA, whose rows were never applied.
// Rules carried over from those owners and from the RA reader
// (tests/native/original_race_world_tob_prefix50la.cpp:1290-1364):
//  - row count from staticmodelparams/numberofstaticmodelparams;
//  - rows model1..modelN; a missing section keeps the previously selected
//    section (271A74 ignores the 157D00 failure), starting from the
//    staticmodelparams section itself;
//  - field lookup is the packed 1583AC midpoint search, so duplicate labels
//    resolve exactly as in the original (TC model3);
//  - the row name is slt up to its LAST dot (1169D0);
//  - an instance's name is its own slt up to the FIRST dot, at most 50
//    characters (27A524); comparison is ASCII case-insensitive (308F00);
//  - only Standard (type 1) statics are written (271B4C);
//  - later rows overwrite earlier ones (source row order, then world order).
// Host memory only: writes go to the caller's per-instance vector, sized
// from the world, every index bounds-checked before the write.
#include "../assets/import_world48f.h"
#include "../assets/original_parameter_lookup50cr.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace rr::game {
struct StaticModelParamsReceipt50PS {
    std::size_t declaredRows = 0;      // numberofstaticmodelparams
    std::size_t rowsApplied = 0;       // rows that named a model and a value
    std::size_t rowsMissingSection = 0;// modelN absent: previous section reused
    std::size_t rowsMissingFields = 0; // no slt/allowshadowreceiving in the selected section
    std::size_t rowsUnknownValue = 0;  // value neither T/TRUE nor F/FALSE: no store
    std::size_t rowsUnmatched = 0;     // row matched no loaded type-1 static
    std::size_t stores = 0;            // individual instance writes
    std::size_t instances = 0, standardInstances = 0, receiving = 0;
    // Instances whose 27A524 name differs from the old geometry-member rule
    // (basename up to the last dot). Logged so the switch is visible.
    std::size_t nameRuleDiffers = 0;
};

namespace static_model_params50ps_detail {
inline const assets::ParameterBlock48F* findBlock(const assets::ParameterDocument48F& p,
    const std::string& name) noexcept {
    for (const auto& b : p.blocks) if (b.name == name) return &b;
    return nullptr;
}
// 1583AC: the shared midpoint search (src/assets/original_parameter_lookup50cr.h).
inline const assets::ParameterField48F* packedField(const assets::ParameterBlock48F& b,
    const std::string& name) noexcept {
    return assets::packedField1583AC(b, name);
}
inline char fold(char c) noexcept { return c >= 'A' && c <= 'Z' ? static_cast<char>(c - 'A' + 'a') : c; }
inline bool equalFolded(const std::string& a, const std::string& b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) if (fold(a[i]) != fold(b[i])) return false;
    return true;
}
// 158920 (shared rule): a present field always yields a value (T*/F*, ON/OFF, Y*/N*, else
// strtol base 0). 271B4C stores it into +148; 27AC74 keeps it as a flag (nonzero = receives).
inline int booleanValue(const std::string& s) noexcept {
    return assets::integer158920(s) != 0 ? 1 : 0;
}
inline std::string instanceStem(const std::string& slt) {
    const auto dot = slt.find('.');
    std::string stem = slt.substr(0, dot == std::string::npos ? slt.size() : dot);
    if (stem.size() > 50) stem.resize(50);
    return stem;
}
inline std::string geometryStem(const std::string& member) {
    const auto slash = member.find_last_of("/\\");
    std::string name = member.substr(slash == std::string::npos ? 0 : slash + 1);
    const auto dot = name.find_last_of('.');
    if (dot != std::string::npos) name.resize(dot);
    return name;
}
} // namespace static_model_params50ps_detail

// world: the course's decoded world (actualWorld): its instances and their SCB
// parameter blocks (names, StaticType). tob: the course's own TOB bytes
// (track.documents.tob, 05.02) -- 271A00 runs in the TOB stage; no SCB carries
// [StaticModelParams]. The TOB is decoded again with duplicate labels retained
// so the 1583AC lookup sees the original label table.
// 271AE0 reads AllowShadowReceiving with 158B30 (158920 T/TRUE/F/FALSE, any
// case) and default 1: an absent or unrecognised value stores 1.
// receiving: output, one entry per world instance, 1 by default (27A208).
inline bool applyStaticModelParams50PS(const assets::World48F& world,
    const std::vector<std::uint8_t>& tob,
    std::vector<std::uint32_t>& receiving, StaticModelParamsReceipt50PS& receipt,
    std::string& error) {
    namespace d = static_model_params50ps_detail;
    receipt = {};
    error.clear();
    const std::size_t instanceCount = world.instances.size();
    if (instanceCount > 100000) { error = "static model params: instance count outside bounds"; return false; }
    std::vector<std::uint32_t> next(instanceCount, 1u);
    assets::ParameterDocument48F table;
    assets::DecodeError48C why{};
    assets::WorldLimits48F limits{};
    limits.retainDuplicateLabels = true;
    if (tob.empty()) { receipt.instances = instanceCount; receipt.receiving = instanceCount; receiving = std::move(next); return true; }
    if (!assets::decodeParameters48F(tob, table, why, nullptr, limits)) {
        error = "static model params: track TOB rejected"; return false;
    }
    // Names and types of the loaded statics, in world order.
    std::vector<std::string> names(instanceCount);
    std::vector<bool> standard(instanceCount, false);
    for (std::size_t i = 0; i < instanceCount; ++i) {
        const auto& instance = world.instances[i];
        if (instance.parameter_block >= world.parameters.blocks.size()) {
            error = "static model params: instance parameter block outside world"; return false;
        }
        const auto& block = world.parameters.blocks[instance.parameter_block];
        const auto* slt = d::packedField(block, "slt");
        if (!slt) continue; // no model name: never matched, keeps the default
        const auto* type = d::packedField(block, "statictype");
        standard[i] = !type || type->value == "Standard";
        names[i] = d::instanceStem(slt->value);
        if (names[i].empty()) standard[i] = false;
        if (standard[i]) ++receipt.standardInstances;
        if (!d::equalFolded(names[i], d::geometryStem(instance.geometry_member))) ++receipt.nameRuleDiffers;
    }
    const auto* declared = d::findBlock(table, "staticmodelparams");
    std::size_t rows = 0;
    if (declared) {
        const auto* number = d::packedField(*declared, "numberofstaticmodelparams");
        if (number) {
            const long value = std::strtol(number->value.c_str(), nullptr, 10);
            if (value < 0 || value > 100000) { error = "static model params: row count outside bounds"; return false; }
            rows = static_cast<std::size_t>(value);
        }
    }
    receipt.declaredRows = rows;
    const assets::ParameterBlock48F* selected = declared;
    for (std::size_t row = 0; row < rows; ++row) {
        const auto* requested = d::findBlock(table, "model" + std::to_string(row + 1));
        if (requested) selected = requested; else ++receipt.rowsMissingSection;
        const auto* slt = selected ? d::packedField(*selected, "slt") : nullptr;
        const auto* shadow = selected ? d::packedField(*selected, "allowshadowreceiving") : nullptr;
        if (!slt) { ++receipt.rowsMissingFields; continue; } // empty name: matches no model
        const auto dot = slt->value.rfind('.');
        const std::string modelName = slt->value.substr(0, dot == std::string::npos ? slt->value.size() : dot);
        int value = shadow ? d::booleanValue(shadow->value) : 1;
        if (!shadow) ++receipt.rowsMissingFields;
        if (modelName.empty()) { ++receipt.rowsMissingFields; continue; }
        if (value < 0) { ++receipt.rowsUnknownValue; value = 1; } // 158B30 default
        ++receipt.rowsApplied;
        std::size_t matched = 0;
        for (std::size_t i = 0; i < instanceCount; ++i) {
            if (!standard[i] || !d::equalFolded(names[i], modelName)) continue;
            if (i >= next.size()) { error = "static model params: write outside instance table"; return false; }
            next[i] = static_cast<std::uint32_t>(value);
            ++matched; ++receipt.stores;
        }
        if (!matched) ++receipt.rowsUnmatched;
    }
    receipt.instances = instanceCount;
    for (auto v : next) receipt.receiving += v ? 1u : 0u;
    receiving = std::move(next);
    return true;
}
} // namespace rr::game
