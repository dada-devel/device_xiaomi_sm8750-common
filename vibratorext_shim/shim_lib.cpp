#define LOG_TAG "VibratorExtShim"

#include <android/binder_ibinder.h>
#include <android/binder_manager.h>
#include <dlfcn.h>
#include <log/log.h>

// Original function pointer
static binder_status_t (*real_AIBinder_setExtension)(AIBinder*, AIBinder*) = nullptr;

extern "C" binder_status_t AIBinder_setExtension(AIBinder* binder, AIBinder* ext) {
    // Get the real function on first call
    if (!real_AIBinder_setExtension) {
        real_AIBinder_setExtension = reinterpret_cast<decltype(real_AIBinder_setExtension)>(
            dlsym(RTLD_NEXT, "AIBinder_setExtension"));
        if (!real_AIBinder_setExtension) {
            ALOGE("Failed to find real AIBinder_setExtension");
            return STATUS_FAILED_TRANSACTION;
        }
    }

    // Call the real function
    binder_status_t status = real_AIBinder_setExtension(binder, ext);
    ALOGI("AIBinder_setExtension called, status=%d", status);

    if (status == STATUS_OK && ext != nullptr) {
        // Also register the extension as a named service
        const char* extServiceName = "vendor.hardware.vibratorfeature.IVibratorExt/default";
        
        // Increment strong reference before registering
        AIBinder_incStrong(ext);
        
        binder_status_t addStatus = AServiceManager_addService(ext, extServiceName);
        if (addStatus == STATUS_OK) {
            ALOGI("Successfully registered IVibratorExt as '%s'", extServiceName);
        } else {
            ALOGE("Failed to register IVibratorExt: %d", addStatus);
            AIBinder_decStrong(ext);
        }
    }

    return status;
}
