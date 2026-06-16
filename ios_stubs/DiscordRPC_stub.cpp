// Discord RPC stub for iOS
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE

// Stub implementation
void Discord_Initialize(const char* applicationId, void* handlers, int autoRegister, const char* optionalSteamId) {
    // No-op on iOS
}

void Discord_Shutdown(void) {
    // No-op on iOS
}

void Discord_RunCallbacks(void) {
    // No-op on iOS
}

void Discord_UpdatePresence(void* presence) {
    // No-op on iOS
}

void Discord_ClearPresence(void) {
    // No-op on iOS
}

void Discord_Respond(const char* userId, int reply) {
    // No-op on iOS
}

#endif
#endif
