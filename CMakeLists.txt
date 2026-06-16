cmake_minimum_required(VERSION 3.15)

project(CasioEmuMsvc)

# Chỉ build iOS, không build các plugin
if(IOS)
    add_subdirectory(CasioEmuMsvc)
else()
    # Các platform khác giữ nguyên
    add_subdirectory(CasioEmuMsvc)
    add_subdirectory(PythonPlugin)
    add_subdirectory(Cw2toolsPlugin)
    add_subdirectory(McpPlugin)
    add_subdirectory(discord-rpc)
    add_subdirectory(sentry-native)
endif()
