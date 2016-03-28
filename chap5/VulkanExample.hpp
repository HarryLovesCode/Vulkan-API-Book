#ifndef VULKAN_EXAMPLE_HPP
#define VULKAN_EXAMPLE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <vulkan/vulkan.h>
#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <xcb/xcb.h>
#endif

#define GET_INSTANCE_PROC_ADDR(inst, entry)                              \
  {                                                                      \
    fp##entry = (PFN_vk##entry)vkGetInstanceProcAddr(inst, "vk" #entry); \
    if (!fp##entry)                                                      \
      exitOnError("vkGetInstanceProcAddr failed to find vk" #entry);     \
  }

#define GET_DEVICE_PROC_ADDR(dev, entry)                              \
  {                                                                   \
    fp##entry = (PFN_vk##entry)vkGetDeviceProcAddr(dev, "vk" #entry); \
    if (!fp##entry)                                                   \
      exitOnError("vkGetDeviceProcAddr failed to find vk" #entry);    \
  }

class VulkanExample {
 private:
  void exitOnError(const char *msg);
  void initInstance();
  void initDevices();
  void initSurface();

  const char *applicationName = "Vulkan Example";
  const char *engineName = "Vulkan Engine";

  const int windowWidth = 1280;
  const int windowHeight = 720;

  VkInstance instance;
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkSurfaceKHR surface;
  VkFormat colorFormat;
  VkColorSpaceKHR colorSpace;
  uint32_t queueIndex;

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
#if defined(_WIN32)
  HINSTANCE windowInstance;
  HWND window;
#elif defined(__linux__)
  xcb_connection_t *connection;
  xcb_window_t window;
  xcb_screen_t *screen;
  xcb_atom_t wmProtocols;
  xcb_atom_t wmDeleteWin;
#endif
 public:
  VulkanExample();
  virtual ~VulkanExample();

#if defined(_WIN32)
  void initWindow(HINSTANCE hInstance);
#elif defined(__linux__)
  void initWindow();
#endif
  void renderLoop();
};

#endif  // VULKAN_EXAMPLE_HPP
