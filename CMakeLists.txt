cmake_minimum_required(VERSION 3.15)
project(CasioEmuIOS)

# ── Môi trường iOS ────────────────────────────────────────────────
set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0")

# ── SDL (submodule) ───────────────────────────────────────────────
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# ── asmjit: KHÔNG dùng trên iOS (JIT bị Apple chặn) ─────────────
# iOS không cho phép sinh và thực thi code ở runtime (W^X policy)
# => bỏ hoàn toàn asmjit, chỉ dùng interpreter

# ── Thu thập source files ─────────────────────────────────────────
file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/third_party/miniz/*.c
)

# ── Loại bỏ các file Windows/Desktop-only ────────────────────────
list(FILTER APP_SOURCES EXCLUDE REGEX
    ".*(sdl_win32_extra|DiscordRPC|CrashHandler|Web/core_api|Web/web_main|Plugin/PluginMan|Plugin/PluginApi_Impl|Rop/RopCompilerUI|legacy/trash|JitU8).*"
)

message(STATUS "Source files: ${APP_SOURCES}")

# ── Tạo app bundle ────────────────────────────────────────────────
add_executable(CasioEmuApp MACOSX_BUNDLE ${APP_SOURCES})

# ── Include paths ─────────────────────────────────────────────────
target_include_directories(CasioEmuApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Gui/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Chipset
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Peripheral
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/Ext
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/third_party/miniz
)

# ── Compile definitions ───────────────────────────────────────────
target_compile_definitions(CasioEmuApp PRIVATE
    CASIO_IOS=1          # dùng trong code để #ifdef bỏ phần Windows
    NO_JIT=1             # tắt JIT, dùng interpreter
    NO_DISCORD=1
    NO_SENTRY=1
    NO_PLUGINS=1
)

# ── Liên kết SDL2 ─────────────────────────────────────────────────
if(TARGET SDL2::SDL2)
    target_link_libraries(CasioEmuApp PRIVATE SDL2::SDL2 SDL2::SDL2main)
    target_include_directories(CasioEmuApp PRIVATE ${SDL2_SOURCE_DIR}/include)
elseif(TARGET SDL2-static)
    target_link_libraries(CasioEmuApp PRIVATE SDL2-static)
    target_include_directories(CasioEmuApp PRIVATE ${SDL2_SOURCE_DIR}/include)
endif()

# ── Liên kết iOS frameworks ───────────────────────────────────────
target_link_libraries(CasioEmuApp PRIVATE
    "-framework UIKit"
    "-framework Foundation"
    "-framework CoreGraphics"
    "-framework CoreMotion"
    "-framework AVFoundation"
    "-framework AudioToolbox"
    "-framework GameController"
)

# ── Xcode bundle settings ─────────────────────────────────────────
set_target_properties(CasioEmuApp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER        "com.casioemu.ios"
    MACOSX_BUNDLE_BUNDLE_NAME           "CasioEmu"
    MACOSX_BUNDLE_BUNDLE_VERSION        "1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING  "1.0"
    MACOSX_BUNDLE_INFO_PLIST            "${CMAKE_SOURCE_DIR}/Info.plist"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY  ""
    XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED  "NO"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "14.0"
    XCODE_ATTRIBUTE_ENABLE_BITCODE      "NO"
)
