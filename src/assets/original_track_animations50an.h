#pragma once
// Common reader for a track's [Animations] / [AnimationN] blocks and the trigger actions that
// address them. One reader for every course: counts, names and flags come from the track's own
// TOB, never from a course name, SHA or byte size.
//
// Original (SLUS_202.68, Toy.cpp), see work\scratch\track-data-gaps\anim\ANIM.md:
//   2713E0 Scene::LoadDynamicModels -> DynamicModels::Read 2D9C60 (vtable 3D5390 +0x74)
//   2D9FB0 AnimationType: "KeyFramed"->1, "Procedural"->2, anything else -> KeyFramed (2D9E90)
//   2D8230 KeyFramedTempData::Read  IsGlobal=0 UseLighting=1 StartingZone=1(-1) MCF="none" UseCollision=1
//   2D8A60 KeyFramedModel::Read     ReferenceName, NumberOfMotions, MotionToPlay%d, NumberOfSequences=1,
//          NumberInSequence%d=1 (>0), MotionSequence%d / ProbabilityList%d (comma lists, "none" ->
//          all 1 / all 100), LoopSequence%d=1, SequenceActive%d=1, NameOfSequence%d
//   2D84B0 DynamicModel::Read       Look, Up, Offset (vectors, default 0), Yaw, Pitch, Roll, NumberOfSounds
//   2796A0 LoopSoundSet::Read       NumberOfLoopingSounds, sections "<this section>LoopSound%d"
//   158920 int/bool                 first letter T/Y->1, F/N->0 (case-insensitive), ON/OFF, else strtol(base 0)
//   2DA0F0 AnimationTriggerAction::Read TimeTillReset, AnimationNumber=-1, AnimationNameRef, SpecificObject
//   2DA190 setup: number==-1 -> last model whose ReferenceName matches; else number-1; 1..count;
//          an Animation action clears its target's SequenceActive1 (target waits for the trigger)
// Keys the original never reads: OneTimeOnly / UseBlendedKeys in [AnimationN], OneTimeOnly in an
// action block. They are ignored here too.
//
// Host-side bounds: the original grows its arrays without limit and copies strings unbounded.
// This reader refuses (does not truncate) anything beyond TrackAnimationLimits50AN, any input
// the original would turn into a crash or a garbage read (missing AnimationN block, short
// MotionSequence list, motion index outside 1..NumberOfMotions, NumberInSequence 0), and
// duplicate labels it consumes. Output is written only on success.
#include "import_world48f.h"
#include <algorithm>
#include <array>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace rr { namespace assets {

// The one place that defines this reader's capacity. Stock maximum (13 courses): 14 animations
// (MC), 1 motion, 1 sequence of 1, 1 loop sound, 0 timed sounds, 22 triggers x 2 actions.
struct TrackAnimationLimits50AN {
    std::uint32_t animations = 64, motions = 16, sequences = 16, perSequence = 64;
    std::uint32_t loopSounds = 8, timedSounds = 8, triggers = 256, actionsPerTrigger = 16;
    std::uint32_t zones = 256;            // StartingZone admission when zoneCount is 0
    std::size_t nameBytes = 0x103;        // original 0x104-byte buffers incl. NUL
    std::size_t mcfStemBytes = 50;        // 2D8230 strcspn('.') capped at 0x32
};
enum class TrackAnimationType50AN : std::uint8_t { keyFramed = 1, procedural = 2 };
struct TrackAnimationSequence50AN {
    std::uint32_t count = 1;                  // NumberInSequence%d
    std::vector<std::uint32_t> motions;       // MotionSequence%d, 1-based into motionNames
    std::vector<std::int32_t> probability;    // ProbabilityList%d, 100 = always
    bool loop = true, active = true;          // LoopSequence%d, SequenceActive%d (authored)
    std::string name;                         // NameOfSequence%d ("none" default)
};
struct TrackAnimationLoopSound50AN {
    std::string section;                      // e.g. "animation1loopsound1"; fields read by the sound owner
    // Fields the sound class reads (279090) [field list I, from the strings block at 3BAF..].
    // Kept as authored text plus the parsed values the positional sound owner needs.
    std::string soundResourceFile, attachToObject;
    bool looping = true, startDisabled = false;
    float minDistance = 0.f, maxDistance = 0.f;
};
struct TrackAnimation50AN {
    std::uint32_t number = 0;                 // 1-based AnimationN
    TrackAnimationType50AN type = TrackAnimationType50AN::keyFramed;
    std::string mcfStem;                      // MCF value before '.', e.g. "xsailbarge" -> %s.mcf / %s.col
    std::string referenceName = "none";
    bool isGlobal = false, useLighting = true, useCollision = true;
    std::uint32_t startingZone = 0;           // 0-based (authored StartingZone - 1)
    std::array<float, 3> look{}, up{}, offset{};
    float yawDegrees = 0.f, pitchDegrees = 0.f, rollDegrees = 0.f;
    bool orientationFromAngles = true;        // 2D9264: Look == (0,0,0) -> Yaw/Pitch/Roll
    std::vector<std::string> motionNames;     // MotionToPlay%d (motion library names)
    std::vector<TrackAnimationSequence50AN> sequences;
    std::uint32_t timedSoundCount = 0;        // NumberOfSounds (0 on every stock course)
    std::vector<TrackAnimationLoopSound50AN> loopSounds;
    // Start state after load and after every race (re)start (2D94D4 copy, 2DA190, 2702E0/101B20).
    std::vector<bool> initialActive;
    std::uint32_t initialSequence = 0;        // 2D9418: first sequence with SequenceActive==1, else 0
    // PORT EXTENSION, NOT ORIGINAL (N64 data only): "StartHidden" in [AnimationN]. The original
    // executable has no such key; absent on every stock TOB, so stock rows stay visible.
    bool startHidden = false;
    std::uint32_t parentAnimation = 0; // One-based earlier N64 animation row, zero for a top-level row.
    bool transformOnly = false;
    bool loadLoop = false; // Re-register N64 scenery at its authored initial time on race restart.
    bool independentChannels = false; // N64 animated scenery repeats each channel at its own last key.
    bool holdLastKey = false; // N64 non-looping motion retains its terminal time for a later reversal.
};
enum class TrackAnimationActionKind50AN : std::uint8_t { animation, resetAnimation, hideAnimation };
struct TrackAnimationAction50AN {
    std::uint32_t trigger = 0, action = 0;    // 1-based TriggerN / ActionM
    TrackAnimationActionKind50AN kind = TrackAnimationActionKind50AN::animation;
    std::int32_t animationNumber = -1;
    std::string animationNameRef, specificObject;
    float timeTillReset = 0.f;
    std::uint32_t target = 0;                 // resolved 0-based animation index
};
struct TrackAnimationTable50AN {
    std::vector<TrackAnimation50AN> animations;  // empty = "No Animations defined." (valid)
    std::vector<TrackAnimationAction50AN> actions;
};
struct TrackAnimationDiagnostic50AN {
    std::string message;
    std::uint32_t animation = 0, trigger = 0, action = 0;
};

namespace track_animations50AN_detail {
struct Fail { const char* message; };
inline void need(bool ok, const char* message) { if (!ok) throw Fail{message}; }
inline const ParameterBlock48F* findBlock(const ParameterDocument48F& p, const std::string& name) {
    const ParameterBlock48F* found = nullptr;
    for (const auto& b : p.blocks)
        if (b.name == name) { need(!found, "duplicate parameter block"); found = &b; }
    return found;
}
inline const std::string* findField(const ParameterBlock48F& b, const std::string& key) {
    const std::string* found = nullptr;
    for (const auto& f : b.fields)
        if (f.name == key) { need(!found, "duplicate parameter label"); found = &f.value; }
    return found;
}
inline std::string indexed(const char* stem, std::uint32_t i) { return std::string(stem) + std::to_string(i); }
inline char upper(char c) { return c >= 'a' && c <= 'z' ? char(c - 'a' + 'A') : c; }
// 158920: T/Y/F/N by first letter, ON/OFF, else strtol(base 0) clamped to int32 (junk ignored).
inline std::int32_t integerValue(const std::string& s) {
    if (!s.empty()) {
        const char c = upper(s[0]);
        if (c == 'T' || c == 'Y') return 1;
        if (c == 'F' || c == 'N') return 0;
    }
    std::string u; for (char c : s) u += upper(c);
    if (u == "ON") return 1;
    if (u == "OFF") return 0;
    errno = 0; char* end = nullptr; const long v = std::strtol(s.c_str(), &end, 0);
    need(errno == 0 && v >= INT32_MIN && v <= INT32_MAX, "integer value out of range");
    return static_cast<std::int32_t>(v);
}
inline std::int32_t integerField(const ParameterBlock48F& b, const std::string& key, std::int32_t def) {
    const auto* v = findField(b, key); return v ? integerValue(*v) : def;
}
inline bool boolField(const ParameterBlock48F& b, const std::string& key, bool def) {
    return integerField(b, key, def ? 1 : 0) != 0;
}
inline float floatValue(const std::string& s) {
    const double d = std::strtod(s.c_str(), nullptr);       // 158A70: atof, junk ignored
    const float f = static_cast<float>(d);
    need(std::isfinite(f), "float value not finite");
    return f;
}
inline float floatField(const ParameterBlock48F& b, const std::string& key, float def) {
    const auto* v = findField(b, key); return v ? floatValue(*v) : def;
}
// 158C00: up to 0x80 bytes, strtok "," "," "\n", atof each; missing key -> default (0,0,0).
inline std::array<float, 3> vectorField(const ParameterBlock48F& b, const std::string& key) {
    std::array<float, 3> out{};
    const auto* v = findField(b, key);
    if (!v) return out;
    need(v->size() < 0x80, "vector value longer than the original 0x80-byte buffer");
    std::size_t at = 0;
    for (unsigned i = 0; i < 3; ++i) {
        const std::size_t end = i < 2 ? v->find(',', at) : v->find('\n', at);
        need(i == 2 || end != std::string::npos, "vector value has fewer than three components");
        out[i] = floatValue(v->substr(at, end == std::string::npos ? std::string::npos : end - at));
        at = end == std::string::npos ? v->size() : end + 1;
    }
    return out;
}
inline std::string stringField(const ParameterBlock48F& b, const std::string& key, const char* def,
                               std::size_t limit) {
    const auto* v = findField(b, key);
    std::string s = v ? *v : std::string(def);
    need(s.size() <= limit, "string value longer than the original buffer");
    return s;
}
// 2D8A60: "none" -> every entry = fill; else strtok(",") + atoi for the first count tokens.
inline std::vector<std::int32_t> listField(const ParameterBlock48F& b, const std::string& key,
                                           std::uint32_t count, std::int32_t fill, std::size_t limit) {
    const std::string s = stringField(b, key, "none", limit);
    std::vector<std::int32_t> out;
    if (s == "none") { out.assign(count, fill); return out; }
    std::size_t at = 0;
    while (out.size() < count) {
        while (at < s.size() && s[at] == ',') ++at;             // strtok skips empty tokens
        if (at >= s.size()) break;
        std::size_t end = s.find(',', at);
        if (end == std::string::npos) end = s.size();
        out.push_back(static_cast<std::int32_t>(std::atoi(s.substr(at, end - at).c_str())));
        at = end;
    }
    // The original leaves the remaining entries uninitialised (malloc) and later reads them.
    need(out.size() == count, "list has fewer entries than NumberInSequence");
    return out;
}
inline std::uint32_t boundedCount(std::int32_t v, std::uint32_t limit, const char* message) {
    need(v >= 0 && static_cast<std::uint32_t>(v) <= limit, message);
    return static_cast<std::uint32_t>(v);
}
} // namespace track_animations50AN_detail

// zoneCount: the scene's zone count (Create 2D90DC asserts StartingZone-1 < zones); 0 = use
// limits.zones. Output and diagnostic are replaced; output is untouched on failure.
inline bool readTrackAnimations50AN(const ParameterDocument48F& p, std::uint32_t zoneCount,
                                    const TrackAnimationLimits50AN& limits,
                                    TrackAnimationTable50AN& output,
                                    TrackAnimationDiagnostic50AN& diagnostic) noexcept {
    namespace d = track_animations50AN_detail;
    diagnostic = {};
    try {
        TrackAnimationTable50AN t;
        const std::uint32_t zones = zoneCount ? zoneCount : limits.zones;
        if (const auto* root = d::findBlock(p, "animations")) {
            const std::uint32_t count = d::boundedCount(d::integerField(*root, "numberofanimations", 0),
                                                        limits.animations, "NumberOfAnimations outside limits");
            t.animations.reserve(count);
            for (std::uint32_t i = 1; i <= count; ++i) {
                diagnostic.animation = i;
                const std::string section = d::indexed("animation", i);
                const auto* b = d::findBlock(p, section);
                d::need(b, "AnimationN block missing (original asserts or leaves a hole it later calls)");
                TrackAnimation50AN a; a.number = i;
                a.type = d::stringField(*b, "animationtype", "none", limits.nameBytes) == "Procedural"
                    ? TrackAnimationType50AN::procedural : TrackAnimationType50AN::keyFramed;
                // KeyFramedTempData 2D8230
                a.isGlobal = d::boolField(*b, "isglobal", false);
                a.useLighting = d::boolField(*b, "uselighting", true);
                const std::int32_t zone = d::integerField(*b, "startingzone", 1) - 1;
                d::need(zone >= 0 && static_cast<std::uint32_t>(zone) < zones, "Invalid StartingZone for dynamic model");
                a.startingZone = static_cast<std::uint32_t>(zone);
                const std::string mcf = d::stringField(*b, "mcf", "none", limits.nameBytes);
                a.mcfStem = mcf.substr(0, std::min(mcf.find('.'), limits.mcfStemBytes));
                a.useCollision = d::boolField(*b, "usecollision", true);
                // KeyFramedModel 2D8A60
                a.referenceName = d::stringField(*b, "referencename", "none", limits.nameBytes);
                const std::uint32_t motions = d::boundedCount(d::integerField(*b, "numberofmotions", 0),
                                                              limits.motions, "NumberOfMotions outside limits");
                for (std::uint32_t m = 1; m <= motions; ++m)
                    a.motionNames.push_back(d::stringField(*b, d::indexed("motiontoplay", m), "none", limits.nameBytes));
                const std::uint32_t sequences = d::boundedCount(d::integerField(*b, "numberofsequences", 1),
                                                                limits.sequences, "NumberOfSequences outside limits");
                for (std::uint32_t s = 1; s <= sequences; ++s) {
                    TrackAnimationSequence50AN q;
                    const std::int32_t n = d::integerField(*b, d::indexed("numberinsequence", s), 1);
                    d::need(n > 0, "NumberInSequence must be greater than zero.");
                    q.count = d::boundedCount(n, limits.perSequence, "NumberInSequence outside limits");
                    for (const auto v : d::listField(*b, d::indexed("motionsequence", s), q.count, 1, limits.nameBytes)) {
                        // 2D9590 reads motions[list-1]: index must address a real MotionToPlay.
                        d::need(v >= 1 && static_cast<std::uint32_t>(v) <= motions, "MotionSequence entry outside 1..NumberOfMotions");
                        q.motions.push_back(static_cast<std::uint32_t>(v));
                    }
                    q.probability = d::listField(*b, d::indexed("probabilitylist", s), q.count, 100, limits.nameBytes);
                    q.loop = d::boolField(*b, d::indexed("loopsequence", s), true);
                    q.active = d::boolField(*b, d::indexed("sequenceactive", s), true);
                    q.name = d::stringField(*b, d::indexed("nameofsequence", s), "none", limits.nameBytes);
                    a.sequences.push_back(std::move(q));
                }
                // DynamicModel 2D84B0
                a.look = d::vectorField(*b, "look");
                a.up = d::vectorField(*b, "up");
                a.offset = d::vectorField(*b, "offset");
                a.yawDegrees = d::floatField(*b, "yaw", 0.f);
                a.pitchDegrees = d::floatField(*b, "pitch", 0.f);
                a.rollDegrees = d::floatField(*b, "roll", 0.f);
                a.orientationFromAngles = a.look[0] == 0.f && a.look[1] == 0.f && a.look[2] == 0.f;
                a.timedSoundCount = d::boundedCount(d::integerField(*b, "numberofsounds", 0),
                                                    limits.timedSounds, "NumberOfSounds outside limits");
                // LoopSoundSet 2796A0: "<section>LoopSound%d"
                const std::uint32_t loops = d::boundedCount(d::integerField(*b, "numberofloopingsounds", 0),
                                                            limits.loopSounds, "NumberOfLoopingSounds outside limits");
                for (std::uint32_t k = 1; k <= loops; ++k) {
                    TrackAnimationLoopSound50AN s; s.section = section + d::indexed("loopsound", k);
                    const auto* sb = d::findBlock(p, s.section);
                    d::need(sb != nullptr, "loop sound block missing");
                    s.soundResourceFile = d::stringField(*sb, "soundresourcefile", "", limits.nameBytes);
                    s.attachToObject = d::stringField(*sb, "attachtoobject", "", limits.nameBytes);
                    s.looping = d::boolField(*sb, "looping", true);
                    s.startDisabled = d::boolField(*sb, "startdisabled", false);
                    s.minDistance = d::floatField(*sb, "mindistance", 0.f);
                    s.maxDistance = d::floatField(*sb, "maxdistance", 0.f);
                    a.loopSounds.push_back(std::move(s));
                }
                // PORT EXTENSION, NOT ORIGINAL: N64 StartHidden (inert on stock data).
                a.startHidden = d::boolField(*b, "starthidden", false);
                const auto parent = d::integerField(*b, "n64parentanimation", 0);
                d::need(parent >= 0 && static_cast<std::uint32_t>(parent) < i, "Animated parent must precede its child");
                a.parentAnimation = static_cast<std::uint32_t>(parent);
                a.transformOnly = d::boolField(*b, "n64transformonly", false);
                a.loadLoop = d::boolField(*b, "n64loadloop", false);
                a.independentChannels = d::boolField(*b, "n64independentchannels", false);
                a.holdLastKey = d::boolField(*b, "holdlastkey", false);
                for (const auto& q : a.sequences) a.initialActive.push_back(q.active);
                for (std::uint32_t s = 0; s < a.sequences.size(); ++s)
                    if (a.sequences[s].active) { a.initialSequence = s; break; }
                t.animations.push_back(std::move(a));
            }
            diagnostic.animation = 0;
        }
        // Trigger actions that address animations (2DA0F0 / 2DA190). Trigger geometry and the
        // other action types belong to the common trigger reader.
        if (const auto* root = d::findBlock(p, "triggers")) {
            const std::uint32_t triggers = d::boundedCount(d::integerField(*root, "numberoftriggers", 0),
                                                           limits.triggers, "NumberOfTriggers outside limits");
            for (std::uint32_t tr = 1; tr <= triggers; ++tr) {
                diagnostic.trigger = tr;
                const auto* tb = d::findBlock(p, d::indexed("trigger", tr));
                if (!tb) continue;                       // the trigger reader owns this error
                const std::uint32_t actions = d::boundedCount(d::integerField(*tb, "numberofactions", 0),
                                                              limits.actionsPerTrigger, "NumberOfActions outside limits");
                for (std::uint32_t ac = 1; ac <= actions; ++ac) {
                    diagnostic.action = ac;
                    const auto* ab = d::findBlock(p, d::indexed("trigger", tr) + d::indexed("action", ac));
                    if (!ab) continue;
                    const auto* type = d::findField(*ab, "actiontype");
                    if (!type) continue;
                    TrackAnimationAction50AN x; x.trigger = tr; x.action = ac;
                    if (*type == "Animation") x.kind = TrackAnimationActionKind50AN::animation;
                    else if (*type == "ResetAnimation") x.kind = TrackAnimationActionKind50AN::resetAnimation;
                    else if (*type == "HideAnimation") x.kind = TrackAnimationActionKind50AN::hideAnimation;
                    else continue;
                    x.timeTillReset = d::floatField(*ab, "timetillreset", 0.f);
                    x.animationNumber = d::integerField(*ab, "animationnumber", -1);
                    x.animationNameRef = d::stringField(*ab, "animationnameref", "", 0x27);
                    x.specificObject = d::stringField(*ab, "specificobject", "", limits.nameBytes);
                    std::int64_t target = -1;
                    if (x.animationNumber == -1) {
                        for (std::size_t i = 0; i < t.animations.size(); ++i)      // last match wins
                            if (t.animations[i].referenceName == x.animationNameRef) target = static_cast<std::int64_t>(i);
                    } else {
                        target = static_cast<std::int64_t>(x.animationNumber) - 1;
                    }
                    d::need(target >= 0 && target < static_cast<std::int64_t>(t.animations.size()),
                            "Trigger References non existant Animation");
                    x.target = static_cast<std::uint32_t>(target);
                    if (x.kind == TrackAnimationActionKind50AN::animation && !t.animations[x.target].initialActive.empty())
                        t.animations[x.target].initialActive[0] = false;              // 2DA2F4 / 101B20
                    t.actions.push_back(std::move(x));
                }
                diagnostic.action = 0;
            }
            diagnostic.trigger = 0;
        }
        output = std::move(t);
        return true;
    } catch (const track_animations50AN_detail::Fail& f) {
        diagnostic.message = f.message;
    } catch (...) {
        diagnostic.message = "track animation reader allocation failed";
    }
    return false;
}

}} // namespace rr::assets
