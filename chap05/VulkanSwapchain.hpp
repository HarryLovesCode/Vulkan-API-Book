#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <cassert>
#include <cstring>
#include <vector>

#include "VulkanTools.hpp"

#define GET_INSTANCE_PROC_ADDR(inst, entry)                              \
  {                                                                      \
    fp##entry = (PFN_vk##entry)vkGetInstanceProcAddr(inst, "vk" #entry); \
    if (!fp##entry)                                                      \
      VulkanTools::exitOnError(                                          \
          "vkGetInstanceProcAddr failed to find vk" #entry);             \
  }

#define GET_DEVICE_PROC_ADDR(dev, entry)                              \
  {                                                                   \
    fp##entry = (PFN_vk##entry)vkGetDeviceProcAddr(dev, "vk" #entry); \
    if (!fp##entry)                                                   \
      VulkanTools::exitOnError(                                       \
          "vkGetDeviceProcAddr failed to find vk" #entry);            \
  }

class VulkanSwapchain {
private:
  VkInstance instance;
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkSurfaceKHR surface;

  PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
  PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
    fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
  PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
  PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
    fpGetPhysicalDeviceSurfacePresentModesKHR;

  PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
  PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
  PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
  PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
  PFN_vkQueuePresentKHR fpQueuePresentKHR;

public:
  VkSwapchainKHR swapchain;

  uint32_t imageCount;
  uint32_t queueIndex;

  VkFormat colorFormat;
  VkColorSpaceKHR colorSpace;

  void init(VkInstance instance, VkPhysicalDevice physicalDevice,
            VkDevice device) {
    this->instance = instance;
    this->physicalDevice = physicalDevice;
    this->device = device;

    GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
    GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
    GET_DEVICE_PROC_ADDR(device, CreateSwapchainKHR);
    GET_DEVICE_PROC_ADDR(device, DestroySwapchainKHR);
    GET_DEVICE_PROC_ADDR(device, GetSwapchainImagesKHR);
    GET_DEVICE_PROC_ADDR(device, AcquireNextImageKHR);
    GET_DEVICE_PROC_ADDR(device, QueuePresentKHR);
  }

  void createSurface(
#if defined(_WIN32)
      HINSTANCE windowInstance, HWND window
#elif defined(__linux__)
      xcb_connection_t *connection, xcb_window_t window
#endif
      ) {
#if defined(_WIN32)
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.hinstance = windowInstance;
    surfaceCreateInfo.hwnd = window;
    VkResult result =
        vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
#elif defined(__linux__)
    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.connection = connection;
    surfaceCreateInfo.window = window;
    VkResult result =
        vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
#endif

    assert(result == VK_SUCCESS);

    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);

    assert(queueCount >= 1);

    std::vector<VkQueueFamilyProperties> queueProperties(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount,
                                             queueProperties.data());

    queueIndex = UINT32_MAX;
    std::vector<VkBool32> supportsPresenting(queueCount);

    for (uint32_t i = 0; i < queueCount; i++) {
      fpGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface,
                                           &supportsPresenting[i]);
      if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
        if (supportsPresenting[i] == VK_TRUE) {
          queueIndex = i;
          break;
        }
      }
    }

    assert(queueIndex != UINT32_MAX);

    uint32_t formatCount = 0;
    result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                                  &formatCount, NULL);

    assert(result == VK_SUCCESS && formatCount >= 1);

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    result = fpGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice, surface, &formatCount, surfaceFormats.data());

    assert(result == VK_SUCCESS);

    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
      colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
    else
      colorFormat = surfaceFormats[0].format;

    colorSpace = surfaceFormats[0].colorSpace;
  }
};

#endif  // VULKAN_SWAPCHAIN_HPP
