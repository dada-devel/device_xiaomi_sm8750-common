/*
 * Vibrator HAL shim - re-registers Xiaomi's vibratorfeature as "default"
 * so the Android framework can find it.
 */

#define LOG_TAG "vibrator_shim"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <aidl/android/hardware/vibrator/IVibrator.h>

using aidl::android::hardware::vibrator::IVibrator;

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);

    const std::string xiaomiInstance = std::string(IVibrator::descriptor) + "/vibratorfeature";
    const std::string defaultInstance = std::string(IVibrator::descriptor) + "/default";

    // Wait for Xiaomi's vibratorfeature service
    LOG(INFO) << "Waiting for " << xiaomiInstance;
    AServiceManager_waitForService(xiaomiInstance.c_str());

    // Get the binder for vibratorfeature
    ndk::SpAIBinder binder(AServiceManager_checkService(xiaomiInstance.c_str()));
    if (binder.get() == nullptr) {
        LOG(ERROR) << "Failed to get " << xiaomiInstance;
        return 1;
    }

    // Register it as "default"
    binder_status_t status = AServiceManager_addService(binder.get(), defaultInstance.c_str());
    if (status != STATUS_OK) {
        LOG(ERROR) << "Failed to register as " << defaultInstance << ": " << status;
        return 1;
    }

    LOG(INFO) << "Successfully registered " << xiaomiInstance << " as " << defaultInstance;

    ABinderProcess_joinThreadPool();
    return 0;
}
