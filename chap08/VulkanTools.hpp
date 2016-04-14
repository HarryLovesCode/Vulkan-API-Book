#ifndef VULKAN_TOOLS_HPP
#define VULKAN_TOOLS_HPP

#include <stdio.h>
#if defined(_WIN32)
#include <Windows.h>
#endif
#include <vulkan\vulkan.h>

#define APPLICATION_NAME "Vulkan Example"
#define ENGINE_NAME "Vulkan Engine"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

namespace VulkanTools {
void exitOnError(const char *msg);
void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                    VkImageAspectFlags aspects, VkImageLayout oldLayout,
                    VkImageLayout newLayout);
}

#endif  // VULKAN_TOOLS_HPP
