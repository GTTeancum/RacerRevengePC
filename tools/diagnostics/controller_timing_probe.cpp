// Standalone controller timing probe (no game code). Opens the first SDL game
// controller with the same SDL2.dll the game ships, then times the calls the
// game makes on a controller: SDL_PumpEvents, button/axis reads and
// SDL_GameControllerRumble (start, renew, stop) - the same arguments the game
// uses (100 ms duration, low motor = large*257, high motor = 0 or 65535).
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <chrono>
#include <cstdio>

using Clock = std::chrono::steady_clock;
static double ms(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration<double, std::milli>(b - a).count();
}

int main() {
    SDL_SetMainReady();
    auto t0 = Clock::now();
    if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0) {
        std::printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    std::printf("SDL_Init ms=%.1f\n", ms(t0, Clock::now()));
    SDL_version v; SDL_GetVersion(&v);
    std::printf("SDL %d.%d.%d joysticks=%d\n", v.major, v.minor, v.patch, SDL_NumJoysticks());
    SDL_GameController* pad = nullptr;
    for (int i = 0; i < SDL_NumJoysticks() && !pad; ++i) {
        if (!SDL_IsGameController(i)) continue;
        t0 = Clock::now();
        pad = SDL_GameControllerOpen(i);
        std::printf("open index=%d ms=%.1f name=%s type=%d\n", i, ms(t0, Clock::now()),
            pad ? SDL_GameControllerName(pad) : "(failed)",
            pad ? static_cast<int>(SDL_GameControllerGetType(pad)) : -1);
    }
    if (!pad) { std::printf("no game controller\n"); SDL_Quit(); return 2; }
    std::printf("has_rumble=%d\n", SDL_GameControllerHasRumble(pad));

    // Idle reads, as every game frame does.
    double worstPump = 0, worstRead = 0;
    for (int frame = 0; frame < 120; ++frame) {
        t0 = Clock::now(); SDL_PumpEvents(); const auto t1 = Clock::now();
        for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; ++b) (void)SDL_GameControllerGetButton(pad, static_cast<SDL_GameControllerButton>(b));
        for (int a = 0; a < SDL_CONTROLLER_AXIS_MAX; ++a) (void)SDL_GameControllerGetAxis(pad, static_cast<SDL_GameControllerAxis>(a));
        const auto t2 = Clock::now();
        if (ms(t0, t1) > worstPump) worstPump = ms(t0, t1);
        if (ms(t1, t2) > worstRead) worstRead = ms(t1, t2);
        SDL_Delay(16);
    }
    std::printf("idle 120 frames: worst_pump_ms=%.2f worst_read_ms=%.2f\n", worstPump, worstRead);

    // Rumble sequence matching the game's transport: first start, renewals
    // every frame, stop, then start again (as after closing Pause).
    auto rumble = [&](const char* tag, Uint16 low, Uint16 high, Uint32 duration) {
        const auto a = Clock::now();
        const int r = SDL_GameControllerRumble(pad, low, high, duration);
        const auto b = Clock::now();
        SDL_PumpEvents();
        const auto c = Clock::now();
        std::printf("%s result=%d rumble_ms=%.2f next_pump_ms=%.2f\n", tag, r, ms(a, b), ms(b, c));
    };
    rumble("first_start", 120 * 257, 0, 100);
    for (int i = 0; i < 5; ++i) { SDL_Delay(16); rumble("renew", 120 * 257, 0, 100); }
    SDL_Delay(16); rumble("stop", 0, 0, 0);
    SDL_Delay(500);
    rumble("restart_after_stop", 120 * 257, 0, 100);
    SDL_Delay(16); rumble("renew", 120 * 257, 0, 100);
    SDL_Delay(16); rumble("stop", 0, 0, 0);
    SDL_GameControllerClose(pad);
    SDL_Quit();
    return 0;
}
