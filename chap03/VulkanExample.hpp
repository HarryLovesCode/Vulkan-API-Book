#ifndef VULKAN_EXAMPLE_HPP
#define VULKAN_EXAMPLE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <cassert>
#include <vector>

class VulkanExample {
 private:
  void initInstance();
  void initDevices();

  const char *applicationName = "Vulkan Example";
  const char *engineName = "Vulkan Engine";

  VkInstance instance;
  VkPhysicalDevice physicalDevice;
  VkDevice device;

 public:
  VulkanExample();
  virtual ~VulkanExample();
};

#endif  // VULKAN_EXAMPLE_HPP
