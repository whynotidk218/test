cmake_minimum_required(VERSION 3.15)

project(CasioEmuIOS C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASMJIT_STATIC ON)

add_compile_definitions(
    ASMJIT_NO_JIT
    ASMJIT_NO_COMPILER
)

# SDL2_image config
set(SDLIMAGE_VENDORED ON)
set(SDLIMAGE_BACKEND_IMAGEIO OFF)
set(SDLIMAGE_BACKEND_STB ON)

# Force static build
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL2IMAGE_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_STATIC ON  CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# SDL2 và SDL2_image (ở thư mục gốc)
add_subdirectory(../SDL2 SDL2_build)
add_subdirectory(../SDL2_image SDL2_image_build)

get_property(allTargets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
message(STATUS "Targets: ${allTargets}")

# ------------------------------------------------------------
# Sources
# ------------------------------------------------------------

file(GLOB_RECURSE APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/miniz/*.c"
)

# Filter out incompatible files
list(FILTER APP_SOURCES EXCLUDE REGEX
    ".*(sdl_win32_extra|Ext/DiscordRPC|CrashHandler|Web/core_api|Web/web_main|Plugin/PluginMan|Plugin/PluginApi_Impl|Rop/RopCompilerUI|legacy/trash|JitU8|casioemu).*\\.(cpp|c)$"
)

# Add stub files (ở thư mục gốc)
list(APPEND APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../ios_stubs/DiscordRPC_stub.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ios_stubs/CrashHandler_stub.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ios_stubs/Plugin_stub.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ios_stubs/casioemu_ios.cpp"
)

# iOS entry files (ở thư mục gốc)
if(APPLE)
    list(APPEND APP_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/../ios/main.m"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ios/AppDelegate.m"
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Chipset
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Peripheral
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Ext
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StartupUi
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/miniz
    ${CMAKE_CURRENT_SOURCE_DIR}/../ios_stubs
)

# ------------------------------------------------------------
# Defines
# ------------------------------------------------------------

target_compile_definitions(CasioEmuApp PRIVATE
    CASIO_IOS=1
    NO_JIT=1
    NO_DISCORD=1
    DISABLE_SENTRY=1
    NO_PLUGINS=1
    SDL_MAIN_HANDLED
)

# ------------------------------------------------------------
# Link SDL + iOS frameworks
# ------------------------------------------------------------

target_link_libraries(CasioEmuApp PRIVATE
    SDL2::SDL2-static
    SDL2_image::SDL2_image-static
)

if(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "iOS")
    target_link_libraries(CasioEmuApp PRIVATE
        "-framework UIKit"
        "-framework Foundation"
        "-framework CoreGraphics"
        "-framework CoreMotion"
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework GameController"
        "-framework CoreHaptics"
        "-framework Metal"
        "-framework OpenGLES"
        "-framework QuartzCore"
    )
endif()

# ------------------------------------------------------------
# iOS bundle + Code Signing (disabled for SideStore)
# ------------------------------------------------------------

if(APPLE)
    set_target_properties(CasioEmuApp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
        XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    )
endif()
