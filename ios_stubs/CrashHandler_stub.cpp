// iOS stub: CrashHandler (Windows-only) không dùng trên iOS
// File rỗng - chỉ cần để linker không tìm symbol
// Crash handler stub for iOS
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE

#include <stdio.h>

void InitCrashHandler(void) {
    // No-op on iOS
}

void ReportCrash(const char* message) {
    printf("Crash: %s\n", message);
}

void SetCrashHandlerPath(const char* path) {
    // No-op on iOS
}

#endif
#endif
