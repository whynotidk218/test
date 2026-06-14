// iOS stub: Plugin system không dùng trên iOS
#include <string>

// casioemu.cpp gọi LoadPlugins() khi __ANDROID__ hoặc _WIN32
// Trên iOS không có plugin nên stub rỗng
namespace casioemu {
    void LoadPlugins() {}
}
