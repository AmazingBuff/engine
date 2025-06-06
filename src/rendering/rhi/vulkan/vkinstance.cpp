//
// Created by AmazingBuff on 2025/5/6.
//

#include "vkinstance.h"
#include "vkadapter.h"
#include "utils/vk_macro.h"
#include "utils/vk_utils.h"
#include "rendering/rhi/exts/vk_exts.h"

AMAZING_NAMESPACE_BEGIN

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data)
{
    std::string type;
    switch (message_type)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        type = "General";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        type = "Validation";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        type = "Performance";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
        type = "Device Address Binding";
        break;
    default:
        break;
    }

    // todo: adjust logger
    switch (message_severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        std::cout << "[Vulkan, Verbose, " << type << "]: " << callback_data->pMessage << std::endl;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        std::cout << "[Vulkan, Info, " << type << "]: " << callback_data->pMessage << std::endl;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        std::cerr << "[Vulkan, Warning, " << type << "]: " << callback_data->pMessage << std::endl;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        std::cerr << "[Vulkan, Error, " << type << "]: " << callback_data->pMessage << std::endl;
        break;
    default:
        return VK_TRUE;
    }

    return VK_FALSE;
}


VKInstance::VKInstance(GPUInstanceCreateInfo const& info) : m_instance(nullptr), m_debug_messenger(nullptr)
{
    VK_CHECK_RESULT(volkInitialize());

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Renderer",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Amazing Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_1,
    };

    Vector<const char*> instance_layers;
    Vector<const char*> instance_extensions(std::size(VK_Instance_Extensions));
    for (size_t i = 0; i < std::size(VK_Instance_Extensions); i++)
        instance_extensions[i] = VK_Instance_Extensions[i];
    Vector<const char*> device_layers;
    Vector<const char*> device_extensions(std::size(VK_Device_Extensions));
    for (size_t i = 0; i < std::size(VK_Device_Extensions); i++)
        device_extensions[i] = VK_Device_Extensions[i];

    if (info.enable_debug_layer)
    {
        instance_layers.emplace_back(VK_Validation_Layer_Name);
        instance_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    if (GPUVulkanInstanceCreateExtensions const* extensions = static_cast<GPUVulkanInstanceCreateExtensions const*>(info.next))
    {
        instance_layers.resize(instance_layers.size() + extensions->layers.size());
        for (size_t i = 0; i < extensions->layers.size(); i++)
            instance_layers[i + instance_layers.size()] = extensions->layers[i];

        instance_extensions.resize(instance_extensions.size() + extensions->instance_extensions.size());
        for (size_t i = 0; i < extensions->instance_extensions.size(); i++)
            instance_extensions[i + instance_extensions.size()] = extensions->instance_extensions[i];

        device_extensions.resize(device_extensions.size() + extensions->device_extensions.size());
        for (size_t i = 0; i < extensions->device_extensions.size(); i++)
            device_extensions[i + device_extensions.size()] = extensions->device_extensions[i];
    }

    Set<const char*> required_layers(instance_layers.begin(), instance_layers.end());
    Vector<VkLayerProperties> available_layers = enumerate_properties(vkEnumerateInstanceLayerProperties);
    for (VkLayerProperties const& layer : available_layers)
        required_layers.erase(layer.layerName);

    RENDERING_ASSERT(required_layers.empty(), "required layers are not satisfied!");

    Set<const char*> required_extensions(instance_extensions.begin(), instance_extensions.end());
    Vector<VkExtensionProperties> available_extensions = enumerate_properties(vkEnumerateInstanceExtensionProperties, nullptr);
    for (VkExtensionProperties const& extension : available_extensions)
        required_extensions.erase(extension.extensionName);

    RENDERING_ASSERT(required_extensions.empty(), "required extensions are not satisfied!");

    VkInstanceCreateInfo create_info{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = static_cast<uint32_t>(instance_layers.size()),
        .ppEnabledLayerNames = instance_layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size()),
        .ppEnabledExtensionNames = instance_extensions.data(),
    };

    VkValidationFeaturesEXT validation_features{
        .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
    };
    VkValidationFeatureEnableEXT validation_feature_enable[] = {
        VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
    };
    if (info.enable_gpu_based_validation)
    {
        validation_features.enabledValidationFeatureCount = array_size(validation_feature_enable);
        validation_features.pEnabledValidationFeatures = validation_feature_enable;
        create_info.pNext = &validation_features;
    }

    VK_CHECK_RESULT(vkCreateInstance(&create_info, VK_Allocation_Callbacks_Ptr, &m_instance));

    volkLoadInstanceOnly(m_instance);

    // query device
    Vector<VkPhysicalDevice> adapters = enumerate_properties(vkEnumeratePhysicalDevices, m_instance);
    m_adapters.resize(adapters.size());
    for (size_t i = 0; i < adapters.size(); i++)
    {
        // query adapter
        VKAdapter* adapter = PLACEMENT_NEW(VKAdapter, sizeof(VKAdapter));
        adapter->m_physical_device = adapters[i];
        adapter->m_device_layers.resize(device_layers.size());
        adapter->m_device_extensions.resize(device_extensions.size());

        // device layers
        Vector<VkLayerProperties> layer_properties = enumerate_properties(vkEnumerateDeviceLayerProperties, adapters[i]);
        uint32_t layer_index = 0;
        for (VkLayerProperties const& layer : layer_properties)
        {
            for (const char* layer_name : device_layers)
            {
                if (strcmp(layer_name, layer.layerName) == 0)
                {
                    adapter->m_device_layers[layer_index] = layer_name;
                    layer_index++;
                }
            }
        }
        for (; layer_index < device_layers.size(); layer_index++)
            adapter->m_device_layers.pop_back();

        if (adapter->m_device_layers.size() != device_layers.size())
            RENDERING_LOG_WARNING("some required device layers are not satisfied, may cause problem!");

        // device extensions
        Vector<VkExtensionProperties> device_extension_properties = enumerate_properties(vkEnumerateDeviceExtensionProperties, adapters[i], nullptr);
        uint32_t extension_index = 0;
        for (VkExtensionProperties const& extension : device_extension_properties)
        {
            for (const char* extension_name : device_extensions)
            {
                if (strcmp(extension_name, extension.extensionName) == 0)
                {
                    adapter->m_device_extensions[extension_index] = extension_name;
                    extension_index++;
                }
            }
        }
        for (; extension_index < device_extensions.size(); extension_index++)
            adapter->m_device_extensions.pop_back();

        if (adapter->m_device_extensions.size() != device_extensions.size())
            RENDERING_LOG_WARNING("some required device extensions are not satisfied, may cause problem!");


        adapter->m_ref_instance = this;
        adapter->record_adapter_detail();

        m_adapters[i] = adapter;
    }

    //sort by gpu type
    sort(m_adapters.begin(), m_adapters.end(), [](const VKAdapter* a, const VKAdapter* b)
    {
        const uint32_t orders[] = {4, 1, 0, 2, 3};
        return orders[a->m_vulkan_detail.device_properties.deviceType] < orders[b->m_vulkan_detail.device_properties.deviceType];
    });

    // debug util
    if (info.enable_debug_layer)
    {
        VkDebugUtilsMessengerCreateInfoEXT messenger_create_info{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_callback,
            .pUserData = nullptr,
        };

        VK_CHECK_RESULT(vkCreateDebugUtilsMessengerEXT(m_instance, &messenger_create_info, VK_Allocation_Callbacks_Ptr, &m_debug_messenger));
    }
}

VKInstance::~VKInstance()
{
    for (VKAdapter* adapter : m_adapters)
    {
        PLACEMENT_DELETE(VKAdapter, adapter);
    }

    if (m_debug_messenger)
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, VK_Allocation_Callbacks_Ptr);
    vkDestroyInstance(m_instance, VK_Allocation_Callbacks_Ptr);
}

void VKInstance::enum_adapters(Vector<GPUAdapter*>& adapters) const
{
    adapters.resize(m_adapters.size());
    memcpy(adapters.data(), m_adapters.data(), sizeof(VKAdapter*) * m_adapters.size());
}

AMAZING_NAMESPACE_END