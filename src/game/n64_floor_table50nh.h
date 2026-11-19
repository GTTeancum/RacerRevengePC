#pragma once
// N64 floor table ("surface under the spline cursor") carried by a converted N64 track as
// the archive member tracks\<code>.nft. PORT EXTENSION, NOT ORIGINAL REVENGE: the original
// never opens this member (it reads members by name) and no stock archive has it.
//
// N64 (racer-n64\notes\n64-hazards-spec.md sections 7-8): at track load the N64 bakes one
// mesh per (band, tenth) slot (0x8007F098, PC swrSpline_BakeSampleTrackMesh) and in flight
// reads the slot of the pod's cursor (0x80080408, PC swrRace_GetTrackMeshAtCursor). The
// converter writes that table with each slot's START placed on the converted map line.
// Lookup on link (from, to) at parameter u: the sample of that link with the largest start
// <= u, else the last sample of the link before (the N64 trunc(t x 10) rule moved onto the
// converted line).
//
// Consumers: the zero-g racing-line table (native_zero_gravity50zg.h) as the floor under the
// line in flight, and the N64 hazard carrier test (n64_track_hazards_runtime50nh.h). Absent
// member = the zero-g table keeps its own line sampling (logged which one is used).
//
// Host memory only; nothing here writes game, guest or racer memory. Every index is
// bounds-checked; N64FloorLimits50NH is the one place this layout's capacities are defined.
#include "../assets/import_decode48c.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace rr::game {

struct N64FloorLimits50NH {
    static constexpr std::size_t bytes = 2u * 1024u * 1024u;  // the port's parameter-file cap
    static constexpr std::size_t surfaces = 1024;
    static constexpr std::size_t samples = 65536;
    static constexpr std::size_t objectsPerSurface = 64;
    static constexpr std::size_t nameBytes = 50;               // 27A524 instance-name rule
    static constexpr std::size_t slots = 8;                    // race slots
    static constexpr std::size_t graphs = 2;                   // map, AI (zero-g line graphs)
    static constexpr std::uint32_t version = 1;
    static constexpr std::uint32_t samplesPerLink = 10;        // N64 ForEachSample step
};
inline constexpr std::uint32_t n64NoFloorOwner50NH = std::numeric_limits<std::uint32_t>::max();

struct N64FloorSurface50NH {
    std::string mesh;                     // N64 mesh offset (provenance)
    std::vector<std::string> objects;     // static COL instance names (lower case); "none" = empty
    bool ambiguous = false;
    std::uint32_t firstSlot = 0;
    bool declared = false;
};
struct N64FloorSample50NH {
    std::uint32_t band = 0, tenth = 0, from = 0, to = 0;
    float t = 0.f;                        // slot start on the converted link (from, to)
    std::uint32_t surface = 0;            // 1..S, 0 = NULL (no face down or up)
    float dist = 0.f;                     // checking only
    char dir = 'D';                       // checking only: D down ray, U up retry
    bool declared = false;
};
struct N64FloorTable50NH {
    std::uint32_t version = 0, samplesPerLink = 0;
    std::string lineGraph;
    std::vector<N64FloorSurface50NH> surfaces; // index 0 = surface 1
    std::vector<N64FloorSample50NH> samples;
};

namespace n64_floor50NH_detail {
inline std::string lower(std::string s) { for (auto& c : s) if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a'); return s; }
inline std::string trim(const std::string& s) {
    std::size_t a = 0, b = s.size();
    while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r')) ++a;
    while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\r')) --b;
    return s.substr(a, b - a);
}
inline bool unsignedValue(const std::string& s, std::uint32_t& out, std::uint64_t limit) {
    if (s.empty() || s.size() > 10) return false;
    std::uint64_t v = 0;
    for (char c : s) { if (c < '0' || c > '9') return false; v = v * 10u + std::uint64_t(c - '0'); }
    if (v > limit) return false;
    out = static_cast<std::uint32_t>(v); return true;
}
inline bool floatValue(const std::string& s, float& out) {
    if (s.empty() || s.size() > 32) return false;
    char* end = nullptr; const float v = std::strtof(s.c_str(), &end);
    if (!end || end == s.c_str() || *end != '\0' || !std::isfinite(v)) return false;
    out = v; return true;
}
inline std::vector<std::string> split(const std::string& s) {
    std::vector<std::string> out; std::size_t at = 0;
    while (true) {
        const auto c = s.find(',', at);
        out.push_back(trim(s.substr(at, c == std::string::npos ? std::string::npos : c - at)));
        if (c == std::string::npos) break;
        at = c + 1;
    }
    return out;
}
struct Line { std::string key, value; bool section = false; std::size_t number = 0; };
// Lines without comments; a section line carries its lower-case name in key.
inline bool lines(const std::string& text, std::vector<Line>& out, std::string& error) {
    std::size_t at = 0, n = 0;
    while (at < text.size()) {
        const auto end = std::min(text.find('\n', at), text.size());
        std::string l = trim(text.substr(at, end - at)); at = end + 1; ++n;
        if (const auto semi = l.find(';'); semi != std::string::npos) l = trim(l.substr(0, semi));
        if (l.empty()) continue;
        Line x; x.number = n;
        if (l[0] == '[') {
            if (l.back() != ']') { error = "section header (line " + std::to_string(n) + ")"; return false; }
            x.section = true; x.key = lower(trim(l.substr(1, l.size() - 2)));
        } else {
            const auto eq = l.find('=');
            if (eq == std::string::npos) { error = "line without '=' (line " + std::to_string(n) + ")"; return false; }
            x.key = lower(trim(l.substr(0, eq))); x.value = trim(l.substr(eq + 1));
        }
        out.push_back(std::move(x));
    }
    return true;
}
} // namespace n64_floor50NH_detail

// Text reader (two passes: header counts, then surfaces and samples). Keys by name,
// case-insensitive; unknown keys and sections ignored (as the .spl readers do). Every count
// and field is bounds-checked; the table is written only on success.
inline bool readN64FloorTable50NH(const std::vector<std::uint8_t>& bytes, N64FloorTable50NH& out, std::string& error) {
    namespace d = n64_floor50NH_detail;
    using L = N64FloorLimits50NH;
    error.clear();
    if (bytes.empty() || bytes.size() > L::bytes) { error = "member size outside 1..2 MiB"; return false; }
    std::vector<d::Line> lines;
    if (!d::lines(std::string(bytes.begin(), bytes.end()), lines, error)) return false;
    auto fail = [&](const d::Line& l, const char* why) { error = std::string(why) + " (line " + std::to_string(l.number) + ")"; return false; };
    N64FloorTable50NH t;
    std::uint32_t surfaceCount = 0, sampleCount = 0;
    bool header = false, counts[2] = {false, false};
    // Pass 1: [FloorTable].
    for (std::size_t i = 0, inHeader = 0; i < lines.size(); ++i) {
        const auto& l = lines[i];
        if (l.section) {
            if (l.key == "floortable") { if (header) return fail(l, "duplicate [FloorTable]"); header = true; inHeader = 1; }
            else inHeader = 0;
            continue;
        }
        if (!inHeader) continue;
        if (l.key == "version") { if (!d::unsignedValue(l.value, t.version, 1000)) return fail(l, "Version"); }
        else if (l.key == "samplesperlink") { if (!d::unsignedValue(l.value, t.samplesPerLink, 1000)) return fail(l, "SamplesPerLink"); }
        else if (l.key == "linegraph") t.lineGraph = d::lower(l.value);
        else if (l.key == "numsurfaces") {
            if (counts[0] || !d::unsignedValue(l.value, surfaceCount, L::surfaces)) return fail(l, "NumSurfaces outside 0..1024");
            counts[0] = true;
        } else if (l.key == "numsamples") {
            if (counts[1] || !d::unsignedValue(l.value, sampleCount, L::samples) || !sampleCount) return fail(l, "NumSamples outside 1..65536");
            counts[1] = true;
        }
    }
    if (!header) { error = "no [FloorTable]"; return false; }
    if (t.version != L::version) { error = "Version must be 1"; return false; }
    if (t.samplesPerLink != L::samplesPerLink) { error = "SamplesPerLink must be 10"; return false; }
    if (t.lineGraph != "map") { error = "LineGraph must be map"; return false; }
    if (!counts[0] || !counts[1]) { error = "NumSurfaces / NumSamples missing"; return false; }
    t.surfaces.assign(surfaceCount, {}); t.samples.assign(sampleCount, {});
    // Pass 2: [Surface<i>] and [Samples].
    enum class Section { other, surface, samples } section = Section::other;
    std::size_t surface = 0; bool samplesSeen = false;
    for (const auto& l : lines) {
        if (l.section) {
            section = Section::other;
            if (l.key == "samples") { if (samplesSeen) return fail(l, "duplicate [Samples]"); samplesSeen = true; section = Section::samples; }
            else if (l.key.rfind("surface", 0) == 0) {
                std::uint32_t i = 0;
                if (!d::unsignedValue(l.key.substr(7), i, L::surfaces) || i < 1 || i > surfaceCount)
                    return fail(l, "[Surface<i>] outside 1..NumSurfaces");
                if (t.surfaces[i - 1].declared) return fail(l, "duplicate [Surface<i>]");
                t.surfaces[i - 1].declared = true; section = Section::surface; surface = i - 1;
            }
            continue;
        }
        if (section == Section::surface) {
            auto& s = t.surfaces[surface];
            if (l.key == "mesh") s.mesh = l.value;
            else if (l.key == "objects") {
                s.objects.clear();
                if (d::lower(l.value) != "none")
                    for (auto& n : d::split(l.value)) {
                        if (n.empty() || n.size() > L::nameBytes) return fail(l, "Objects name length");
                        if (s.objects.size() >= L::objectsPerSurface) return fail(l, "Objects exceed 64 names");
                        s.objects.push_back(d::lower(n));
                    }
            } else if (l.key == "ambiguous") {
                if (l.value != "T" && l.value != "F" && l.value != "TRUE" && l.value != "FALSE") return fail(l, "Ambiguous");
                s.ambiguous = l.value == "T" || l.value == "TRUE";
            } else if (l.key == "firstslot") { if (!d::unsignedValue(l.value, s.firstSlot, 0xffffffffull)) return fail(l, "FirstSlot"); }
        } else if (section == Section::samples) {
            if (l.key.size() < 2 || l.key[0] != 's') continue; // unknown key
            std::uint32_t n = 0;
            if (!d::unsignedValue(l.key.substr(1), n, L::samples) || n >= sampleCount) return fail(l, "S<n> outside 0..NumSamples-1");
            auto& q = t.samples[n];
            if (q.declared) return fail(l, "duplicate S<n>");
            const auto f = d::split(l.value);
            if (f.size() != 8) return fail(l, "S<n> needs 8 fields");
            if (!d::unsignedValue(f[0], q.band, 1000000) || !d::unsignedValue(f[1], q.tenth, 1000) ||
                !d::unsignedValue(f[2], q.from, 1000000) || !d::unsignedValue(f[3], q.to, 1000000) ||
                !d::floatValue(f[4], q.t) || !d::unsignedValue(f[5], q.surface, surfaceCount) ||
                !d::floatValue(f[6], q.dist) || f[7].size() != 1 || (f[7][0] != 'D' && f[7][0] != 'U'))
                return fail(l, "S<n> field");
            if (!(q.t >= 0.f && q.t <= 1.f)) return fail(l, "S<n> t outside [0,1]");
            if (q.tenth >= L::samplesPerLink) return fail(l, "S<n> tenth outside 0..9");
            q.dir = f[7][0]; q.declared = true;
        }
    }
    for (std::size_t i = 0; i < t.surfaces.size(); ++i) if (!t.surfaces[i].declared) { error = "[Surface" + std::to_string(i + 1) + "] missing"; return false; }
    for (std::size_t i = 0; i < t.samples.size(); ++i) if (!t.samples[i].declared) { error = "S" + std::to_string(i) + " missing"; return false; }
    // Each N64 slot (band x 10 + tenth) exactly once.
    {
        std::vector<std::pair<std::uint32_t, std::uint32_t>> slots; slots.reserve(t.samples.size());
        for (const auto& q : t.samples) slots.push_back({q.band, q.tenth});
        std::sort(slots.begin(), slots.end());
        if (std::adjacent_find(slots.begin(), slots.end()) != slots.end()) { error = "an N64 slot (band, tenth) appears twice"; return false; }
    }
    out = std::move(t); return true;
}

// The table bound to the port's own line graphs (the zero-g map and AI graphs). Built once
// per course; read per racer step. Graph g is bound only when every NFT link is a link of
// that graph and (for a graph other than the one the NFT names) its nodes and links equal
// the named graph's, so the same progress means the same place.
class N64FloorTableOwner50NH {
public:
    using L = N64FloorLimits50NH;
    // archive: the course RES; member: tracks\<code>.nft (from the world member name);
    // graphs: [0] map, [1] AI (each with nodes[i].line[3], next[], nextCount);
    // ownerName(k) / ownerCount: the static collision owners, SLT name to the first dot
    // (27A524 rule, lower case; SurfaceRules50ST numbers and names them).
    template<class Graphs, class Rules>
    void load(const assets::ResArchive48C& archive, const char* member, const Graphs& graphs, const Rules& rules) {
        reset();
        try {
            if (!member || !*member) { log("absent", "no_member_name"); return; }
            bool present = false;
            for (const auto& e : archive.entries) if (n64_floor50NH_detail::lower(e.name) == n64_floor50NH_detail::lower(member)) present = true;
            if (!present) { std::cerr << "n64_floor_table member=" << member << " present=0 source=line_sampling\n"; return; }
            std::vector<std::uint8_t> bytes; assets::DecodeError48C why{};
            if (!assets::extractResMember48C(archive, member, bytes, why)) { refuse(member, "member extract"); return; }
            N64FloorTable50NH t; std::string error;
            if (!readN64FloorTable50NH(bytes, t, error)) { refuse(member, error); return; }
            // Surfaces -> static owners (any instance of a mesh stands for it).
            std::vector<std::vector<std::uint32_t>> owners(t.surfaces.size());
            std::size_t unresolvedNames = 0, noneSurfaces = 0, mixedZeroG = 0;
            for (std::size_t s = 0; s < t.surfaces.size(); ++s) {
                if (t.surfaces[s].objects.empty()) ++noneSurfaces;
                for (const auto& name : t.surfaces[s].objects) {
                    bool hit = false;
                    for (std::size_t k = 0; k < rules.ownerCount(); ++k)
                        if (rules.ownerName(static_cast<std::uint32_t>(k)) == name) {
                            if (owners[s].size() >= L::objectsPerSurface * 4u) { refuse(member, "surface owner list exceeds capacity"); return; }
                            owners[s].push_back(static_cast<std::uint32_t>(k)); hit = true;
                        }
                    if (!hit) ++unresolvedNames;
                }
                for (const auto o : owners[s])
                    if (rules.ownerZeroGravity(o) != rules.ownerZeroGravity(owners[s].front())) { ++mixedZeroG; break; }
            }
            if (unresolvedNames) { refuse(member, std::to_string(unresolvedNames) + " Objects names name no static owner"); return; }
            std::array<bool, L::graphs> bound{};
            std::array<std::vector<std::vector<Entry>>, L::graphs> links{};
            for (std::size_t g = 0; g < L::graphs && g < graphs.size(); ++g) {
                const auto& graph = graphs[g];
                if (!graph.ready()) continue;
                if (g != 0) { // NFT names the map graph: another graph binds only when identical to it
                    const auto& m = graphs[0];
                    bool same = m.nodes.size() == graph.nodes.size();
                    for (std::size_t i = 0; same && i < m.nodes.size(); ++i) {
                        const auto& a = m.nodes[i]; const auto& b = graph.nodes[i];
                        same = a.nextCount == b.nextCount;
                        for (unsigned k = 0; same && k < 3; ++k) same = std::fabs(a.line[k] - b.line[k]) <= .01f;
                        for (std::uint32_t j = 0; same && j < a.nextCount; ++j) same = a.next[j] == b.next[j];
                    }
                    if (!same) { std::cerr << "n64_floor_table graph=" << g << " bound=0 reason=differs_from_map\n"; continue; }
                }
                // Per node: per next slot: (start, surface) in progress order.
                std::vector<std::vector<Entry>> per(graph.nodes.size() * maxNext);
                bool ok = true; std::string why;
                for (const auto& q : t.samples) {
                    if (q.from >= graph.nodes.size() || q.to >= graph.nodes.size()) { ok = false; why = "sample link node outside the graph"; break; }
                    const auto& a = graph.nodes[q.from]; std::uint32_t slot = maxNext;
                    for (std::uint32_t j = 0; j < a.nextCount && j < maxNext; ++j) if (a.next[j] == q.to) { slot = j; break; }
                    if (slot == maxNext) { ok = false; why = "sample link is not a link of the graph"; break; }
                    auto& list = per[std::size_t(q.from) * maxNext + slot];
                    if (!list.empty() && !(q.t > list.back().start)) { ok = false; why = "sample starts not increasing on a link"; break; }
                    list.push_back({q.t, q.surface});
                }
                if (!ok) { std::cerr << "n64_floor_table graph=" << g << " bound=0 reason=" << why << '\n'; continue; }
                links[g] = std::move(per); bound[g] = true;
            }
            if (!bound[0]) { refuse(member, "map graph not bound"); return; }
            // Carry-in: the last sample of the link before (unique predecessor link).
            for (std::size_t g = 0; g < L::graphs; ++g) if (bound[g]) {
                const auto& graph = graphs[g];
                carry_[g].assign(graph.nodes.size() * maxNext, 0u);
                std::size_t multiple = 0;
                for (std::size_t i = 0; i < graph.nodes.size(); ++i) {
                    // predecessors of node i: links (p -> i)
                    std::uint32_t found = 0; bool have = false; std::size_t count = 0;
                    for (std::size_t p = 0; p < graph.nodes.size(); ++p)
                        for (std::uint32_t j = 0; j < graph.nodes[p].nextCount && j < maxNext; ++j)
                            if (graph.nodes[p].next[j] == i) {
                                const auto& list = links[g][p * maxNext + j];
                                if (!list.empty()) { if (!have) { found = list.back().surface; have = true; } ++count; }
                            }
                    if (count > 1) ++multiple;
                    for (std::uint32_t j = 0; j < maxNext; ++j) carry_[g][i * maxNext + j] = found;
                }
                std::cerr << "n64_floor_table graph=" << (g ? "ai" : "map") << " bound=1 links_with_samples="
                          << std::count_if(links[g].begin(), links[g].end(), [](const auto& v) { return !v.empty(); })
                          << " multiple_predecessors=" << multiple << '\n';
            }
            table_ = std::move(t); owners_ = std::move(owners); links_ = std::move(links); bound_ = bound; loaded_ = true;
            std::size_t nulls = 0, up = 0;
            for (const auto& q : table_.samples) { if (!q.surface) ++nulls; if (q.dir == 'U') ++up; }
            std::cerr << "n64_floor_table member=" << member << " present=1 source=nft samples=" << table_.samples.size()
                      << " surfaces=" << table_.surfaces.size() << " null_slots=" << nulls << " up_rays=" << up
                      << " objects_none=" << noneSurfaces << " mixed_zero_g_surfaces=" << mixedZeroG
                      << " map=" << bound_[0] << " ai=" << bound_[1] << '\n';
        } catch (...) { reset(); std::cerr << "n64_floor_table refused reason=allocation source=line_sampling\n"; }
    }
    void reset() noexcept {
        loaded_ = false; bound_ = {}; table_ = {}; owners_.clear();
        for (auto& l : links_) l.clear();
        for (auto& c : carry_) c.clear();
        lastSurface_.fill(0); lastValid_.fill(false);
    }
    bool loaded() const noexcept { return loaded_; }
    bool bound(std::size_t graph) const noexcept { return loaded_ && graph < L::graphs && bound_[graph]; }
    // Surface (1..S, 0 = NULL) at (from, next slot, t). False when the graph is not bound.
    bool surfaceAt(std::size_t graph, std::uint32_t from, std::uint32_t nextSlot, float t, std::uint32_t& surface) const noexcept {
        if (!bound(graph) || nextSlot >= maxNext) return false;
        const std::size_t index = std::size_t(from) * maxNext + nextSlot;
        if (index >= links_[graph].size() || index >= carry_[graph].size()) return false;
        const auto& list = links_[graph][index];
        surface = carry_[graph][index];
        for (const auto& e : list) { if (e.start <= t) surface = e.surface; else break; }
        return true;
    }
    // First (representative) static owner of a surface, for the surface rules.
    std::uint32_t owner(std::uint32_t surface) const noexcept {
        if (!loaded_ || surface == 0 || surface > owners_.size() || owners_[surface - 1].empty()) return n64NoFloorOwner50NH;
        return owners_[surface - 1].front();
    }
    // Every static owner holding faces of that surface's mesh (carrier test).
    const std::vector<std::uint32_t>* owners(std::uint32_t surface) const noexcept {
        if (!loaded_ || surface == 0 || surface > owners_.size()) return nullptr;
        return &owners_[surface - 1];
    }
    // The zero-g table's hook: when the graph is bound, replace the line sample's owner by
    // the NFT surface's (floor height and normal stay the port's own ray result; the N64
    // table carries the mesh only). store must outlive the step (one per slot here).
    template<class Sample>
    void apply(std::size_t slot, std::size_t graph, std::uint32_t from, std::uint32_t nextSlot, float t,
               const Sample*& sample, Sample& store) noexcept {
        if (slot >= L::slots) return;
        std::uint32_t surface = 0;
        if (!surfaceAt(graph, from, nextSlot, t, surface)) { lastValid_[slot] = false; return; }
        store = sample ? *sample : Sample{};
        store.owner = owner(surface);
        sample = surface ? &store : nullptr;
        lastSurface_[slot] = surface; lastValid_[slot] = true;
    }
    // The NFT surface the slot's last zero-g step read (0 = NULL or not bound).
    std::uint32_t lastSurface(std::size_t slot) const noexcept { return slot < L::slots && lastValid_[slot] ? lastSurface_[slot] : 0u; }
    bool lastValid(std::size_t slot) const noexcept { return slot < L::slots && lastValid_[slot]; }
    std::size_t surfaceCount() const noexcept { return loaded_ ? table_.surfaces.size() : 0; }
private:
    static constexpr std::uint32_t maxNext = 8; // >= maxUndertrackNext50ET (checked by the graph's own nextCount)
    struct Entry { float start = 0.f; std::uint32_t surface = 0; };
    static void log(const char* state, const char* reason) { std::cerr << "n64_floor_table member=" << state << " present=0 source=line_sampling reason=" << reason << '\n'; }
    void refuse(const char* member, const std::string& why) {
        reset(); std::cerr << "n64_floor_table member=" << member << " present=1 refused=\"" << why << "\" source=line_sampling\n";
    }
    bool loaded_ = false;
    std::array<bool, L::graphs> bound_{};
    N64FloorTable50NH table_;
    std::vector<std::vector<std::uint32_t>> owners_;
    std::array<std::vector<std::vector<Entry>>, L::graphs> links_{};
    std::array<std::vector<std::uint32_t>, L::graphs> carry_{};
    std::array<std::uint32_t, L::slots> lastSurface_{};
    std::array<bool, L::slots> lastValid_{};
};

// tracks\<code>.nft from the course's world member (tracks\<code>.scb): data-derived, no course name.
inline std::string n64FloorMember50NH(const char* worldMember) {
    if (!worldMember) return {};
    std::string s(worldMember); const auto dot = s.rfind('.');
    if (dot == std::string::npos) return {};
    return s.substr(0, dot) + ".nft";
}

} // namespace rr::game
