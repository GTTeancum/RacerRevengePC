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
// 1583AC: midpoint search over the sorted label table, then both endpoints.
inline const assets::ParameterField48F* packedField(const assets::ParameterBlock48F& b,
    const std::string& name) noexcept {
    if (b.fields.empty()) return nullptr;
    if (b.fields.size() == 1) return b.fields[0].name == name ? &b.fields[0] : nullptr;
    std::size_t low = 0, high = b.fields.size() - 1, middle = (high - low) / 2;
    for (;;) {
        const int comparison = name.compare(b.fields[middle].name);
        if (!comparison) return &b.fields[middle];
        if (comparison >= 0) low = middle; else high = middle;
        if (high - low < 2) break;
        middle = low + (high - low) / 2;
    }
    if (b.fields[low].name == name) return &b.fields[low];
    if (b.fields[high].name == name) return &b.fields[high];
    return nullptr;
}
inline char fold(char c) noexcept { return c >= 'A' && c <= 'Z' ? static_cast<char>(c - 'A' + 'a') : c; }
inline bool equalFolded(const std::string& a, const std::string& b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) if (fold(a[i]) != fold(b[i])) return false;
    return true;
}
// 158920 boolean field. -1 = not a recognised value (no store).
inline int booleanValue(std::string s) noexcept {
    for (auto& c : s) if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
    if (s == "T" || s == "TRUE") return 1;
    if (s == "F" || s == "FALSE") return 0;
    return -1;
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

// world: the course's decoded world (actualWorld). Its parameters carry the
// exact TOB bytes; they are decoded again with duplicate labels retained so
// the 1583AC lookup sees the original label table.
// receiving: output, one entry per world instance, 1 by default (27A208).
inline bool applyStaticModelParams50PS(const assets::World48F& world,
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
    if (!assets::decodeParameters48F(world.parameters.original_bytes, table, why, nullptr, limits)) {
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
        if (!slt || !shadow) { ++receipt.rowsMissingFields; continue; }
        const auto dot = slt->value.rfind('.');
        const std::string modelName = slt->value.substr(0, dot == std::string::npos ? slt->value.size() : dot);
        const int value = d::booleanValue(shadow->value);
        if (modelName.empty()) { ++receipt.rowsMissingFields; continue; }
        if (value < 0) { ++receipt.rowsUnknownValue; continue; }
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
