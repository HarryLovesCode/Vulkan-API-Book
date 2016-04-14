#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#include <cassert>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <vulkan/vulkan.h>

#include "VulkanTools.hpp"

#define GET_INSTANCE_PROC_ADDR(inst, entry)                              \
  {                                                                      \
    fp##entry = (PFN_vk##entry)vkGetInstanceProcAddr(inst, "vk" #entry); \
    if (!fp##entry)                                                      \
      VulkanTools::exitOnError("vkGetInstanceProcAddr failed to find vk" #entry);     \
  }

#define GET_DEVICE_PROC_ADDR(dev, entry)                              \
  {                                                                   \
    fp##entry = (PFN_vk##entry)vkGetDeviceProcAddr(dev, "vk" #entry); \
    if (!fp##entry)                                                   \
      VulkanTools::exitOnError("vkGetDeviceProcAddr failed to find vk" #entry);    \
  }

struct SwapChainBuffer {
  VkImage image;
  VkImageView view;
  VkFramebuffer frameBuffer;
};

class VulkanSwapchain {
 private:
  VkInstance instance;
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkSurfaceKHR surface;
  VkFormat colorFormat;
  VkColorSpaceKHR colorSpace;
  uint32_t queueIndex;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> images;
  std::vector<SwapChainBuffer> buffers;

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
      xcb_connection_t* connection, xcb_window_t window
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

  void create(VkCommandBuffer cmdBuffer) {
    VkSurfaceCapabilitiesKHR caps = {};
    VkResult result =
        fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

    if (result != VK_SUCCESS)
      VulkanTools::exitOnError("Failed to get physical device surface capabilities");

    VkExtent2D swapchainExtent = {};

    if (caps.currentExtent.width == -1 || caps.currentExtent.height == -1) {
      swapchainExtent.width = WINDOW_WIDTH;
      swapchainExtent.height = WINDOW_HEIGHT;
    } else {
      swapchainExtent = caps.currentExtent;
    }

    uint32_t presentModeCount = 0;
    result = fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                                       &presentModeCount, NULL);

    assert(result == VK_SUCCESS);
    assert(presentModeCount >= 1);

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    result = fpGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice, surface, &presentModeCount, presentModes.data());

    assert(result == VK_SUCCESS);

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    for (uint32_t i = 0; i < presentModeCount; i++) {
      if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
        presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        break;
      }

      if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
        presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    assert(caps.maxImageCount >= 1);

    uint32_t imageCount = caps.minImageCount + 1;

    if (imageCount > caps.maxImageCount) imageCount = caps.maxImageCount;

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = colorFormat;
    swapchainCreateInfo.imageColorSpace = colorSpace;
    swapchainCreateInfo.imageExtent = {swapchainExtent.width,
                                       swapchainExtent.height};
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 1;
    swapchainCreateInfo.pQueueFamilyIndices = {0};
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;

    result = fpCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);

    assert(result == VK_SUCCESS);

    result = fpGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL);

    assert(result == VK_SUCCESS);

    images.resize(imageCount);
    buffers.resize(imageCount);

    result =
        fpGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());

    assert(result == VK_SUCCESS);
  }
};

#endif  // VULKAN_SWAPCHAIN_HPP
