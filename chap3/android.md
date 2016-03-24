# Android

In order to show something on the screen, we have to deal with **Window System Integration (WSI)**. The documentation I'm going to be using can be found [here](https://www.khronos.org/registry/vulkan/specs/1.0/refguide/Vulkan-1.0-web.pdf). To follow along, you can go to section **29.2.1**. 

# `VkAndroidSurfaceCreateInfoKHR`

Because we're getting into platform specific code, documentation can be a little hard to find. If you clicked on the documentation I linked, you'll see it's a quick reference guide. Looking at the documentation, we'll see in order to use `vkCreateAndroidSurfaceKHR`, we'll need to create a `VkAndroidSurfaceCreateInfoKHR` object. The definition looks like this...

```cpp
typedef struct VkAndroidSurfaceCreateInfoKHR {
    VkStructureType sType;
    const void* pNext;
    VkAndroidSurfaceCreateFlagsKHR flags;
    ANativeWindow* window;
} VkAndroidSurfaceCreateInfoKHR;
```

Following the valid usage from other sections, we can say that creating a `VkAndroidSurfaceCreateInfoKHR` object looks like...

```cpp
ANativeWindow * window;

VkAndroidSurfaceCreateInfoKHR createInfo{};
createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
createInfo.pNext = NULL;
createInfo.flags = 0;
createInfo.window = window;
```

# `vkCreateAndroidSurfaceKHR`

Again, because this is platform specific code, we don't have great documentation. The definition for this method looks like this...

```cpp
VkResult vkCreateAndroidSurfaceKHR(
    VkInstance instance, 
    const VkAndroidSurfaceCreateInfoKHR* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface);
```

Following valid usage from other sections, we can say that calling `vkCreateAndroidSurfaceKHR` looks like...

```cpp
VkResult result = vkCreateAndroidSurfaceKHR(instance, &createInfo, NULL, &surface);

if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create surface: %d\n", result);
    exit(EXIT_FAILURE);
}
```