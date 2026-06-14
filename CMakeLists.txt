cmake_minimum_required(VERSION 3.15)
project(CasioEmuIOS)

# 1. Thiết lập môi trường iOS
set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_ARCHITECTURES arm64)

# 2. Nạp các thư viện phụ thuộc (Submodules)
# Nạp SDL
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# Nạp asmjit (đường dẫn đúng tới thư mục third_party)
add_subdirectory(CasioEmuMsvc/third_party/asmjit EXCLUDE_FROM_ALL)

# 3. Thu thập toàn bộ file code .cpp và .c từ thư mục chính
file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CasioEmuMsvc/*.c
)

# In ra danh sách file để kiểm tra (giúp debug nếu thiếu file)
message(STATUS "List of source files found: ${APP_SOURCES}")

# 4. Tạo file chạy app iOS
add_executable(CasioEmuApp ${APP_SOURCES})

# 5. Cấu hình liên kết thư viện
# Liên kết SDL
if(TARGET SDL2::SDL2)
    target_link_libraries(CasioEmuApp PRIVATE SDL2::SDL2 SDL2::SDL2main)
    target_include_directories(CasioEmuApp PRIVATE ${SDL2_SOURCE_DIR}/include)
endif()

# Liên kết asmjit
if(TARGET asmjit::asmjit)
    target_link_libraries(CasioEmuApp PRIVATE asmjit::asmjit)
endif()
