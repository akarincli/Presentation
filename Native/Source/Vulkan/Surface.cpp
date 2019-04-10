//
// Created by 刘予顺 on 2019-04-08.
//

#include "../Vulkan.h"

AK_PUBLIC uint64_t AK_CALL akCreateDisplaySurface(uintptr_t app, uintptr_t window) {
    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(
            reinterpret_cast<SDL_Window*>(window),
            reinterpret_cast<VulkanApplication*>(app)->GetInstance(),
            &surface
    );
    return reinterpret_cast<uint64_t>(surface);
}

AK_PUBLIC void AK_CALL akDestroyDisplaySurface(uintptr_t app, uint64_t surface) {
    vkDestroySurfaceKHR(
            reinterpret_cast<VulkanApplication*>(app)->GetInstance(),
            reinterpret_cast<VkSurfaceKHR>(surface), nullptr
    );
}
