cmake_minimum_required(VERSION 3.15)

project(CasioEmuIOS C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASMJIT_STATIC ON)

add_compile_definitions(
    ASMJIT_NO_JIT
    ASMJIT_NO_COMPILER
)

# SDL2_image phải biết dùng SDL2 trong source tree
set(SDLIMAGE_VENDORED ON)
set(SDLIMAGE_BACKEND_IMAGEIO OFF)
set(SDLIMAGE_BACKEND_STB ON)

# SDL trước, SDL_image sau
add_subdirectory(SDL2)
add_subdirectory(SDL2_image)
get_property(allTargets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
message(STATUS "Targets: ${allTargets}")

# ------------------------------------------------------------
# Sources
# ------------------------------------------------------------

file(GLOB_RECURSE APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/third_party/miniz/*.c"
)

list(FILTER APP_SOURCES EXCLUDE REGEX
".*(sdl_win32_extra|Ext/DiscordRPC|CrashHandler|Web/core_api|Web/web_main|Plugin/PluginMan|Plugin/PluginApi_Impl|Rop/RopCompilerUI|legacy/trash|JitU8).*\\.(cpp|c)$"
)

list(APPEND APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/ios_stubs/DiscordRPC_stub.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ios_stubs/CrashHandler_stub.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ios_stubs/Plugin_stub.cpp"
)

# iOS entry files
if(APPLE)
    list(APPEND APP_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/ios/main.m"
        "${CMAKE_CURRENT_SOURCE_DIR}/ios/AppDelegate.m"
    )
endif()

# ------------------------------------------------------------
# App
# ------------------------------------------------------------

if(APPLE)
    add_executable(CasioEmuApp MACOSX_BUNDLE ${APP_SOURCES})
else()
    add_executable(CasioEmuApp ${APP_SOURCES})
endif()

# ------------------------------------------------------------
# Includes
# ------------------------------------------------------------

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
)

# ------------------------------------------------------------
# Defines
# ------------------------------------------------------------

target_compile_definitions(CasioEmuApp PRIVATE
    CASIO_IOS=1
    NO_JIT=1
    NO_DISCORD=1
    NO_SENTRY=1
    NO_PLUGINS=1
    SDL_MAIN_HANDLED
)

# ------------------------------------------------------------
# Link SDL
# ------------------------------------------------------------

target_link_libraries(CasioEmuApp PRIVATE
    SDL2::SDL2-static
    SDL2_image
)

# ------------------------------------------------------------
# iOS bundle
# ------------------------------------------------------------

if(APPLE)
    set_target_properties(CasioEmuApp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST
        "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    )
endif()
