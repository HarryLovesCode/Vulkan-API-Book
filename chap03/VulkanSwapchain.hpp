#ifndef VULKAN_EXAMPLE_HPP
#define VULKAN_EXAMPLE_HPP

#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanSwapchain {
 private:
  void exitOnError(const char* msg);
  void initInstance();
  void initDevices();

  const char* applicationName = "Vulkan Example";
  const char* engineName = "Vulkan Engine";

  VkInstance instance;
  VkPhysicalDevice physicalDevice;
  VkDevice device;

 public:
  VulkanSwapchain();
  virtual ~VulkanSwapchain();
};

#endif  // VULKAN_EXAMPLE_HPP
