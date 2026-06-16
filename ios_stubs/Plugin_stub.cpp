// Plugin system stub for iOS
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE

#include <stdio.h>

class PluginManager {
public:
    static PluginManager& GetInstance() {
        static PluginManager instance;
        return instance;
    }
    
    void Initialize(void) {
        printf("Plugin system disabled on iOS\n");
    }
    
    void Shutdown(void) {
        // No-op
    }
    
    void LoadPlugins(const char* path) {
        // No-op on iOS
    }
    
private:
    PluginManager() = default;
};

// C API stubs
extern "C" {
    void Plugin_Init(void) {
        PluginManager::GetInstance().Initialize();
    }
    
    void Plugin_Shutdown(void) {
        PluginManager::GetInstance().Shutdown();
    }
    
    void Plugin_LoadFromDir(const char* dir) {
        PluginManager::GetInstance().LoadPlugins(dir);
    }
}

#endif
#endif
