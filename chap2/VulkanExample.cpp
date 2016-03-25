#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanExample {
private:
    void initInstance();

    const std::string applicationName = "Vulkan Example";
    const std::string engineName = "vkEngine";

    VkInstance instance;
public:
    VulkanExample();
    virtual ~VulkanExample();
};

VulkanExample::VulkanExample()
{
    this->initInstance();
}

VulkanExample::~VulkanExample()
{
    vkDestroyInstance(instance, NULL);
}

void VulkanExample::initInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.pEngineName = engineName.c_str();
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 4);

    std::vector<const char*> enabledExtensions = { 
        VK_KHR_SURFACE_EXTENSION_NAME 
    };

#if defined(_WIN32)
    enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__ANDROID__)
    enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
    enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "vkCreateInstance failed: %d\n", result);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char * argv[])
{
    VulkanExample ve = VulkanExample();
}
