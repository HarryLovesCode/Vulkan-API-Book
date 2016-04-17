#ifndef VULKAN_TOOLS_HPP
#define VULKAN_TOOLS_HPP

#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <Windows.h>
#endif

#define APPLICATION_NAME "Vulkan Example"
#define ENGINE_NAME "Vulkan Engine"

namespace VulkanTools {
void exitOnError(const char *msg);
}

#endif  // VULKAN_TOOLS_HPP
