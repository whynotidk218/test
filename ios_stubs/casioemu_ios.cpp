// ios_stubs/casioemu_ios.cpp
// Thay thế casioemu.cpp trên iOS:
// - Không có int main() (iOS dùng UIApplicationMain từ ios/main.m)
// - Không include sdl_win32_extra.h hay Windows headers
// - Cung cấp hàm casioemu_ios_start() để AppDelegate gọi

#include "Config.hpp"
#include "Ui.hpp"
#include "Emulator.hpp"
#include "Localization.h"
#include "Logger.hpp"
#include <Gui.h>
#include <ThemeManager.h>
#include "DiscordRPC.h"
#include "Gui/TouchMouseTranslator.h"
#include "imgui_impl_sdl2.h"

#include <SDL.h>
#include <SDL_image.h>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>

using namespace casioemu;

SDL_Surface* background;
SDL_Texture* bg_txt;
bool low_perf_ext = false;

static bool IsPointInImGuiWindow(float x, float y) {
    ImGuiContext* ctx = ImGui::GetCurrentContext();
    if (!ctx) return false;
    ImGuiContext& g = *ctx;
    ImVec2 p(x, y);
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || ImGui::IsAnyItemActive()) return true;
    for (int i = g.Windows.Size - 1; i >= 0; --i) {
        ImGuiWindow* window = g.Windows[i];
        if (!window || !window->WasActive || window->Hidden) continue;
        if ((window->Flags & ImGuiWindowFlags_NoMouseInputs) ||
            (window->Flags & ImGuiWindowFlags_NoTitleBar)) continue;
        if (window->OuterRectClipped.Contains(p)) return true;
    }
    return false;
}

// Điểm vào cho iOS - AppDelegate gọi hàm này trên background thread
// model_path: đường dẫn tới ROM model (ví dụ Documents/fx991vn.bin)
extern "C" void casioemu_ios_start(const char* model_path_cstr) {
    std::string model_path = model_path_cstr ? model_path_cstr : "";

    g_local.Load();
    ThemeManager::Instance().LoadSettings();
    DiscordRPC::Init();
    DiscordRPC::UpdatePresence("");

    std::map<std::string, std::string> argv_map;
    argv_map["model"] = model_path;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_Log("IMG_Init failed: %s", IMG_GetError());
        SDL_Quit();
        return;
    }

    if (argv_map["model"].empty()) {
        SDL_Log("[iOS] No model path provided. Exiting.");
        SDL_Quit();
        return;
    }

    bool no_dbg = true; // Không debug UI trên iOS
    low_perf_ext = false;

    // Tắt mouse-touch translation gây double events trên iOS
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");

    Emulator emulator(argv_map);

    DiscordRPC::UpdatePresence(emulator.ModelDefinition.model_name);

    TouchMouseTranslator touchTranslator(
        SDL_GetWindowID(emulator.window),
        [&](const SDL_Event& translatedEvent, TouchTarget target) {
            if (target == TouchTarget::ImGui) {
                ImGui_ImplSDL2_ProcessEvent(&translatedEvent);
                return;
            }
            emulator.UIEvent(translatedEvent);
        },
        [&](float x, float y) -> bool {
            return IsPointInImGuiWindow(x, y);
        });

    bool guiCreated = false;
    auto frame_event = SDL_RegisterEvents(1);
    bool busy = false;
    bool running = true;

    std::thread t3([&]() {
        SDL_Event se{};
        se.type = frame_event;
        se.user.windowID = SDL_GetWindowID(emulator.window);
        while (running) {
            if (!busy)
                SDL_PushEvent(&se);
            SDL_Delay(40); // ~25fps, tiết kiệm pin iOS
        }
    });
    t3.detach();

    bool done = false;
    while (!done) {
        SDL_Event event;
        if (!SDL_WaitEvent(&event)) continue;

        if (event.type == SDL_QUIT) {
            done = true;
        } else if (event.type == SDL_FINGERDOWN ||
                   event.type == SDL_FINGERUP ||
                   event.type == SDL_FINGERMOTION) {
            touchTranslator.HandleEvent(event,
                /* windowW */ 0, /* windowH */ 0);
        } else if (event.type == frame_event) {
            busy = true;
            emulator.Tick();
            busy = false;
        } else {
            emulator.UIEvent(event);
        }
    }

    running = false;

    DiscordRPC::Shutdown();
    IMG_Quit();
    SDL_Quit();
}
