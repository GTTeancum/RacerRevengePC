#pragma once
#include "../../platform/linux/native/game_host50er.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>

namespace rr::platform {
// Borrows the already-current application host; owns no window, context, GL
// objects or gameplay state. Construct/use on the host's main thread. The
// caller must perform its normal loading poll immediately before draw(), so
// host.info() contains the actual SDL drawable size and minimized observation.
// A minimized/zero-sized drawable is a successful no-op (the title is updated).
// Never pumps events. No application allocations, shaders or buffers are used.
class PlayerLoadingPresenter50DU final {
    using U = std::uint32_t;
    using I = std::int32_t;
    using B = std::uint8_t;
    static constexpr U drawFramebuffer = 0x8CA9, drawFramebufferBinding = 0x8CA6;
    static constexpr U drawBuffer0 = 0x8825, back = 0x0405, colorBufferBit = 0x4000;
    static constexpr U scissorTest = 0x0C11, scissorBox = 0x0C10;
    static constexpr U framebufferSrgb = 0x8DB9, rasterizerDiscard = 0x8C89;
    static constexpr U clearColorValue = 0x0C22, colorWriteMask = 0x0C23;
    static constexpr I maxSavedDrawBuffers = 64;
    // Windows builds are x64 (one calling convention); Linux uses its native ABI.
    struct Api {
        void (*getIntegerv)(U, I*) = nullptr;
        void (*getIntegeri_v)(U, U, I*) = nullptr;
        void (*getFloatv)(U, float*) = nullptr;
        void (*getBooleani_v)(U, U, B*) = nullptr;
        const B* (*getStringi)(U, U) = nullptr;
        B (*isEnabled)(U) = nullptr;
        B (*isEnabledi)(U, U) = nullptr;
        void (*enable)(U) = nullptr;
        void (*disable)(U) = nullptr;
        void (*enablei)(U, U) = nullptr;
        void (*disablei)(U, U) = nullptr;
        void (*bindFramebuffer)(U, U) = nullptr;
        void (*drawBuffer)(U) = nullptr;
        void (*drawBuffers)(I, const U*) = nullptr;
        void (*colorMaski)(U, B, B, B, B) = nullptr;
        void (*clearColor)(float, float, float, float) = nullptr;
        void (*clear)(U) = nullptr;
        void (*scissor)(I, I, I, I) = nullptr;
        void (*scissorIndexed)(U, I, I, I, I) = nullptr;
        U (*getError)() = nullptr;
        bool indexedScissor = false;
        I drawBufferCount = 0;
    } gl_{};
    struct Saved {
        I drawFbo = 0;
        std::array<U, maxSavedDrawBuffers> defaultDrawBuffers{};
        std::array<I, 4> scissor{};
        std::array<float, 4> clearColor{};
        std::array<B, 4> colorMask{};
        B scissorEnabled = 0, srgbEnabled = 0, discardEnabled = 0;
    };
    GameHost50ER& host_;
    const std::thread::id thread_;
    const std::chrono::steady_clock::time_point activityStarted_ = std::chrono::steady_clock::now();
    bool loaded_ = false;
    I width_ = 0, height_ = 0;

    bool load() noexcept {
        if (loaded_) return true;
#define RR_LOADING50DU_LOAD(member, name) \
        gl_.member = reinterpret_cast<decltype(gl_.member)>(host_.procAddress(name)); \
        if (!gl_.member) return false
        RR_LOADING50DU_LOAD(getIntegerv, "glGetIntegerv");
        RR_LOADING50DU_LOAD(getIntegeri_v, "glGetIntegeri_v");
        RR_LOADING50DU_LOAD(getFloatv, "glGetFloatv");
        RR_LOADING50DU_LOAD(getBooleani_v, "glGetBooleani_v");
        RR_LOADING50DU_LOAD(getStringi, "glGetStringi");
        RR_LOADING50DU_LOAD(isEnabled, "glIsEnabled");
        RR_LOADING50DU_LOAD(isEnabledi, "glIsEnabledi");
        RR_LOADING50DU_LOAD(enable, "glEnable");
        RR_LOADING50DU_LOAD(disable, "glDisable");
        RR_LOADING50DU_LOAD(enablei, "glEnablei");
        RR_LOADING50DU_LOAD(disablei, "glDisablei");
        RR_LOADING50DU_LOAD(bindFramebuffer, "glBindFramebuffer");
        RR_LOADING50DU_LOAD(drawBuffer, "glDrawBuffer");
        RR_LOADING50DU_LOAD(drawBuffers, "glDrawBuffers");
        RR_LOADING50DU_LOAD(colorMaski, "glColorMaski");
        RR_LOADING50DU_LOAD(clearColor, "glClearColor");
        RR_LOADING50DU_LOAD(clear, "glClear");
        RR_LOADING50DU_LOAD(scissor, "glScissor");
        RR_LOADING50DU_LOAD(getError, "glGetError");
        // Do not hide an upstream GL failure by draining the error queue.
        // As with any glGetError observer, a reported error is consumed.
        if (gl_.getError() != 0) return false;
        I major = 0, minor = 0, extensions = 0;
        gl_.getIntegerv(0x821B, &major); // GL_MAJOR_VERSION
        gl_.getIntegerv(0x821C, &minor); // GL_MINOR_VERSION
        gl_.getIntegerv(0x8824, &gl_.drawBufferCount); // GL_MAX_DRAW_BUFFERS
        gl_.indexedScissor = major > 4 || (major == 4 && minor >= 1);
        if (!gl_.indexedScissor) {
            gl_.getIntegerv(0x821D, &extensions); // GL_NUM_EXTENSIONS
            for (I i = 0; i < extensions; ++i) {
                const auto* extension = gl_.getStringi(0x1F03, static_cast<U>(i));
                if (extension && std::strcmp(reinterpret_cast<const char*>(extension),
                        "GL_ARB_viewport_array") == 0) {
                    gl_.indexedScissor = true;
                    break;
                }
            }
        }
        if (gl_.indexedScissor) {
            RR_LOADING50DU_LOAD(scissorIndexed, "glScissorIndexed");
        }
#undef RR_LOADING50DU_LOAD
        if (gl_.getError() != 0 || gl_.drawBufferCount < 1 ||
            gl_.drawBufferCount > maxSavedDrawBuffers) return false;
        loaded_ = true;
        return true;
    }
    void setScissorEnabled(bool enabled) noexcept {
        if (gl_.indexedScissor) {
            if (enabled) gl_.enablei(scissorTest, 0); else gl_.disablei(scissorTest, 0);
        } else {
            if (enabled) gl_.enable(scissorTest); else gl_.disable(scissorTest);
        }
    }
    void setScissor(I x, I y, I w, I h) noexcept {
        if (gl_.indexedScissor) gl_.scissorIndexed(0, x, y, w, h);
        else gl_.scissor(x, y, w, h);
    }
    void restore(const Saved& s) noexcept {
        // We changed only the DEFAULT framebuffer's draw-buffer state. Restore
        // it before rebinding the caller's FBO; its own MRT map is untouched.
        bool single = true;
        for (I i = 1; i < gl_.drawBufferCount; ++i)
            single = single && s.defaultDrawBuffers[static_cast<std::size_t>(i)] == 0;
        if (single) gl_.drawBuffer(s.defaultDrawBuffers[0]);
        else gl_.drawBuffers(gl_.drawBufferCount, s.defaultDrawBuffers.data());
        gl_.bindFramebuffer(drawFramebuffer, static_cast<U>(s.drawFbo));
        gl_.clearColor(s.clearColor[0], s.clearColor[1], s.clearColor[2], s.clearColor[3]);
        gl_.colorMaski(0, s.colorMask[0], s.colorMask[1], s.colorMask[2], s.colorMask[3]);
        setScissor(s.scissor[0], s.scissor[1], s.scissor[2], s.scissor[3]);
        setScissorEnabled(s.scissorEnabled != 0);
        if (s.srgbEnabled) gl_.enable(framebufferSrgb); else gl_.disable(framebufferSrgb);
        if (s.discardEnabled) gl_.enable(rasterizerDiscard); else gl_.disable(rasterizerDiscard);
        // GL_READ_FRAMEBUFFER_BINDING and read-buffer selection were never
        // changed. Neither were viewport, program, VAO, textures, depth/stencil,
        // blend, pixel transfer state, nor indexed masks/scissors above index 0.
    }
    void rectangle(I x, I top, I w, I h) noexcept {
        const I left = std::max<I>(0, x), right = std::min<I>(width_, x + w);
        const I upper = std::max<I>(0, top), lower = std::min<I>(height_, top + h);
        if (left >= right || upper >= lower) return;
        setScissor(left, height_ - lower, right - left, lower - upper);
        gl_.clear(colorBufferBit);
    }
    static const B* glyph(char c) noexcept {
        // Five bits per row, top to bottom. ASCII uppercase, digits and the
        // punctuation used by stage labels; unknown bytes render as '?'.
        static constexpr B letters[][7] = {
            {14,17,17,31,17,17,17}, {30,17,17,30,17,17,30},
            {14,17,16,16,16,17,14}, {30,17,17,17,17,17,30},
            {31,16,16,30,16,16,31}, {31,16,16,30,16,16,16},
            {14,17,16,23,17,17,15}, {17,17,17,31,17,17,17},
            {31,4,4,4,4,4,31}, {7,2,2,2,18,18,12},
            {17,18,20,24,20,18,17}, {16,16,16,16,16,16,31},
            {17,27,21,21,17,17,17}, {17,25,21,19,17,17,17},
            {14,17,17,17,17,17,14}, {30,17,17,30,16,16,16},
            {14,17,17,17,21,18,13}, {30,17,17,30,20,18,17},
            {15,16,16,14,1,1,30}, {31,4,4,4,4,4,4},
            {17,17,17,17,17,17,14}, {17,17,17,17,17,10,4},
            {17,17,17,21,21,21,10}, {17,17,10,4,10,17,17},
            {17,17,10,4,4,4,4}, {31,1,2,4,8,16,31}
        };
        static constexpr B digits[][7] = {
            {14,17,19,21,25,17,14}, {4,12,4,4,4,4,14},
            {14,17,1,2,4,8,31}, {30,1,1,14,1,1,30},
            {2,6,10,18,31,2,2}, {31,16,16,30,1,1,30},
            {14,16,16,30,17,17,14}, {31,1,2,4,8,8,8},
            {14,17,17,14,17,17,14}, {14,17,17,15,1,1,14}
        };
        static constexpr B blank[7] = {}, dash[7] = {0,0,0,31,0,0,0};
        static constexpr B dot[7] = {0,0,0,0,0,12,12}, colon[7] = {0,12,12,0,12,12,0};
        static constexpr B slash[7] = {1,2,2,4,8,8,16}, question[7] = {14,17,1,2,4,0,4};
        static constexpr B underscore[7] = {0,0,0,0,0,0,31};
        if (c >= 'A' && c <= 'Z') return letters[c - 'A'];
        if (c >= '0' && c <= '9') return digits[c - '0'];
        switch (c) {
            case ' ': return blank; case '-': return dash; case '.': return dot;
            case ':': return colon; case '/': return slash; case '_': return underscore;
            default: return question;
        }
    }
    void text(const char* value, I length, I x, I top, I scale) noexcept {
        for (I n = 0; n < length; ++n) {
            const B* rows = glyph(value[n]);
            for (I row = 0; row < 7; ++row) {
                for (I column = 0; column < 5;) {
                    if (!(rows[row] & (1u << (4 - column)))) { ++column; continue; }
                    const I first = column++;
                    while (column < 5 && (rows[row] & (1u << (4 - column)))) ++column;
                    // Merge adjacent lit pixels into one scissored clear.
                    rectangle(x + (n * 6 + first) * scale, top + row * scale,
                        (column - first) * scale, scale);
                }
            }
        }
    }
public:
    explicit PlayerLoadingPresenter50DU(GameHost50ER& host) noexcept
        : host_(host), thread_(std::this_thread::get_id()) {}
    PlayerLoadingPresenter50DU(const PlayerLoadingPresenter50DU&) = delete;
    PlayerLoadingPresenter50DU& operator=(const PlayerLoadingPresenter50DU&) = delete;

    bool draw(const char* stage, bool closing = false) noexcept {
        if (thread_ != std::this_thread::get_id() || !host_.created() ||
            !host_.hasWindow() || !host_.current()) return false;
        char label[97]{};
        if (!stage || !*stage) stage = "Please wait";
        I length = 0;
        for (; length < 96 && stage[length]; ++length) {
            unsigned char c = static_cast<unsigned char>(stage[length]);
            if (c >= 'a' && c <= 'z') c = static_cast<unsigned char>(c - 'a' + 'A');
            label[length] = c >= 32 && c <= 126 ? static_cast<char>(c) : ' ';
        }
        char title[160]{};
        std::snprintf(title, sizeof title, "Racer Revenge | %s - %s",
            closing ? "Closing" : "Loading", label);
        GameHostDiagnostic50ER diagnostic{};
        if (!host_.setWindowTitle(title, diagnostic)) return false;
        const auto info = host_.info();
        width_ = info.drawableWidth;
        height_ = info.drawableHeight;
        if (info.minimized || width_ <= 0 || height_ <= 0) return true;
        if (!load() || gl_.getError() != 0) return false;

        Saved saved{};
        gl_.getIntegerv(drawFramebufferBinding, &saved.drawFbo);
        gl_.getFloatv(clearColorValue, saved.clearColor.data());
        gl_.getBooleani_v(colorWriteMask, 0, saved.colorMask.data());
        if (gl_.indexedScissor) {
            gl_.getIntegeri_v(scissorBox, 0, saved.scissor.data());
            saved.scissorEnabled = gl_.isEnabledi(scissorTest, 0);
        } else {
            gl_.getIntegerv(scissorBox, saved.scissor.data());
            saved.scissorEnabled = gl_.isEnabled(scissorTest);
        }
        saved.srgbEnabled = gl_.isEnabled(framebufferSrgb);
        saved.discardEnabled = gl_.isEnabled(rasterizerDiscard);
        if (gl_.getError() != 0) return false; // No state changed yet.
        gl_.bindFramebuffer(drawFramebuffer, 0);
        for (I i = 0; i < gl_.drawBufferCount; ++i) {
            I buffer = 0;
            gl_.getIntegerv(drawBuffer0 + static_cast<U>(i), &buffer);
            saved.defaultDrawBuffers[static_cast<std::size_t>(i)] = static_cast<U>(buffer);
        }
        if (gl_.getError() != 0) {
            gl_.bindFramebuffer(drawFramebuffer, static_cast<U>(saved.drawFbo));
            return false;
        }
        gl_.drawBuffer(back);
        gl_.colorMaski(0, 1, 1, 1, 1);
        gl_.disable(framebufferSrgb);
        gl_.disable(rasterizerDiscard);
        setScissorEnabled(false);
        gl_.clearColor(0.018f, 0.025f, 0.043f, 1.f);
        gl_.clear(colorBufferBit);
        setScissorEnabled(true);

        const I headlineScale = std::max<I>(1, std::min<I>(8, std::min(width_ / 60, height_ / 34)));
        const I stageScale = std::max<I>(1, std::min<I>(3, std::min(width_ / 240, height_ / 100)));
        const I margin = std::min<I>(24, width_ / 8);
        const I columns = std::max<I>(1, std::min<I>(96, (width_ - 2 * margin) / (6 * stageScale)));
        const I lines = std::min<I>(4, (length + columns - 1) / columns);
        const I gap = std::max<I>(12, headlineScale * 2);
        const I blockHeight = 7 * headlineScale + gap + lines * 10 * stageScale + gap + 2 * stageScale;
        const I top = std::max<I>(0, (height_ - blockHeight) / 2);
        gl_.clearColor(0.25f, 0.76f, 1.f, 1.f);
        text(closing ? "CLOSING" : "LOADING", 7, (width_ - 41 * headlineScale) / 2,
            top, headlineScale);
        gl_.clearColor(0.82f, 0.88f, 0.94f, 1.f);
        for (I line = 0; line < lines; ++line) {
            const I count = std::min<I>(columns, length - line * columns);
            text(label + line * columns, count, (width_ - (count * 6 - 1) * stageScale) / 2,
                top + 7 * headlineScale + gap + line * 10 * stageScale, stageScale);
        }
        // Indeterminate activity only, never an estimate of work completed.
        // Animation advances on each caller-driven draw; no timer/event owner.
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - activityStarted_).count();
        const I active = static_cast<I>((elapsed < 0 ? 0 : elapsed / 180) % 5);
        const I indicatorLeft = (width_ - 32 * stageScale) / 2;
        const I indicatorTop = top + 7 * headlineScale + gap + lines * 10 * stageScale + gap;
        for (I segment = 0; segment < 5; ++segment) {
            if (segment == active) gl_.clearColor(0.25f, 0.76f, 1.f, 1.f);
            else gl_.clearColor(0.10f, 0.22f, 0.30f, 1.f);
            rectangle(indicatorLeft + segment * 7 * stageScale, indicatorTop,
                4 * stageScale, 2 * stageScale);
        }
        restore(saved);
        if (gl_.getError() != 0) return false;
        // The host's void SDL swap proves submission, not compositor completion.
        return host_.swapWindow(diagnostic);
    }
};
} // namespace rr::platform
