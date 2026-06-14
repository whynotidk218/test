cmake_minimum_required(VERSION 3.15)
project(CasioEmuIOS CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── SDL2 từ install prefix ────────────────────────────────────────
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# ── Thu thập source files ─────────────────────────────────────────
file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/third_party/miniz/*.c
)

# ── Loại bỏ file Windows/Desktop/JIT-only ────────────────────────
list(FILTER APP_SOURCES EXCLUDE REGEX
    ".*(sdl_win32_extra\
|Ext/DiscordRPC\
|CrashHandler\
|Web/core_api\
|Web/web_main\
|Plugin/PluginMan\
|Plugin/PluginApi_Impl\
|Rop/RopCompilerUI\
|legacy/trash\
|JitU8\
).*\\.(cpp|c)$"
)

# ── Thêm stub iOS (thay thế các file bị loại bỏ) ─────────────────
# Stub DiscordRPC, CrashHandler, Plugin do casioemu.cpp include trực tiếp
list(APPEND APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ios_stubs/DiscordRPC_stub.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ios_stubs/CrashHandler_stub.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ios_stubs/Plugin_stub.cpp
)

message(STATUS "Source files (${CMAKE_SYSTEM_NAME}):")
foreach(f ${APP_SOURCES})
    message(STATUS "  ${f}")
endforeach()

# ── Tạo executable ────────────────────────────────────────────────
add_executable(CasioEmuApp ${APP_SOURCES})

# ── Include paths ─────────────────────────────────────────────────
target_include_directories(CasioEmuApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Chipset
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Peripheral
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Ext
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Gui/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/StartupUi
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/third_party/miniz
    ${CMAKE_CURRENT_SOURCE_DIR}/ios_stubs
    ${SDL2_INCLUDE_DIRS}
)

# ── Compile definitions ───────────────────────────────────────────
target_compile_definitions(CasioEmuApp PRIVATE
    CASIO_IOS=1       # tắt code #ifdef _WIN32 / __ANDROID__
    NO_JIT=1          # không dùng asmjit (iOS cấm JIT)
    NO_DISCORD=1      # dùng stub thay thế
    NO_SENTRY=1       # không dùng sentry
    NO_PLUGINS=1      # không load plugin .dll/.so
    SDL_MAIN_HANDLED  # tránh SDL override main()
)

# ── Liên kết thư viện ─────────────────────────────────────────────
target_link_libraries(CasioEmuApp PRIVATE
    SDL2::SDL2-static
    SDL2_image::SDL2_image-static
)
