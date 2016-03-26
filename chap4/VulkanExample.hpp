#ifndef VULKAN_EXAMPLE_HPP
#define VULKAN_EXAMPLE_HPP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <vulkan/vulkan.h>
#if defined(_WIN32)
#include <Windows.h>
#elif defined (_LINUX)
#include <xcb/xcb.h>
#endif

class VulkanExample {
private:
    void exitOnError(const char * msg);
    void initInstance();
    void initDevices();
    void initWindow();

    const char * applicationName = "Vulkan Example";
    const char * engineName = "Vulkan Engine";

    const int windowWidth = 1280;
    const int windowHeight = 720;

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
#if defined(__linux__)
    xcb_connection_t * connection;
    xcb_window_t window;
    xcb_screen_t * screen;
#endif
public:
    VulkanExample();
    virtual ~VulkanExample();

    void renderLoop();
};

#endif // VULKAN_EXAMPLE_HPP
