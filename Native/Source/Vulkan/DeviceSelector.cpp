//
// Created by 刘予顺 on 2019-04-08.
//

#include "../Vulkan.h"
#include <set>
#include <map>
#include <any>
#include <string>
#include <vector>
#include <optional>

namespace {
    enum class FilterNames : int {
        MemoryLowerLimit = 0,
        Texture1DMaxDimension = 0x100,
        Texture2DMaxDimension,
        Texture3DMaxDimension,
        RequireSwapChainAndPresent = 0x200,
        RequireGraphics,
        RequireCompute,
        SurfaceAttachment = 0x300
    };

    enum class MetaNames : int {
        ExtensionSupportDetails,
        SwapChainSupportDetails,
        DeviceQueueSupportDetails
    };

    union FilterContent {
        bool Bool;
        uint32_t UInt;
        uint64_t UInt64;
        double Double;
        float Float;
        uintptr_t Handle;
        constexpr FilterContent(bool val) noexcept : Bool(val) {}
        constexpr FilterContent(uint32_t val) noexcept : UInt(val) {}
        constexpr FilterContent(uint64_t val) noexcept : UInt64(val) {}
        constexpr FilterContent(float val) noexcept : Float(val) {}
        constexpr FilterContent(double val) noexcept : Double(val) {}
        constexpr FilterContent(uintptr_t val) noexcept : Handle(val) {}
    };

    using FiltersT = std::map<FilterNames, FilterContent>;
    using MetaT = std::map<MetaNames, std::any>;
    using ExtensionMap = std::map<std::string, uint32_t>;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct Selector {
        virtual ~Selector() = default;
        virtual void Init(const FiltersT& filters) noexcept = 0;
        virtual bool Filter(VkPhysicalDevice device, MetaT& meta) noexcept = 0;
        auto CheckExtension(MetaT& meta, const char* name) {
            auto& extensions = *std::any_cast<ExtensionMap>(&meta[MetaNames::ExtensionSupportDetails]);
            return extensions.find(name) != extensions.end();
        }
    };

    class SelectorSwapChain : public Selector {
    public:
        void Init(const FiltersT& filters) noexcept override {
            if (filters.find(FilterNames::RequireSwapChainAndPresent)->second.Bool) {
                bypass = false;
                surface = reinterpret_cast<VkSurfaceKHR>(filters.find(FilterNames::SurfaceAttachment)->second.UInt64);
            }
            else {
                bypass = true;
            }
        }

        bool Filter(VkPhysicalDevice device, MetaT& meta) noexcept override {
            if (bypass) {
                return true;
            } else {
                if (CheckExtension(meta, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
                    auto details = QuerySwapChainSupport(device);
                    const auto result = !details.formats.empty() && !details.presentModes.empty();
                    meta.insert_or_assign(MetaNames::SwapChainSupportDetails, std::move(details));
                    return result;
                }
                return false;
            }
        }
    private:
        bool bypass;
        VkSurfaceKHR surface;

        auto GetDeviceSurfaceFormats(VkPhysicalDevice device) {
            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
            std::vector<VkSurfaceFormatKHR> formats(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());
            return formats;
        }

        auto GetDeviceSurfacePresentModes(VkPhysicalDevice device) {
            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
            std::vector<VkPresentModeKHR> presentModes(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.data());
            return presentModes;
        }

        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) {
            SwapChainSupportDetails details;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
            details.formats = GetDeviceSurfaceFormats(device);
            details.presentModes = GetDeviceSurfacePresentModes(device);
            return details;
        }
    };

    class SelectorDeviceQueue : public Selector {
    public:
        void Init(const FiltersT& filters) noexcept override {
            demandPresent = filters.find(FilterNames::RequireSwapChainAndPresent) != filters.end();
            demandGraphics = filters.find(FilterNames::RequireGraphics) != filters.end();
            demandCompute = filters.find(FilterNames::RequireCompute) != filters.end();
            if (demandPresent) {
                surface = reinterpret_cast<VkSurfaceKHR>(filters.find(FilterNames::SurfaceAttachment)->second.UInt64);
            }
            mask = 0;
            if (demandPresent)
                mask |= 0b1;
            if (demandCompute)
                mask |= 0b10;
            if (demandGraphics)
                mask |= 0b100;
        }

        bool Filter(VkPhysicalDevice device, MetaT& meta) noexcept override {
            const auto queueFamilies = GetDeviceQueueFamilyProperties(device);
            meta.insert_or_assign(MetaNames::DeviceQueueSupportDetails, queueFamilies);
            int i = 0;
            uint8_t rmask = 0;
            for (const auto& queueFamily : queueFamilies) {
                if (queueFamily.queueCount > 0) {
                    if (demandCompute && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                        rmask |= 0b10;
                    }
                    if (demandGraphics && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                        rmask |= 0b100;
                    }
                    if (demandPresent) {
                        VkBool32 presentSupport = false;
                        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
                        if (presentSupport) {
                            rmask |= 0b1;
                        }
                    }
                }

                if (rmask == mask) {
                    return true;
                }
                i++;
            }
            return false;
        }
    private:
        std::vector<VkQueueFamilyProperties> GetDeviceQueueFamilyProperties(VkPhysicalDevice device) {
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
            return queueFamilies;
        }

        bool demandPresent, demandGraphics, demandCompute;
        VkSurfaceKHR surface;
        uint8_t mask;
    };

    class SelectorExtensionCapability : public Selector {
    public:
        void Init(const FiltersT& filters) noexcept override {}

        bool Filter(VkPhysicalDevice device, MetaT& meta) noexcept override {
            ExtensionMap extensions;
            for (auto&& x : GetDeviceExtensionProperties((device)))
                extensions.insert_or_assign(x.extensionName, x.specVersion);
            meta.insert_or_assign(MetaNames::ExtensionSupportDetails, extensions);
            return true;
        }
    private:
        std::vector<VkExtensionProperties> GetDeviceExtensionProperties(VkPhysicalDevice device) {
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
            return availableExtensions;
        }
    };

    class SelectorHardwareProperty : public Selector {
    public:
        void Init(const FiltersT& filters) noexcept override {

        }

        bool Filter(VkPhysicalDevice device, MetaT& meta) noexcept override {
            return true;
        }
    };

    class DeviceSelector {
        using FilterArray = std::vector<std::unique_ptr<Selector>>;
    public:
        DeviceSelector(const VulkanApplication& application, const FiltersT& filters) {
            FilterArray selectors = GetFilters();
            InitFilters(filters, selectors);
            MetaT meta {};
            for (const auto& device : ListDevices(application)) {
                meta.clear();
                if (ApplyFilters(device, selectors, meta)) {
                    compatableDevices.emplace_back(device, meta);
                }
            }
            if (compatableDevices.empty())
                throw std::runtime_error("failed to find a suitable GPU!");
        }

    private:
        std::vector<std::pair<VkPhysicalDevice, MetaT>> compatableDevices;

        void InitFilters(const FiltersT& filters, const FilterArray & selectors) const {
            for (auto&& x : selectors) {
                x->Init(filters);
            }
        }

        bool ApplyFilters(VkPhysicalDevice device, const FilterArray& filters, MetaT& meta) {
            for (auto&& x : filters)
                if (!x->Filter(device, meta))
                    return false;
            return true;
        }

        std::vector<VkPhysicalDevice> ListDevices(const VulkanApplication& application) {
            auto instance = application.GetInstance();
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
            return devices;
        }

        FilterArray GetFilters() const {
            FilterArray array;
            array.push_back(std::move(std::make_unique<SelectorExtensionCapability>()));
            array.push_back(std::move(std::make_unique<SelectorHardwareProperty>()));
            array.push_back(std::move(std::make_unique<SelectorSwapChain>()));
            array.push_back(std::move(std::make_unique<SelectorDeviceQueue>()));
            return array;
        }
    };
}

AK_PUBLIC uintptr_t AK_CALL akCreateDeviceSelectorFilter() {
    return reinterpret_cast<uintptr_t>(new FiltersT());
}

AK_PUBLIC void AK_CALL akDeviceSelectorFilterSetBool(uintptr_t handle, int name, bool value) {
    reinterpret_cast<FiltersT*>(handle)->insert_or_assign(static_cast<FilterNames>(name), value);
}

AK_PUBLIC void AK_CALL akDeviceSelectorFilterSetUInt(uintptr_t handle, int name, uint32_t value) {
    reinterpret_cast<FiltersT*>(handle)->insert_or_assign(static_cast<FilterNames>(name), value);
}

AK_PUBLIC void AK_CALL akDeviceSelectorFilterSetUInt64(uintptr_t handle, int name, uint64_t value) {
    reinterpret_cast<FiltersT*>(handle)->insert_or_assign(static_cast<FilterNames>(name), value);
}

AK_PUBLIC void AK_CALL akDeviceSelectorFilterSetDouble(uintptr_t handle, int name, double value) {
    reinterpret_cast<FiltersT*>(handle)->insert_or_assign(static_cast<FilterNames>(name), value);
}

AK_PUBLIC void AK_CALL akDeviceSelectorFilterSetHandle(uintptr_t handle, int name, uintptr_t value) {
    reinterpret_cast<FiltersT*>(handle)->insert_or_assign(static_cast<FilterNames>(name), value);
}

AK_PUBLIC bool AK_CALL akDeviceSelectorFilterGetBool(uintptr_t handle, int name) {
    return reinterpret_cast<FiltersT*>(handle)->find(static_cast<FilterNames>(name))->second.Bool;
}

AK_PUBLIC uint32_t AK_CALL akDeviceSelectorFilterGetUInt(uintptr_t handle, int name) {
    return reinterpret_cast<FiltersT*>(handle)->find(static_cast<FilterNames>(name))->second.UInt;
}

AK_PUBLIC uint64_t AK_CALL akDeviceSelectorFilterGetUInt64(uintptr_t handle, int name) {
    return reinterpret_cast<FiltersT*>(handle)->find(static_cast<FilterNames>(name))->second.UInt64;
}

AK_PUBLIC double AK_CALL akDeviceSelectorFilterGetDouble(uintptr_t handle, int name) {
    return reinterpret_cast<FiltersT*>(handle)->find(static_cast<FilterNames>(name))->second.Double;
}

AK_PUBLIC uintptr_t AK_CALL akDeviceSelectorFilterGetHandle(uintptr_t handle, int name) {
    return reinterpret_cast<FiltersT*>(handle)->find(static_cast<FilterNames>(name))->second.Handle;
}

AK_PUBLIC void AK_CALL akDestroyDeviceSelectorFilter(uintptr_t handle) {
    delete reinterpret_cast<FiltersT*>(handle);
}

AK_PUBLIC uintptr_t AK_CALL akFilterDevices(uintptr_t appHandle, uintptr_t filter) {
    return reinterpret_cast<uintptr_t>(
            new DeviceSelector(
                    *reinterpret_cast<VulkanApplication*>(appHandle),
                    *reinterpret_cast<FiltersT*>(filter)
            ));
}

AK_PUBLIC uintptr_t AK_CALL akOpenDevice(uintptr_t filter){

}

AK_PUBLIC void AK_CALL akReleaseDeviceFilterResults(uintptr_t handle) {
    delete reinterpret_cast<DeviceSelector*>(handle);
}
