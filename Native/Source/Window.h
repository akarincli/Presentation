#pragma once

#include <cstddef>
#include <cstdint>
#include "Config.h"

AK_PUBLIC uintptr_t AK_CALL akCreateWindow(
        const char* caption,
        int32_t display, int32_t positionX, int32_t positionY, int32_t width, int32_t height
        ) noexcept;

AK_PUBLIC void AK_CALL akShowWindow(uintptr_t handle) noexcept;

AK_PUBLIC void AK_CALL akHideWindow(uintptr_t handle) noexcept;

AK_PUBLIC void AK_CALL akMinimizeWindow(uintptr_t handle) noexcept;

AK_PUBLIC void AK_CALL akMaximizeWindow(uintptr_t handle) noexcept;

AK_PUBLIC void AK_CALL akDestroyWindow(uintptr_t handle) noexcept;
