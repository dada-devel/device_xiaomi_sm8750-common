/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aidl/android/hardware/power/BnPower.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>

#define Touch_Doubletap_Mode 14

#define TOUCH_DEV_PATH "/dev/xiaomi-touch"
#define TOUCH_ID 0
#define BUF_SIZE 1032

// Ioctl commands
#define TOUCH_IOC_SETMODE    0xc4085400
#define TOUCH_IOC_SELECT_ID  0x40005403

namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {

using ::aidl::android::hardware::power::Mode;

bool isDeviceSpecificModeSupported(Mode type, bool* _aidl_return) {
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE:
            *_aidl_return = true;
            return true;
        default:
            return false;
    }
}

bool setDeviceSpecificMode(Mode type, bool enabled) {
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE: {
            int fd = open(TOUCH_DEV_PATH, O_RDWR);
            if (fd < 0) return false;
            
            // Select touch ID
            ioctl(fd, TOUCH_IOC_SELECT_ID, TOUCH_ID);
            
            // Buffer format for sm8750:
            // byte 0: touch_id
            // byte 1: cmd (0 = set value)
            // bytes 2-3: mode (16-bit)
            // bytes 8-11: value (32-bit)
            uint8_t buf[BUF_SIZE] = {0};
            buf[0] = TOUCH_ID;
            buf[1] = 0;  // cmd = 0 for SET_CUR_VALUE
            *reinterpret_cast<uint16_t*>(&buf[2]) = Touch_Doubletap_Mode;
            *reinterpret_cast<uint32_t*>(&buf[8]) = enabled ? 1 : 0;
            
            ioctl(fd, TOUCH_IOC_SETMODE, buf);
            
            close(fd);
            return true;
        }
        default:
            return false;
    }
}

}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace android
}  // namespace aidl
