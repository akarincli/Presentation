#pragma once

#include <stdexcept>
#include "Config.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include <vulkan/vulkan.h>
#include "Application.h"

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

inline void VulkanHandleSDLError() {
    throw std::runtime_error(SDL_GetError());
}

class VulkanApplication : public Application {
public:
    void Setup();
    void TearDown();
    VkInstance GetInstance() const noexcept { return instance; }
private:
    void CreateInstance();
    void SetupDebugCallback();
    std::vector<const char*> GetInstanceRequiredExtensions();
    VkApplicationInfo GetAppInfo() noexcept;
    VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator) noexcept;
    void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator) noexcept;
    VkInstance instance;
    VkDebugUtilsMessengerEXT callback;
};

// NOTE VkSurface is a 64 type
