#ifndef VULKAN_EXAMPLE_HPP
#define VULKAN_EXAMPLE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanSwapchain {
 private:
  void exitOnError(const char* msg);
  void initInstance();

  const char* applicationName = "Vulkan Example";
  const char* engineName = "Vulkan Engine";

  VkInstance instance;

 public:
  VulkanSwapchain();
  virtual ~VulkanSwapchain();
};

#endif  // VULKAN_EXAMPLE_HPP
