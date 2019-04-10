#include "../Vulkan.h"

#include <array>
#include <vector>
#include <iostream>

namespace {
    VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    constexpr std::array<const char*, 1> ValidationLayers = {
            "VK_LAYER_LUNARG_standard_validation"
    };

    bool CheckValidationLayerSupport() noexcept {
        if constexpr(enableValidationLayers) {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : ValidationLayers) {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers) {
                    if (strcmp(layerName, layerProperties.layerName)==0) {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) {
                    return false;
                }
            }
        }
        return true;
    }

    class Validation {
    public:
        static void FillInstanceCreateOption(VkInstanceCreateInfo& createInfo) noexcept{
            if constexpr(enableValidationLayers) {
                createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
                createInfo.ppEnabledLayerNames = ValidationLayers.data();
            }
            else {
                createInfo.enabledLayerCount = 0;
            }
        }

        static void CheckValidationLayer(){
            if (!CheckValidationLayerSupport())
                throw std::runtime_error("validation layers requested, but not available!");
        }
    };
}

void VulkanApplication::Setup() {
    CreateInstance();
    SetupDebugCallback();
}

void VulkanApplication::CreateInstance() {
    SDL_Vulkan_LoadLibrary(nullptr);
    Validation::CheckValidationLayer();
    const auto appInfo = GetAppInfo();
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    const auto extensions = GetInstanceRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    Validation::FillInstanceCreateOption(createInfo);
    if (vkCreateInstance(&createInfo, nullptr, &instance)!=VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void VulkanApplication::SetupDebugCallback() {
    if constexpr (enableValidationLayers) {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;

        if (CreateDebugUtilsMessengerEXT(&createInfo, nullptr)!=VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug callback!");
        }
    }
}

std::vector<const char*> VulkanApplication::GetInstanceRequiredExtensions() {
    unsigned int count;
    if (!SDL_Vulkan_GetInstanceExtensions(nullptr, &count, nullptr)) VulkanHandleSDLError();

    std::vector<const char*> extensions = {
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME // Sample additional extension
    };

    size_t additional_extension_count = extensions.size();
    extensions.resize(additional_extension_count+count);

    if (!SDL_Vulkan_GetInstanceExtensions(nullptr, &count,
            extensions.data()+additional_extension_count))
        VulkanHandleSDLError();

    return extensions;
}

VkApplicationInfo VulkanApplication::GetAppInfo() noexcept {
    VkApplicationInfo appInfo = {};
    const auto version = VK_MAKE_VERSION(VersionMajor, VersionMinor, VersionRevision);
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Akarin Application";
    appInfo.applicationVersion = version;
    appInfo.pEngineName = "Akarin Native";
    appInfo.engineVersion = version;
    appInfo.apiVersion = VK_API_VERSION_1_0;
    return appInfo;
}

VkResult VulkanApplication::CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator) noexcept {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
            "vkCreateDebugUtilsMessengerEXT");
    if (func!=nullptr) {
        return func(instance, pCreateInfo, pAllocator, &callback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanApplication::DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator) noexcept {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
            "vkDestroyDebugUtilsMessengerEXT");
    if (func!=nullptr) {
        func(instance, callback, pAllocator);
    }
}

void VulkanApplication::TearDown() {
    if constexpr(enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(nullptr);
    }
    vkDestroyInstance(instance, nullptr);

}


AK_PUBLIC uintptr_t AK_CALL akAppInit() noexcept{
    auto handle = new VulkanApplication();
    handle->Init();
    handle->Setup();
    return reinterpret_cast<uintptr_t>(handle);
}

AK_PUBLIC void AK_CALL akAppControlHandOver(uintptr_t handle) noexcept {
    auto hdc = reinterpret_cast<VulkanApplication*>(handle);
    hdc->ControlHandOver();
}

AK_PUBLIC void AK_CALL akAppStop(uintptr_t handle) noexcept {
    auto hdc = reinterpret_cast<VulkanApplication*>(handle);
    hdc->Stop();
}

AK_PUBLIC void AK_CALL akAppFinalize(uintptr_t handle) noexcept {
    auto hdc = reinterpret_cast<VulkanApplication*>(handle);
    hdc->TearDown();
    hdc->Finalize();
    delete hdc;
}
